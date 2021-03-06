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

#ifndef LUX_SWCSPECTRUM_H
#define LUX_SWCSPECTRUM_H
// spectrum.h*
#include "lux.h"

#include <boost/serialization/access.hpp>

namespace lux
{

#define WAVELENGTH_SAMPLES 4
#define WAVELENGTH_START 380.f
#define WAVELENGTH_END   720.f
static const float inv_WAVELENGTH_SAMPLES = 1.f / WAVELENGTH_SAMPLES;

#define Scalar float

class SWCSpectrum {
	friend class boost::serialization::access;
public:
	// SWCSpectrum Public Methods
	SWCSpectrum(Scalar v = 0.f) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] = v;
	}
	SWCSpectrum(const SpectrumWavelengths &sw, const RGBColor &s);

	SWCSpectrum(const SpectrumWavelengths &sw, const SPD &s);

	SWCSpectrum(const float cs[WAVELENGTH_SAMPLES]) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] = cs[i];
	}
	friend ostream &operator<<(ostream &, const SWCSpectrum &);
	SWCSpectrum operator+(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] += s2.c[i];
		return ret;
	}
	SWCSpectrum &operator+=(const SWCSpectrum &s2) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] += s2.c[i];
		return *this;
	}
  // Needed for addition of textures
	SWCSpectrum operator+(Scalar a) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] += a;
		return ret;
	}
  // Needed for addition of textures
  friend inline
	SWCSpectrum operator+(Scalar a, const SWCSpectrum &s) {
		return s + a;
	}
	SWCSpectrum operator-(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] -= s2.c[i];
		return ret;
	}
	SWCSpectrum &operator-=(const SWCSpectrum &s2) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] -= s2.c[i];
		return *this;
	}
  // Needed for subtraction of textures
  friend inline
	SWCSpectrum operator-(Scalar a, const SWCSpectrum &s) {
		return s - a;
	}
  // Needed for subtraction of textures
	SWCSpectrum operator-(Scalar a) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] -= a;
		return ret;
	}
	SWCSpectrum operator/(const SWCSpectrum &s2) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] /= s2.c[i];
		return ret;
	}
	SWCSpectrum &operator/=(const SWCSpectrum &sp) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] /= sp.c[i];
		return *this;
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
	SWCSpectrum operator*(Scalar a) const {
		SWCSpectrum ret = *this;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] *= a;
		return ret;
	}
	SWCSpectrum &operator*=(Scalar a) {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			c[i] *= a;
		return *this;
	}
	friend inline
	SWCSpectrum operator*(Scalar a, const SWCSpectrum &s) {
		return s * a;
	}
	SWCSpectrum operator/(Scalar a) const {
		return *this * (1.f / a);
	}
	SWCSpectrum &operator/=(Scalar a) {
		return *this *= (1.f / a);
	}
	void AddWeighted(Scalar w, const SWCSpectrum &s) {
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
			if (c[i] != 0.f) return false;
		return true;
	}
    Scalar Max() const {
        Scalar result = c[0];
        for (int i = 1; i < WAVELENGTH_SAMPLES; i++)
            result = std::max(result, c[i]);
        return result;
    }
    Scalar Min() const {
        Scalar result = c[0];
        for (int i = 1; i < WAVELENGTH_SAMPLES; i++)
            result = std::min(result, c[i]);
        return result;
    }
	friend SWCSpectrum Sqrt(const SWCSpectrum &s) {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = sqrtf(s.c[i]);
		return ret;
	}
	friend SWCSpectrum Pow(const SWCSpectrum &s, const SWCSpectrum &e) {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = powf(s.c[i], e.c[i]);
		return ret;
	}
	friend SWCSpectrum Pow(const SWCSpectrum &s, float e) {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = powf(s.c[i], e);
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
	friend SWCSpectrum Ln(const SWCSpectrum &s) {
		SWCSpectrum ret;
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			ret.c[i] = logf(s.c[i]);
		return ret;
	}
	SWCSpectrum Clamp(Scalar low = 0.f,
	               Scalar high = INFINITY) const {
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
	bool IsInf() const {
		for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
			if (isinf(c[i])) return true;
		return false;
	}
	Scalar Y(const SpectrumWavelengths &sw) const;
	inline Scalar Filter(const SpectrumWavelengths &sw) const;

//	bool operator<(const SWCSpectrum &s2) const {
//		return y() < s2.y();
//		return false;
//	}
	friend class lux::ParamSet;
	
	// SWCSpectrum Public Data
	Scalar c[WAVELENGTH_SAMPLES];
	
private:
	template<class Archive>
			void serialize(Archive & ar, const unsigned int version)
			{
				for (int i = 0; i < WAVELENGTH_SAMPLES; ++i)
					ar & c[i];
			}
};

}//namespace lux

// SpectrumWavelengths needs the SWCSpectrum class fully defined
#include "spectrumwavelengths.h"

// This is one of the most used functions so make it an inline candidate
// However it requires SpectrumWavelengths to be fully defined
inline Scalar lux::SWCSpectrum::Filter(const SpectrumWavelengths &sw) const {
	if (sw.single)
		return c[sw.single_w];
	Scalar result = 0.f;
	for (u_int i = 0; i < WAVELENGTH_SAMPLES; ++i)
		result += c[i];
	return result * inv_WAVELENGTH_SAMPLES;
}

#endif // LUX_SWCSPECTRUM_H
