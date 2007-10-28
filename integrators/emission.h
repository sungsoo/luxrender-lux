/***************************************************************************
 *   Copyright (C) 1998-2007 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of Lux Renderer.                                    *
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
 *   Lux Renderer website : http://www.luxrender.org                       *
 ***************************************************************************/

// emission.cpp*
#include "volume.h"
#include "transport.h"
#include "scene.h"
// EmissionIntegrator Declarations
class EmissionIntegrator : public VolumeIntegrator {
public:
	// EmissionIntegrator Public Methods
	EmissionIntegrator(float ss) { stepSize = ss; }
	void RequestSamples(Sample *sample, const Scene *scene);
	Spectrum Transmittance(const Scene *, const Ray &ray, const Sample *sample, float *alpha) const;
	Spectrum Li(MemoryArena &arena, const Scene *, const RayDifferential &ray, const Sample *sample, float *alpha) const;
	virtual EmissionIntegrator* clone() const; // Lux (copy) constructor for multithreading
	IntegrationSampler* HasIntegrationSampler(IntegrationSampler *is) { return NULL; }; // Not implemented
	static VolumeIntegrator *CreateVolumeIntegrator(const ParamSet &params);
private:
	// EmissionIntegrator Private Data
	float stepSize;
	int tauSampleOffset, scatterSampleOffset;
};
