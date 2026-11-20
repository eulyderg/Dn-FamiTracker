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

#pragma once
#include "Instrument.h"

struct epsm_fm_data {
	uint8_t Feedback = 0;

	uint8_t Detune[4] = {3, 3, 3, 3};
	uint8_t Mult[4] = {1, 1, 1, 1};
	uint8_t KeyScale[4] = {0, 0, 0, 0};
	uint8_t TotalLevel[4] = {127, 127, 0, 0};

	uint8_t Attack[4] = {31, 31, 31, 31};
	uint8_t Decay[4] = {31, 31, 31, 31};
	uint8_t Sustain[4] = {0, 0, 0, 0};
	uint8_t Fade[4] = {0, 0, 0, 0};
	uint8_t Release[4] = {15, 15, 8, 12};

	uint8_t SSGEG[4] = {0, 0, 0, 0};

	bool LFO[4] = {false, false, false, false};
};

const epsm_fm_data DEFAULT_EPSM_INST;

class CInstrumentEPSM : public CInstrument {
public:
	CInstrumentEPSM();
	CInstrument* Clone() const;
	void Setup();
	void Store(CDocumentFile *pDocFile);
	bool Load(CDocumentFile *pDocFile);
	void SaveFile(CInstrumentFile *pFile);
	bool LoadFile(CInstrumentFile *pFile, int iVersion);
	int  Compile(CChunk *pChunk, int Index);
	bool CanRelease() const;

	void SetFMData(epsm_fm_data FMData);
	void ResetFMData();
	epsm_fm_data m_pFMData;

protected:
	virtual void CloneFrom(const CInstrument* pInst);
};
