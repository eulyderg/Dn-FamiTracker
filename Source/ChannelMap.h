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


// CChannelMap

class CChannelMap
{
public:
	CChannelMap();
	~CChannelMap();
	void SetupSoundChips();

	int			GetChipCount() const;
	LPCTSTR		GetChipName(int Index) const;
	chip_id_t	GetChipIdent(int Index) const;
	int			GetChipIndex(chip_id_t Ident) const;
	CInstrument	*GetChipInstrument(chip_id_t Chip) const;

	// Active channel map
	void			ResetChannels();
	void			RegisterChannel(CTrackerChannel *pChannel, chan_id_t ChannelType, chip_id_t ChipType);
	CTrackerChannel	*GetChannel(int Index) const;
	chan_id_t		GetChannelType(int Index) const;
	chip_id_t		GetChipType(int Index) const;

protected:
	void AddChip(chip_id_t Ident, inst_type_t Inst, LPCTSTR pName);

protected:
	// Chips
	int				m_iAddedChips;
	chip_id_t		m_iChipIdents[CHIP_COUNT];
	LPCTSTR			m_pChipNames[CHIP_COUNT];
	inst_type_t		m_iChipInstType[CHIP_COUNT];		// // //

	// Current set
	CTrackerChannel	*m_pChannels[CHANNELS];
	int				m_iRegisteredChannels;
	chan_id_t		m_iChannelTypes[CHANNELS];
	chip_id_t		m_iChannelChip[CHANNELS];

};
