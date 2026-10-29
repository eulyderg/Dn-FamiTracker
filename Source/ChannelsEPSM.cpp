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
#include "InstHandler.h"
#include "InstHandlerEPSM.h"
#include "Sequence.h"
#include <map>

CChannelHandlerEPSM::CChannelHandlerEPSM() :
	FrequencyChannelHandler(0x7FF, 0xF)
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

void CEPSMSSGChannel::SetMode(int Chan, int Square, int Noise)
{
	Chan -= CHANID_S5B_CH1;

	switch (Chan) {
	case 0:
		s_iModes &= 0x36;
		break;
	case 1:
		s_iModes &= 0x2D;
		break;
	case 2:
		s_iModes &= 0x1B;
		break;
	}

	s_iModes |= (Noise << (3 + Chan)) | (Square << Chan);
}

// Class functions
CEPSMSSGChannel::CEPSMSSGChannel()
{
	m_bUpdate			 = false;
	m_bEnvelopeEnabled   = false;
	m_iAutoEnvelopeShift = 0;
}

void CEPSMSSGChannel::RefreshChannel()
{
	
}

void CEPSMSSGChannel::ResetChannel()
{
	CChannelHandler::ResetChannel();
	m_iDefaultDuty = m_iDutyPeriod = S5B_MODE_SQUARE;
	s_iDefaultNoise = s_iNoiseFreq = 0;
}

int CEPSMSSGChannel::getDutyMax() const
{
	return 0x0F;
}

inst_type_t CEPSMSSGChannel::GetNewInst() const
{
	return INST_S5B;
}

epsm_channel_type CEPSMSSGChannel::GetChannelType() const
{
	return EPSM_SSG;
}

void CEPSMSSGChannel::ClearRegisters()
{

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
	
}

void CEPSMSSGChannel::HandleRelease()
{
	
}

void CEPSMSSGChannel::HandleNote(int Note, int Octave)
{
	
}

int CEPSMSSGChannel::RunNote(int Octave, int Note)
{
	return CChannelHandler::RunNote(Octave, Note);
}

bool CEPSMSSGChannel::CreateInstHandler(inst_type_t Type)
{
	return CChannelHandler::CreateInstHandler(Type);
}

int CEPSMSSGChannel::CalculateVolume() const
{
	return 0;
}

int CEPSMSSGChannel::CalculatePeriod(bool MultiplyByHarmonic) const
{
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPSM FM Channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CEPSMFMChannel::CEPSMFMChannel()
{
	m_iCommand = CMD_NONE;
}

void CEPSMFMChannel::RefreshChannel()
{
	
}

void CEPSMFMChannel::ResetChannel()
{
	
}

int CEPSMFMChannel::getDutyMax() const
{
	return 0x00;
}

inst_type_t CEPSMFMChannel::GetNewInst() const
{
	return INST_EPSM;
}

epsm_channel_type CEPSMFMChannel::GetChannelType() const
{
	return EPSM_FM;
}

void CEPSMFMChannel::ClearRegisters()
{
	
}

void CEPSMFMChannel::HandleNoteData(stChanNote* pNoteData, int EffColumns)
{
	FrequencyChannelHandler::HandleNoteData(pNoteData, EffColumns);		// // //

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

}

void CEPSMFMChannel::HandleRelease()
{

}

void CEPSMFMChannel::HandleNote(int Note, int Octave)
{

}

int CEPSMFMChannel::RunNote(int Octave, int Note)
{
	return CChannelHandler::RunNote(Octave, Note);
}

bool CEPSMFMChannel::CreateInstHandler(inst_type_t Type)
{
	return CChannelHandler::CreateInstHandler(Type);
}

int CEPSMFMChannel::CalculateVolume() const
{
	return 0;
}

int CEPSMFMChannel::CalculatePeriod(bool MultiplyByHarmonic) const
{
	return 1;
}

void CEPSMFMChannel::SetupSlide()
{
	CChannelHandler::SetupSlide();
	//CorrectOctave();
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPSM Sample Channels
///////////////////////////////////////////////////////////////////////////////////////////////////////////

CEPSMSampleChannel::CEPSMSampleChannel()
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

inst_type_t CEPSMSampleChannel::GetNewInst() const
{
	return INST_NONE;
}

epsm_channel_type CEPSMSampleChannel::GetChannelType() const
{
	return EPSM_SAMPLE;
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
