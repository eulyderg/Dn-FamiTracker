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

CEPSM::CEPSM()
{
	m_pRegisterLogger->AddRegisterRange(0x000, 0x1FF);
	Reset();
}

CEPSM::~CEPSM()
{
	SAFE_RELEASE_ARRAY(m_pBuffer);
}

void CEPSM::Reset()
{
	// reset emulation core
	m_ymf288.reset();

	// reset buffer information
	m_iBufferPtr = 0;
	m_iTime = 0;
	m_BlipEPSM.clear();

	m_iSSGVolume[0] = m_iSSGVolume[1] = m_iSSGVolume[2] = 0;
	m_iSSGPeriod[0] = m_iSSGPeriod[1] = m_iSSGPeriod[2] = 0;
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
	m_ymf288.reset();
	m_ymf288.set_fidelity(ymfm::OPN_FIDELITY_MAX);

	m_iMaxSamples = (uint32_t(SampleRate) / FrameRate) * 2;	// Allow some overflow

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
	switch (Address) {
		case 0x401C:
			m_ymf288.write_address(Value);
			m_iAddress = Value;
			break;
		case 0x401D:
			m_ymf288.write_data(Value);
			switch (m_iAddress) {
				// TODO: Pitch
				case 0x00: m_iSSGPeriod[0] = (m_iSSGPeriod[0] & 0xF00) | Value; break;
				case 0x01: m_iSSGPeriod[0] = (m_iSSGPeriod[0] & 0x0FF) | (Value & 0xF) << 8; break;
				case 0x02: m_iSSGPeriod[1] = (m_iSSGPeriod[1] & 0xF00) | Value; break;
				case 0x03: m_iSSGPeriod[1] = (m_iSSGPeriod[1] & 0x0FF) | (Value & 0xF) << 8; break;
				case 0x04: m_iSSGPeriod[2] = (m_iSSGPeriod[2] & 0xF00) | Value; break;
				case 0x05: m_iSSGPeriod[2] = (m_iSSGPeriod[2] & 0x0FF) | (Value & 0xF) << 8; break;
				case 0x07:
					m_iSSGToneEnabled[0] = (Value & 0b001) ? 0 : 1;
					m_iSSGToneEnabled[1] = (Value & 0b010) ? 0 : 1;
					m_iSSGToneEnabled[2] = (Value & 0b100) ? 0 : 1; break;
				// TODO: Envelopes
				case 0x08: m_iSSGVolume[0] = Value & 0xF; break;
				case 0x09: m_iSSGVolume[1] = Value & 0xF; break;
				case 0x0A: m_iSSGVolume[2] = Value & 0xF; break;
				case 0x0B: m_iSSGEnvPeriod = (m_iSSGEnvPeriod & 0xFF00) | Value; break;
				case 0x0C: m_iSSGEnvPeriod = (m_iSSGEnvPeriod & 0x00FF) | (Value << 8); break;
				case 0x0D: m_iSSGEnvMode   = Value & 0xF; break;


				// FM
				case 0x30:
					m_pFMData[0].Detune[0] = (Value >> 4) & 0b1111;
					m_pFMData[0].Mult[0] = Value & 0b1111;
					break;
				case 0x31:
					m_pFMData[1].Detune[0] = (Value >> 4) & 0b1111;
					m_pFMData[1].Mult[0] = Value & 0b1111;
					break;
				case 0x32:
					m_pFMData[2].Detune[0] = (Value >> 4) & 0b1111;
					m_pFMData[2].Mult[0] = Value & 0b1111;
					break;

				case 0x34:
					m_pFMData[0].Detune[1] = (Value >> 4) & 0b1111;
					m_pFMData[0].Mult[1] = Value & 0b1111;
					break;
				case 0x35:
					m_pFMData[1].Detune[1] = (Value >> 4) & 0b1111;
					m_pFMData[1].Mult[1] = Value & 0b1111;
					break;
				case 0x36:
					m_pFMData[2].Detune[1] = (Value >> 4) & 0b1111;
					m_pFMData[2].Mult[1] = Value & 0b1111;
					break;

				case 0x38:
					m_pFMData[0].Detune[2] = (Value >> 4) & 0b1111;
					m_pFMData[0].Mult[2] = Value & 0b1111;
					break;
				case 0x39:
					m_pFMData[1].Detune[2] = (Value >> 4) & 0b1111;
					m_pFMData[1].Mult[2] = Value & 0b1111;
					break;
				case 0x3A:
					m_pFMData[2].Detune[2] = (Value >> 4) & 0b1111;
					m_pFMData[2].Mult[2] = Value & 0b1111;
					break;

				case 0x3C:
					m_pFMData[0].Detune[3] = (Value >> 4) & 0b1111;
					m_pFMData[0].Mult[3] = Value & 0b1111;
					break;
				case 0x3D:
					m_pFMData[1].Detune[3] = (Value >> 4) & 0b1111;
					m_pFMData[1].Mult[3] = Value & 0b1111;
					break;
				case 0x3E:
					m_pFMData[2].Detune[3] = (Value >> 4) & 0b1111;
					m_pFMData[2].Mult[3] = Value & 0b1111;
					break;

				case 0x40: m_pFMData[0].TotalLevel[0] = Value & 0b1111111; break;
				case 0x41: m_pFMData[1].TotalLevel[0] = Value & 0b1111111; break;
				case 0x42: m_pFMData[2].TotalLevel[0] = Value & 0b1111111; break;

				case 0x44: m_pFMData[0].TotalLevel[1] = Value & 0b1111111; break;
				case 0x45: m_pFMData[1].TotalLevel[1] = Value & 0b1111111; break;
				case 0x46: m_pFMData[2].TotalLevel[1] = Value & 0b1111111; break;

				case 0x48: m_pFMData[0].TotalLevel[2] = Value & 0b1111111; break;
				case 0x49: m_pFMData[1].TotalLevel[2] = Value & 0b1111111; break;
				case 0x4A: m_pFMData[2].TotalLevel[2] = Value & 0b1111111; break;

				case 0x4C: m_pFMData[0].TotalLevel[3] = Value & 0b1111111; break;
				case 0x4D: m_pFMData[1].TotalLevel[3] = Value & 0b1111111; break;
				case 0x4E: m_pFMData[2].TotalLevel[3] = Value & 0b1111111; break;

				case 0xB0: m_pFMData[0].Feedback = (Value >> 3) & 0b111; break;
				case 0xB1: m_pFMData[1].Feedback = (Value >> 3) & 0b111; break;
				case 0xB2: m_pFMData[2].Feedback = (Value >> 3) & 0b111; break;

				default: break;
			}
			break;
		case 0x401E:
			m_ymf288.write_address_hi(Value);
			m_iAddress = Value | 0x100;
			break;
		case 0x401F: m_ymf288.write_data_hi(Value); break;
	}
}

void CEPSM::Log(uint16_t Address, uint8_t Value)
{
	switch (Address) {
		case 0x401C: m_pRegisterLogger->SetPort(Value); break;
		case 0x401D: m_pRegisterLogger->Write(Value); break;
		case 0x401E: m_pRegisterLogger->SetPort(Value | 0x100); break;
		case 0x401F: m_pRegisterLogger->Write(Value); break;
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
	uint32_t WantSamples = Output.count_samples(m_iTime);

	// Hacky solution for higher clock rate. Need proper resampling.
	const double rateRatio = double(m_ymf288.sample_rate(CAPU::BASE_FREQ_EPSM)) / double(Output.sample_rate());
	static double clockCount = double(m_iTime);
	
	static int32_t LastSample = 0;

	// Generate EPSM samples
	while (m_iBufferPtr < WantSamples) {
		// Hacky solution for higher clock rate.
		// TODO: proper resampling of multiple samples
		int32_t clocks = uint32_t(clockCount) - uint32_t(clockCount - rateRatio);
		clockCount -= rateRatio;
		
		while (clocks > 0) {
			m_ymf288.generate(&m_ymf288output);
			clocks--;
		}

		int32_t FMSampleL = m_ymf288output.data[0];
		int32_t FMSampleR = m_ymf288output.data[1];
		int32_t SSGSample = m_ymf288output.data[2];

		int32_t Sample = (FMSampleL + FMSampleR)/2 + SSGSample;

		for (int i = 3; i < 9; i++) {
			//m_ChannelLevels[i].update(static_cast<uint8_t>((255.0 * ((i) + 1.0) / 4096.0)));
		}

		if (Sample > 32767)
			Sample = 32767;
		if (Sample < -32768)
			Sample = -32768;

		m_pBuffer[m_iBufferPtr++] = int16_t((Sample + LastSample) >> 1);
		LastSample = Sample;
	}

	Output.mix_samples((blip_amplitude_t*)m_pBuffer, WantSamples);

	m_iBufferPtr -= WantSamples;

	m_iTime = 0;
}

double CEPSM::GetFreq(int Channel) const
{
	if ((0 <= Channel && Channel < 3) && m_iSSGToneEnabled[Channel] && (m_iSSGPeriod[Channel] > 0)) {
		// SSG channels
		return CAPU::BASE_FREQ_EPSM / (64.0 * m_iSSGPeriod[Channel]);
	} else if (3 <= Channel && Channel < 9) {
		// FM channels
		//return 0;
	} else if (9 <= Channel && Channel < 15) {
		// ADPCM channels
		//return 0;     
	} else if (Channel >= 15 && (m_iSSGEnvMode & 0b1000) && !(m_iSSGEnvMode & 0b0001) && (m_iSSGEnvPeriod > 0)) {
		// SSG envelope
		return CAPU::BASE_FREQ_EPSM / (((m_iSSGEnvMode & 0b0010) ? 2048. : 1024.) * m_iSSGEnvPeriod);
	}
	return 0;
}

int CEPSM::GetChannelLevel(int Channel)
{
	if (0 <= Channel && Channel < 3) {
		return m_iSSGVolume[Channel];
	} else if (3 <= Channel && Channel < 9) {
		//return 0;
	} else if (9 <= Channel && Channel < 15) {
		//return 0;
	}
	return 0;
}

int CEPSM::GetChannelLevelRange(int Channel) const
{
	if (0 <= Channel && Channel < 3) {
		return 0xF;
	}
	else if (3 <= Channel && Channel < 9) {
		//return 127;
	}
	else if (9 <= Channel && Channel < 15) {
		//return 127;
	}
	return 127;
}

void CEPSM::UpdateMixLevel(double v, bool UseSurveyMix)
{
	SetDirectVolume(v);
	
	// emu2413's waveform output ranges from -4095...4095
	m_SynthEPSM.volume(v, UseSurveyMix ? 8191 : 10000);
}