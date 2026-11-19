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
#include "InstrumentEPSM.h"		// // //
#include "ModuleException.h"		// // //
#include "Chunk.h"
#include "DocumentFile.h"

/*
 * class InstrumentEPSM
 *
 */

CInstrumentEPSM::CInstrumentEPSM() : CInstrument(INST_EPSM), m_pFMData()
{
	for (uint8_t op = 0; op < 4; op++) {
		m_pFMData.Detune[op] = 0;
		m_pFMData.Mult[op] = 0;
		m_pFMData.KeyScale[op] = 0;
		m_pFMData.TotalLevel[op] = 0;

		m_pFMData.Attack[op] = 0;
		m_pFMData.Decay[op] = 0;
		m_pFMData.Sustain[op] = 15;
		m_pFMData.Fade[op] = 31;
		m_pFMData.Release[op] = 0;

		m_pFMData.LFO[op] = false;

		m_pFMData.SSGEG[op] = 0;
	}

	m_pFMData.Feedback = 0;
}

CInstrument *CInstrumentEPSM::Clone() const
{
	CInstrumentEPSM* inst = new CInstrumentEPSM();		// // //
	inst->CloneFrom(this);
	return inst;
}

void CInstrumentEPSM::CloneFrom(const CInstrument *pInst)
{
	CInstrument::CloneFrom(pInst);
	
	if (auto pNew = dynamic_cast<const CInstrumentEPSM*>(pInst)) {
		m_pFMData = pNew->m_pFMData;
	}
}

void CInstrumentEPSM::Setup() { }

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
	return false;	// This can use release but disable it when previewing notes
}

void CInstrumentEPSM::SetFMData(epsm_fm_data FMData)
{
	m_pFMData = FMData;
}

epsm_fm_data CInstrumentEPSM::GetFMData() const
{
	return m_pFMData;
}