///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxluxframe__
#define __wxluxframe__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/statbmp.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/aui/auibook.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/clrpicker.h>

///////////////////////////////////////////////////////////////////////////

namespace lux
{
	#define ID_RESUMEITEM 1000
	#define ID_PAUSEITEM 1001
	#define ID_STOPITEM 1002
	#define ID_TOOL_BAR 1003
	#define ID_STATUS_BAR 1004
	#define ID_SIDE_PANE 1005
	#define ID_VIEWER_RULERS 1006
	#define ID_PAN_MODE 1007
	#define ID_ZOOM_MODE 1008
	#define ID_RENDER_COPY 1009
	#define ID_CLEAR_LOG 1010
	#define ID_FULL_SCREEN 1011
	#define ID_RESUMETOOL 1012
	#define ID_PAUSETOOL 1013
	#define ID_STOPTOOL 1014
	#define ID_NUM_THREADS 1015
	#define ID_ADD_THREAD 1016
	#define ID_REMOVE_THREAD 1017
	#define ID_PANTOOL 1018
	#define ID_ZOOMTOOL 1019
	#define ID_REFINETOOL 1020
	#define ID_LIGHTGROUPS 1021
	#define ID_TONEMAP 1022
	#define ID_TONEMAPOPTIONSPANEL 1023
	#define ID_TAB_TONEMAP_TOGGLE 1024
	#define ID_TAB_TONEMAP 1025
	#define ID_TM_KERNELCHOICE 1026
	#define ID_TONEMAPREINHARDOPTIONSPANEL 1027
	#define ID_TM_REINHARD_PRESCALE 1028
	#define ID_TM_REINHARD_PRESCALE_TEXT 1029
	#define ID_TM_REINHARD_POSTSCALE 1030
	#define ID_TM_REINHARD_POSTSCALE_TEXT 1031
	#define ID_TM_REINHARD_BURN 1032
	#define ID_TM_REINHARD_BURN_TEXT 1033
	#define ID_TONEMAPLINEAROPTIONSPANEL 1034
	#define ID_TM_LINEAR_SENSITIVITY 1035
	#define ID_TM_LINEAR_SENSITIVITY_TEXT 1036
	#define ID_TM_LINEAR_EXPOSURE 1037
	#define ID_TM_LINEAR_EXPOSURE_TEXT 1038
	#define ID_TM_LINEAR_FSTOP 1039
	#define ID_TM_LINEAR_FSTOP_TEXT 1040
	#define ID_TM_LINEAR_GAMMA 1041
	#define ID_TM_LINEAR_GAMMA_TEXT 1042
	#define ID_TONEMAPCONTRASTOPTIONSPANEL 1043
	#define ID_TM_CONTRAST_YWA 1044
	#define ID_TM_CONTRAST_YWA_TEXT 1045
	#define ID_BLOOMOPTIONSPANEL 1046
	#define ID_TAB_LENSEFFECTS_TOGGLE 1047
	#define ID_TAB_LENSEFFECTS 1048
	#define ID_TORGB_BLOOMRADIUS 1049
	#define ID_TORGB_BLOOMRADIUS_TEXT 1050
	#define ID_COMPUTEBLOOMLAYER 1051
	#define ID_TORGB_BLOOMWEIGHT 1052
	#define ID_TORGB_BLOOMWEIGHT_TEXT 1053
	#define ID_VIGNETTING_ENABLED 1054
	#define ID_VIGNETTINGAMOUNT 1055
	#define ID_VIGNETTINGAMOUNT_TEXT 1056
	#define ID_ABERRATION_ENABLED 1057
	#define ID_ABERRATIONAMOUNT 1058
	#define ID_ABERRATIONAMOUNT_TEXT 1059
	#define ID_GLARE_ENABLED 1060
	#define ID_GLAREAMOUNT 1061
	#define ID_GLAREAMOUNT_TEXT 1062
	#define ID_GLAREBLADES 1063
	#define ID_GLARERADIUS 1064
	#define ID_GLARERADIUS_TEXT 1065
	#define ID_COLORSPACEOPTIONSPANEL 1066
	#define ID_TAB_COLORSPACE_TOGGLE 1067
	#define ID_TAB_COLORSPACE 1068
	#define ID_TORGB_COLORSPACECHOICE 1069
	#define ID_TORGB_XWHITE 1070
	#define ID_TORGB_XWHITE_TEXT 1071
	#define ID_TORGB_YWHITE 1072
	#define ID_TORGB_YWHITE_TEXT 1073
	#define ID_TORGB_XRED 1074
	#define ID_TORGB_XRED_TEXT 1075
	#define ID_TORGB_YRED 1076
	#define ID_TORGB_YRED_TEXT 1077
	#define ID_TORGB_XGREEN 1078
	#define ID_TORGB_XGREEN_TEXT 1079
	#define ID_TORGB_YGREEN 1080
	#define ID_TORGB_YGREEN_TEXT 1081
	#define ID_TORGB_XBLUE 1082
	#define ID_TORGB_XBLUE_TEXT 1083
	#define ID_TORGB_YBLUE 1084
	#define ID_TORGB_YBLUE_TEXT 1085
	#define ID_GAMMAOPTIONSPANEL 1086
	#define ID_TAB_GAMMA_TOGGLE 1087
	#define ID_TAB_GAMMA 1088
	#define ID_TORGB_GAMMA 1089
	#define ID_TORGB_GAMMA_TEXT 1090
	#define ID_HISTOGRAMPANEL 1091
	#define ID_TAB_HISTOGRAM_TOGGLE 1092
	#define ID_TAB_HISTOGRAM 1093
	#define ID_HISTOGRAM_CHANNEL 1094
	#define ID_HISTOGRAM_LOG 1095
	#define ID_NOISEOPTIONSPANEL 1096
	#define ID_TAB_NOISEREDUCTION_TOGGLE 1097
	#define ID_TAB_NOISEREDUCTION 1098
	#define ID_GREYC_ENABLED 1099
	#define ID_GREYC_FASTAPPROX 1100
	#define ID_GREYC_ITERATIONS 1101
	#define ID_GREYC_ITERATIONS_TEXT 1102
	#define ID_GREYC_AMPLITUDE 1103
	#define ID_GREYC_AMPLITUDE_TEXT 1104
	#define ID_GREYC_GAUSSPREC 1105
	#define ID_GREYC_GAUSSPREC_TEXT 1106
	#define ID_GREYC_ALPHA 1107
	#define ID_GREYC_ALPHA_TEXT 1108
	#define ID_GREYC_SIGMA 1109
	#define ID_GREYC_SIGMA_TEXT 1110
	#define ID_GREYC_SHARPNESS 1111
	#define ID_GREYC_SHARPNESS_TEXT 1112
	#define ID_GREYC_ANISO 1113
	#define ID_GREYC_ANISO_TEXT 1114
	#define ID_GREYC_SPATIAL 1115
	#define ID_GREYC_SPATIAL_TEXT 1116
	#define ID_GREYC_ANGULAR 1117
	#define ID_GREYC_ANGULAR_TEXT 1118
	#define ID_GREYC_INTERPOLATIONCHOICE 1119
	#define ID_CHIU_ENABLED 1120
	#define ID_CHIU_RADIUS 1121
	#define ID_CHIU_RADIUS_TEXT 1122
	#define ID_CHIU_INCLUDECENTER 1123
	#define ID_TM_RESET 1124
	#define ID_AUTO_TONEMAP 1125
	#define ID_TM_APPLY 1126
	#define ID_SERVER_TEXT 1127
	#define ID_ADD_SERVER 1128
	#define ID_REMOVE_SERVER 1129
	#define ID_SERVER_UPDATE_INT 1130
	#define ID_NETWORK_TREE 1131
	#define ID_LG_NAME 1132
	#define ID_TAB_LG_TOGGLE 1133
	#define ID_TAB_LG 1134
	#define ID_LG_SCALE 1135
	#define ID_LG_SCALE_TEXT 1136
	#define ID_LG_RGB_ENABLED 1137
	#define ID_LG_RGBCOLOR 1138
	#define ID_LG_TEMPERATURE_ENABLED 1139
	#define ID_LG_TEMPERATURE 1140
	#define ID_LG_TEMPERATURE_TEXT 1141
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class LuxMainFrame
	///////////////////////////////////////////////////////////////////////////////
	class LuxMainFrame : public wxFrame 
	{
		private:
		
		protected:
			wxMenuBar* m_menubar;
			wxMenu* m_file;
			wxMenu* m_render;
			wxMenu* m_view;
			wxMenu* m_help;
			wxAuiNotebook* m_auinotebook;
			wxPanel* m_renderPage;
			wxToolBar* m_renderToolBar;
			wxStaticText* m_ThreadText;
			wxToolBar* m_viewerToolBar;
			wxAuiNotebook* m_outputNotebook;
			wxScrolledWindow* m_LightGroups;
			wxBoxSizer* m_LightGroupsSizer;
			wxScrolledWindow* m_Tonemap;
			wxPanel* m_TonemapOptionsPanel;
			wxPanel* m_Tab_ToneMapPanel;
			wxStaticBitmap* m_tonemapBitmap;
			wxStaticText* m_ToneMapStaticText;
			
			wxStaticBitmap* m_Tab_ToneMapToggleIcon;
			wxStaticBitmap* m_Tab_ToneMapIcon;
			wxPanel* m_Tab_Control_ToneMapPanel;
			wxStaticText* m_ToneMapKernelStaticText;
			wxChoice* m_TM_kernelChoice;
			wxPanel* m_TonemapReinhardOptionsPanel;
			wxStaticText* m_TM_Reinhard_prescaleStaticText;
			wxSlider* m_TM_Reinhard_prescaleSlider;
			wxTextCtrl* m_TM_Reinhard_prescaleText;
			wxStaticText* m_TM_Reinhard_postscaleStaticText;
			wxSlider* m_TM_Reinhard_postscaleSlider;
			wxTextCtrl* m_TM_Reinhard_postscaleText;
			wxStaticText* m_TM_Reinhard_burnStaticText;
			wxSlider* m_TM_Reinhard_burnSlider;
			wxTextCtrl* m_TM_Reinhard_burnText;
			wxPanel* m_TonemapLinearOptionsPanel;
			wxStaticText* m_TM_Linear_sensitivityStaticText;
			wxSlider* m_TM_Linear_sensitivitySlider;
			wxTextCtrl* m_TM_Linear_sensitivityText;
			wxStaticText* m_TM_Linear_exposureStaticText;
			wxSlider* m_TM_Linear_exposureSlider;
			wxTextCtrl* m_TM_Linear_exposureText;
			wxStaticText* m_TM_Linear_fstopStaticText;
			wxSlider* m_TM_Linear_fstopSlider;
			wxTextCtrl* m_TM_Linear_fstopText;
			wxStaticText* m_TM_Linear_gammaStaticText;
			wxSlider* m_TM_Linear_gammaSlider;
			wxTextCtrl* m_TM_Linear_gammaText;
			wxPanel* m_TonemapContrastOptionsPanel;
			wxStaticText* m_TM_contrast_YwaStaticText;
			wxSlider* m_TM_contrast_ywaSlider;
			wxTextCtrl* m_TM_contrast_ywaText;
			wxPanel* m_BloomOptionsPanel;
			wxPanel* m_Tab_LensEffectsPanel;
			wxStaticBitmap* m_bloomBitmap;
			wxStaticText* m_TORGB_lensfxStaticText;
			
			wxStaticBitmap* m_Tab_LensEffectsToggleIcon;
			wxStaticBitmap* m_Tab_LensEffectsIcon;
			wxPanel* m_Tab_Control_LensEffectsPanel;
			wxAuiNotebook* m_LensEffectsAuiNotebook;
			wxPanel* m_bloomPanel;
			wxStaticText* m_TORGB_bloomweightStaticText11;
			wxSlider* m_TORGB_bloomradiusSlider;
			wxTextCtrl* m_TORGB_bloomradiusText;
			wxButton* m_computebloomlayer;
			wxStaticText* m_TORGB_bloomweightStaticText1;
			wxSlider* m_TORGB_bloomweightSlider;
			wxTextCtrl* m_TORGB_bloomweightText;
			wxPanel* m_vignettingPanel;
			wxCheckBox* m_vignettingenabledCheckBox;
			wxStaticText* m_vignettingamountStaticText;
			wxStaticText* m_staticText39;
			
			wxStaticText* m_staticText40;
			
			wxStaticText* m_staticText41;
			wxSlider* m_vignettingamountSlider;
			wxTextCtrl* m_vignettingamountText;
			wxPanel* m_aberrationPanel;
			wxCheckBox* m_aberrationEnabled;
			wxStaticText* m_aberrationamountStaticText;
			wxSlider* m_aberrationamountSlider;
			wxTextCtrl* m_aberrationamountText;
			wxPanel* m_glarePanel;
			wxCheckBox* m_glareEnabled;
			
			wxStaticText* m_glareamountStaticText;
			wxSlider* m_glareamountSlider;
			wxTextCtrl* m_glareamountText;
			wxStaticText* m_glarebladesStaticText;
			wxSpinCtrl* m_glarebladesSpin;
			
			wxStaticText* m_glareradiusStaticText;
			wxSlider* m_glareradiusSlider;
			wxTextCtrl* m_glareradiusText;
			wxPanel* m_ColorSpaceOptionsPanel;
			wxPanel* m_Tab_ColorSpacePanel;
			wxStaticBitmap* m_colorspaceBitmap;
			wxStaticText* m_TORGB_colorspaceStaticText;
			
			wxStaticBitmap* m_Tab_ColorSpaceToggleIcon;
			wxStaticBitmap* m_Tab_ColorSpaceIcon;
			wxPanel* m_Tab_Control_ColorSpacePanel;
			wxStaticText* m_TORGB_colorspacepresetsStaticText;
			wxChoice* m_TORGB_colorspaceChoice;
			wxAuiNotebook* m_ColorSpaceAuiNotebook;
			wxPanel* m_ColorSpaceWhitepointPanel;
			wxStaticText* m_TORGB_whitexStaticText;
			wxSlider* m_TORGB_xwhiteSlider;
			wxTextCtrl* m_TORGB_xwhiteText;
			wxStaticText* m_TORGB_whiteyStaticText;
			wxSlider* m_TORGB_ywhiteSlider;
			wxTextCtrl* m_TORGB_ywhiteText;
			wxPanel* m_ColorSpaceRGBPanel;
			wxStaticText* m_TORGB_rgbxyStaticText;
			wxSlider* m_TORGB_xredSlider;
			wxTextCtrl* m_TORGB_xredText;
			wxSlider* m_TORGB_yredSlider;
			wxTextCtrl* m_TORGB_yredText;
			wxSlider* m_TORGB_xgreenSlider;
			wxTextCtrl* m_TORGB_xgreenText;
			wxSlider* m_TORGB_ygreenSlider;
			wxTextCtrl* m_TORGB_ygreenText;
			wxSlider* m_TORGB_xblueSlider;
			wxTextCtrl* m_TORGB_xblueText;
			wxSlider* m_TORGB_yblueSlider;
			wxTextCtrl* m_TORGB_yblueText;
			wxPanel* m_GammaOptionsPanel;
			wxPanel* m_Tab_GammaPanel;
			wxStaticBitmap* m_gammaBitmap;
			wxStaticText* m_GammaStaticText;
			
			wxStaticBitmap* m_Tab_GammaToggleIcon;
			wxStaticBitmap* m_Tab_GammaIcon;
			wxPanel* m_Tab_Control_GammaPanel;
			wxSlider* m_TORGB_gammaSlider;
			wxTextCtrl* m_TORGB_gammaText;
			wxPanel* m_HistogramPanel;
			wxPanel* m_Tab_HistogramPanel;
			wxStaticBitmap* m_histogramBitmap;
			wxStaticText* m_HistogramstaticText;
			
			wxStaticBitmap* m_Tab_HistogramToggleIcon;
			wxStaticBitmap* m_Tab_HistogramIcon;
			wxPanel* m_Tab_Control_HistogramPanel;
			wxStaticText* m_staticText43;
			wxChoice* m_Histogram_Choice;
			
			wxStaticText* m_staticText431;
			wxCheckBox* m_HistogramLogCheckBox;
			wxPanel* m_NoiseOptionsPanel;
			wxPanel* m_Tab_LensEffectsPanel1;
			wxStaticBitmap* m_NoiseReductionBitmap;
			wxStaticText* m_NoiseReductionStaticText;
			
			wxStaticBitmap* m_Tab_NoiseReductionToggleIcon;
			wxStaticBitmap* m_Tab_NoiseReductionIcon;
			wxPanel* m_Tab_Control_NoiseReductionPanel;
			wxAuiNotebook* m_NoiseReductionAuiNotebook;
			wxPanel* m_GREYCPanel;
			wxNotebook* m_GREYCNotebook;
			wxPanel* m_GREYCRegPanel;
			wxCheckBox* m_greyc_EnabledCheckBox;
			wxCheckBox* m_greyc_fastapproxCheckBox;
			wxStaticText* m_GREYCIterationsStaticText;
			wxSlider* m_greyc_iterationsSlider;
			wxTextCtrl* m_greyc_iterationsText;
			wxStaticText* m_GREYCamplitureStaticText;
			wxSlider* m_greyc_amplitudeSlider;
			wxTextCtrl* m_greyc_amplitudeText;
			wxStaticText* m_GREYCgaussprecStaticText;
			wxSlider* m_greyc_gausprecSlider;
			wxTextCtrl* m_greyc_gaussprecText;
			wxPanel* m_GREYCFilterPanel;
			wxStaticText* m_GREYCAlphaStaticText;
			wxSlider* m_greyc_alphaSlider;
			wxTextCtrl* m_greyc_alphaText;
			wxStaticText* m_GREYCSigmaStaticText;
			wxSlider* m_greyc_sigmaSlider;
			wxTextCtrl* m_greyc_sigmaText;
			wxStaticText* m_GREYCsharpnessStaticText;
			wxSlider* m_greyc_sharpnessSlider;
			wxTextCtrl* m_greyc_sharpnessText;
			wxStaticText* m_GREYCAnisoStaticText;
			wxSlider* m_greyc_anisoSlider;
			wxTextCtrl* m_greyc_anisoText;
			wxPanel* m_GREYCAdvancedPanel;
			wxStaticText* m_GREYCspatialStaticText;
			wxSlider* m_greyc_spatialSlider;
			wxTextCtrl* m_greyc_spatialText;
			wxStaticText* m_GREYCangularStaticText;
			wxSlider* m_greyc_angularSlider;
			wxTextCtrl* m_greyc_angularText;
			wxStaticText* m_GREYCinterpolationStaticText;
			wxChoice* m_GREYCinterpolationChoice;
			wxPanel* m_ChiuPanel;
			wxCheckBox* m_chiu_enableCheckBox;
			wxSlider* m_chiu_radiusSlider;
			wxTextCtrl* m_chiu_radiusText;
			wxCheckBox* m_chiu_includecenterCheckBox;
			wxButton* m_TM_resetButton;
			wxCheckBox* m_auto_tonemapCheckBox;
			wxButton* m_TM_applyButton;
			wxNotebook* m_outputNotebook2;
			wxPanel* m_logPage;
			wxTextCtrl* m_logTextCtrl;
			wxPanel* m_networkPage;
			wxToolBar* m_networkToolBar;
			wxStaticText* m_serverStaticText;
			wxTextCtrl* m_serverTextCtrl;
			wxStaticText* m_updateStaticText;
			wxSpinCtrl* m_serverUpdateSpin;
			wxTreeCtrl* m_networkTreeCtrl;
			wxStatusBar* m_statusBar;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnExit( wxCloseEvent& event ){ event.Skip(); }
			virtual void OnOpen( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnMenu( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnMouse( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnScroll( wxScrollEvent& event ){ event.Skip(); }
			virtual void OnFocus( wxFocusEvent& event ){ event.Skip(); }
			virtual void OnText( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnSpin( wxSpinEvent& event ){ event.Skip(); }
			virtual void OnSpinText( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnTreeSelChanged( wxTreeEvent& event ){ event.Skip(); }
			
		
		public:
			LuxMainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("LuxRender"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
			~LuxMainFrame();
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	/// Class LightGroupPanel
	///////////////////////////////////////////////////////////////////////////////
	class LightGroupPanel : public wxPanel 
	{
		private:
		
		protected:
			wxBoxSizer* m_LG_MainSizer;
			wxPanel* m_LG_MainPanel;
			wxBoxSizer* m_LG_SubSizer;
			wxPanel* m_Tab_LightGroupPanel;
			wxStaticBitmap* m_lightgroupBitmap;
			wxStaticText* m_lightgroupStaticText;
			wxStaticText* m_LG_name;
			
			wxStaticBitmap* m_Tab_LightGroupToggleIcon;
			wxStaticBitmap* m_Tab_LightGroupIcon;
			wxPanel* m_Tab_Control_LightGroupPanel;
			wxStaticText* m_LG_scaleLabel;
			wxSlider* m_LG_scaleSlider;
			wxTextCtrl* m_LG_scaleText;
			wxCheckBox* m_LG_rgbEnabled;
			wxColourPickerCtrl* m_LG_rgbPicker;
			wxCheckBox* m_LG_temperatureEnabled;
			wxStaticBitmap* m_BarBlackBodyStaticBitmap;
			wxSlider* m_LG_temperatureSlider;
			wxTextCtrl* m_LG_temperatureText;
			
			// Virtual event handlers, overide them in your derived class
			virtual void OnMouse( wxMouseEvent& event ){ event.Skip(); }
			virtual void OnScroll( wxScrollEvent& event ){ event.Skip(); }
			virtual void OnFocus( wxFocusEvent& event ){ event.Skip(); }
			virtual void OnText( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnCheckBox( wxCommandEvent& event ){ event.Skip(); }
			virtual void OnColourChanged( wxColourPickerEvent& event ){ event.Skip(); }
			
		
		public:
			LightGroupPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 396,235 ), long style = wxNO_BORDER|wxTAB_TRAVERSAL );
			~LightGroupPanel();
		
	};
	
} // namespace lux

#endif //__wxluxframe__
