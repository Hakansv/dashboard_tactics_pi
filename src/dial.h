/******************************************************************************
 * $Id: dial.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Jean-Eudes Onfray
 *           (Inspired by original work from Andreas Heiming)
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#ifndef __Dial_H__
#define __Dial_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "instrument.h"

#define ANGLE_OFFSET            90      // 0 degrees are at 12 o´clock

typedef enum
{
      DIAL_LABEL_NONE,
      DIAL_LABEL_HORIZONTAL,
      DIAL_LABEL_ROTATED
} DialLabelOption;

typedef enum
{
      DIAL_MARKER_NONE,
      DIAL_MARKER_SIMPLE,
      DIAL_MARKER_REDGREEN,
      DIAL_MARKER_REDGREENBAR
} DialMarkerOption;

typedef enum
{
      DIAL_POSITION_NONE,
      DIAL_POSITION_INSIDE,
      DIAL_POSITION_TOPLEFT,
      DIAL_POSITION_TOPRIGHT,
      DIAL_POSITION_BOTTOMLEFT,
      DIAL_POSITION_BOTTOMRIGHT
#ifdef _TACTICSPI_H_
      ,
      DIAL_POSITION_TOPINSIDE
#endif // _TACTICSPI_H_
} DialPositionOption;

extern double rad2deg(double angle);
extern double deg2rad(double angle);

//+------------------------------------------------------------------------------
//|
//| CLASS:
//|    DashboardInstrument_Dial
//|
//| DESCRIPTION:
//|    This class creates a speedometer style control
//|
//+------------------------------------------------------------------------------
class DashboardInstrument_Dial: public DashboardInstrument
{
public:
    DashboardInstrument_Dial( wxWindow *parent, wxWindowID id, wxString title,
#ifdef _TACTICSPI_H_
                              unsigned long long cap_flag,
#else
                              int cap_flag,
#endif // _TACTICSPI_H_
                              int s_angle, int r_angle, int s_value, int e_value);

    ~DashboardInstrument_Dial(void){}

    wxSize GetSize( int orient, wxSize hint );
#ifdef _TACTICSPI_H_
    virtual void SetData(unsigned long long st, double data, wxString unit, long long timestamp=0LL);
    virtual void timeoutEvent(void);
#ifndef __DERIVEDTIMEOUT_OVERRIDE__
    virtual void derivedTimeoutEvent(void){};
#else
    virtual void derivedTimeoutEvent(void);
#endif // __DERIVEDTIMEOUT_OVERRIDE__
#else
    void SetData(int, double, wxString);
#endif // _TACTICSPI_H_
    void SetOptionMarker(double step, DialMarkerOption option, int offset) {
        m_MarkerStep = step; m_MarkerOption = option; m_MarkerOffset = offset;
    }
    void SetOptionLabel(double step, DialLabelOption option, wxArrayString labels=wxArrayString()) {
        m_LabelStep = step; m_LabelOption = option; m_LabelArray = labels;
    }
    void SetOptionMainValue(wxString format, DialPositionOption option) {
        m_MainValueFormat = format; m_MainValueOption = option;
    }
    void SetOptionExtraValue(
#ifdef _TACTICSPI_H_
        unsigned long long cap,
#else
        int cap,
#endif // _TACTICSPI_H_
        wxString format, DialPositionOption option) {
        m_ExtraValueCap = cap; m_cap_flag |= cap; m_ExtraValueFormat = format; m_ExtraValueOption = option;
    }

private:

protected:
    int m_cx;
    int m_cy;
    int m_radius;
    int m_AngleStart;
    int m_AngleRange;
    double m_MainValue;
#ifdef _TACTICSPI_H_
    unsigned long long m_MainValueCap;
    int m_s_angle;
    int m_s_value;
#else
    int m_MainValueCap;
#endif // _TACTICSPI_H_
    double m_MainValueMin;
    double m_MainValueMax;
    wxString m_MainValueFormat;
    wxString m_MainValueUnit;
    DialPositionOption m_MainValueOption;
    double m_ExtraValue;
#ifdef _TACTICSPI_H_
    unsigned long long m_ExtraValueCap;
#else
    int m_ExtraValueCap;
#endif // _TACTICSPI_H_
    wxString m_ExtraValueFormat;
    wxString m_ExtraValueUnit;
    DialPositionOption m_ExtraValueOption;
    DialMarkerOption m_MarkerOption;
    int m_MarkerOffset;
    double m_MarkerStep;
    double m_LabelStep;
    DialLabelOption m_LabelOption;
    wxArrayString m_LabelArray;

    virtual void Draw(wxGCDC* dc);
    virtual void DrawFrame(wxGCDC* dc);
    virtual void DrawMarkers(wxGCDC* dc);
    virtual void DrawLabels(wxGCDC* dc);
    virtual void DrawBackground(wxGCDC* dc);
    virtual void DrawData(wxGCDC* dc, double value, wxString unit, wxString format, DialPositionOption position);
    virtual void DrawForeground(wxGCDC* dc);
};

/* Shared functions */
void DrawCompassRose( wxGCDC* dc, int cx, int cy, int radius, int startangle, bool showlabels );
void DrawBoat( wxGCDC* dc, int cx, int cy, int radius );

#endif // __Dial_H__

