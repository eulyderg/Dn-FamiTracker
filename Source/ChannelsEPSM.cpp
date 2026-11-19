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

// This file handles playing of EPSM channels

#include "stdafx.h"
#include "FamiTrackerTypes.h"
#include "Instrument.h"
#include "ChannelHandler.h"
#include "ChannelsEPSM.h"
#include "ChannelsS5B.h"
#include "InstHandlerEPSM.h"
#include "SeqInstHandlerS5B.h"
#include "Sequence.h"

#include <map>

CChannelHandlerEPSM::CChannelHandlerEPSM(int MaxPeriod, int MaxVolume) :
	FrequencyChannelHandler(MaxPeriod, MaxVolume)
{
	m_iVolume = VOL_COLUMN_MAX;
	m_iChannel = CHANID_EPSM_SSG1;
}

void CChannelHandlerEPSM::SetChannelID(int ID)
{
	CChannelHandler::SetChannelID(ID);
	m_iChannel = ID - CHANID_EPSM_SSG1;
}

void CChannelHandlerEPSM::RegWrite(uint8_t Reg, uint8_t Value)
{
	WriteRegister(0x9000 + Reg, Value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPSM SSG Channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: share code with 5B?
uint8_t CEPSMSSGChannel::s_iModes        = 0;
uint8_t CEPSMSSGChannel::s_iNoiseFreq    = 0;
uint8_t CEPSMSSGChannel::s_iNoisePrev    = -1;
uint8_t CEPSMSSGChannel::s_iDefaultNoise = 0;
uint8_t CEPSMSSGChannel::s_iEnvFreqHi    = 0;
uint8_t CEPSMSSGChannel::s_iEnvFreqLo    = 0;
bool    CEPSMSSGChannel::s_bEnvTrigger   = false;
uint8_t CEPSMSSGChannel::s_iEnvType      = 0;

// Class functions
CEPSMSSGChannel::CEPSMSSGChannel() :
	CChannelHandlerEPSM(0xFFF, 0xF)
{
	m_bUpdate            = false;
	m_bEnvelopeEnabled   = false;
	m_iAutoEnvelopeShift = 0;
	m_iDefaultDuty       = S5B_MODE_SQUARE;
	s_iDefaultNoise      = 0;
}

void CEPSMSSGChannel::RefreshChannel()
{
	uint16_t Period = CalculatePeriod();
	uint8_t LoPeriod = Period & 0xFF;
	uint8_t HiPeriod = Period >> 8;
	uint8_t Volume = CalculateVolume();

	uint8_t Noise = (m_bGate && (m_iDutyPeriod & S5B_MODE_NOISE)) ? 0 : 1;
	uint8_t Square = (m_bGate && (m_iDutyPeriod & S5B_MODE_SQUARE)) ? 0 : 1;
	uint8_t Envelope = (m_bGate && (m_iDutyPeriod & S5B_MODE_ENVELOPE)) ? 0x10 : 0;

	UpdateAutoEnvelope(Period);
	SetMode(m_iChannelID, Square, Noise);

	WriteReg((m_iChannelID - CHANID_EPSM_SSG1) * 2, LoPeriod);
	WriteReg((m_iChannelID - CHANID_EPSM_SSG1) * 2 + 1, HiPeriod);
	WriteReg((m_iChannelID - CHANID_EPSM_SSG1) + 8, Volume | Envelope);

	if (Envelope && (m_bTrigger || m_bUpdate))
		s_bEnvTrigger = true;
	m_bUpdate = false;

	if (m_iChannelID == CHANID_EPSM_SSG3)
		UpdateRegs();
}

void CEPSMSSGChannel::ResetChannel()
{
	CChannelHandler::ResetChannel();
	m_iDefaultDuty = m_iDutyPeriod = S5B_MODE_SQUARE;
	s_iDefaultNoise = s_iNoiseFreq = 0;
	s_iNoisePrev = -1;
	m_bEnvelopeEnabled = false;
	m_iAutoEnvelopeShift = 0;
	s_iEnvFreqHi = 0;
	s_iEnvFreqLo = 0;
	s_iEnvType = 0;
	s_bEnvTrigger = false;
}

int CEPSMSSGChannel::getDutyMax() const
{
	return 0x07;
}

inst_type_t CEPSMSSGChannel::GetNewInstType() const
{
	return INST_S5B;
}

void CEPSMSSGChannel::ClearRegisters()
{
	WriteReg(8 + m_iChannelID - CHANID_EPSM_SSG1, 0);		// Clear volume
}

void CEPSMSSGChannel::HandleNoteData(stChanNote* pNoteData, int EffColumns)
{
	CChannelHandler::HandleNoteData(pNoteData, EffColumns);
	if (m_iDutyPeriod & S5B_MODE_NOISE) {
		s_iNoiseFreq = s_iDefaultNoise;
	}
}

using EffParamT = unsigned char;
static const std::map<EffParamT, s5b_mode_t> VXX_TO_DUTY = {
	{1 << 0, S5B_MODE_SQUARE},
	{1 << 1, S5B_MODE_NOISE},
	{1 << 2, S5B_MODE_ENVELOPE},
};

bool CEPSMSSGChannel::HandleEffect(effect_t EffNum, uint8_t EffParam)
{
	switch (EffNum) {
	case EF_SUNSOFT_NOISE: // W
		s_iDefaultNoise = s_iNoiseFreq = EffParam & 0x1F;
		return true;
	case EF_SUNSOFT_ENV_HI: // I
		s_iEnvFreqHi = EffParam;
		return true;
	case EF_SUNSOFT_ENV_LO: // J
		s_iEnvFreqLo = EffParam;
		return true;
	case EF_SUNSOFT_ENV_TYPE: // H
		s_bEnvTrigger = true;
		s_iEnvType = EffParam & 0x0F;
		m_bUpdate = true;
		m_bEnvelopeEnabled = EffParam != 0;
		m_iAutoEnvelopeShift = EffParam >> 4;
		return true;
	case EF_DUTY_CYCLE: {
		uint8_t duty = 0;
		for (auto const& [VXX, DUTY] : VXX_TO_DUTY) {
			if (EffParam & VXX) {
				duty |= DUTY;
			}
		}
		m_iDefaultDuty = m_iDutyPeriod = duty;
		return true;
	}
	default: return CChannelHandler::HandleEffect(EffNum, EffParam);
	}
}

void CEPSMSSGChannel::HandleEmptyNote()
{

}

void CEPSMSSGChannel::HandleCut()
{
	CutNote();
	m_iDutyPeriod = S5B_MODE_SQUARE;
	m_iNote = 0;
}

void CEPSMSSGChannel::HandleRelease()
{
	if (!m_bRelease)
		ReleaseNote();	
}

void CEPSMSSGChannel::HandleNote(int Note, int Octave)
{
	CChannelHandler::HandleNote(Note, Octave);

	if (this->m_iDefaultDuty & S5B_MODE_NOISE) {
		s_iNoiseFreq = s_iDefaultNoise;
	}
}

int CEPSMSSGChannel::RunNote(int Octave, int Note)
{
	return CChannelHandler::RunNote(Octave, Note);
}

bool CEPSMSSGChannel::CreateInstHandler(inst_type_t Type)
{
	switch (Type) {
	case INST_2A03: case INST_VRC6: case INST_N163: case INST_S5B: case INST_FDS:
		switch (m_iInstTypeCurrent) {
		case INST_2A03: case INST_VRC6: case INST_N163: case INST_S5B: case INST_FDS: break;
		default:
			m_pInstHandler.reset(new CSeqInstHandlerS5B(this, 0x0F, Type == INST_S5B ? 0x40 : 0));
			return true;
		}
	}
	return false;
}

int CEPSMSSGChannel::CalculateVolume() const
{
	return LimitVolume((m_iVolume >> VOL_COLUMN_SHIFT) + m_iInstVolume - 15 - GetTremolo());
}

int CEPSMSSGChannel::ConvertDuty(int Duty) const
{
	switch (m_iInstTypeCurrent) {
	case INST_2A03: case INST_VRC6: case INST_N163:
		return S5B_MODE_SQUARE;
	default:
		return Duty;
	}
}

void CEPSMSSGChannel::SetMode(int Chan, int Square, int Noise)
{
	Chan -= CHANID_EPSM_SSG1;

	switch (Chan) {
	case 0:
		s_iModes &= 0b110110;
		break;
	case 1:
		s_iModes &= 0b101101;
		break;
	case 2:
		s_iModes &= 0b011011;
		break;
	}

	s_iModes |= (Noise << (3 + Chan)) | (Square << Chan);
}

void CEPSMSSGChannel::UpdateAutoEnvelope(int Period)		// // // 050B
{
	if (m_bEnvelopeEnabled && m_iAutoEnvelopeShift) {
		if (m_iAutoEnvelopeShift > 8) {
			Period >>= m_iAutoEnvelopeShift - 8 - 1;		// // // round off
			if (Period & 0x01) ++Period;
			Period >>= 1;
		}
		else if (m_iAutoEnvelopeShift < 8)
			Period <<= 8 - m_iAutoEnvelopeShift;
		s_iEnvFreqLo = Period & 0xFF;
		s_iEnvFreqHi = Period >> 8;
	}
}

void CEPSMSSGChannel::UpdateRegs()
{
	// Done only once
	if (s_iNoiseFreq != s_iNoisePrev)
		WriteReg(0x06, (s_iNoisePrev = s_iNoiseFreq) ^ 0x1F);
	WriteReg(0x07, s_iModes);
	WriteReg(0x0B, s_iEnvFreqLo);
	WriteReg(0x0C, s_iEnvFreqHi);
	if (s_bEnvTrigger)
		WriteReg(0x0D, s_iEnvType);
	s_bEnvTrigger = false;
}

void CEPSMSSGChannel::WriteReg(uint8_t Reg, uint8_t Value)
{
	WriteRegister(0x401C, Reg);
	WriteRegister(0x401D, Value);
}

void CEPSMSSGChannel::SetNoiseFreq(int Pitch)
{
	s_iNoiseFreq = Pitch;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPSM FM Channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t CEPSMFMChannel::m_iLFOFreq = 0;
uint8_t CEPSMFMChannel::m_iGates = 0;

CEPSMFMChannel::CEPSMFMChannel() :
	CChannelHandlerEPSM(0x3FFF, 0x7F)
{
	ResetChannel();
}

void CEPSMFMChannel::RefreshChannel()
{
	int Volume = CalculateVolume();
	int Fnum = CalculatePeriod();
	int Bnum = !m_bLinearPitch ? m_iOctave :
		((GetPeriod() + GetVibrato() - GetFinePitch() - GetPitch()) >> LINEAR_PITCH_AMOUNT) / NOTE_RANGE;

	// Write custom instrument
	if (m_iDutyPeriod == 0 && m_iCommand == CMD_NOTE_TRIGGER) {
		WritePatch();
	}

	if (!m_bGate) {
		m_iCommand = CMD_NOTE_HALT;
	}

	uint16_t A0 = (m_iChannelIndex >= 3) ? 0x100 : 0x000;
	uint16_t offset = A0 + m_iChannelIndex % 3;
	uint8_t bit = 1 << (m_iChannelIndex % 3);

	switch (m_iCommand) {
	case CMD_NOTE_TRIGGER:
		m_iCommand = CMD_NOTE_ON;
		m_iGates |= bit;
		WriteReg(0x28 + A0, bit);
		break;
	case CMD_NOTE_ON:
		//Cmd = m_bHold ? OPL_NOTE_ON : OPL_SUSTAIN_ON;
		break;
	case CMD_NOTE_HALT:
		m_iGates &= ~bit;
		WriteReg(0x28 + A0, bit);
		break;
	case CMD_NOTE_RELEASE:
		m_iGates &= ~bit;
		WriteReg(0x28 + A0, bit);
		break;
	}

	// Write frequency
	//WriteReg(0x10 + m_iChannel, Fnum & 0xFF);

	if (m_iChannelIndex == 5) {
		WriteReg(0x22, m_iLFOFreq);
	}
}

void CEPSMFMChannel::ResetChannel()
{
	m_iCommand = CMD_NONE;
	m_iChannelIndex = m_iChannelID - CHANID_EPSM_FM1;

	m_iGates = 0;

	m_iOctave = 0;
	m_iOldOctave = 0;

	m_bHold = false;

	ResetFMData();
}

void CEPSMFMChannel::ResetFMData()
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

void CEPSMFMChannel::SetFMData(epsm_fm_data FMData)
{
	m_pFMData = FMData;
}

int CEPSMFMChannel::getDutyMax() const
{
	return 0x00;
}

inst_type_t CEPSMFMChannel::GetNewInstType() const
{
	return INST_EPSM;
}

void CEPSMFMChannel::ClearRegisters()
{
	m_iNote = 0;
	m_iOctave = 0;
	m_iOldOctave = 0;
	m_iEffect = EF_NONE;

	m_iCommand = CMD_NOTE_HALT;

	ResetFMData();

	uint16_t A0 = (m_iChannelIndex >= 3) ? 0x100 : 0x000;
	uint16_t offset = A0 + m_iChannelIndex % 3;

	for (uint16_t address = 0x30; address < 0xA0; address++) {
		WriteReg(address + offset, 0);
	}

	WriteReg(0xB0 + offset, 0);
}

void CEPSMFMChannel::HandleNoteData(stChanNote* pNoteData, int EffColumns)
{
	FrequencyChannelHandler::HandleNoteData(pNoteData, EffColumns);

	if (m_iCommand == CMD_NOTE_TRIGGER && pNoteData->Instrument == HOLD_INSTRUMENT) {
		m_iCommand = CMD_NOTE_ON;
	}
}

bool CEPSMFMChannel::HandleEffect(effect_t EffNum, uint8_t EffParam)
{
	return FrequencyChannelHandler::HandleEffect(EffNum, EffParam);
}

void CEPSMFMChannel::HandleEmptyNote()
{
	
}

void CEPSMFMChannel::HandleCut()
{
	RegisterKeyState(-1);
	m_bGate = false;
	m_iCommand = CMD_NOTE_HALT;
}

void CEPSMFMChannel::UpdateNoteRelease()
{
	// Note release (Lxx)
	if (m_iNoteRelease > 0) {
		m_iNoteRelease--;
		if (m_iNoteRelease == 0) {
			HandleRelease();
		}
	}
}

void CEPSMFMChannel::HandleRelease()
{
	if (!m_bRelease) {
		m_iCommand = CMD_NOTE_RELEASE;
		RegisterKeyState(-1);
	}
}


void CEPSMFMChannel::HandleNote(int Note, int Octave)
{
	CChannelHandler::HandleNote(Note, Octave);

	m_bHold = true;

	if (m_iPortaSpeed > 0 && m_iEffect == EF_PORTAMENTO &&
		m_iCommand != CMD_NOTE_HALT && m_iCommand != CMD_NOTE_RELEASE)		// // // 050B
		CorrectOctave();
	else
		m_iCommand = CMD_NOTE_TRIGGER;
}

int CEPSMFMChannel::RunNote(int Octave, int Note)
{
	return CChannelHandler::RunNote(Octave, Note);
}

bool CEPSMFMChannel::CreateInstHandler(inst_type_t Type)
{
	return CChannelHandler::CreateInstHandler(Type);
}

// TODO
int CEPSMFMChannel::CalculateVolume() const
{
	return 0;
}

// TODO
int CEPSMFMChannel::CalculatePeriod(bool MultiplyByHarmonic) const
{
	return 1;
}

void CEPSMFMChannel::SetupSlide()
{
	CChannelHandler::SetupSlide();
	CorrectOctave();
}

void CEPSMFMChannel::CorrectOctave()
{
	// Set current frequency to the one with highest octave
	if (m_bLinearPitch)
		return;

	if (m_iOldOctave == -1) {
		m_iOldOctave = m_iOctave;
		return;
	}

	int Offset = m_iOctave - m_iOldOctave;
	if (Offset > 0) {
		m_iPeriod >>= Offset;
		m_iOldOctave = m_iOctave;
	}
	else if (Offset < 0) {
		// Do nothing
		m_iPortaTo >>= -Offset;
		m_iOctave = m_iOldOctave;
	}
}

/*int CEPSMFMChannel::TriggerNote(int Note)
{
	m_iTriggeredNote = Note;
	RegisterKeyState(Note);
	if (m_iCommand != CMD_NOTE_TRIGGER && m_iCommand != CMD_NOTE_HALT)
		m_iCommand = CMD_NOTE_ON;
	m_iOctave = Note / NOTE_RANGE;

	return m_bLinearPitch ? (Note << LINEAR_PITCH_AMOUNT) : GetFnum(Note);		// // //
}*/

void CEPSMFMChannel::WritePatch()
{
	uint16_t A0 = (m_iChannelIndex >= 3) ? 0x100 : 0x000;
	uint16_t offset = A0 + m_iChannelIndex % 3; 

	WriteReg(0x30 + offset, m_pFMData.Detune[0] << 4 | m_pFMData.Mult[0]);
	WriteReg(0x34 + offset, m_pFMData.Detune[1] << 4 | m_pFMData.Mult[1]);
	WriteReg(0x38 + offset, m_pFMData.Detune[2] << 4 | m_pFMData.Mult[2]);
	WriteReg(0x3C + offset, m_pFMData.Detune[3] << 4 | m_pFMData.Mult[3]);

	WriteReg(0x40 + offset, m_pFMData.TotalLevel[0]);
	WriteReg(0x44 + offset, m_pFMData.TotalLevel[1]);
	WriteReg(0x48 + offset, m_pFMData.TotalLevel[2]);
	WriteReg(0x4C + offset, m_pFMData.TotalLevel[3]);

	WriteReg(0x50 + offset, m_pFMData.KeyScale[0] << 6 | m_pFMData.Attack[0]);
	WriteReg(0x54 + offset, m_pFMData.KeyScale[1] << 6 | m_pFMData.Attack[1]);
	WriteReg(0x58 + offset, m_pFMData.KeyScale[2] << 6 | m_pFMData.Attack[2]);
	WriteReg(0x5C + offset, m_pFMData.KeyScale[3] << 6 | m_pFMData.Attack[3]);

	WriteReg(0x60 + offset, m_pFMData.Decay[0]);
	WriteReg(0x64 + offset, m_pFMData.Decay[1]);
	WriteReg(0x68 + offset, m_pFMData.Decay[2]);
	WriteReg(0x6C + offset, m_pFMData.Decay[3]);

	WriteReg(0x70 + offset, m_pFMData.Fade[0]);
	WriteReg(0x74 + offset, m_pFMData.Fade[1]);
	WriteReg(0x78 + offset, m_pFMData.Fade[2]);
	WriteReg(0x7C + offset, m_pFMData.Fade[3]);

	WriteReg(0x80 + offset, m_pFMData.Sustain[0] << 4 | m_pFMData.Release[0]);
	WriteReg(0x84 + offset, m_pFMData.Sustain[1] << 4 | m_pFMData.Release[1]);
	WriteReg(0x88 + offset, m_pFMData.Sustain[2] << 4 | m_pFMData.Release[2]);
	WriteReg(0x8C + offset, m_pFMData.Sustain[3] << 4 | m_pFMData.Release[3]);

	WriteReg(0x90 + offset, m_pFMData.Fade[0]);
	WriteReg(0x94 + offset, m_pFMData.Fade[1]);
	WriteReg(0x98 + offset, m_pFMData.Fade[2]);
	WriteReg(0x9C + offset, m_pFMData.Fade[3]);

	WriteReg(0xB0 + offset, m_pFMData.Feedback);
}

void CEPSMFMChannel::WriteReg(uint16_t Reg, uint8_t Value)
{
	if ((Reg & 0x100) == 0) {
		WriteRegister(0x401C, Reg & 0xFF);
		WriteRegister(0x401D, Value);
	} else {
		WriteRegister(0x401E, Reg & 0xFF);
		WriteRegister(0x401F, Value);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPSM Sample Channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CEPSMSampleChannel::CEPSMSampleChannel() :
	CChannelHandlerEPSM(0x1, 0xF)
{
	
}

void CEPSMSampleChannel::RefreshChannel()
{
	
}

void CEPSMSampleChannel::ResetChannel()
{
	
}

int CEPSMSampleChannel::getDutyMax() const
{
	return 0x00;
}

inst_type_t CEPSMSampleChannel::GetNewInstType() const
{
	return INST_NONE;
}

void CEPSMSampleChannel::ClearRegisters()
{
	
}

void CEPSMSampleChannel::HandleNoteData(stChanNote* pNoteData, int EffColumns)
{
	CChannelHandler::HandleNoteData(pNoteData, EffColumns);		// // //
}

bool CEPSMSampleChannel::HandleEffect(effect_t EffNum, uint8_t EffParam)
{
	return CChannelHandler::HandleEffect(EffNum, EffParam);
}

void CEPSMSampleChannel::HandleEmptyNote()
{

}

void CEPSMSampleChannel::HandleCut()
{

}

void CEPSMSampleChannel::HandleRelease()
{

}

void CEPSMSampleChannel::HandleNote(int Note, int Octave)
{

}

int CEPSMSampleChannel::RunNote(int Octave, int Note)
{
	return CChannelHandler::RunNote(Octave, Note);
}

bool CEPSMSampleChannel::CreateInstHandler(inst_type_t Type)
{
	return CChannelHandler::CreateInstHandler(Type);
}

int CEPSMSampleChannel::CalculateVolume() const
{
	return 0;
}

int CEPSMSampleChannel::CalculatePeriod(bool MultiplyByHarmonic ) const
{
	return 1;
}
