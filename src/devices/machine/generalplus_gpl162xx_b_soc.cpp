// license:BSD-3-Clause
// copyright-holders:David Haywood
/*****************************************************************************

    GPL16218B
    GPL16238B (this could be GPAC500B)
    GPL16248VB
    GPL16258VB (this could be GPAC800B)

	generalplus_gpl162xx_lcdtype.cpp appears to also use a GPL162xxB series
	chip, just ignoring the video output, it does however use a custom OTP
	which might mean it's a slight variarion on the above which documentation
	indicates only have the OTP provided by GeneralPlus

**********************************************************************/

#include "emu.h"
#include "generalplus_gpl162xx_b_soc.h"



generalplus_gpl162xx_b_base::generalplus_gpl162xx_b_base(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, address_map_constructor internal) :
	generalplus_gpl162xx_base_device(mconfig, type, tag, owner, clock, internal)
{
}

generalplus_gpl16218b_device::generalplus_gpl16218b_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	generalplus_gpl162xx_b_base(mconfig, GPL16218B, tag, owner, clock, address_map_constructor(FUNC(generalplus_gpl16218b_device::gpl16218b_map), this))
{
}

generalplus_gpl16218b_device::generalplus_gpl16218b_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, address_map_constructor internal) :
	generalplus_gpl162xx_b_base(mconfig, type, tag, owner, clock, internal)
{
}


generalplus_gpl16238b_device::generalplus_gpl16238b_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	generalplus_gpl16218b_device(mconfig, GPL16238B, tag, owner, clock, address_map_constructor(FUNC(generalplus_gpl16238b_device::gpl16238b_map), this))
{
}

generalplus_gpl16238b_device::generalplus_gpl16238b_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, address_map_constructor internal) :
	generalplus_gpl16218b_device(mconfig, type, tag, owner, clock, internal)
{
}


generalplus_gpl16248vb_device::generalplus_gpl16248vb_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	generalplus_gpl16238b_device(mconfig, GPL16248VB, tag, owner, clock, address_map_constructor(FUNC(generalplus_gpl16248vb_device::gpl16248vb_map), this))
{
}

generalplus_gpl16248vb_device::generalplus_gpl16248vb_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, address_map_constructor internal) :
	generalplus_gpl16238b_device(mconfig, type, tag, owner, clock, internal)
{
}

generalplus_gpl16258vb_device::generalplus_gpl16258vb_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	generalplus_gpl16248vb_device(mconfig, GPL16258VB, tag, owner, clock, address_map_constructor(FUNC(generalplus_gpl16258vb_device::gpl16258vb_map), this))
{
}

void generalplus_gpl16218b_device::gpl16218b_map(address_map &map)
{
	map(0x000000, 0x002fff).ram().share("mainram"); // 12K * 16
	gcm394_internal_map(map);
}

void generalplus_gpl16238b_device::gpl16238b_map(address_map &map)
{
	map(0x000000, 0x002fff).ram().share("mainram"); // 12K * 16
	gcm394_internal_map(map);
}

void generalplus_gpl16248vb_device::gpl16248vb_map(address_map &map)
{
	map(0x000000, 0x002fff).ram().share("mainram"); // 12K * 16
	gcm394_internal_map(map);
}

void generalplus_gpl16258vb_device::gpl16258vb_map(address_map &map)
{
	map(0x000000, 0x002fff).ram().share("mainram"); // 12K * 16
	gcm394_internal_map(map);
}


void generalplus_gpl16248vb_device::device_add_mconfig(machine_config & config)
{
	generalplus_gpl16238b_device::device_add_mconfig(config);
	m_spg_video->set_has_vga_modes();
}

void generalplus_gpl16258vb_device::device_add_mconfig(machine_config & config)
{
	generalplus_gpl16248vb_device::device_add_mconfig(config);
	m_spg_video->set_has_3d_sprite_modes();
}


ROM_START( gpl1658vb )
	ROM_REGION16_BE( 0x2000, "internal", 0 )
	ROM_LOAD16_WORD_SWAP("gpl16258vb_bootrom.bin", 0x00000, 0x2000, NO_DUMP )
ROM_END

const tiny_rom_entry *generalplus_gpl16258vb_device::device_rom_region() const
{
	return ROM_NAME( gpl1658vb );
}

ROM_START( gpl1648vb )
	ROM_REGION16_BE( 0x2000, "internal", 0 )
	ROM_LOAD16_WORD_SWAP("gpl16248vb_bootrom.bin", 0x00000, 0x2000, NO_DUMP )
ROM_END

const tiny_rom_entry *generalplus_gpl16248vb_device::device_rom_region() const
{
	return ROM_NAME( gpl1648vb );
}

ROM_START( gpl1638b )
	ROM_REGION16_BE( 0x2000, "internal", 0 )
	ROM_LOAD16_WORD_SWAP("gpl16238b_bootrom.bin", 0x00000, 0x2000, NO_DUMP )
ROM_END

const tiny_rom_entry *generalplus_gpl16238b_device::device_rom_region() const
{
	return ROM_NAME( gpl1638b );
}

// it isn't clear if the GPL162318B has a standard internal ROM

DEFINE_DEVICE_TYPE(GPL16218B, generalplus_gpl16218b_device, "gpl16218b", "GeneralPlus GPL16218B System-on-a-Chip") // aka GPAC500B
DEFINE_DEVICE_TYPE(GPL16238B, generalplus_gpl16238b_device, "gpl16238b", "GeneralPlus GPL16238B System-on-a-Chip")
DEFINE_DEVICE_TYPE(GPL16248VB, generalplus_gpl16248vb_device, "gpl16248vb", "GeneralPlus GPL16248VB System-on-a-Chip")
DEFINE_DEVICE_TYPE(GPL16258VB, generalplus_gpl16258vb_device, "gpl16258vb", "GeneralPlus GPL16258VB System-on-a-Chip") // aka GPAC800B (not used by JAKKS?)
