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

#ifndef TONEMAPWIDGET_H
#define TONEMAPWIDGET_H

#include <QtGui/QWidget>

#define TM_REINHARD_YWA_RANGE 1.0f
#define TM_REINHARD_PRESCALE_RANGE 8.0f
#define TM_REINHARD_POSTSCALE_RANGE 8.0f
#define TM_REINHARD_BURN_RANGE 12.0f

#define TM_LINEAR_EXPOSURE_LOG_MIN -3.f
#define TM_LINEAR_EXPOSURE_LOG_MAX 2.f
#define TM_LINEAR_SENSITIVITY_RANGE 6400.0f
#define TM_LINEAR_FSTOP_RANGE 128.0f
#define TM_LINEAR_GAMMA_RANGE 5.0f

#define TM_CONTRAST_YWA_LOG_MIN -4.f
#define TM_CONTRAST_YWA_LOG_MAX 4.f

#define NUM_SENSITITIVITY_PRESETS 26
#define NUM_EXPOSURE_PRESETS 11
#define NUM_FSTOP_PRESETS 17

namespace Ui
{
	class ToneMapWidget;
}

class ToneMapWidget : public QWidget
{
	Q_OBJECT

public:

	ToneMapWidget(QWidget *parent = 0);
	~ToneMapWidget();

	//void SetWidgetsEnabled(bool enabled);
    
	void updateWidgetValues();
	void resetValues();
	void resetFromFilm (bool useDefaults);

private:

	Ui::ToneMapWidget *ui;

	int m_TM_kernel;
	double m_TM_reinhard_prescale;
	double m_TM_reinhard_postscale;
	double m_TM_reinhard_burn;

	double m_TM_linear_exposure;
	double m_TM_linear_sensitivity;
	double m_TM_linear_fstop;
	double m_TM_linear_gamma;

	double m_TM_contrast_ywa;

	int sensitivityToPreset(double value);
	int exposureToPreset(double value);
	int fstopToPreset(double value);

signals:
	void valuesChanged();

private slots:

	void setTonemapKernel (int choice);
	void setSensitivityPreset (int choice);
	void setExposurePreset (int choice);
	void setFStopPreset (int choice);
	void prescaleChanged (int value);
	void prescaleChanged (double value);
	void postscaleChanged (int value);
	void postscaleChanged (double value);
	void burnChanged (int value);
	void burnChanged (double value);

	void sensitivityChanged (int value);
	void sensitivityChanged (double value);
	void exposureChanged (int value);
	void exposureChanged (double value);
	void fstopChanged (int value);
	void fstopChanged (double value);
	void gammaLinearChanged (int value);
	void gammaLinearChanged (double value);

	void ywaChanged (int value);
	void ywaChanged (double value);

};

#endif // TONEMAPWIDGET_H

