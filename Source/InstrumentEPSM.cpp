/*
** FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2005-2020 Jonathan Liss
**
** 0CC-FamiTracker is (C) 2014-2018 HertzDevil
**
** Dn-FamiTracker is (C) 2020-2026 D.P.C.M.
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
#include "Instrument.h"
#include "InstrumentEPSM.h"
#include "ModuleException.h"
#include "Chunk.h"
#include "DocumentFile.h"

CInstrumentEPSM::CInstrumentEPSM() : CInstrument(INST_EPSM)
{
	ResetFMData();
}

void CInstrumentEPSM::ResetFMData()
{
	SetFMData(DEFAULT_EPSM_INST);
}

void CInstrumentEPSM::SetFMData(epsm_fm_data FMData)
{
	for (uint8_t op = 0; op < 4; op++) {
		m_pFMData.Detune[op] = FMData.Detune[op];
		m_pFMData.Mult[op] = FMData.Mult[op];
		m_pFMData.KeyScale[op] = FMData.KeyScale[op];
		m_pFMData.TotalLevel[op] = FMData.TotalLevel[op];

		m_pFMData.Attack[op] = FMData.Attack[op];
		m_pFMData.Decay[op] = FMData.Decay[op];
		m_pFMData.Sustain[op] = FMData.Sustain[op];
		m_pFMData.Fade[op] = FMData.Fade[op];
		m_pFMData.Release[op] = FMData.Release[op];

		m_pFMData.LFO[op] = FMData.LFO[op];

		m_pFMData.SSGEG[op] = FMData.SSGEG[op];
	}

	m_pFMData.Feedback = FMData.Feedback;
}

CInstrument *CInstrumentEPSM::Clone() const
{
	CInstrumentEPSM* inst = new CInstrumentEPSM();
	inst->CloneFrom(this);
	return inst;
}

void CInstrumentEPSM::CloneFrom(const CInstrument *pInst) {
	CInstrument::CloneFrom(pInst);
	
	if (auto pNew = dynamic_cast<const CInstrumentEPSM*>(pInst)) {
		//(*pNew).
	}
}

void CInstrumentEPSM::Setup() {

}

void CInstrumentEPSM::Store(CDocumentFile *pDocFile)
{
	
}

bool CInstrumentEPSM::Load(CDocumentFile *pDocFile)
{
	return true;
}

void CInstrumentEPSM::SaveFile(CInstrumentFile *pFile)
{
	
}

bool CInstrumentEPSM::LoadFile(CInstrumentFile *pFile, int iVersion)
{
	return true;
}

int CInstrumentEPSM::Compile(CChunk *pChunk, int Index)
{
	return 0;
}

bool CInstrumentEPSM::CanRelease() const
{
	return true;
}