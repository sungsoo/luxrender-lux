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

// heightfield.cpp*
#include "heightfield.h"

using namespace lux;

// Heightfield Method Definitions
Heightfield::Heightfield(const Transform &o2w, bool ro, int x, int y,
		const float *zs)
	: Shape(o2w, ro) {
	nx = x;
	ny = y;
	z = new float[nx*ny];
	memcpy(z, zs, nx*ny*sizeof(float));
}
Heightfield::~Heightfield() {
	delete[] z;
}
BBox Heightfield::ObjectBound() const {
	float minz = z[0], maxz = z[0];
	for (int i = 1; i < nx*ny; ++i) {
		if (z[i] < minz) minz = z[i];
		if (z[i] > maxz) maxz = z[i];
	}
	return BBox(Point(0,0,minz), Point(1,1,maxz));
}
bool Heightfield::CanIntersect() const {
	return false;
}
void Heightfield::Refine(vector<boost::shared_ptr<Shape> > &refined) const {
	int ntris = 2*(nx-1)*(ny-1);
	refined.reserve(ntris);
	int *verts = new int[3*ntris];
	Point *P = new Point[nx*ny];
	float *uvs = new float[2*nx*ny];
	int nverts = nx*ny;
	int x, y;
	// Compute heightfield vertex positions
	int pos = 0;
	for (y = 0; y < ny; ++y) {
		for (x = 0; x < nx; ++x) {
			P[pos].x = uvs[2*pos]   = (float)x / (float)(nx-1);
			P[pos].y = uvs[2*pos+1] = (float)y / (float)(ny-1);
			P[pos].z = z[pos];
			++pos;
		}
	}
	// Fill in heightfield vertex offset array
	int *vp = verts;
	for (y = 0; y < ny-1; ++y) {
		for (x = 0; x < nx-1; ++x) {
	#define VERT(x,y) ((x)+(y)*nx)
			*vp++ = VERT(x, y);
			*vp++ = VERT(x+1, y);
			*vp++ = VERT(x+1, y+1);
	
			*vp++ = VERT(x, y);
			*vp++ = VERT(x+1, y+1);
			*vp++ = VERT(x, y+1);
		}
	#undef VERT
	}
	ParamSet paramSet;
	paramSet.AddInt("indices", verts, 3*ntris);
	paramSet.AddFloat("uv", uvs, 2 * nverts);
	paramSet.AddPoint("P", P, nverts);
	refined.push_back(MakeShape("trianglemesh",
			ObjectToWorld, reverseOrientation, paramSet));
	delete[] P;
	delete[] uvs;
	delete[] verts;
}
Shape* Heightfield::CreateShape(const Transform &o2w,
		bool reverseOrientation, const ParamSet &params) {
	int nu = params.FindOneInt("nu", -1);
	int nv = params.FindOneInt("nv", -1);
	int nitems;
	const float *Pz = params.FindFloat("Pz", &nitems);
	BOOST_ASSERT(nitems == nu*nv);
	BOOST_ASSERT(nu != -1 && nv != -1 && Pz != NULL);
	return new Heightfield(o2w, reverseOrientation, nu, nv, Pz);
}
