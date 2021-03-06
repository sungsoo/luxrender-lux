/***************************************************************************
 *   Copyright (C) 1998-2013 by authors (see AUTHORS.txt)                  *
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

// multimix.cpp*
#include "lux.h"
#include "spectrum.h"
#include "texture.h"
#include "color.h"
#include "fresnelgeneral.h"
#include "paramset.h"

#include <sstream>
using std::stringstream;

namespace lux
{

// MultiMixTexture Declarations
template <class T>
class MultiMixTexture : public Texture<T> {
public:
	// MultiMixTexture Public Methods
	MultiMixTexture(u_int n, const float *w,
			vector<boost::shared_ptr<Texture<T> > > &t) :
			Texture<T>("MultiMixTexture-" + boost::lexical_cast<string>(this)),
			weights(w, w + n),
			tex(t) { }
	virtual ~MultiMixTexture() { }

	virtual T Evaluate(const SpectrumWavelengths &sw,
		const DifferentialGeometry &dg) const
	{
		T ret = 0.f;
		for (u_int i = 0; i < tex.size(); ++i)
		{
			ret += weights[i] * tex[i]->Evaluate(sw, dg);
		}
		return ret;
	}

	virtual float Y() const
	{
		float ret = 0;
		for (u_int i = 0; i < tex.size(); ++i)
		{
			ret += weights[i] * tex[i]->Y();
		}
		return ret;
	}

	virtual float Filter() const
	{
		float ret = 0;
		for (u_int i = 0; i < tex.size(); ++i)
		{
			ret += weights[i] * tex[i]->Filter();
		}
		return ret;
	}

	virtual void GetDuv(const SpectrumWavelengths &sw,
		const DifferentialGeometry &dg, float delta,
		float *du, float *dv) const
	{
		float dua = 0.f, dva = 0.f;
		float duat, dvat;
		for (u_int i = 0; i < tex.size(); ++i)
		{
			tex[i]->GetDuv(sw, dg, delta, &duat, &dvat);
			dua += weights[i] * duat;
			dva += weights[i] * dvat;
		}

		*du = dua;
		*dv = dva;
	}
	virtual void GetMinMaxFloat(float *minValue, float *maxValue) const {
		*minValue = 0.f;
		*maxValue = 0.f;
		for (u_int i = 0; i < tex.size() - 1; ++i) {
			float minv, maxv;
			tex[i]->GetMinMaxFloat(&minv, &maxv);
			const float wminv = weights[i] * minv;
			const float wmaxv = weights[i] * maxv;
			*minValue += min(wminv, wmaxv);
			*maxValue += max(wminv, wmaxv);
		}
	}
	virtual void SetIlluminant()
	{
		// Update sub-textures
		for (u_int i = 0; i < tex.size(); ++i)
			tex[i]->SetIlluminant();
	}
	static Texture<float> * CreateFloatTexture(const Transform &tex2world, const ParamSet &tp);
	static Texture<SWCSpectrum> * CreateSWCSpectrumTexture(const Transform &tex2world, const ParamSet &tp);
	static Texture<FresnelGeneral> * CreateFresnelTexture(const Transform &tex2world, const ParamSet &tp);
private:
	vector<float> weights;
	vector<boost::shared_ptr<Texture<T> > > tex;
};

// MultiMixTexture Method Definitions
template <class T> Texture<float> * MultiMixTexture<T>::CreateFloatTexture(const Transform &tex2world,
	const ParamSet &tp) {

	u_int n;
	const float *w = tp.FindFloat("weights", &n);

	vector<boost::shared_ptr<Texture<float> > > tex;

	tex.reserve(n);
	for (u_int i = 0; i < n; ++i) {
		stringstream ss;
		ss << "tex" << (i + 1);
		tex.push_back(tp.GetFloatTexture(ss.str(), 0.f));
	}

	return new MultiMixTexture<float>(n, w, tex);
}

template <class T> Texture<SWCSpectrum> * MultiMixTexture<T>::CreateSWCSpectrumTexture(const Transform &tex2world,
	const ParamSet &tp) {

	u_int n;
	const float *w = tp.FindFloat("weights", &n);

	vector<boost::shared_ptr<Texture<SWCSpectrum> > > tex;

	tex.reserve(n);
	for (u_int i = 0; i < n; ++i) {
		stringstream ss;
		ss << "tex" << (i + 1);
		tex.push_back(tp.GetSWCSpectrumTexture(ss.str(), 0.f));
	}

	return new MultiMixTexture<SWCSpectrum>(n, w, tex);
}

template <class T> Texture<FresnelGeneral> * MultiMixTexture<T>::CreateFresnelTexture(const Transform &tex2world,
	const ParamSet &tp) {

	u_int n;
	const float *w = tp.FindFloat("weights", &n);

	vector<boost::shared_ptr<Texture<FresnelGeneral> > > tex;

	tex.reserve(n);
	for (u_int i = 0; i < n; ++i) {
		stringstream ss;
		ss << "tex" << (i + 1);
		tex.push_back(tp.GetFresnelTexture(ss.str(), 1.f));
	}

	return new MultiMixTexture<FresnelGeneral>(n, w, tex);
}

}//namespace lux

