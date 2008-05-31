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

// metrosampler.h*

#ifndef LUX_METROSAMPLER_H
#define LUX_METROSAMPLER_H

#include "sampling.h"
#include "paramset.h"
#include "film.h"

namespace lux
{

class MetropolisSampler : public Sampler {
public:
	MetropolisSampler(int xStart, int xEnd, int yStart, int yEnd,
			int maxRej, float largeProb, float rng, int sw, bool useV);
	~MetropolisSampler();

	virtual MetropolisSampler* clone() const;
	u_int GetTotalSamplePos() { return 0; }
	int RoundSize(int size) const { return size; }
	bool GetNextSample(Sample *sample, u_int *use_pos);
	float *GetLazyValues(Sample *sample, u_int num, u_int pos);
	void AddSample(float imageX, float imageY, const Sample &sample, const Ray &ray, const XYZColor &L, float alpha, int id=0);
	void AddSample(const Sample &sample);
	static Sampler *CreateSampler(const ParamSet &params, const Film *film);
	void GetBufferType(BufferType *t) { *t = BUF_TYPE_PER_SCREEN; }

	bool large;
	float LY, V;
	int normalSamples, totalSamples, totalTimes, maxRejects, consecRejects, stamp;
	float pLarge, range, weight, alpha;
	float *sampleImage;
	int *timeImage, *offset;
	static int initCount, initSamples;
	static float meanIntensity;
	vector <Sample::Contribution> oldContributions;
	float *strataSamples;
	int strataWidth, strataSqr, currentStrata;
	bool useVariance;
};

}//namespace lux

#endif // LUX_METROSAMPLER_H

