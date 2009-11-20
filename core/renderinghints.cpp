/***************************************************************************
 *   Copyright (C) 1998-2009 by authors (see AUTHORS.txt )                 *
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

#include "renderinghints.h"
#include "error.h"
#include "transport.h"
#include "light.h"
#include "mc.h"
#include "spectrumwavelengths.h"

#include <boost/assert.hpp>

using namespace lux;

//------------------------------------------------------------------------------
// Light Rendering Hints
//------------------------------------------------------------------------------

void LightRenderingHints::Init(const ParamSet &params) {
	importance = max(params.FindOneFloat("importance", 1.f), 0.f);
}

//------------------------------------------------------------------------------
// Light Sampling Strategies
//------------------------------------------------------------------------------

//******************************************************************************
// Light Sampling Strategies: LightStrategyAllUniform
//******************************************************************************

void LightStrategyAllUniform::RequestSamples(vector<u_int> &structure) const {
	structure.push_back(2);	// light position sample
	structure.push_back(1);	// light number/portal sample
	structure.push_back(2);	// bsdf direction sample for light
	structure.push_back(1);	// bsdf component sample for light
}

u_int LightStrategyAllUniform::SampleLights(
	const TsPack *tspack, const Scene *scene, const u_int shadowRayCount,
	const Point &p, const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, const float *sampleData, const SWCSpectrum &scale,
	vector<SWCSpectrum> &L) const {
	// Sample all lights in the scene
	const u_int nLights = scene->lights.size();
	if (nLights == 0)
		return 0;

	const SWCSpectrum newScale = scale / shadowRayCount;
	const u_int sampleCount = this->RequestSamplesCount();
	u_int nContribs = 0;
	for (u_int i = 0; i < shadowRayCount; ++i) {
		const u_int offset = i * sampleCount;
		const float *lightSample = &sampleData[offset];
		const float *lightNum = &sampleData[offset + 2];
		const float *bsdfSample = &sampleData[offset + 3];
		const float *bsdfComponent =  &sampleData[offset + 5];

		SWCSpectrum Ll;
		for (u_int i = 0; i < nLights; ++i) {
			const Light *light = scene->lights[i];
			Ll = EstimateDirect(tspack, scene, light,
				p, n, wo, bsdf, sample, lightSample[0], lightSample[1], *lightNum,
				bsdfSample[0], bsdfSample[1], *bsdfComponent);

			if (!Ll.Black()) {
				L[light->group] += Ll * newScale;
				++nContribs;
			}
		}
	}

	return nContribs;
}

//******************************************************************************
// Light Sampling Strategies: LightStrategyOneUniform
//******************************************************************************

void LightStrategyOneUniform::RequestSamples(vector<u_int> &structure) const {
	structure.push_back(2);	// light position sample
	structure.push_back(1);	// light number/portal sample
	structure.push_back(2);	// bsdf direction sample for light
	structure.push_back(1);	// bsdf component sample for light
}

u_int LightStrategyOneUniform::SampleLights(
	const TsPack *tspack, const Scene *scene, const u_int shadowRayCount,
	const Point &p, const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, const float *sampleData, const SWCSpectrum &scale,
	vector<SWCSpectrum> &L) const {
	// Randomly choose a single light to sample
	const u_int nLights = scene->lights.size();
	if (nLights == 0)
		return 0;

	const SWCSpectrum newScale = static_cast<float>(nLights) * scale / shadowRayCount;
	const u_int sampleCount = this->RequestSamplesCount();
	u_int nContribs = 0;
	for (u_int i = 0; i < shadowRayCount; ++i) {
		const u_int offset = i * sampleCount;
		const float *lightSample = &sampleData[offset];
		const float *lightNum = &sampleData[offset + 2];
		const float *bsdfSample = &sampleData[offset + 3];
		const float *bsdfComponent =  &sampleData[offset + 5];

		float ls3 = *lightNum * nLights;
		const u_int lightNumber = min(Floor2UInt(ls3), nLights - 1);
		ls3 -= lightNumber;
		const Light *light = scene->lights[lightNumber];
		SWCSpectrum Ll = EstimateDirect(tspack, scene, light,
				p, n, wo, bsdf, sample, lightSample[0], lightSample[1], ls3,
				bsdfSample[0], bsdfSample[1], *bsdfComponent);

		if (!Ll.Black()) {
			L[light->group] += Ll * newScale;
			++nContribs;
		}
	}

	return nContribs;
}

//******************************************************************************
// Light Sampling Strategies: LightStrategyOneImportance
//******************************************************************************

LightStrategyOneImportance::LightStrategyOneImportance(const Scene *scene) {
	// Compute light importance CDF
	const u_int nLights = scene->lights.size();
	lightImportance = new float[nLights];
	lightCDF = new float[nLights + 1];

	for (u_int i = 0; i < nLights; ++i)
		lightImportance[i] = scene->lights[i]->GetRenderingHints()->GetImportance();

	ComputeStep1dCDF(lightImportance, nLights, &totalImportance, lightCDF);
}

void LightStrategyOneImportance::RequestSamples(vector<u_int> &structure) const {
	structure.push_back(2);	// light position sample
	structure.push_back(1);	// light number/portal sample
	structure.push_back(2);	// bsdf direction sample for light
	structure.push_back(1);	// bsdf component sample for light
}

u_int LightStrategyOneImportance::SampleLights(
	const TsPack *tspack, const Scene *scene, const u_int shadowRayCount,
	const Point &p, const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, const float *sampleData, const SWCSpectrum &scale,
	vector<SWCSpectrum> &L) const {
	// Choose a single light to sample according the importance
	const u_int nLights = scene->lights.size();
	if (nLights == 0)
		return 0;

	const SWCSpectrum newScale = scale / shadowRayCount;
	const u_int sampleCount = this->RequestSamplesCount();
	u_int nContribs = 0;
	for (u_int i = 0; i < shadowRayCount; ++i) {
		const u_int offset = i * sampleCount;
		const float *lightSample = &sampleData[offset];
		const float *lightNum = &sampleData[offset + 2];
		const float *bsdfSample = &sampleData[offset + 3];
		const float *bsdfComponent =  &sampleData[offset + 5];

		float ls3 = *lightNum * nLights;
		float lightPdf;
		u_int lightNumber = Floor2UInt(SampleStep1d(lightImportance, lightCDF,
			totalImportance, nLights, *lightNum, &lightPdf) * nLights);
		lightNumber = min(lightNumber, nLights - 1);
		ls3 -= lightNumber;
		const Light *light = scene->lights[lightNumber];

		SWCSpectrum Ll = EstimateDirect(tspack, scene, light,
				p, n, wo, bsdf, sample, lightSample[0], lightSample[1], ls3,
				bsdfSample[0], bsdfSample[1], *bsdfComponent);

		if (!Ll.Black()) {
			Ll *=  newScale / lightPdf;
			L[light->group] += Ll;
			++nContribs;
		}
	}

	return nContribs;
}

//******************************************************************************
// Light Sampling Strategies: LightStrategyOnePowerImportance
//******************************************************************************

LightStrategyOnePowerImportance::LightStrategyOnePowerImportance(const Scene *scene) {
	// Compute light power CDF
	const u_int nLights = scene->lights.size();
	lightPower = new float[nLights];
	lightCDF = new float[nLights + 1];

	// Averge the light power
	for (u_int i = 0; i < nLights; ++i) {
		const Light *l = scene->lights[i];
		lightPower[i] = l->GetRenderingHints()->GetImportance() * l->Power(scene);
	}

	ComputeStep1dCDF(lightPower, nLights, &totalPower, lightCDF);
}

void LightStrategyOnePowerImportance::RequestSamples(vector<u_int> &structure) const {
	structure.push_back(2);	// light position sample
	structure.push_back(1);	// light number/portal sample
	structure.push_back(2);	// bsdf direction sample for light
	structure.push_back(1);	// bsdf component sample for light
}

u_int LightStrategyOnePowerImportance::SampleLights(
	const TsPack *tspack, const Scene *scene, const u_int shadowRayCount,
	const Point &p, const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, const float *sampleData, const SWCSpectrum &scale,
	vector<SWCSpectrum> &L) const {
	// Choose a single light to sample according the importance
	const u_int nLights = scene->lights.size();
	if (nLights == 0)
		return 0;

	const SWCSpectrum newScale = scale / shadowRayCount;
	const u_int sampleCount = this->RequestSamplesCount();
	u_int nContribs = 0;
	for (u_int i = 0; i < shadowRayCount; ++i) {
		const u_int offset = i * sampleCount;
		const float *lightSample = &sampleData[offset];
		const float *lightNum = &sampleData[offset + 2];
		const float *bsdfSample = &sampleData[offset + 3];
		const float *bsdfComponent =  &sampleData[offset + 5];

		float ls3 = *lightNum * nLights;
		float lightPdf;
		u_int lightNumber = Floor2UInt(SampleStep1d(lightPower, lightCDF,
			totalPower, nLights, *lightNum, &lightPdf) * nLights);
		lightNumber = min(lightNumber, nLights - 1);
		ls3 -= lightNumber;
		const Light *light = scene->lights[lightNumber];

		SWCSpectrum Ll = EstimateDirect(tspack, scene, light,
				p, n, wo, bsdf, sample, lightSample[0], lightSample[1], ls3,
				bsdfSample[0], bsdfSample[1], *bsdfComponent);

		if (!Ll.Black()) {
			Ll *=  newScale / lightPdf;
			L[light->group] += Ll;
			++nContribs;
		}
	}

	return nContribs;
}

//******************************************************************************
// Light Sampling Strategies: LightStrategyAllPowerImportance
//******************************************************************************

u_int LightStrategyAllPowerImportance::SampleLights(
	const TsPack *tspack, const Scene *scene, const u_int shadowRayCount,
	const Point &p, const Normal &n, const Vector &wo, BSDF *bsdf,
	const Sample *sample, const float *sampleData, const SWCSpectrum &scale,
	vector<SWCSpectrum> &L) const {
	// Choose a single light to sample according the importance
	u_int nLights = scene->lights.size();
	if (nLights == 0)
		return 0;

	const u_int sampleCount = this->RequestSamplesCount();
	u_int nContribs = 0;
	if (nLights > shadowRayCount) {
		// We don't have enough shadow rays, just work as LightStrategyOnePowerImportance
		const SWCSpectrum newScale = scale / shadowRayCount;
		for (u_int i = 0; i < shadowRayCount; ++i) {
			const u_int offset = i * sampleCount;
			const float *lightSample = &sampleData[offset];
			const float *lightNum = &sampleData[offset + 2];
			const float *bsdfSample = &sampleData[offset + 3];
			const float *bsdfComponent =  &sampleData[offset + 5];

			float ls3 = *lightNum * nLights;
			float lightPdf;
			u_int lightNumber = Floor2UInt(SampleStep1d(lightPower, lightCDF,
				totalPower, nLights, *lightNum, &lightPdf) * nLights);
			lightNumber = min(lightNumber, nLights - 1);
			ls3 -= lightNumber;
			const Light *light = scene->lights[lightNumber];

			SWCSpectrum Ll = EstimateDirect(tspack, scene, light,
					p, n, wo, bsdf, sample, lightSample[0], lightSample[1], ls3,
					bsdfSample[0], bsdfSample[1], *bsdfComponent);

			if (!Ll.Black()) {
				Ll *=  newScale / lightPdf;
				L[light->group] += Ll;
				++nContribs;
			}
		}
	} else {
		// We have enough shadow ray, trace at least one ray for each light source
		for (u_int i = 0; i < nLights; ++i) {
			const u_int offset = i * sampleCount;
			const float *lightSample = &sampleData[offset];
			const float *lightNum = &sampleData[offset + 2];
			const float *bsdfSample = &sampleData[offset + 3];
			const float *bsdfComponent =  &sampleData[offset + 5];

			const Light *light = scene->lights[i];
			SWCSpectrum Ll = EstimateDirect(tspack, scene, light,
					p, n, wo, bsdf, sample, lightSample[0], lightSample[1], *lightNum,
					bsdfSample[0], bsdfSample[1], *bsdfComponent);

			if (!Ll.Black()) {
				L[light->group] += Ll * scale;
				++nContribs;
			}
		}

		// Use lightCDF for any left shadow ray
		const SWCSpectrum newScale = scale / (shadowRayCount - nLights);
		for (u_int i = nLights; i < shadowRayCount; ++i) {
			const u_int offset = i * sampleCount;
			const float *lightSample = &sampleData[offset];
			const float *lightNum = &sampleData[offset + 2];
			const float *bsdfSample = &sampleData[offset + 3];
			const float *bsdfComponent =  &sampleData[offset + 5];

			float ls3 = *lightNum * nLights;
			float lightPdf;
			u_int lightNumber = Floor2UInt(SampleStep1d(lightPower, lightCDF,
				totalPower, nLights, *lightNum, &lightPdf) * nLights);
			lightNumber = min(lightNumber, nLights - 1);
			ls3 -= lightNumber;
			const Light *light = scene->lights[lightNumber];

			SWCSpectrum Ll = EstimateDirect(tspack, scene, light,
					p, n, wo, bsdf, sample, lightSample[0], lightSample[1], ls3,
					bsdfSample[0], bsdfSample[1], *bsdfComponent);

			if (!Ll.Black()) {
				Ll *=  newScale / lightPdf;
				L[light->group] += Ll;
				++nContribs;
			}
		}
	}

	return nContribs;
}

//******************************************************************************
// Light Sampling Strategies: LightStrategyOneLogPowerImportance
//******************************************************************************

LightStrategyOneLogPowerImportance::LightStrategyOneLogPowerImportance(const Scene *scene) {
	// Compute light power CDF
	const u_int nLights = scene->lights.size();
	lightPower = new float[nLights];
	lightCDF = new float[nLights + 1];

	// Averge the light power
	for (u_int i = 0; i < nLights; ++i) {
		const Light *l = scene->lights[i];
		lightPower[i] = logf(l->GetRenderingHints()->GetImportance() * l->Power(scene));
	}

	ComputeStep1dCDF(lightPower, nLights, &totalPower, lightCDF);
}

//------------------------------------------------------------------------------
// SurfaceIntegrator Rendering Hints
//------------------------------------------------------------------------------

void SurfaceIntegratorRenderingHints::Init(const ParamSet &params) {
	shadowRayCount = max(params.FindOneInt("shadowraycount", 1), 1);

	// For sompatibility with past versions
	string oldst = params.FindOneString("strategy", "auto");
	string newst = params.FindOneString("lightstrategy", "auto");
	string st;
	if ((oldst == newst) || (oldst == "auto"))
		st = newst;
	else
		st = oldst;

	if (st == "one") lightStrategyType = LightStrategy::SAMPLE_ONE_UNIFORM;
	else if (st == "all") lightStrategyType = LightStrategy::SAMPLE_ALL_UNIFORM;
	else if (st == "auto") lightStrategyType = LightStrategy::SAMPLE_AUTOMATIC;
	else if (st == "importance") lightStrategyType = LightStrategy::SAMPLE_ONE_IMPORTANCE;
	else if (st == "powerimp") lightStrategyType = LightStrategy::SAMPLE_ONE_POWER_IMPORTANCE;
	else if (st == "allpowerimp") lightStrategyType = LightStrategy::SAMPLE_ALL_POWER_IMPORTANCE;
	else if (st == "logpowerimp") lightStrategyType = LightStrategy::SAMPLE_ONE_LOG_POWER_IMPORTANCE;
	else {
		std::stringstream ss;
		ss << "Strategy  '" << st << "' unknown. Using \"auto\".";
		luxError(LUX_BADTOKEN, LUX_WARNING, ss.str().c_str());
		lightStrategyType = LightStrategy::SAMPLE_AUTOMATIC;
	}
}

void SurfaceIntegratorRenderingHints::CreateLightStrategy(const Scene *scene) {
	switch (lightStrategyType) {
		case LightStrategy::SAMPLE_ALL_UNIFORM:
			lightStrategy = new LightStrategyAllUniform();
			return;
		case LightStrategy::SAMPLE_ONE_UNIFORM:
			lightStrategy =  new LightStrategyOneUniform();
			return;
		case LightStrategy::SAMPLE_AUTOMATIC:
			if (scene->lights.size() > 5)
				lightStrategyType = LightStrategy::SAMPLE_ONE_UNIFORM;
			else
				lightStrategyType = LightStrategy::SAMPLE_ALL_UNIFORM;

			this->CreateLightStrategy(scene);
			return;
		case LightStrategy::SAMPLE_ONE_IMPORTANCE:
			lightStrategy = new LightStrategyOneImportance(scene);
			return;
		case LightStrategy::SAMPLE_ONE_POWER_IMPORTANCE:
			lightStrategy = new LightStrategyOnePowerImportance(scene);
			return;
		case LightStrategy::SAMPLE_ALL_POWER_IMPORTANCE:
			lightStrategy = new LightStrategyAllPowerImportance(scene);
			return;
		case LightStrategy::SAMPLE_ONE_LOG_POWER_IMPORTANCE:
			lightStrategy = new LightStrategyOneLogPowerImportance(scene);
			return;
		default:
			BOOST_ASSERT(false);
	}
}

void SurfaceIntegratorRenderingHints::RequestLightSamples(vector<u_int> &structure) {
	// Request samples for each shadow ray we have to trace
	for (u_int i = 0; i <  shadowRayCount; ++i)
		lightStrategy->RequestSamples(structure);
}
