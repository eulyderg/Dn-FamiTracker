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

#pragma once

#include "SoundChip.h"
#include "ChannelLevelState.h"

#include "ymfm/ymfm_opn.h"

class CEPSM : public CSoundChip {
public:
	CEPSM();
	virtual ~CEPSM();

	void Reset() override;
	void UpdateFilter(blip_eq_t eq) override;
	void SetClockRate(uint32_t Rate) override;
	void Process(uint32_t Time, Blip_Buffer& Output) override;
	void EndFrame(Blip_Buffer& Output, gsl::span<int16_t> TempBuffer) override;

	void Write(uint16_t Address, uint8_t Value) override;
	uint8_t Read(uint16_t Address, bool& Mapped) override;

	double GetFreq(int Channel) const override;		// // //
	int GetChannelLevel(int Channel) override;
	int GetChannelLevelRange(int Channel) const override;

	void SetSampleSpeed(uint32_t SampleRate, double ClockRate, uint32_t FrameRate);
	void SetDirectVolume(double Volume);
	void Log(uint16_t Address, uint8_t Value) override;		// // //

	void UpdateMixLevel(double v, bool UseSurveyMix = false);

	//void UpdatePatchSet(int PatchSelection, bool UseExternalOPLLChip, uint8_t *PatchSet);

	// Report some basic information about the chip
	uint8_t GetChannelCount() const override { return CHANNELS_EPSM; };			// TODO: Dynamically calculate this?
	chan_id_t GetFirstChannelID() const override { return CHANID_EPSM_SSG1; };	//

protected:
	//ymfm::ymf288 m_ymf288;
	
private:
	uint32_t	m_iTime;
	uint32_t	m_iMaxSamples = 0;

	int16_t		*m_pBuffer = NULL;
	uint32_t	m_iBufferPtr;

	uint8_t		m_iSoundReg = 0;

	double		m_DirectVolume = 1.0f;

	Blip_Buffer	m_BlipEPSM;
	Blip_Synth<blip_good_quality> m_SynthEPSM;
};
