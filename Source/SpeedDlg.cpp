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

#include "stdafx.h"
#include "../resource.h"
#include "SpeedDlg.h"

const int RATE_MIN = 16;		// // //
const int RATE_MAX = 400;

// CSpeedDlg dialog

IMPLEMENT_DYNAMIC(CSpeedDlg, CDialog)
CSpeedDlg::CSpeedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpeedDlg::IDD, pParent), m_iSpeed(0)
{
}

CSpeedDlg::~CSpeedDlg()
{
}

void CSpeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpeedDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SPEED_SLD, &CSpeedDlg::OnNMCustomdrawSpeedSld)
	ON_EN_CHANGE(IDC_SPEED_EDIT, &CSpeedDlg::OnEnChangeSpeedEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPEED_SPIN, &CSpeedDlg::OnDeltaposSpeedSpin)
END_MESSAGE_MAP()


// CSpeedDlg message handlers


int CSpeedDlg::GetSpeedFromDlg(int InitialSpeed)
{
	m_iSpeed = InitialSpeed;
	CDialog::DoModal();
	return m_iSpeed;
}

void CSpeedDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	m_iSpeed = ((CSliderCtrl*)pScrollBar)->GetPos();

	CSpinButtonCtrl* Spin = static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPEED_SPIN));
	Spin->SetPos(m_iSpeed);

	CString str;
	str.Format(_T("%i"), m_iSpeed);
	SetDlgItemText(IDC_SPEED_EDIT, str);
}

BOOL CSpeedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl* Slider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SPEED_SLD));
	CEditView* Edit = static_cast<CEditView*>(GetDlgItem(IDC_SPEED_EDIT));
	CSpinButtonCtrl* Spin = static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPEED_SPIN));

	// Playing at FPS < 0.5*RATE_MIN will overflow blip_buffer.
	Slider->SetRange(RATE_MIN, RATE_MAX);
	Spin->SetRange(RATE_MIN, RATE_MAX);

	Slider->SetPos(m_iSpeed);
	Spin->SetPos(m_iSpeed);

	CString str;
	str.Format(_T("%i"), m_iSpeed);
	SetDlgItemText(IDC_SPEED_EDIT, str);

	m_bInitialized = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpeedDlg::OnBnClickedCancel()
{
	m_iSpeed = 0;
	OnCancel();
}


void CSpeedDlg::OnNMCustomdrawSpeedSld(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CSpeedDlg::OnEnChangeSpeedEdit()
{
	if (m_bInitialized) {
		CString str;
		GetDlgItemText(IDC_SPEED_EDIT, str);
		int pos = _ttoi(str);

		if (pos < RATE_MIN) pos = RATE_MIN;
		if (pos > RATE_MAX) pos = RATE_MAX;

		CSliderCtrl* Slider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SPEED_SLD));
		Slider->SetPos(pos);

		m_iSpeed = pos;
	}
}

void CSpeedDlg::OnDeltaposSpeedSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString str;
	GetDlgItemText(IDC_SPEED_SPIN, str);

	int speed = _ttoi(str);
	int newspeed = speed - ((NMUPDOWN*)pNMHDR)->iDelta;

	str.Format(_T("%i"), newspeed);
	SetDlgItemText(IDC_SPEED_SPIN, str);
}