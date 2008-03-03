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

// plastic.cpp*
#include "plastic.h"
#include "bxdf.h"
#include "lambertian.h"
#include "fresneldielectric.h"
#include "microfacet.h"
#include "blinn.h"
#include "paramset.h"

using namespace lux;

// Plastic Method Definitions
BSDF *Plastic::GetBSDF(const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading) const {
	// Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
	DifferentialGeometry dgs;
	if (bumpMap)
		Bump(bumpMap, dgGeom, dgShading, &dgs);
	else
		dgs = dgShading;
	BSDF *bsdf = BSDF_ALLOC( BSDF)(dgs, dgGeom.nn);
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum kd(Kd->Evaluate(dgs).Clamp(0.f, 1.f));
	BxDF *diff = BSDF_ALLOC( Lambertian)(kd);
	Fresnel *fresnel =
		BSDF_ALLOC( FresnelDielectric)(1.5f, 1.f);
    // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
	SWCSpectrum ks(Ks->Evaluate(dgs).Clamp(0.f, 1.f));
	float rough = roughness->Evaluate(dgs);
	BxDF *spec = BSDF_ALLOC( Microfacet)(ks, fresnel,
		BSDF_ALLOC( Blinn)(1.f / rough));
	bsdf->Add(diff);
	bsdf->Add(spec);
	return bsdf;
}
// Plastic Dynamic Creation Routine
Material* Plastic::CreateMaterial(const Transform &xform,
		const TextureParams &mp) {
	boost::shared_ptr<Texture<Spectrum> > Kd = mp.GetSpectrumTexture("Kd", Spectrum(1.f));
	boost::shared_ptr<Texture<Spectrum> > Ks = mp.GetSpectrumTexture("Ks", Spectrum(1.f));
	boost::shared_ptr<Texture<float> > roughness = mp.GetFloatTexture("roughness", .1f);
	boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap", 0.f);
	return new Plastic(Kd, Ks, roughness, bumpMap);
}
