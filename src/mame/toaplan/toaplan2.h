// license:BSD-3-Clause
// copyright-holders:Quench, Yochizo, David Haywood
#ifndef MAME_TOAPLAN_TOAPLAN2_H
#define MAME_TOAPLAN_TOAPLAN2_H

#pragma once

#include "cpu/m68000/m68000.h"
#include "machine/bankdev.h"
#include "machine/eepromser.h"
#include "machine/gen_latch.h"
#include "machine/ticket.h"
#include "machine/upd4992.h"

#include "toaplan2_screen.h"
#include "toaplan2_txlayer.h"

#include "sound/okim6295.h"
#include "emupal.h"
#include "screen.h"
#include "tilemap.h"

/**************** Machine stuff ******************/

class toaplan2_state : public driver_device
{
public:
	toaplan2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_oki(*this, "oki%u", 1U)
	{ }


	optional_device_array<okim6295_device, 2> m_oki;

};



#endif // MAME_TOAPLAN_TOAPLAN2_H
