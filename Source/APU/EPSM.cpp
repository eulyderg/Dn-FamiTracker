/*
** Dn-FamiTracker - NES/Famicom sound tracker
** Copyright (C) 2020-2025 D.P.C.M.
** FamiTracker Copyright (C) 2005-2020 Jonathan Liss
** 0CC-FamiTracker Copyright (C) 2014-2018 HertzDevil
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program. If not, see https://www.gnu.org/licenses/.
*/

#include "../stdafx.h"
#include "../FamiTracker.h"
#include "../Settings.h"
#include "APU.h"
#include "EPSM.h"
#include "../RegisterState.h"		// // //

//const uint32_t CVRC7::OPLL_CLOCK = CAPU::BASE_FREQ_VRC7;	// Clock frequency

CEPSM::CEPSM()
{
	m_pRegisterLogger->AddRegisterRange(0x00, 0x3F);		// // //
	Reset();
}

CEPSM::~CEPSM()
{
	SAFE_RELEASE_ARRAY(m_pBuffer);
}

void CEPSM::Reset()
{
	// reset emulation core
	//m_ymf288.reset();

	// reset buffer information
	m_iBufferPtr = 0;
	m_iTime = 0;
	m_BlipEPSM.clear();
}

void CEPSM::UpdateFilter(blip_eq_t eq)
{
	m_SynthEPSM.treble_eq(eq);
	m_BlipEPSM.set_sample_rate(eq.sample_rate);
	m_BlipEPSM.bass_freq(0);
}

void CEPSM::SetClockRate(uint32_t Rate)
{
	m_BlipEPSM.clock_rate(Rate);
}

void CEPSM::SetSampleSpeed(uint32_t SampleRate, double ClockRate, uint32_t FrameRate)
{
	m_iMaxSamples = (SampleRate / FrameRate) * 2;	// Allow some overflow
    
	SAFE_RELEASE_ARRAY(m_pBuffer);
	m_pBuffer = new int16_t[m_iMaxSamples];
	memset(m_pBuffer, 0, sizeof(int16_t) * m_iMaxSamples);
}

void CEPSM::SetDirectVolume(double Volume)
{
	m_DirectVolume = Volume;
}

void CEPSM::Write(uint16_t Address, uint8_t Value)
{
	static constexpr uint16_t offset = 0x9000;
	//switch (Address) {
	//	case offset+0x00:
	//		break;
	//}
	//m_ymf288.write_address(Address - offset);
	//m_ymf288.write_data(Value);
}

void CEPSM::Log(uint16_t Address, uint8_t Value)		// // //
{
	switch (Address) {
	case 0x9010: m_pRegisterLogger->SetPort(Value); break;
	case 0x9030: m_pRegisterLogger->Write(Value); break;
	}
}

uint8_t CEPSM::Read(uint16_t Address, bool &Mapped)
{
	return 0;
}

void CEPSM::Process(uint32_t Time, Blip_Buffer& Output)
{
	// This cannot run in sync, fetch all samples at end of frame instead
	m_iTime += Time;
}

void CEPSM::EndFrame(Blip_Buffer& Output, gsl::span<int16_t> TempBuffer)
{
	/*uint32_t WantSamples = Output.count_samples(m_iTime);

	static int32_t LastSample = 0;

	// Generate EPSM samples
	while (m_iBufferPtr < WantSamples) {
		int32_t RawSample = OPLL_calc(m_pOPLLInt);

		for (int i = 0; i < 15; i++)
			m_ChannelLevels[i].update(0);

		// Apply direct volume, hacky workaround
		int32_t Sample = static_cast<int32_t>(double(RawSample) * m_DirectVolume);

		if (Sample > 32767)
			Sample = 32767;
		if (Sample < -32768)
			Sample = -32768;

		m_pBuffer[m_iBufferPtr++] = int16_t((Sample + LastSample) >> 1);
		LastSample = Sample;
	}

	Output.mix_samples((blip_amplitude_t*)m_pBuffer, WantSamples);

	m_iBufferPtr -= WantSamples;

	m_iTime = 0;*/
}

double CEPSM::GetFreq(int Channel) const		// // //
{
	return 0.;
}

int CEPSM::GetChannelLevel(int Channel)
{
	/*ASSERT(0 <= Channel && Channel < 9);
	if (0 <= Channel && Channel < 6) {
		return m_ChannelLevels[Channel].getLevel();
	}*/
	return 0;
}

int CEPSM::GetChannelLevelRange(int Channel) const
{
	return 127;
}

void CEPSM::UpdateMixLevel(double v, bool UseSurveyMix)
{
	// The output of emu2413 is resampled. This means
	// that the emulator output suffers no multiplex hiss and
	// bit depth quantization.
	// TODO: replace emu2413 with Nuked-OPLL for better multiplexing accuracy?

	// hacky solution, since VRC7 uses asynchronous direct buffer writes
	SetDirectVolume(v);
	
	// emu2413's waveform output ranges from -4095...4095
	m_SynthEPSM.volume(v, UseSurveyMix ? 8191 : 10000);
}