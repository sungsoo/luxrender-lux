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

#ifndef LUX_SPECTRUM_H
#define LUX_SPECTRUM_H
// spectrum.h*
#include "lux.h"
#include "color.h"

#include <boost/thread/tss.hpp>

namespace lux
{

#define WAVELENGTH_SAMPLES 8
static const float inv_WAVELENGTH_SAMPLES = 1. / WAVELENGTH_SAMPLES;


// Spectrum Declarations

#ifndef DISABLED_LUX_USE_SSE

class  Spectrum {
	friend class boost::serialization::access;
public:
	// Spectrum Public Methods
	Spectrum(float v = 0.f) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] = v;
	}
	
	Spectrum(float cs[COLOR_SAMPLES]) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] = cs[i];
	}
	friend ostream &operator<<(ostream &, const Spectrum &);
	Spectrum &operator+=(const Spectrum &s2) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] += s2.c[i];
		return *this;
	}
	Spectrum operator+(const Spectrum &s2) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] += s2.c[i];
		return ret;
	}
	Spectrum operator-(const Spectrum &s2) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] -= s2.c[i];
		return ret;
	}
	Spectrum operator/(const Spectrum &s2) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] /= s2.c[i];
		return ret;
	}
	Spectrum operator*(const Spectrum &sp) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] *= sp.c[i];
		return ret;
	}
	Spectrum &operator*=(const Spectrum &sp) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] *= sp.c[i];
		return *this;
	}
	Spectrum operator*(float a) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] *= a;
		return ret;
	}
	Spectrum &operator*=(float a) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] *= a;
		return *this;
	}
	friend inline
	Spectrum operator*(float a, const Spectrum &s) {
		return s * a;
	}
	Spectrum operator/(float a) const {
		return *this * (1.f / a);
	}
	Spectrum &operator/=(float a) {
		float inv = 1.f / a;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] *= inv;
		return *this;
	}
	void AddWeighted(float w, const Spectrum &s) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] += w * s.c[i];
	}
	bool operator==(const Spectrum &sp) const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			if (c[i] != sp.c[i]) return false;
		return true;
	}
	bool operator!=(const Spectrum &sp) const {
		return !(*this == sp);
	}
	bool Black() const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			if (c[i] != 0.) return false;
		return true;
	}
	Spectrum Sqrt() const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = sqrtf(c[i]);
		return ret;
	}
	Spectrum Pow(const Spectrum &e) const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = c[i] > 0 ? powf(c[i], e.c[i]) : 0.f;
		return ret;
	}
	Spectrum operator-() const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = -c[i];
		return ret;
	}
	friend Spectrum Exp(const Spectrum &s) {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = expf(s.c[i]);
		return ret;
	}
	Spectrum Clamp(float low = 0.f,
	               float high = INFINITY) const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = ::Clamp(c[i], low, high);
		return ret;
	}
	bool IsNaN() const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			if (isnan(c[i])) return true;
		return false;
	}
	void Print(FILE *f) const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			fprintf(f, "%f ", c[i]);
	}
	XYZColor ToXYZ() const {
		float xyz[3];
		xyz[0] = xyz[1] = xyz[2] = 0.;
		for (int i = 0; i < COLOR_SAMPLES; ++i) {
			xyz[0] += XWeight[i] * c[i];
			xyz[1] += YWeight[i] * c[i];
			xyz[2] += ZWeight[i] * c[i];
		}
		return XYZColor(xyz);
	}
	float y() const {
		float v = 0.;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			v += YWeight[i] * c[i];
		return v;
	}
	bool operator<(const Spectrum &s2) const {
		return y() < s2.y();
	}
	friend class lux::ParamSet;
	
	// Spectrum Public Data
	float c[COLOR_SAMPLES];
	static const int CIEstart = 360;
	static const int CIEend = 830;
	static const int nCIE = CIEend-CIEstart+1;
	static const float CIE_X[nCIE];
	static const float CIE_Y[nCIE];
	static const float CIE_Z[nCIE];
	
protected:
	// Spectrum Private Data
	static float XWeight[COLOR_SAMPLES];
	static float YWeight[COLOR_SAMPLES];
	static float ZWeight[COLOR_SAMPLES];
	friend Spectrum FromXYZ(float x, float y, float z);
	
private:
	template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				for (int i = 0; i < COLOR_SAMPLES; ++i)
					ar & c[i];
			}
};

#else //LUX_USE_SSE

#define COLOR_VECTORS 1

class  _MM_ALIGN16 Spectrum {
public:
	// Spectrum Public Methods
	Spectrum(float v = 0.f) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] = v;
	}
	Spectrum(float cs[COLOR_SAMPLES]) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] = cs[i];
	}
	Spectrum(__m128 cv[COLOR_VECTORS])
    {
    	for (int i = 0; i < COLOR_VECTORS; ++i)
    		cvec[i]=cv[i];
    }
	
	friend ostream &operator<<(ostream &, const Spectrum &);
	Spectrum &operator+=(const Spectrum &s2) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] += s2.c[i];
		return *this;
	}
	Spectrum operator+(const Spectrum &s2) const {
		Spectrum ret;// = *this;
		for (int i = 0; i < COLOR_VECTORS; ++i)
			ret.cvec[i]=_mm_add_ps(cvec[i],s2.cvec[i]);
		
		//for (int i = 0; i < COLOR_SAMPLES; ++i)
		//	ret.c[i] += s2.c[i];
		return ret;
	}
	Spectrum operator-(const Spectrum &s2) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] -= s2.c[i];
		return ret;
	}
	Spectrum operator/(const Spectrum &s2) const {
		Spectrum ret;// = *this;
		for (int i = 0; i < COLOR_VECTORS; ++i)
			ret.cvec[i]=_mm_div_ps(cvec[i],s2.cvec[i]);
		//for (int i = 0; i < COLOR_SAMPLES; ++i)
		//	ret.c[i] /= s2.c[i];
		return ret;
	}
	Spectrum operator*(const Spectrum &sp) const {
		Spectrum ret;// = *this;
		for (int i = 0; i < COLOR_VECTORS; ++i)
			ret.cvec[i]=_mm_mul_ps(cvec[i],sp.cvec[i]);
		
		/*for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] *= sp.c[i];*/
		return ret;
	}
	Spectrum &operator*=(const Spectrum &sp) {
		//for (int i = 0; i < COLOR_SAMPLES; ++i)
		//	c[i] *= sp.c[i];
		
		for (int i = 0; i < COLOR_VECTORS; ++i)
			cvec[i]=_mm_mul_ps(cvec[i],sp.cvec[i]);
		
		return *this;
	}
	Spectrum operator*(float a) const {
		Spectrum ret = *this;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] *= a;
		return ret;
	}
	Spectrum &operator*=(float a) {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] *= a;
		return *this;
	}
	friend inline
	Spectrum operator*(float a, const Spectrum &s) {
		return s * a;
	}
	Spectrum operator/(float a) const {
		return *this * (1.f / a);
	}
	Spectrum &operator/=(float a) {
		float inv = 1.f / a;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			c[i] *= inv;
		return *this;
	}
	void AddWeighted(float w, const Spectrum &s) {
		//for (int i = 0; i < COLOR_SAMPLES; ++i)
		//	c[i] += w * s.c[i];
		__m128 wvec=_mm_set_ps1(w);
		
		for (int i = 0; i < COLOR_VECTORS; ++i)
			cvec[i]=_mm_add_ps(cvec[i],_mm_mul_ps(s.cvec[i],wvec));
	}
	bool operator==(const Spectrum &sp) const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			if (c[i] != sp.c[i]) return false;
		
		//for (int i = 0; i < COLOR_VECTORS; ++i)	
		//	if(_mm_cmpneq_ps(cvec[i],sp.cvec[i])) return false;
		return true;
	}
	bool operator!=(const Spectrum &sp) const {
		return !(*this == sp);
	}
	bool Black() const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			if (c[i] != 0.) return false;
		return true;
	}
	Spectrum Sqrt() const {
		Spectrum ret;
		//for (int i = 0; i < COLOR_SAMPLES; ++i)
		//	ret.c[i] = sqrtf(c[i]);
			
		for (int i = 0; i < COLOR_VECTORS; ++i)	
			ret.cvec[i]=_mm_sqrt_ps(cvec[i]);
		return ret;
	}
	Spectrum Pow(const Spectrum &e) const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = c[i] > 0 ? powf(c[i], e.c[i]) : 0.f;
		return ret;
	}
	Spectrum operator-() const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = -c[i];
		return ret;
	}
	friend Spectrum Exp(const Spectrum &s) {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = expf(s.c[i]);
		return ret;
	}
	Spectrum Clamp(float low = 0.f,
	               float high = INFINITY) const {
		Spectrum ret;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			ret.c[i] = ::Clamp(c[i], low, high);
		return ret;
	}
	bool IsNaN() const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			if (isnan(c[i])) return true;
		return false;
	}
	void Print(FILE *f) const {
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			fprintf(f, "%f ", c[i]);
	}
	void XYZ(float xyz[3]) const {
		xyz[0] = xyz[1] = xyz[2] = 0.;
		for (int i = 0; i < COLOR_SAMPLES; ++i) {
			xyz[0] += XWeight[i] * c[i];
			xyz[1] += YWeight[i] * c[i];
			xyz[2] += ZWeight[i] * c[i];
		}
	}
	float y() const {
		float v = 0.;
		for (int i = 0; i < COLOR_SAMPLES; ++i)
			v += YWeight[i] * c[i];
		return v;
	}
	bool operator<(const Spectrum &s2) const {
		return y() < s2.y();
	}
	friend class ParamSet;
	
	// Spectrum Public Data
	//float c[COLOR_SAMPLES];
	
	union
    {
     	__m128  cvec[COLOR_VECTORS];
      float c[COLOR_SAMPLES];
    };
	
	
	static const int CIEstart = 360;
	static const int CIEend = 830;
	static const int nCIE = CIEend-CIEstart+1;
	static const float CIE_X[nCIE];
	static const float CIE_Y[nCIE];
	static const float CIE_Z[nCIE];

/*	
	void* operator new(size_t t) { return _mm_malloc(t,16); }
    void operator delete(void* ptr, size_t t) { _mm_free(ptr); }
    void* operator new[](size_t t) { return _mm_malloc(t,16); }
    void operator delete[] (void* ptr) { _mm_free(ptr); }
    void* operator new(long unsigned int i, Spectrum*) { return new Spectrum[i]; }
    */
	
protected:
	// Spectrum Private Data
	static float XWeight[COLOR_SAMPLES];
	static float YWeight[COLOR_SAMPLES];
	static float ZWeight[COLOR_SAMPLES];
	friend Spectrum FromXYZ(float x, float y, float z);
};

#endif

Spectrum FromXYZ(float x, float y, float z);


class  SWCSpectrum {
	friend class boost::serialization::access;
public:
	// Spectrum Public Methods
	SWCSpectrum(double v = 0.f) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] = v;
	}
	SWCSpectrum(Spectrum s) {
		FromSpectrum(s);
	}	
	SWCSpectrum(const SPD *s) {
		FromSPD(s);
	}	
	SWCSpectrum(double cs[WAVELENGTH_SAMPLES]) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] = cs[i];
	}
	friend ostream &operator<<(ostream &, const SWCSpectrum &);
	SWCSpectrum &operator+=(const SWCSpectrum &s2) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] += s2.c[i];
		return *this;
	}
	SWCSpectrum operator+(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] += s2.c[i];
		return ret;
	}
	SWCSpectrum operator-(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] -= s2.c[i];
		return ret;
	}
	SWCSpectrum operator/(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] /= s2.c[i];
		return ret;
	}
	SWCSpectrum operator*(const SWCSpectrum &sp) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] *= sp.c[i];
		return ret;
	}
	SWCSpectrum &operator*=(const SWCSpectrum &sp) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= sp.c[i];
		return *this;
	}
	SWCSpectrum operator*(double a) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] *= a;
		return ret;
	}
	SWCSpectrum &operator*=(double a) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= a;
		return *this;
	}
	friend inline
	SWCSpectrum operator*(double a, const SWCSpectrum &s) {
		return s * a;
	}
	SWCSpectrum operator/(double a) const {
		return *this * (1.f / a);
	}
	SWCSpectrum &operator/=(double a) {
		double inv = 1.f / a;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= inv;
		return *this;
	}
	void AddWeighted(double w, const SWCSpectrum &s) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] += w * s.c[i];
	}
	bool operator==(const SWCSpectrum &sp) const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (c[i] != sp.c[i]) return false;
		return true;
	}
	bool operator!=(const SWCSpectrum &sp) const {
		return !(*this == sp);
	}
	bool Black() const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (c[i] != 0.) return false;
		return true;
	}
	SWCSpectrum Sqrt() const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = sqrtf(c[i]);
		return ret;
	}
	SWCSpectrum Pow(const SWCSpectrum &e) const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = c[i] > 0 ? powf(c[i], e.c[i]) : 0.f;
		return ret;
	}
	SWCSpectrum operator-() const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = -c[i];
		return ret;
	}
	friend SWCSpectrum Exp(const SWCSpectrum &s) {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = expf(s.c[i]);
		return ret;
	}
	SWCSpectrum Clamp(double low = 0.f,
	               double high = INFINITY) const {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = ::Clamp(c[i], low, high);
		return ret;
	}
	bool IsNaN() const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (isnan(c[i])) return true;
		return false;
	}
	void Print(FILE *f) const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			fprintf(f, "%f ", c[i]);
	}
	XYZColor ToXYZ() const;
	void FromSpectrum(Spectrum s);
	void FromSPD(const SPD *s);

	double y() const;
	bool operator<(const SWCSpectrum &s2) const {
		return y() < s2.y();
	}
	friend class lux::ParamSet;
	
	// SWCSpectrum Public Data
	double c[WAVELENGTH_SAMPLES];
	static const int CIEstart = 360;
	static const int CIEend = 830;
	static const int nCIE = CIEend-CIEstart+1;
	static const float CIE_X[nCIE];
	static const float CIE_Y[nCIE];
	static const float CIE_Z[nCIE];
	
private:
	template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
					ar & c[i];
			}
};

// 380 to 720 nm, 34nm spacing, 10 values
 static float rgb2spect_white[] =	{ 1.0000, 1.0000, 0.9999, 0.9993, 0.9992, 0.9998, 1.0000, 1.0000, 1.0000, 1.0000 };
 static float rgb2spect_cyan[] =		{ 0.9710, 0.9426, 1.0007, 1.0007, 1.0007, 1.0007, 0.1564, 0.0000, 0.0000, 0.0000 };
 static float rgb2spect_magenta[] =	{ 1.0000, 1.0000, 0.9685, 0.2229, 0.0000, 0.0458, 0.8369, 1.0000, 1.0000, 0.0000 };
 static float rgb2spect_yellow[] =	{ 0.0001, 0.0000, 0.1088, 0.6651, 1.0000, 1.0000, 0.9996, 0.9586, 0.9685, 0.9959 };
 static float rgb2spect_red[] =		{ 0.1012, 0.0515, 0.0000, 0.0000, 0.0000, 0.0000, 0.8325, 1.0149, 1.0149, 0.9840 };
 static float rgb2spect_green[] =	{ 0.0000, 0.0000, 0.0273, 0.7937, 1.0000, 0.9418, 0.1719, 0.0000, 0.0000, 1.0149 };
 static float rgb2spect_blue[] =		{ 1.0000, 1.0000, 0.8916, 0.3323, 0.0000, 0.0000, 0.0003, 0.0369, 0.0483, 0.0496 };

class	SpectrumWavelengths {
public:

	// SpectrumWavelengths Public Methods
	SpectrumWavelengths() { single = false; single_w = 0; }

	void Sample(float u1) {
		single = false; single_w = 0;

		const float offset = float(CIEend - CIEstart) * inv_WAVELENGTH_SAMPLES;
		float waveln = CIEstart + u1 * offset;
		for (u_int i = 0; i < WAVELENGTH_SAMPLES; ++i) {
			// create stratified random wavelengths
			// from left (CIEstart) to right (CIEend)
			w[i] = waveln;
			waveln += offset; 
		} 

		ComputeRGBConversionSpectra();
	}

	void CreateSpectrum(SWCSpectrum &spectrum, SPD &rgbspectrum);
	void ComputeRGBConversionSpectra();

	float SampleSingle() {
		if(!single) {
			int i = Floor2Int(lux::random::floatValue() * WAVELENGTH_SAMPLES);
			single = true;
			single_w = i;
		}
		return w[single_w];
	}

	// Wavelength in nm
	float w[WAVELENGTH_SAMPLES];	

	bool single;
	int  single_w;

	SWCSpectrum spect_w;	// white
	SWCSpectrum spect_c;	// cyan
	SWCSpectrum spect_m;	// magenta
	SWCSpectrum spect_y;	// yellow
	SWCSpectrum spect_r;	// red
	SWCSpectrum spect_g;	// green
	SWCSpectrum spect_b;	// blue

	static const int CIEstart = 360;
	static const int CIEend = 830;
	static const int nCIE = CIEend-CIEstart+1;
};


}//namespace lux

#endif // LUX_SPECTRUM_H
