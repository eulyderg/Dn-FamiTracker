/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2020 Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2018 HertzDevil
**
** Dn-FamiTracker is (C) 2020-2024 D.P.C.M.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Library General Public License for more details. To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
*/

#include <iterator>
#include <sstream>
#include "stdafx.h"
#include "../resource.h"        // // //
#include "Instrument.h"
#include "InstrumentEPSM.h"		// // //
#include "InstrumentEditPanel.h"
#include "InstrumentEditorEPSM.h"
#include "Clipboard.h"

#include "FamiTracker.h"
#include "FamiTrackerDoc.h"
#include "Settings.h"

static constexpr int CUSTOM_PATCH = 0;

// CInstrumentEditorEPSM dialog

IMPLEMENT_DYNAMIC(CInstrumentEditorEPSM, CInstrumentEditPanel)

CInstrumentEditorEPSM::CInstrumentEditorEPSM(CWnd* pParent /*=NULL*/)
	: CInstrumentEditPanel(CInstrumentEditorEPSM::IDD, pParent)
{
}

CInstrumentEditorEPSM::~CInstrumentEditorEPSM()
{
}

void CInstrumentEditorEPSM::DoDataExchange(CDataExchange* pDX)
{
	CInstrumentEditPanel::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInstrumentEditorEPSM, CInstrumentEditPanel)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	/*_WM_CONTEXTMENU()
	ON_COMMAND(IDC_COPY, &CInstrumentEditorEPSM::OnCopy)
	ON_COMMAND(IDC_PASTE, &CInstrumentEditorEPSM::OnPaste)*/
END_MESSAGE_MAP()


// CInstrumentSettingsEPSM message handlers

BOOL CInstrumentEditorEPSM::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Get active document
	m_pDocument = GetDocument();

	return TRUE;
}

void CInstrumentEditorEPSM::SelectInstrument(std::shared_ptr<CInstrument> pInst)
{
	m_pInstrument = std::dynamic_pointer_cast<CInstrumentEPSM>(pInst);
	ASSERT(m_pInstrument);

	SetupSlider(IDC_M_FB1, 7);
	SetSliderVal(IDC_M_FB1, false, m_pInstrument->m_pFMData.Feedback);

	SetupSlider(IDC_M_MUL1, 15);
	SetupSlider(IDC_M_MUL2, 15);
	SetupSlider(IDC_M_MUL3, 15);
	SetupSlider(IDC_M_MUL4, 15);
	SetSliderVal(IDC_M_MUL1, false, m_pInstrument->m_pFMData.Mult[0]);
	SetSliderVal(IDC_M_MUL2, false, m_pInstrument->m_pFMData.Mult[1]);
	SetSliderVal(IDC_M_MUL3, false, m_pInstrument->m_pFMData.Mult[2]);
	SetSliderVal(IDC_M_MUL4, false, m_pInstrument->m_pFMData.Mult[3]);
	SetSliderTic(IDC_M_MUL1, false, 1);
	SetSliderTic(IDC_M_MUL2, false, 1);
	SetSliderTic(IDC_M_MUL3, false, 1);
	SetSliderTic(IDC_M_MUL4, false, 1);

	SetupSlider(IDC_M_VOL1, 127);
	SetupSlider(IDC_M_VOL2, 127);
	SetupSlider(IDC_M_VOL3, 127);
	SetupSlider(IDC_M_VOL4, 127);
	SetSliderVal(IDC_M_VOL1, true, m_pInstrument->m_pFMData.TotalLevel[0]);
	SetSliderVal(IDC_M_VOL2, true, m_pInstrument->m_pFMData.TotalLevel[1]);
	SetSliderVal(IDC_M_VOL3, true, m_pInstrument->m_pFMData.TotalLevel[2]);
	SetSliderVal(IDC_M_VOL4, true, m_pInstrument->m_pFMData.TotalLevel[3]);

	SetupSlider(IDC_M_DET1, 6);
	SetupSlider(IDC_M_DET2, 6);
	SetupSlider(IDC_M_DET3, 6);
	SetupSlider(IDC_M_DET4, 6);
	SetSliderVal(IDC_M_DET1, false, m_pInstrument->m_pFMData.Detune[0]);
	SetSliderVal(IDC_M_DET2, false, m_pInstrument->m_pFMData.Detune[1]);
	SetSliderVal(IDC_M_DET3, false, m_pInstrument->m_pFMData.Detune[2]);
	SetSliderVal(IDC_M_DET4, false, m_pInstrument->m_pFMData.Detune[3]);
	SetSliderTic(IDC_M_DET1, false, 3);
	SetSliderTic(IDC_M_DET2, false, 3);
	SetSliderTic(IDC_M_DET3, false, 3);
	SetSliderTic(IDC_M_DET4, false, 3);

	SetupSlider(IDC_M_ATT1, 31);
	SetupSlider(IDC_M_ATT2, 31);
	SetupSlider(IDC_M_ATT3, 31);
	SetupSlider(IDC_M_ATT4, 31);
	SetSliderVal(IDC_M_ATT1, true, m_pInstrument->m_pFMData.Attack[0]);
	SetSliderVal(IDC_M_ATT2, true, m_pInstrument->m_pFMData.Attack[1]);
	SetSliderVal(IDC_M_ATT3, true, m_pInstrument->m_pFMData.Attack[2]);
	SetSliderVal(IDC_M_ATT4, true, m_pInstrument->m_pFMData.Attack[3]);
	SetupSlider(IDC_M_DEC1, 31);
	SetupSlider(IDC_M_DEC2, 31);
	SetupSlider(IDC_M_DEC3, 31);
	SetupSlider(IDC_M_DEC4, 31);
	SetSliderVal(IDC_M_DEC1, true, m_pInstrument->m_pFMData.Decay[0]);
	SetSliderVal(IDC_M_DEC2, true, m_pInstrument->m_pFMData.Decay[1]);
	SetSliderVal(IDC_M_DEC3, true, m_pInstrument->m_pFMData.Decay[2]);
	SetSliderVal(IDC_M_DEC4, true, m_pInstrument->m_pFMData.Decay[3]);
	SetupSlider(IDC_M_SUS1, 15);
	SetupSlider(IDC_M_SUS2, 15);
	SetupSlider(IDC_M_SUS3, 15);
	SetupSlider(IDC_M_SUS4, 15);
	SetSliderVal(IDC_M_SUS1, false, m_pInstrument->m_pFMData.Sustain[0]);
	SetSliderVal(IDC_M_SUS2, false, m_pInstrument->m_pFMData.Sustain[1]);
	SetSliderVal(IDC_M_SUS3, false, m_pInstrument->m_pFMData.Sustain[2]);
	SetSliderVal(IDC_M_SUS4, false, m_pInstrument->m_pFMData.Sustain[3]);
	SetupSlider(IDC_M_FAD1, 31);
	SetupSlider(IDC_M_FAD2, 31);
	SetupSlider(IDC_M_FAD3, 31);
	SetupSlider(IDC_M_FAD4, 31);
	SetSliderVal(IDC_M_FAD1, true, m_pInstrument->m_pFMData.Fade[0]);
	SetSliderVal(IDC_M_FAD2, true, m_pInstrument->m_pFMData.Fade[1]);
	SetSliderVal(IDC_M_FAD3, true, m_pInstrument->m_pFMData.Fade[2]);
	SetSliderVal(IDC_M_FAD4, true, m_pInstrument->m_pFMData.Fade[3]);
	SetupSlider(IDC_M_REL1, 15);
	SetupSlider(IDC_M_REL2, 15);
	SetupSlider(IDC_M_REL3, 15);
	SetupSlider(IDC_M_REL4, 15);
	SetSliderVal(IDC_M_REL1, true, m_pInstrument->m_pFMData.Release[0]);
	SetSliderVal(IDC_M_REL2, true, m_pInstrument->m_pFMData.Release[1]);
	SetSliderVal(IDC_M_REL3, true, m_pInstrument->m_pFMData.Release[2]);
	SetSliderVal(IDC_M_REL4, true, m_pInstrument->m_pFMData.Release[3]);
}

BOOL CInstrumentEditorEPSM::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

HBRUSH CInstrumentEditorEPSM::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CInstrumentEditorEPSM::SetupSlider(int Slider, int Max)
{
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	pSlider->SetRangeMax(Max);
}

int CInstrumentEditorEPSM::GetSliderVal(int Slider, bool Reversed)
{
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	int Value = Reversed ? pSlider->GetRangeMax() - pSlider->GetPos() : pSlider->GetPos();
	return Value;
}

void CInstrumentEditorEPSM::SetSliderVal(int Slider, bool Reversed, int Value)
{
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	Value = Reversed ? pSlider->GetRangeMax() - Value : Value;
	pSlider->SetPos(Value);
}

void CInstrumentEditorEPSM::SetSliderTic(int Slider, bool Reversed, int Tic)
{
	CSliderCtrl* pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	int Value = Reversed ? pSlider->GetRangeMax() - Tic : Tic;
	pSlider->SetTic(Value);
}

void CInstrumentEditorEPSM::OnBnClickedCheckbox()
{
	SetFocus();
}

void CInstrumentEditorEPSM::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateInstrument();
	SetFocus();
	CInstrumentEditPanel::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentEditorEPSM::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateInstrument();
	SetFocus();
	CInstrumentEditPanel::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentEditorEPSM::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{

}

void CInstrumentEditorEPSM::OnCopy() {}

void CInstrumentEditorEPSM::OnPaste() {}

BOOL CInstrumentEditorEPSM::PreTranslateMessage(MSG* pMsg)
{
	return CInstrumentEditPanel::PreTranslateMessage(pMsg);
}

// all of our target values are uint8_t
void CInstrumentEditorEPSM::SliderUpdate(int Slider, bool Reversed, uint8_t* target)
{
	CSliderCtrl* pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	int Value = Reversed ? pSlider->GetRangeMax() - pSlider->GetPos() : pSlider->GetPos();
	
	(*target) = uint8_t(Value);
}

void CInstrumentEditorEPSM::UpdateInstrument()
{
	SliderUpdate(IDC_M_FB1, false, &(m_pInstrument->m_pFMData.Feedback));
	SliderUpdate(IDC_M_MUL1, false, &(m_pInstrument->m_pFMData.Mult[0]));
	SliderUpdate(IDC_M_MUL2, false, &(m_pInstrument->m_pFMData.Mult[1]));
	SliderUpdate(IDC_M_MUL3, false, &(m_pInstrument->m_pFMData.Mult[2]));
	SliderUpdate(IDC_M_MUL4, false, &(m_pInstrument->m_pFMData.Mult[3]));
	SliderUpdate(IDC_M_VOL1, false, &(m_pInstrument->m_pFMData.TotalLevel[0]));
	SliderUpdate(IDC_M_VOL2, false, &(m_pInstrument->m_pFMData.TotalLevel[1]));
	SliderUpdate(IDC_M_VOL3, false, &(m_pInstrument->m_pFMData.TotalLevel[2]));
	SliderUpdate(IDC_M_VOL4, false, &(m_pInstrument->m_pFMData.TotalLevel[3]));
	SliderUpdate(IDC_M_DET1, false, &(m_pInstrument->m_pFMData.Detune[0]));
	SliderUpdate(IDC_M_DET2, false, &(m_pInstrument->m_pFMData.Detune[1]));
	SliderUpdate(IDC_M_DET3, false, &(m_pInstrument->m_pFMData.Detune[2]));
	SliderUpdate(IDC_M_DET4, false, &(m_pInstrument->m_pFMData.Detune[3]));
	SliderUpdate(IDC_M_ATT1, true, &(m_pInstrument->m_pFMData.Attack[0]));
	SliderUpdate(IDC_M_ATT2, true, &(m_pInstrument->m_pFMData.Attack[1]));
	SliderUpdate(IDC_M_ATT3, true, &(m_pInstrument->m_pFMData.Attack[2]));
	SliderUpdate(IDC_M_ATT4, true, &(m_pInstrument->m_pFMData.Attack[3]));
	SliderUpdate(IDC_M_DEC1, true, &(m_pInstrument->m_pFMData.Decay[0]));
	SliderUpdate(IDC_M_DEC2, true, &(m_pInstrument->m_pFMData.Decay[1]));
	SliderUpdate(IDC_M_DEC3, true, &(m_pInstrument->m_pFMData.Decay[2]));
	SliderUpdate(IDC_M_DEC4, true, &(m_pInstrument->m_pFMData.Decay[3]));
	SliderUpdate(IDC_M_SUS1, true, &(m_pInstrument->m_pFMData.Sustain[0]));
	SliderUpdate(IDC_M_SUS2, true, &(m_pInstrument->m_pFMData.Sustain[1]));
	SliderUpdate(IDC_M_SUS3, true, &(m_pInstrument->m_pFMData.Sustain[2]));
	SliderUpdate(IDC_M_SUS4, true, &(m_pInstrument->m_pFMData.Sustain[3]));
	SliderUpdate(IDC_M_FAD1, true, &(m_pInstrument->m_pFMData.Fade[0]));
	SliderUpdate(IDC_M_FAD2, true, &(m_pInstrument->m_pFMData.Fade[1]));
	SliderUpdate(IDC_M_FAD3, true, &(m_pInstrument->m_pFMData.Fade[2]));
	SliderUpdate(IDC_M_FAD4, true, &(m_pInstrument->m_pFMData.Fade[3]));
	SliderUpdate(IDC_M_REL1, true, &(m_pInstrument->m_pFMData.Release[0]));
	SliderUpdate(IDC_M_REL2, true, &(m_pInstrument->m_pFMData.Release[1]));
	SliderUpdate(IDC_M_REL3, true, &(m_pInstrument->m_pFMData.Release[2]));
	SliderUpdate(IDC_M_REL4, true, &(m_pInstrument->m_pFMData.Release[3]));
}