/***************************************************************************
 *   Copyright (C) 1998-2008 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of LuxRender.                                       *
 *                                                                         *
 *   Lux Renderer is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Lux Renderer is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   This project is based on PBRT ; see http://www.pbrt.org               *
 *   Lux Renderer website : http://www.luxrender.net                       *
 ***************************************************************************/

// infinite.cpp*
#include "infinite.h"
#include "imagereader.h"
#include "mc.h"
#include "paramset.h"
#include "blackbody.h"
using namespace lux;

// InfiniteAreaLight Method Definitions
InfiniteAreaLight::~InfiniteAreaLight() {
	delete radianceMap;
	delete SPDbase;
}
InfiniteAreaLight
	::InfiniteAreaLight(const Transform &light2world, const Spectrum &l, int ns, const string &texmap, float gain, float gamma)
	: Light(light2world, ns) {
	radianceMap = NULL;
	if (texmap != "") {
		auto_ptr<ImageData> imgdata(ReadImage(texmap));
		if(imgdata.get()!=NULL)
		{
			radianceMap = imgdata->createMIPMap<Spectrum>(BILINEAR, 8.f, 
				TEXTURE_REPEAT, 1.f, gamma);
		}
		else
			radianceMap=NULL;
	}

	// Base illuminant SPD
	SPDbase = new BlackbodySPD();
	SPDbase->Normalize();
	SPDbase->Scale(gain);

	// Base RGB spectrum
	Lbase = l;
}

SWCSpectrum
	InfiniteAreaLight::Le(const RayDifferential &r) const {
	Vector w = r.d;
	// Compute infinite light radiance for direction
	
	Spectrum L = Lbase;
	if (radianceMap != NULL) {
		Vector wh = Normalize(WorldToLight(w));
		float s = SphericalPhi(wh) * INV_TWOPI;
		float t = SphericalTheta(wh) * INV_PI;
		L *= radianceMap->Lookup(s, t);
	}

	return SWCSpectrum(SPDbase) * SWCSpectrum(L);
}
SWCSpectrum InfiniteAreaLight::Sample_L(const Point &p,
		const Normal &n, float u1, float u2, float u3,
		Vector *wi, float *pdf,
		VisibilityTester *visibility) const {
	if(!havePortalShape) {
		// Sample cosine-weighted direction on unit sphere
		float x, y, z;
		ConcentricSampleDisk(u1, u2, &x, &y);
		z = sqrtf(max(0.f, 1.f - x*x - y*y));
		if (u3 < .5) z *= -1;
		*wi = Vector(x, y, z);
		// Compute _pdf_ for cosine-weighted infinite light direction
		*pdf = fabsf(wi->z) * INV_TWOPI;
		// Transform direction to world space
		Vector v1, v2;
		CoordinateSystem(Normalize(Vector(n)), &v1, &v2);
		*wi = Vector(v1.x * wi->x + v2.x * wi->y + n.x * wi->z,
					 v1.y * wi->x + v2.y * wi->y + n.y * wi->z,
					 v1.z * wi->x + v2.z * wi->y + n.z * wi->z);
	} else {
	    // Sample Portal
		int shapeidx = 0;
		if(nrPortalShapes > 1) 
			shapeidx = min<float>(nrPortalShapes - 1,
					Floor2Int(u3 * nrPortalShapes));
		Normal ns;
		Point ps;
		bool found = false;
		for (int i = 0; i < nrPortalShapes; ++i) {
			ps = PortalShapes[shapeidx]->Sample(p, u1, u2, &ns);
			*wi = Normalize(ps - p);
			if (Dot(*wi, ns) < 0.f) {
				found = true;
				break;
			}

			if (++shapeidx >= nrPortalShapes)
				shapeidx = 0;
		}

		if (found)
			*pdf = PortalShapes[shapeidx]->Pdf(p, *wi);
		else {
			*pdf = 0.f;
			return SWCSpectrum(0.f);
		}
	}
	visibility->SetRay(p, *wi);
	return Le(RayDifferential(p, *wi));
}
float InfiniteAreaLight::Pdf(const Point &, const Normal &n,
		const Vector &wi) const {
	return AbsDot(n, wi) * INV_TWOPI;
}
SWCSpectrum InfiniteAreaLight::Sample_L(const Point &p,
		float u1, float u2, float u3, Vector *wi, float *pdf,
		VisibilityTester *visibility) const {
	if(!havePortalShape) {
		*wi = UniformSampleSphere(u1, u2);
		*pdf = UniformSpherePdf();
	} else {
	    // Sample a random Portal
		int shapeidx = 0;
		if(nrPortalShapes > 1) 
			shapeidx = min<float>(nrPortalShapes - 1,
					Floor2Int(lux::random::floatValue() * nrPortalShapes));
		Normal ns;
		Point ps;
		bool found = false;
		for (int i = 0; i < nrPortalShapes; ++i) {
			ps = PortalShapes[shapeidx]->Sample(p, u1, u2, &ns);
			*wi = Normalize(ps - p);
			if (Dot(*wi, ns) < 0.f) {
				found = true;
				break;
			}

			if (++shapeidx >= nrPortalShapes)
				shapeidx = 0;
		}

		if (found)
			*pdf = PortalShapes[shapeidx]->Pdf(p, *wi);
		else {
			*pdf = 0.f;
			return SWCSpectrum(0.f);
		}
	}
	visibility->SetRay(p, *wi);
	return Le(RayDifferential(p, *wi));
}
float InfiniteAreaLight::Pdf(const Point &, const Vector &) const {
	return 1.f / (4.f * M_PI);
}
SWCSpectrum InfiniteAreaLight::Sample_L(const Scene *scene,
		float u1, float u2, float u3, float u4,
		Ray *ray, float *pdf) const {
	if(!havePortalShape) {
		// Choose two points _p1_ and _p2_ on scene bounding sphere
		Point worldCenter;
		float worldRadius;
		scene->WorldBound().BoundingSphere(&worldCenter,
											&worldRadius);
		worldRadius *= 1.01f;
		Point p1 = worldCenter + worldRadius *
			UniformSampleSphere(u1, u2);
		Point p2 = worldCenter + worldRadius *
			UniformSampleSphere(u3, u4);
		// Construct ray between _p1_ and _p2_
		ray->o = p1;
		ray->d = Normalize(p2-p1);
		// Compute _InfiniteAreaLight_ ray weight
		Vector to_center = Normalize(worldCenter - p1);
		float costheta = AbsDot(to_center,ray->d);
		*pdf =
			costheta / ((4.f * M_PI * worldRadius * worldRadius));		
	} else {
		// Dade - choose a random portal. This strategy is quite bad if there
		// is more than one portal.
		int shapeidx = 0;
		if(nrPortalShapes > 1) 
			shapeidx = min<float>(nrPortalShapes - 1,
					Floor2Int(lux::random::floatValue() * nrPortalShapes));

		Normal ns;
		ray->o = PortalShapes[shapeidx]->Sample(u1, u2, &ns);
		ray->d = UniformSampleSphere(u3, u4);
		if (Dot(ray->d, ns) < 0.) ray->d *= -1;

		*pdf = PortalShapes[shapeidx]->Pdf(ray->o) * INV_TWOPI / nrPortalShapes;
	}

	return Le(RayDifferential(ray->o, -ray->d));
}
SWCSpectrum InfiniteAreaLight::Sample_L(const Point &p,
		Vector *wi, VisibilityTester *visibility) const {
	float pdf;
	SWCSpectrum L = Sample_L(p, lux::random::floatValue(), lux::random::floatValue(),
		lux::random::floatValue(), wi, &pdf, visibility);
	if (pdf == 0.f) return SWCSpectrum(0.f);
	return L / pdf;
}
Light* InfiniteAreaLight::CreateLight(const Transform &light2world,
		const ParamSet &paramSet) {
	Spectrum L = paramSet.FindOneSpectrum("L", Spectrum(1.0));
	string texmap = paramSet.FindOneString("mapname", "");
	int nSamples = paramSet.FindOneInt("nsamples", 1);

	// Initialize _ImageTexture_ parameters
	float gain = paramSet.FindOneFloat("gain", 1.0f);
	float gamma = paramSet.FindOneFloat("gamma", 1.0f);

	return new InfiniteAreaLight(light2world, L, nSamples, texmap, gain, gamma);
}
