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

// loopsubdiv.cpp*
#include "loopsubdiv.h"
#include "paramset.h"
#include <boost/pool/object_pool.hpp>

using namespace lux;

// LoopSubdiv Method Definitions
LoopSubdiv::LoopSubdiv(const Transform &o2w, bool ro,
        int nfaces, int nvertices,
		const int *vertexIndices,
		const Point *P, int nl)
	: Shape(o2w, ro) {
	nLevels = nl;
	// Allocate _LoopSubdiv_ vertices and faces
	int i;
	SDVertex *verts = new SDVertex[nvertices];
	for (i = 0; i < nvertices; ++i) {
		verts[i] = SDVertex(P[i]);
		vertices.push_back(&verts[i]);
	}
	SDFace *fs = new SDFace[nfaces];
	for (i = 0; i < nfaces; ++i)
		faces.push_back(&fs[i]);
	// Set face to vertex pointers
	const int *vp = vertexIndices;
	for (i = 0; i < nfaces; ++i) {
		SDFace *f = faces[i];
		for (int j = 0; j < 3; ++j) {
			SDVertex *v = vertices[vp[j]];
			f->v[j] = v;
			v->startFace = f;
		}
		vp += 3;
	}
	// Set neighbor pointers in _faces_
	set<SDEdge> edges;
	for (i = 0; i < nfaces; ++i) {
		SDFace *f = faces[i];
		for (int edgeNum = 0; edgeNum < 3; ++edgeNum) {
			// Update neighbor pointer for _edgeNum_
			int v0 = edgeNum, v1 = NEXT(edgeNum);
			SDEdge e(f->v[v0], f->v[v1]);
			if (edges.find(e) == edges.end()) {
				// Handle new edge
				e.f[0] = f;
				e.f0edgeNum = edgeNum;
				edges.insert(e);
			}
			else {
				// Handle previously-seen edge
				e = *edges.find(e);
				e.f[0]->f[e.f0edgeNum] = f;
				f->f[edgeNum] = e.f[0];
				edges.erase(e);
			}
		}
	}
	// Finish vertex initialization
	for (i = 0; i < nvertices; ++i) {
		SDVertex *v = vertices[i];
		SDFace *f = v->startFace;
		do {
			f = f->nextFace(v);
		} while (f && f != v->startFace);
		v->boundary = (f == NULL);
		if (!v->boundary && v->valence() == 6)
			v->regular = true;
		else if (v->boundary && v->valence() == 4)
			v->regular = true;
		else
			v->regular = false;
	}
}
LoopSubdiv::~LoopSubdiv() {
	delete[] vertices[0];
	delete[] faces[0];
}
BBox LoopSubdiv::ObjectBound() const {
	BBox b;
	for (u_int i = 0; i < vertices.size(); i++)
		b = Union(b, vertices[i]->P);
	return b;
}
BBox LoopSubdiv::WorldBound() const {
	BBox b;
	for (u_int i = 0; i < vertices.size(); i++)
		b = Union(b, ObjectToWorld(vertices[i]->P));
	return b;
}
bool LoopSubdiv::CanIntersect() const {
	return false;
}
void
LoopSubdiv::Refine(vector<boost::shared_ptr<Shape> > &refined)
const {
	vector<SDFace *> f = faces;
	vector<SDVertex *> v = vertices;
	boost::object_pool<SDVertex> vertexArena;
	boost::object_pool<SDFace> faceArena;
	for (int i = 0; i < nLevels; ++i) {
		// Update _f_ and _v_ for next level of subdivision
		vector<SDFace *> newFaces;
		vector<SDVertex *> newVertices;
		// Allocate next level of children in mesh tree
		for (u_int j = 0; j < v.size(); ++j) {
			v[j]->child = vertexArena.malloc();//new (vertexArena) SDVertex;
			v[j]->child->regular = v[j]->regular;
			v[j]->child->boundary = v[j]->boundary;
			newVertices.push_back(v[j]->child);
		}
		for (u_int j = 0; j < f.size(); ++j)
			for (int k = 0; k < 4; ++k) {
				f[j]->children[k] = faceArena.malloc();//new (faceArena) SDFace;
				newFaces.push_back(f[j]->children[k]);
			}
		// Update vertex positions and create new edge vertices
		// Update vertex positions for even vertices
		for (u_int j = 0; j < v.size(); ++j) {
			if (!v[j]->boundary) {
				// Apply one-ring rule for even vertex
				if (v[j]->regular)
					v[j]->child->P = weightOneRing(v[j], 1.f/16.f);
				else
					v[j]->child->P =
					    weightOneRing(v[j], beta(v[j]->valence()));
			}
			else {
				// Apply boundary rule for even vertex
				v[j]->child->P = weightBoundary(v[j], 1.f/8.f);
			}
		}
		// Compute new odd edge vertices
		map<SDEdge, SDVertex *> edgeVerts;
		for (u_int j = 0; j < f.size(); ++j) {
			SDFace *face = f[j];
			for (int k = 0; k < 3; ++k) {
				// Compute odd vertex on _k_th edge
				SDEdge edge(face->v[k], face->v[NEXT(k)]);
				SDVertex *vert = edgeVerts[edge];
				if (!vert) {
					// Create and initialize new odd vertex
					vert = vertexArena.malloc();//new (vertexArena) SDVertex;
					newVertices.push_back(vert);
					vert->regular = true;
					vert->boundary = (face->f[k] == NULL);
					vert->startFace = face->children[3];
					// Apply edge rules to compute new vertex position
					if (vert->boundary) {
						vert->P =  0.5f * edge.v[0]->P;
						vert->P += 0.5f * edge.v[1]->P;
					}
					else {
						vert->P =  3.f/8.f * edge.v[0]->P;
						vert->P += 3.f/8.f * edge.v[1]->P;
						vert->P += 1.f/8.f *
							face->otherVert(edge.v[0], edge.v[1])->P;
						vert->P += 1.f/8.f *
							face->f[k]->otherVert(edge.v[0], edge.v[1])->P;
					}
					edgeVerts[edge] = vert;
				}
			}
		}
		// Update new mesh topology
		// Update even vertex face pointers
		for (u_int j = 0; j < v.size(); ++j) {
			SDVertex *vert = v[j];
			int vertNum = vert->startFace->vnum(vert);
			vert->child->startFace =
			    vert->startFace->children[vertNum];
		}
		// Update face neighbor pointers
		for (u_int j = 0; j < f.size(); ++j) {
			SDFace *face = f[j];
			for (int k = 0; k < 3; ++k) {
				// Update children _f_ pointers for siblings
				face->children[3]->f[k] = face->children[NEXT(k)];
				face->children[k]->f[NEXT(k)] = face->children[3];
				// Update children _f_ pointers for neighbor children
				SDFace *f2 = face->f[k];
				face->children[k]->f[k] =
					f2 ? f2->children[f2->vnum(face->v[k])] : NULL;
				f2 = face->f[PREV(k)];
				face->children[k]->f[PREV(k)] =
					f2 ? f2->children[f2->vnum(face->v[k])] : NULL;
			}
		}
		// Update face vertex pointers
		for (u_int j = 0; j < f.size(); ++j) {
			SDFace *face = f[j];
			for (int k = 0; k < 3; ++k) {
				// Update child vertex pointer to new even vertex
				face->children[k]->v[k] = face->v[k]->child;
				// Update child vertex pointer to new odd vertex
				SDVertex *vert =
					edgeVerts[SDEdge(face->v[k], face->v[NEXT(k)])];
				face->children[k]->v[NEXT(k)] = vert;
				face->children[NEXT(k)]->v[k] = vert;
				face->children[3]->v[k] = vert;
			}
		}
		// Prepare for next level of subdivision
		f = newFaces;
		v = newVertices;
	}
	// Push vertices to limit surface
	Point *Plimit = new Point[v.size()];
	for (u_int i = 0; i < v.size(); ++i) {
		if (v[i]->boundary)
			Plimit[i] =
			    weightBoundary(v[i], 1.f/5.f);
		else
			Plimit[i] =
			    weightOneRing(v[i], gamma(v[i]->valence()));
	}
	for (u_int i = 0; i < v.size(); ++i)
		v[i]->P = Plimit[i];
	// Compute vertex tangents on limit surface
	vector<Normal> Ns;
	Ns.reserve(v.size());
	int ringSize = 16;
	Point *Pring = new Point[ringSize];
	for (u_int i = 0; i < v.size(); ++i) {
		SDVertex *vert = v[i];
		Vector S(0,0,0), T(0,0,0);
		int valence = vert->valence();
		if (valence > ringSize) {
			ringSize = valence;
			delete[] Pring;
			Pring = new Point[ringSize];
		}
		vert->oneRing(Pring);
	
		if (!vert->boundary) {
			// Compute tangents of interior face
			for (int k = 0; k < valence; ++k) {
				S += cosf(2.f*M_PI*k/valence) * Vector(Pring[k]);
				T += sinf(2.f*M_PI*k/valence) * Vector(Pring[k]);
			}
		}
		else {
			// Compute tangents of boundary face
			S = Pring[valence-1] - Pring[0];
			if (valence == 2)
				T = Vector(Pring[0] + Pring[1] - 2 * vert->P);
			else if (valence == 3)
				T = Pring[1] - vert->P;
			else if (valence == 4) // regular
				T = Vector(-1*Pring[0] + 2*Pring[1] + 2*Pring[2] +
					-1*Pring[3] + -2*vert->P);
			else {
				float theta = M_PI / float(valence-1);
				T = Vector(sinf(theta) * (Pring[0] + Pring[valence-1]));
				for (int k = 1; k < valence-1; ++k) {
					float wt = (2*cosf(theta) - 2) * sinf((k) * theta);
					T += Vector(wt * Pring[k]);
				}
				T = -T;
			}
		}
		Ns.push_back(Normal(Cross(S, T)));
	}
	// Create _TriangleMesh_ from subdivision mesh
	u_int ntris = u_int(f.size());
	int *verts = new int[3*ntris];
	int *vp = verts;
	u_int totVerts = u_int(v.size());
	map<SDVertex *, int> usedVerts;
	for (u_int i = 0; i < totVerts; ++i)
		usedVerts[v[i]] = i;
	for (u_int i = 0; i < ntris; ++i) {
		for (int j = 0; j < 3; ++j) {
			*vp = usedVerts[f[i]->v[j]];
			++vp;
		}
	}
	ParamSet paramSet;
	paramSet.AddInt("indices", verts, 3*ntris);
	paramSet.AddPoint("P", Plimit, totVerts);
	paramSet.AddNormal("N", &Ns[0], int(Ns.size()));
	refined.push_back(MakeShape("trianglemesh", ObjectToWorld,
			reverseOrientation, paramSet));
	delete[] verts;
	delete[] Plimit;
}
Point LoopSubdiv::weightOneRing(SDVertex *vert, float beta) {
	// Put _vert_ one-ring in _Pring_
	int valence = vert->valence();
	Point *Pring = (Point *)alloca(valence * sizeof(Point));
	vert->oneRing(Pring);
	Point P = (1 - valence * beta) * vert->P;
	for (int i = 0; i < valence; ++i)
		P += beta * Pring[i];
	return P;
}
void SDVertex::oneRing(Point *P) {
	if (!boundary) {
		// Get one ring vertices for interior vertex
		SDFace *face = startFace;
		do {
			*P++ = face->nextVert(this)->P;
			face = face->nextFace(this);
		} while (face != startFace);
	}
	else {
		// Get one ring vertices for boundary vertex
		SDFace *face = startFace, *f2;
		while ((f2 = face->nextFace(this)) != NULL)
			face = f2;
		*P++ = face->nextVert(this)->P;
		do {
			*P++ = face->prevVert(this)->P;
			face = face->prevFace(this);
		} while (face != NULL);
	}
}
Point LoopSubdiv::weightBoundary(SDVertex *vert,
                                 float beta) {
	// Put _vert_ one-ring in _Pring_
	int valence = vert->valence();
	Point *Pring = (Point *)alloca(valence * sizeof(Point));
	vert->oneRing(Pring);
	Point P = (1-2*beta) * vert->P;
	P += beta * Pring[0];
	P += beta * Pring[valence-1];
	return P;
}
Shape* LoopSubdiv::CreateShape(const Transform &o2w,
		bool reverseOrientation, const ParamSet &params) {
	int nlevels = params.FindOneInt("nlevels", 3);
	int nps, nIndices;
	const int *vi = params.FindInt("indices", &nIndices);
	const Point *P = params.FindPoint("P", &nps);
	if (!vi || !P) return NULL;

	// don't actually use this for now...
	string scheme = params.FindOneString("scheme", "loop");

	return new LoopSubdiv(o2w, reverseOrientation, nIndices/3, nps,
		vi, P, nlevels);
}
