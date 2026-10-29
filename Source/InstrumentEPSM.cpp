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

//static const unsigned char VRC7_SINE_PATCH[] = {0x01, 0x21, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x0F};		// // //

CInstrumentEPSM::CInstrumentEPSM() : CInstrument(INST_EPSM), m_iPatch()
{
	//memcpy(m_iRegs, VRC7_SINE_PATCH, sizeof(VRC7_SINE_PATCH));
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
	
	/*if (auto pNew = dynamic_cast<const CInstrumentEPSM*>(pInst)) {
		SetPatch(pNew->GetPatch());
		for (int i = 0; i < 8; ++i)
			SetCustomReg(i, pNew->GetCustomReg(i));
	}*/
}

void CInstrumentEPSM::Setup() { }

void CInstrumentEPSM::Store(CDocumentFile *pDocFile)
{
	//pDocFile->WriteBlockInt(m_iPatch);
	//
	//for (int i = 0; i < 8; ++i)
	//	pDocFile->WriteBlockChar(GetCustomReg(i));
}

bool CInstrumentEPSM::Load(CDocumentFile *pDocFile)
{
	m_iPatch = CModuleException::AssertRangeFmt(pDocFile->GetBlockInt(), 0, 0xF, "EPSM patch number", "%i");

	//for (int i = 0; i < 8; ++i)
	//	SetCustomReg(i, pDocFile->GetBlockChar());

	return true;
}

void CInstrumentEPSM::SaveFile(CInstrumentFile *pFile)
{
	pFile->WriteInt(m_iPatch);

	for (int i = 0; i < 8; ++i)
		pFile->WriteChar(GetCustomReg(i));
}

bool CInstrumentEPSM::LoadFile(CInstrumentFile *pFile, int iVersion)
{
	m_iPatch = pFile->ReadInt();

	for (int i = 0; i < 8; ++i)
		SetCustomReg(i, pFile->ReadChar());

	return true;
}

int CInstrumentEPSM::Compile(CChunk *pChunk, int Index)
{
	/*int Patch = GetPatch();

	pChunk->StoreByte(6);		// // // CHAN_VRC7
	pChunk->StoreByte(Patch << 4);	// Shift up by 4 to make room for volume

	if (Patch == 0) {
		// Write custom patch settings
		for (int i = 0; i < 8; ++i) {
			pChunk->StoreByte(GetCustomReg(i));
		}
	}

	return (Patch == 0) ? 10 : 2;		// // //*/
	return 0;
}

bool CInstrumentEPSM::CanRelease() const
{
	return false;	// This can use release but disable it when previewing notes
}

void CInstrumentEPSM::SetPatch(unsigned int Patch)
{
	//m_iPatch = Patch;
	InstrumentChanged();
}

unsigned int CInstrumentEPSM::GetPatch() const
{
	return m_iPatch;
}

void CInstrumentEPSM::SetCustomReg(int Reg, unsigned char Value)		// // //
{
	//m_iRegs[Reg] = Value;
	InstrumentChanged();
}

unsigned char CInstrumentEPSM::GetCustomReg(int Reg) const		// // //
{
	return m_iRegs[Reg];
}
