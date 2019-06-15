/******************************************************************************
 * $Id: dashboard_pi.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef _DASHBOARDPI_H_
#define _DASHBOARDPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/notebook.h>
#include <wx/fileconf.h>
#include <wx/listctrl.h>
#include <wx/imaglist.h>
#include <wx/spinctrl.h>
#include <wx/aui/aui.h>
#include <wx/fontpicker.h>
#include <wx/unichar.h>
//wx2.9 #include <wx/wrapsizer.h>
#include "ocpn_plugin.h"

#ifdef _INCLUDE_TACTICS_PI_
#ifndef _TACTICSPI_H_
#include "tactics_pi.h"
#endif // _TACTICSPI_H_
#endif // _INCLUDE_TACTICS_PI_

#define     PLUGIN_VERSION_MAJOR    1
#define     PLUGIN_VERSION_MINOR    5

#define     MY_API_VERSION_MAJOR    1
#ifdef _TACTICSPI_H_
#define     MY_API_VERSION_MINOR    12
#else
#define     MY_API_VERSION_MINOR    6
#endif // _TACTICSPI_H_

#include "nmea0183/nmea0183.h"
#include "instrument.h"
#include "speedometer.h"
#include "compass.h"
#include "wind.h"
#include "rudder_angle.h"
#include "gps.h"
#include "depth.h"
#include "clock.h"
#include "wind_history.h"
#include "baro_history.h"
#include "from_ownship.h"
#include "iirfilter.h"

class DashboardWindow;
class DashboardWindowContainer;
class DashboardInstrumentContainer;

#define DASHBOARD_TOOL_POSITION -1          // Request default positioning of toolbar tool

#define gps_watchdog_timeout_ticks  10

class DashboardWindowContainer
{
public:
    DashboardWindowContainer(DashboardWindow *dashboard_window, wxString name, wxString caption, wxString orientation, wxArrayInt inst) {
        m_pDashboardWindow = dashboard_window; m_sName = name; m_sCaption = caption; m_sOrientation = orientation; m_aInstrumentList = inst; m_bIsVisible = false; m_bIsDeleted = false; }

#ifdef _TACTICSPI_H_
    DashboardWindowContainer( DashboardWindowContainer *sourcecont ) {
            m_pDashboardWindow = sourcecont->m_pDashboardWindow;
            m_bIsVisible       = sourcecont->m_bIsVisible;
            m_bIsDeleted       = sourcecont->m_bIsDeleted;
            m_bPersVisible     = sourcecont->m_bPersVisible;
            m_sName            = sourcecont->m_sName;
            m_sCaption         = sourcecont->m_sCaption;
            m_sOrientation     = sourcecont->m_sOrientation;
            m_aInstrumentList  = sourcecont->m_aInstrumentList;
    }
#endif // _TACTICSPI_H_
 
    ~DashboardWindowContainer(){}

DashboardWindow              *m_pDashboardWindow;
    bool                          m_bIsVisible;
    bool                          m_bIsDeleted;
    bool                          m_bPersVisible;  // Persists visibility, even when Dashboard tool is toggled off.
    wxString                      m_sName;
    wxString                      m_sCaption;
    wxString                      m_sOrientation;
    wxArrayInt                    m_aInstrumentList;
};

class DashboardInstrumentContainer
{
public:
    DashboardInstrumentContainer(int id, DashboardInstrument *instrument,
#ifdef _TACTICSPI_H_
    unsigned long long capa
#else
    int capa
#endif // _TACTICSPI_H_
        ) {
        m_ID = id; m_pInstrument = instrument; m_cap_flag = capa;
    }
    ~DashboardInstrumentContainer(){ delete m_pInstrument; }

    DashboardInstrument    *m_pInstrument;
    int                     m_ID;
#ifdef _TACTICSPI_H_
    unsigned long long      m_cap_flag;
#else
    int                     m_cap_flag;
#endif // _TACTICSPI_H_
};

//    Dynamic arrays of pointers need explicit macros in wx261
#ifdef __WX261
WX_DEFINE_ARRAY_PTR(DashboardWindowContainer *, wxArrayOfDashboard);
WX_DEFINE_ARRAY_PTR(DashboardInstrumentContainer *, wxArrayOfInstrument);
#else
WX_DEFINE_ARRAY(DashboardWindowContainer *, wxArrayOfDashboard);
WX_DEFINE_ARRAY(DashboardInstrumentContainer *, wxArrayOfInstrument);
#endif

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------


class dashboard_pi : public
#ifdef _TACTICSPI_H_
    tactics_pi, wxTimer, opencpn_plugin_112
#else
    wxTimer, opencpn_plugin_16
#endif // _TACTICSPI_H_
{
public:
    dashboard_pi(void *ppimgr);
    ~dashboard_pi(void);

    //    The required PlugIn Methods
    int Init(void);
    bool DeInit(void);

    void Notify();

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();

#ifdef _TACTICSPI_H_
    // implementation of parent classes methods (w/ call-backs)
    void OnContextMenuItemCallback(int id);
    void SendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit);
    bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
    bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
#endif // _TACTICSPI_H_

    //    The optional method overrides
    void SetNMEASentence(wxString &sentence);
    void SetPositionFix(PlugIn_Position_Fix &pfix);
    void SetCursorLatLon(double lat, double lon);
    int GetToolbarToolCount(void);
    void OnToolbarToolCallback(int id);
    void ShowPreferencesDialog( wxWindow* parent );
    void SetColorScheme(PI_ColorScheme cs);
    void OnPaneClose( wxAuiManagerEvent& event );
    void UpdateAuiStatus(void);
    bool SaveConfig(void);
    void PopulateContextMenu( wxMenu* menu );
    void ShowDashboard( size_t id, bool visible );
    int GetToolbarItemId(){ return m_toolbar_item_id; }
    int GetDashboardWindowShownCount();
    void SetPluginMessage(wxString &message_id, wxString &message_body);
    #ifdef _TACTICSPI_H_
    wxWindow *pGetPluginFrame(void) { return m_pluginFrame; }
    #endif // _TACTICSPI_H_

private:
    bool LoadConfig(void);
#ifdef _TACTICSPI_H_
    void ApplyConfig( bool init=false );
#else
    void ApplyConfig(void);
#endif // _TACTICSPI_H_

#ifdef _TACTICSPI_H_
    void pSendSentenceToAllInstruments(
        unsigned long long st, double value, wxString unit);
#else
    void SendSentenceToAllInstruments(
        int st, double value, wxString unit);
#endif // _TACTICSPI_H_
    void SendSatInfoToAllInstruments(int cnt, int seq, SAT_INFO sats[4]);
    void SendUtcTimeToAllInstruments( wxDateTime value );

#ifdef _TACTICSPI_H_
    bool              m_bToggledStateVisible;
    int               m_iPlugInRequirements;
    wxWindow         *m_pluginFrame;
#endif // _TACTICSPI_H_
    wxFileConfig     *m_pconfig;
    wxAuiManager     *m_pauimgr;
    int              m_toolbar_item_id;

    wxArrayOfDashboard   m_ArrayOfDashboardWindow;
    int                  m_show_id;
    int                  m_hide_id;

    NMEA0183             m_NMEA0183;                 // Used to parse NMEA Sentences
    short                mPriPosition, mPriCOGSOG, mPriHeadingM, mPriHeadingT, mPriVar, mPriDateTime, mPriAWA, mPriTWA, mPriDepth;
    double               mVar;
    // FFU
    double               mSatsInView;
    double               mHdm;
    wxDateTime           mUTCDateTime;
    int                  m_config_version;
    wxString             m_VDO_accumulator;
    int                  mHDx_Watchdog;
    int                  mHDT_Watchdog;
    int                  mGPS_Watchdog;
    int                  mVar_Watchdog;

    iirfilter            mSOGFilter;
    iirfilter            mCOGFilter;
    //protected:
    //      DECLARE_EVENT_TABLE();
};

class DashboardPreferencesDialog : public
#ifdef _TACTICSPI_H_
    TacticsPreferencesDialog
#else
    wxDialog
#endif // _TACTICSPI_H_
{
public:
    DashboardPreferencesDialog( wxWindow *pparent, wxWindowID id, wxArrayOfDashboard config
#ifdef _TACTICSPI_H_
                                , wxString commonName
#endif // _TACTICSPI_H_
        );
    ~DashboardPreferencesDialog() {}

    void OnCloseDialog(wxCloseEvent& event);
    void OnDashboardSelected(wxListEvent& event);
    void OnDashboardAdd(wxCommandEvent& event);
    void OnDashboardDelete(wxCommandEvent& event);
    void OnInstrumentSelected(wxListEvent& event);
    void OnInstrumentAdd(wxCommandEvent& event);
    void OnInstrumentEdit(wxCommandEvent& event);
    void OnInstrumentDelete(wxCommandEvent& event);
    void OnInstrumentUp(wxCommandEvent& event);
    void OnInstrumentDown(wxCommandEvent& event);
    void SaveDashboardConfig();

    wxArrayOfDashboard            m_Config;
#ifdef _TACTICSPI_H_
    wxNotebook                   *m_itemNotebook;
#endif // _TACTICSPI_H_
    wxFontPickerCtrl             *m_pFontPickerTitle;
    wxFontPickerCtrl             *m_pFontPickerData;
    wxFontPickerCtrl             *m_pFontPickerLabel;
    wxFontPickerCtrl             *m_pFontPickerSmall;
    wxSpinCtrl                   *m_pSpinSpeedMax;
    wxSpinCtrl                   *m_pSpinCOGDamp;
    wxSpinCtrl                   *m_pSpinSOGDamp;
    wxChoice                     *m_pChoiceUTCOffset;
    wxChoice                     *m_pChoiceSpeedUnit;
    wxChoice                     *m_pChoiceDepthUnit;
    wxSpinCtrlDouble             *m_pSpinDBTOffset;
    wxChoice                     *m_pChoiceDistanceUnit;
    wxChoice                     *m_pChoiceWindSpeedUnit;
#ifdef _TACTICSPI_H_
    wxChoice                     *m_pChoiceTemperatureUnit;
#endif // _TACTICSPI_H_

private:
    void UpdateDashboardButtonsState(void);
    void UpdateButtonsState(void);
    int                           curSel;
    wxListCtrl                   *m_pListCtrlDashboards;
    wxBitmapButton               *m_pButtonAddDashboard;
    wxBitmapButton               *m_pButtonDeleteDashboard;
    wxPanel                      *m_pPanelDashboard;
    wxTextCtrl                   *m_pTextCtrlCaption;
    wxCheckBox                   *m_pCheckBoxIsVisible;
    wxChoice                     *m_pChoiceOrientation;
    wxListCtrl                   *m_pListCtrlInstruments;
    wxButton                     *m_pButtonAdd;
    wxButton                     *m_pButtonEdit;
    wxButton                     *m_pButtonDelete;
    wxButton                     *m_pButtonUp;
    wxButton                     *m_pButtonDown;
};

class AddInstrumentDlg : public wxDialog
{
public:
    AddInstrumentDlg(wxWindow *pparent, wxWindowID id);
    ~AddInstrumentDlg() {}

    unsigned int GetInstrumentAdded();

private:
    wxListCtrl*                   m_pListCtrlInstruments;

};

enum
{
    ID_DASHBOARD_WINDOW
};

enum
{
    ID_DASH_PREFS = 999,
    ID_DASH_VERTICAL,
    ID_DASH_HORIZONTAL,
    ID_DASH_UNDOCK
};

class DashboardWindow : public
#ifdef _TACTICSPI_H_
    TacticsWindow
#else
    wxDialog
#endif // _TACTICSPI_H_
{
public:
    DashboardWindow(
        wxWindow *pparent, wxWindowID id, wxAuiManager *auimgr,
        dashboard_pi* plugin,
        int orient, DashboardWindowContainer* mycont
#ifdef _TACTICSPI_H_
        , wxString commonName
#endif // _TACTICSPI_H_
        );
    ~DashboardWindow();

    void SetColorScheme( PI_ColorScheme cs );
    void SetSizerOrientation( int orient );
    int GetSizerOrientation();
    void OnSize( wxSizeEvent& evt );
    void OnContextMenu( wxContextMenuEvent& evt );
    void OnContextMenuSelect( wxCommandEvent& evt );
    bool isInstrumentListEqual( const wxArrayInt& list );
    void SetInstrumentList( wxArrayInt list );
    void SendSentenceToAllInstruments(
#ifdef _TACTICSPI_H_
        unsigned long long st,
#else
        int st,
#endif // _TACTICSPI_H_
       double value, wxString unit );
    void SendSatInfoToAllInstruments( int cnt, int seq, SAT_INFO sats[4] );
    void SendUtcTimeToAllInstruments( wxDateTime value );
    void ChangePaneOrientation( int orient, bool updateAUImgr );
    /*TODO: OnKeyPress pass event to main window or disable focus*/

    DashboardWindowContainer* m_Container;

private:
    wxAuiManager         *m_pauimgr;
    dashboard_pi*         m_plugin;

    //wx2.9      wxWrapSizer*          itemBoxSizer;
    wxBoxSizer*          itemBoxSizer;
    wxArrayOfInstrument  m_ArrayOfInstrument;
};

#endif
