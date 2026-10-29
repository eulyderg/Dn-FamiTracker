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

#include "ChannelsVRC7.h"

enum epsm_channel_type {
	EPSM_FM,
	EPSM_SSG,
	EPSM_SAMPLE
};

//
// Derived channels, EPSM
//

class CChannelHandlerInterfaceEPSM;

class CChannelHandlerEPSM : public FrequencyChannelHandler, public CChannelHandlerInterfaceEPSM {		// // //
public:
	CChannelHandlerEPSM();
	void SetChannelID(int ID) override;

protected:
	uint8_t m_iChannel;

private:
	void RegWrite(uint8_t Reg, uint8_t Value);
};

//////////////////
// SSG channels //
class CEPSMSSGChannel : public CChannelHandlerEPSM {
public:
	CEPSMSSGChannel();
	void RefreshChannel() override;
	void ResetChannel() override;
	int  getDutyMax() const override;
	inst_type_t GetNewInst() const override;
	epsm_channel_type GetChannelType() const;

protected:
	void ClearRegisters() override;
	void HandleNoteData(stChanNote* pNoteData, int EffColumns) override;
	bool HandleEffect(effect_t EffNum, uint8_t EffParam) override;
	void HandleEmptyNote() override;
	void HandleCut() override;
	void HandleRelease() override;
	void HandleNote(int Note, int Octave) override;
	int  RunNote(int Octave, int Note) override;
	bool CreateInstHandler(inst_type_t Type) override;
	int  CalculateVolume() const override;
	int  CalculatePeriod(bool MultiplyByHarmonic = true) const override;

protected:
	// copied from S5B
	static void SetMode(int Chan, int Square, int Noise);

	static uint8_t s_iModes;
	static uint8_t s_iNoiseFreq;
	static uint8_t s_iNoisePrev;
	static uint8_t s_iDefaultNoise;
	static uint8_t s_iEnvFreqHi;
	static uint8_t s_iEnvFreqLo;
	static bool    s_bEnvTrigger;
	static uint8_t s_iEnvType;
	bool    m_bUpdate;
	bool    m_bEnvelopeEnabled;
	uint8_t m_iAutoEnvelopeShift;
};

/////////////////
// FM channels //
class CEPSMFMChannel : public CChannelHandlerEPSM {
public:
	CEPSMFMChannel();
	void RefreshChannel() override;
	void ResetChannel() override;
	int  getDutyMax() const override;
	inst_type_t GetNewInst() const override;
	epsm_channel_type GetChannelType() const;

protected:
	void ClearRegisters() override;
	void HandleNoteData(stChanNote* pNoteData, int EffColumns) override;
	bool HandleEffect(effect_t EffNum, uint8_t EffParam) override;
	void HandleEmptyNote() override;
	void HandleCut() override;
	void HandleRelease() override;
	void HandleNote(int Note, int Octave) override;
	int  RunNote(int Octave, int Note) override;
	bool CreateInstHandler(inst_type_t Type) override;
	int  CalculateVolume() const override;
	int  CalculatePeriod(bool MultiplyByHarmonic = true) const override;

	void SetupSlide() override;
	//int  TriggerNote(int Note) override;

protected:

	// copied from VRC7
	note_command_t m_iCommand;
};

////////////////////
// ADPCM channels //
class CEPSMSampleChannel : public CChannelHandlerEPSM {
public:
	CEPSMSampleChannel();
	void RefreshChannel() override;
	void ResetChannel() override;
	int  getDutyMax() const override;
	inst_type_t GetNewInst() const override;
	epsm_channel_type GetChannelType() const;

protected:
	void ClearRegisters() override;
	void HandleNoteData(stChanNote* pNoteData, int EffColumns) override;
	bool HandleEffect(effect_t EffNum, uint8_t EffParam) override;
	void HandleEmptyNote() override;
	void HandleCut() override;
	void HandleRelease() override;
	void HandleNote(int Note, int Octave) override;
	int  RunNote(int Octave, int Note) override;
	bool CreateInstHandler(inst_type_t Type) override;
	int  CalculateVolume() const override;
	int  CalculatePeriod(bool MultiplyByHarmonic = true) const override;
};