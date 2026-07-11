// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "generalplus_gpl16250_spi.h"

class generalplus_mobigo2_state : public generalplus_gpspispi_game_state
{
public:
	generalplus_mobigo2_state(const machine_config &mconfig, device_type type, const char *tag) :
		generalplus_gpspispi_game_state(mconfig, type, tag)
	{
	}

protected:
};

static INPUT_PORTS_START( mobigo2 )
INPUT_PORTS_END

ROM_START( mobigo2 )
	//ROM_REGION16_BE( 0x40000, "maincpu:internal", ROMREGION_ERASE00 )
	//ROM_LOAD16_WORD_SWAP( "internal.rom", 0x00000, 0x40000, NO_DUMP )

	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASE00)	// appears to be the boot ROM, has GPspispi header
	ROM_LOAD16_WORD_SWAP( "n25s16.u3", 0x00000, 0x200000, CRC(dfbf5029) SHA1(2a079ddd8a13c5f3d8f40fa6d6c3de2dc1573449) )

	ROM_REGION( 0x8400000, "nandrom", ROMREGION_ERASE00 ) // no GPnandnand header so not a boot device
	ROM_LOAD( "mobigo2_bios_ger.bin", 0x00000, 0x8400000, CRC(d5ab613d) SHA1(6fb104057dc3484fa958e2cb20c5dd0c19589f75) ) // SPANSION S34ML01G100TF100
ROM_END

CONS( 2013, mobigo2, 0,      0, generalplus_gpspispi,  mobigo2, generalplus_mobigo2_state, init_spi, "VTech", "MobiGo 2 (Germany)", MACHINE_NO_SOUND | MACHINE_NOT_WORKING )
