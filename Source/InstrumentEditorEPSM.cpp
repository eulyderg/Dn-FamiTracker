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
	/*ON_CBN_SELCHANGE(IDC_PATCH, OnCbnSelchangePatch)
	ON_BN_CLICKED(IDC_M_AM, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_M_VIB, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_M_EG, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_M_KSR2, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_M_DM, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_C_AM, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_C_VIB, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_C_EG, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_C_KSR, &CInstrumentEditorEPSM::OnBnClickedCheckbox)
	ON_BN_CLICKED(IDC_C_DM, &CInstrumentEditorEPSM::OnBnClickedCheckbox)*/
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

	SetupSlider(IDC_M_FB1, 7);
	SetSliderVal(IDC_M_FB1, 0);

	SetupSlider(IDC_M_MUL1, 15);
	SetupSlider(IDC_M_MUL2, 15);
	SetupSlider(IDC_M_MUL3, 15);
	SetupSlider(IDC_M_MUL4, 15);
	SetSliderVal(IDC_M_MUL1, 1);
	SetSliderVal(IDC_M_MUL2, 1);
	SetSliderVal(IDC_M_MUL3, 1);
	SetSliderVal(IDC_M_MUL4, 1);
	SetSliderTic(IDC_M_MUL1, 1);
	SetSliderTic(IDC_M_MUL2, 1);
	SetSliderTic(IDC_M_MUL3, 1);
	SetSliderTic(IDC_M_MUL4, 1);

	SetupSlider(IDC_M_VOL1, 127);
	SetupSlider(IDC_M_VOL2, 127);
	SetupSlider(IDC_M_VOL3, 127);
	SetupSlider(IDC_M_VOL4, 127);
	SetSliderVal(IDC_M_VOL1, 127);
	SetSliderVal(IDC_M_VOL2, 127);
	SetSliderVal(IDC_M_VOL3, 127);
	SetSliderVal(IDC_M_VOL4, 127);

	SetupSlider(IDC_M_DET1, 6);
	SetupSlider(IDC_M_DET2, 6);
	SetupSlider(IDC_M_DET3, 6);
	SetupSlider(IDC_M_DET4, 6);
	SetSliderVal(IDC_M_DET1, 3);
	SetSliderVal(IDC_M_DET2, 3);
	SetSliderVal(IDC_M_DET3, 3);
	SetSliderVal(IDC_M_DET4, 3);
	SetSliderTic(IDC_M_DET1, 3);
	SetSliderTic(IDC_M_DET2, 3);
	SetSliderTic(IDC_M_DET3, 3);
	SetSliderTic(IDC_M_DET4, 3);

	SetupSlider(IDC_M_ATT1, 31);
	SetupSlider(IDC_M_ATT2, 31);
	SetupSlider(IDC_M_ATT3, 31);
	SetupSlider(IDC_M_ATT4, 31);
	SetSliderVal(IDC_M_ATT1, 31); // 0
	SetSliderVal(IDC_M_ATT2, 31); // 0
	SetSliderVal(IDC_M_ATT3, 31); // 0
	SetSliderVal(IDC_M_ATT4, 31); // 0
	SetupSlider(IDC_M_DEC1, 31);
	SetupSlider(IDC_M_DEC2, 31);
	SetupSlider(IDC_M_DEC3, 31);
	SetupSlider(IDC_M_DEC4, 31);
	SetSliderVal(IDC_M_DEC1, 31); // 0
	SetSliderVal(IDC_M_DEC2, 31); // 0
	SetSliderVal(IDC_M_DEC3, 31); // 0
	SetSliderVal(IDC_M_DEC4, 31); // 0
	SetupSlider(IDC_M_SUS1, 15);
	SetupSlider(IDC_M_SUS2, 15);
	SetupSlider(IDC_M_SUS3, 15);
	SetupSlider(IDC_M_SUS4, 15);
	SetSliderVal(IDC_M_SUS1, 0); // 15
	SetSliderVal(IDC_M_SUS2, 0); // 15
	SetSliderVal(IDC_M_SUS3, 0); // 15
	SetSliderVal(IDC_M_SUS4, 0); // 15
	SetupSlider(IDC_M_FAD1, 31);
	SetupSlider(IDC_M_FAD2, 31);
	SetupSlider(IDC_M_FAD3, 31);
	SetupSlider(IDC_M_FAD4, 31);
	SetSliderVal(IDC_M_FAD1, 0); // 31
	SetSliderVal(IDC_M_FAD2, 0); // 31
	SetSliderVal(IDC_M_FAD3, 0); // 31
	SetSliderVal(IDC_M_FAD4, 0); // 31
	SetupSlider(IDC_M_REL1, 15);
	SetupSlider(IDC_M_REL2, 15);
	SetupSlider(IDC_M_REL3, 15);
	SetupSlider(IDC_M_REL4, 15);
	SetSliderVal(IDC_M_REL1, 15); // 0
	SetSliderVal(IDC_M_REL2, 15); // 0
	SetSliderVal(IDC_M_REL3, 15); // 0
	SetSliderVal(IDC_M_REL4, 15); // 0

	return TRUE;
}

void CInstrumentEditorEPSM::SelectInstrument(std::shared_ptr<CInstrument> pInst)
{
	m_pInstrument = std::dynamic_pointer_cast<CInstrumentEPSM>(pInst);
	ASSERT(m_pInstrument);
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

int CInstrumentEditorEPSM::GetSliderVal(int Slider)
{
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	return pSlider->GetPos();
}

void CInstrumentEditorEPSM::SetSliderVal(int Slider, int Value)
{
	CSliderCtrl *pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	pSlider->SetPos(Value);
}

void CInstrumentEditorEPSM::SetSliderTic(int Slider, int Tic)
{
	CSliderCtrl* pSlider = static_cast<CSliderCtrl*>(GetDlgItem(Slider));
	pSlider->SetTic(Tic);
}

void CInstrumentEditorEPSM::LoadPatch(int Num)
{
	
}

void CInstrumentEditorEPSM::SavePatch()
{
	
}

void CInstrumentEditorEPSM::PatchBytesToText(int Patch)
{
	
}

void CInstrumentEditorEPSM::OnBnClickedCheckbox()
{
	//SavePatch();
	SetFocus();
}

void CInstrumentEditorEPSM::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//SavePatch();
	SetFocus();
	CInstrumentEditPanel::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentEditorEPSM::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//SavePatch();
	SetFocus();
	CInstrumentEditPanel::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CInstrumentEditorEPSM::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	/*CMenu menu;
	
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, 1, _T("&Copy"));
	menu.AppendMenu(MF_STRING, 2, _T("Copy as Plain &Text"));		// // //
	menu.AppendMenu(MF_STRING, 3, _T("&Paste"));

	switch (menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, point.x, point.y, this)) {
		case 1: // Copy
			OnCopy();
			break;
		case 2: // // //
			CopyAsPlainText();
			break;
		case 3: // Paste
			OnPaste();
			break;
	}*/
}

void CInstrumentEditorEPSM::OnCopy()
{
	/*CString MML;

	int patch = m_pInstrument->GetPatch();
	// Assemble a MML string
	for (int i = 0; i < 8; ++i)
		MML.AppendFormat(_T("$%02X "), FetchPatchByte(patch, i));
	
	CClipboard Clipboard(this, CF_TEXT);

	if (!Clipboard.IsOpened()) {
		AfxMessageBox(IDS_CLIPBOARD_OPEN_ERROR);
		return;
	}

	Clipboard.SetDataPointer(MML.GetBuffer(), MML.GetLength() + 1);*/
}

void CInstrumentEditorEPSM::CopyAsPlainText()		// // //
{
	/*int patch = m_pInstrument->GetPatch();
	unsigned char reg[8] = {};
	for (int i = 0; i < 8; ++i)
		reg[i] = FetchPatchByte(patch, i);//patch == 0 ? m_pInstrument->GetCustomReg(i) : DEFAULT_PATCHES[PatchBank][patch * 8 + i];
	
	CString MML;
	GetDlgItemTextA(IDC_PATCH, MML);
	MML.Format(_T(";%s\r\n;%s\r\n"), MML, m_pInstrument->GetName());
	MML.AppendFormat(_T(";TL FB\r\n %2d,%2d,\r\n;AR DR SL RR KL MT AM VB EG KR DT\r\n"), reg[2] & 0x3F, reg[3] & 0x07);
	for (int i = 0; i <= 1; i++)
		MML.AppendFormat(_T(" %2d,%2d,%2d,%2d,%2d,%2d,%2d,%2d,%2d,%2d,%2d,\r\n"),
			(reg[4 + i] >> 4) & 0x0F, reg[4 + i] & 0x0F, (reg[6 + i] >> 4) & 0x0F, reg[6 + i] & 0x0F,
			(reg[2 + i] >> 6) & 0x03, reg[i] & 0x0F,
			(reg[i] >> 7) & 0x01, (reg[i] >> 6) & 0x01, (reg[i] >> 5) & 0x01, (reg[i] >> 4) & 0x01,
			(reg[3] >> (4 - i)) & 0x01);
	
	CClipboard Clipboard(this, CF_TEXT);

	if (!Clipboard.IsOpened()) {
		AfxMessageBox(IDS_CLIPBOARD_OPEN_ERROR);
		return;
	}

	Clipboard.SetDataPointer(MML.GetBuffer(), MML.GetLength() + 1);*/
}

uint8_t CInstrumentEditorEPSM::FetchPatchByte(int patch, unsigned char patch_byte)
{
	return 0;
	//if (patch > CUSTOM_PATCH)
	//	return OPLLPatchBytes[(8 * patch) + patch_byte];
	//else
		//return m_pInstrument->GetCustomReg(patch_byte);
}

void CInstrumentEditorEPSM::OnPaste()
{
	/*// Copy from clipboard
	CClipboard Clipboard(this, CF_TEXT);

	if (!Clipboard.IsOpened()) {
		AfxMessageBox(IDS_CLIPBOARD_OPEN_ERROR);
		return;
	}

	if (Clipboard.IsDataAvailable()) {
		LPCTSTR text = (LPCTSTR)Clipboard.GetDataPointer();
		if (text != NULL)
			PatchTextToBytes(text);
	}*/
}

void CInstrumentEditorEPSM::PatchTextToBytes(LPCTSTR pString)
{
	/*std::string str(pString);

	// Convert to register values
	std::istringstream values(str);
	std::istream_iterator<std::string> begin(values);
	std::istream_iterator<std::string> end;

	for (int i = 0; (i < 8) && (begin != end); ++i) {
		int value = CSequenceInstrumentEditPanel::ReadStringValue(*begin++, false);		// // //
		if (value < 0) value = 0;
		if (value > 0xFF) value = 0xFF;
		m_pInstrument->SetCustomReg(i, value);
	}

	int Patch = m_pInstrument->GetPatch();
	if (Patch != CUSTOM_PATCH) {
		SelectPatch(CUSTOM_PATCH);
		static_cast<CComboBox*>(GetDlgItem(IDC_PATCH))->SetCurSel(0);
		LoadPatch(CUSTOM_PATCH);
	}
	else
		LoadPatch(CUSTOM_PATCH); */
}

BOOL CInstrumentEditorEPSM::PreTranslateMessage(MSG* pMsg)
{
	/*int Patch = m_pInstrument->GetPatch();

	if (pMsg->message == WM_KEYDOWN && m_pInstrument != NULL) {
		if (GetFocus() == GetDlgItem(IDC_PATCH_TEXT)) {
			if (pMsg->wParam == VK_RETURN) {
				CString patchtxt;
				CWnd* pPatchText = GetDlgItem(IDC_PATCH_TEXT);
				pPatchText->GetWindowText(patchtxt);
				if (Patch != CUSTOM_PATCH) {
					SelectPatch(CUSTOM_PATCH);
					static_cast<CComboBox*>(GetDlgItem(IDC_PATCH))->SetCurSel(0);
					PatchTextToBytes(patchtxt);
				}
				else
					PatchTextToBytes(patchtxt);
			}
		}
		else if (GetFocus() != GetDlgItem(IDC_PATCH)) {
			switch (pMsg->wParam) {
			case VK_DOWN:
				if (Patch < 15) {
					SelectPatch(Patch + 1);
					static_cast<CComboBox*>(GetDlgItem(IDC_PATCH))->SetCurSel(Patch + 1);
				}
				break;
			case VK_UP:
				if (Patch > CUSTOM_PATCH) {
					SelectPatch(Patch - 1);
					static_cast<CComboBox*>(GetDlgItem(IDC_PATCH))->SetCurSel(Patch - 1);
				}
				break;
			}
		}
	}
	*/
	return CInstrumentEditPanel::PreTranslateMessage(pMsg);
}
