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

// fresnelblend.cpp*
#include "fresnelblend.h"
#include "color.h"
#include "spectrum.h"
#include "mc.h"
#include "sampling.h"
#include "microfacetdistribution.h"
#include <stdarg.h>

using namespace lux;

FresnelBlend::FresnelBlend(const SWCSpectrum &d,
                           const SWCSpectrum &s,
						   MicrofacetDistribution *dist)
	: BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
	  Rd(d), Rs(s) {
	distribution = dist;
}
SWCSpectrum FresnelBlend::f(const Vector &wo,
                         const Vector &wi) const {
	SWCSpectrum diffuse = (28.f/(23.f*M_PI)) * Rd *
		(SWCSpectrum(1.) - Rs) *
		(1 - powf(1 - .5f * fabsf(CosTheta(wi)), 5)) *
		(1 - powf(1 - .5f * fabsf(CosTheta(wo)), 5));
	Vector H = Normalize(wi + wo);
	SWCSpectrum specular = distribution->D(H) /
		(4.f * AbsDot(wi, H) *
		max(fabsf(CosTheta(wi)), fabsf(CosTheta(wo)))) *
		SchlickFresnel(Dot(wi, H));
	return diffuse + specular;
}

SWCSpectrum FresnelBlend::Sample_f(const Vector &wo,
	Vector *wi, float u1, float u2, float *pdf, float *pdfBack, bool reverse) const
{
	u1 *= 2.f;
	if (u1 < 1.f) {
		// Cosine-sample the hemisphere, flipping the direction if necessary
		*wi = CosineSampleHemisphere(u1, u2);
		if (wo.z < 0.) wi->z *= -1.f;
	}
	else {
		u1 -= 1.f;
		distribution->Sample_f(wo, wi, u1, u2, pdf);
	}
	*pdf = Pdf(wo, *wi);
	if (*pdf == 0.f) {
		if (pdfBack)
			*pdfBack = 0.f;
		return SWCSpectrum(0.f);
	}
	if (pdfBack)
		*pdfBack = Pdf(*wi, wo);
	if (reverse)
		return f(*wi, wo) * (wo.z / wi->z);
	else
		return f(wo, *wi);
}
float FresnelBlend::Pdf(const Vector &wo,
		const Vector &wi) const {
	if (!SameHemisphere(wo, wi)) return 0.f;
	return .5f * (fabsf(wi.z) * INV_PI +
		distribution->Pdf(wo, wi));
}

