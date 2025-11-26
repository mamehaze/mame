// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "elan_eu3a05_soc.h"

DEFINE_DEVICE_TYPE(ELAN_EU3A05_SOC,     elan_eu3a05_cpu_device,     "elan_eu3a05_cpu_device",     "ELAN EU3A05 (NTSC)")
DEFINE_DEVICE_TYPE(ELAN_EU3A05_PAL_SOC, elan_eu3a05_pal_cpu_device, "elan_eu3a05_pal_cpu_device", "ELAN EU3A05 (PAL)")
DEFINE_DEVICE_TYPE(ELAN_EU3A13_SOC,     elan_eu3a13_cpu_device,     "elan_eu3a13_cpu_device",     "ELAN EU3A13 (NTSC)")
DEFINE_DEVICE_TYPE(ELAN_EU3A13_PAL_SOC, elan_eu3a13_pal_cpu_device, "elan_eu3a13_pal_cpu_device", "ELAN EU3A13 (PAL)")


elan_eu3a05_cpu_device::elan_eu3a05_cpu_device(const machine_config& mconfig, device_type type, const char* tag, device_t* owner, uint32_t clock) :
	m6502_device(mconfig, type, tag, owner, clock),
	m_extbus_config("extbus", ENDIANNESS_LITTLE, 8, 24),
	m_write_0_callback(*this),
	m_write_1_callback(*this),
	m_write_2_callback(*this),
	m_read_0_callback(*this, 0xff),
	m_read_1_callback(*this, 0xff),
	m_read_2_callback(*this, 0xff),
	m_sys(*this, "sys"),
	m_sound(*this, "eu3a05sound"),
	m_gpio(*this, "gpio"),
	m_screen(*this, "screen"),
	m_vid(*this, "vid"),
	m_palette(*this, "palette")
{
	m_extbus_config.m_addr_width = 24;
	m_extbus_config.m_logaddr_width = 24;
	program_config.m_internal_map = address_map_constructor(FUNC(elan_eu3a05_cpu_device::int_map), this);
	m_fixed_base = 0x3f8000;
}

elan_eu3a05_cpu_device::elan_eu3a05_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	elan_eu3a05_cpu_device(mconfig, ELAN_EU3A05_SOC, tag, owner, clock)
{
}

elan_eu3a05_pal_cpu_device::elan_eu3a05_pal_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	elan_eu3a05_cpu_device(mconfig, ELAN_EU3A05_PAL_SOC, tag, owner, clock)
{
}

elan_eu3a13_cpu_device::elan_eu3a13_cpu_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	elan_eu3a05_cpu_device(mconfig, type, tag, owner, clock)
{
	m_fixed_base = 0x000000;
}

elan_eu3a13_cpu_device::elan_eu3a13_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	elan_eu3a13_cpu_device(mconfig, ELAN_EU3A13_SOC, tag, owner, clock)
{
}

elan_eu3a13_pal_cpu_device::elan_eu3a13_pal_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	elan_eu3a13_cpu_device(mconfig, ELAN_EU3A13_PAL_SOC, tag, owner, clock)
{
}

void elan_eu3a05_cpu_device::device_add_mconfig(machine_config &config)
{
	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	ELAN_EU3A05_SYS(config, m_sys, 0);
	m_sys->set_cpu(":maincpu");
	m_sys->bank_change_callback().set(FUNC(elan_eu3a05_cpu_device::bank_change));

	PALETTE(config, m_palette).set_entries(256);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500));
	m_screen->set_screen_update(FUNC(elan_eu3a05_cpu_device::screen_update));
	m_screen->set_size(32*8, 32*8);
	m_screen->set_visarea(0*8, 32*8-1, 0*8, 28*8-1);
	m_screen->set_palette(m_palette);

	ELAN_EU3A05_GPIO(config, m_gpio, 0);
	m_gpio->read_0_callback().set(FUNC(elan_eu3a05_cpu_device::in0_read));
	m_gpio->read_1_callback().set(FUNC(elan_eu3a05_cpu_device::in1_read));
	m_gpio->read_2_callback().set(FUNC(elan_eu3a05_cpu_device::in2_read));
	m_gpio->write_0_callback().set(FUNC(elan_eu3a05_cpu_device::out0_write));
	m_gpio->write_1_callback().set(FUNC(elan_eu3a05_cpu_device::out1_write));
	m_gpio->write_2_callback().set(FUNC(elan_eu3a05_cpu_device::out2_write));

	ELAN_EU3A05_VID(config, m_vid, 0);
	m_vid->set_cpu(":maincpu");
	m_vid->set_palette("palette");
	m_vid->set_entries(256);

	ELAN_EU3A05_SOUND(config, m_sound, 8000);
	m_sound->space_read_callback().set(FUNC(elan_eu3a05_cpu_device::read_full_space));
	m_sound->add_route(ALL_OUTPUTS, "mono", 1.0);
	/* just causes select sound to loop in Tetris for now!
	m_sound->sound_end_cb<0>().set(FUNC(elan_eu3a05_state::sound_end0));
	m_sound->sound_end_cb<1>().set(FUNC(elan_eu3a05_state::sound_end1));
	m_sound->sound_end_cb<2>().set(FUNC(elan_eu3a05_state::sound_end2));
	m_sound->sound_end_cb<3>().set(FUNC(elan_eu3a05_state::sound_end3));
	m_sound->sound_end_cb<4>().set(FUNC(elan_eu3a05_state::sound_end4));
	m_sound->sound_end_cb<5>().set(FUNC(elan_eu3a05_state::sound_end5));
	*/
}

void elan_eu3a05_pal_cpu_device::device_add_mconfig(machine_config &config)
{
	elan_eu3a05_cpu_device::device_add_mconfig(config);
	m_screen->set_refresh_hz(50);
	m_sys->set_pal();
}

void elan_eu3a13_cpu_device::device_add_mconfig(machine_config &config)
{
	elan_eu3a05_cpu_device::device_add_mconfig(config);
	m_vid->set_is_sudoku();
	m_vid->set_use_spritepages();
	m_sys->set_alt_timer(); // for Carl Edwards'
}

void elan_eu3a13_pal_cpu_device::device_add_mconfig(machine_config &config)
{
	elan_eu3a13_cpu_device::device_add_mconfig(config);
	m_screen->set_refresh_hz(50);
	m_sys->set_pal();
}


void elan_eu3a05_cpu_device::device_start()
{
	m6502_device::device_start();
	m_extbus_space = &space(5);
}

void elan_eu3a05_cpu_device::device_reset()
{
	m6502_device::device_reset();

	/* the 6502 core sets the default stack value to 0x01bd
	   and Tetris does not initialize it to anything else

	   Tetris stores the playfield data at 0x100 - 0x1c7 and
	   has a clear routine that will erase that range and
	   trash the stack

	   It seems likely this 6502 sets it to 0x1ff by default
	   at least.

	   According to
	   http://mametesters.org/view.php?id=6486
	   this isn't right for known 6502 types either
	*/
	set_state_int(M6502_S, 0x1ff);
}

uint8_t elan_eu3a05_cpu_device::bank_r(offs_t offset)
{
	return m_extbus_space->read_byte((m_current_bank * 0x8000) + offset);
}

void elan_eu3a05_cpu_device::bank_w(offs_t offset, uint8_t data)
{
	m_extbus_space->write_byte((m_current_bank * 0x8000) + offset, data);
}

uint8_t elan_eu3a05_cpu_device::read_full_space(offs_t offset)
{
	return m_extbus_space->read_byte(offset);
}

uint8_t elan_eu3a05_cpu_device::read_fixed_bank(offs_t offset)
{
	return m_extbus_space->read_byte(m_fixed_base + offset);
}

uint32_t elan_eu3a05_cpu_device::screen_update(screen_device& screen, bitmap_ind16& bitmap, const rectangle& cliprect)
{
	return m_vid->screen_update(screen, bitmap, cliprect);
}

void elan_eu3a05_cpu_device::int_map(address_map &map)
{
	// can the addresses move around?
	map(0x0000, 0x3fff).ram();
	map(0x4800, 0x49ff).rw(m_vid, FUNC(elan_eu3a05commonvid_device::palette_r), FUNC(elan_eu3a05commonvid_device::palette_w));

	map(0x5000, 0x501f).m(m_sys, FUNC(elan_eu3a05sys_device::map)); // including DMA controller

	map(0x5020, 0x503f).m(m_vid, FUNC(elan_eu3a05vid_device::map));

	// 504x GPIO area?
	map(0x5040, 0x5046).rw(m_gpio, FUNC(elan_eu3a05gpio_device::gpio_r), FUNC(elan_eu3a05gpio_device::gpio_w));
	// 5047
	map(0x5048, 0x504a).w(m_gpio, FUNC(elan_eu3a05gpio_device::gpio_unk_w));

	// 506x unknown
	map(0x5060, 0x506d).ram(); // read/written by tetris (ADC?)

	// 508x sound
	map(0x5080, 0x50bf).m(m_sound, FUNC(elan_eu3a05_sound_device::map));

	map(0x6000, 0xdfff).rw(FUNC(elan_eu3a05_cpu_device::bank_r), FUNC(elan_eu3a05_cpu_device::bank_w));

	map(0xe000, 0xffff).r(FUNC(elan_eu3a05_cpu_device::read_fixed_bank));
	// not sure how these work, might be a modified 6502 core instead.
	map(0xfffa, 0xfffb).r(m_sys, FUNC(elan_eu3a05commonsys_device::nmi_vector_r)); // custom vectors handled with NMI for now
	//map(0xfffe, 0xffff).r(m_sys, FUNC(elan_eu3a05commonsys_device::irq_vector_r));  // allow normal IRQ for brk
}

device_memory_interface::space_config_vector elan_eu3a05_cpu_device::memory_space_config() const
{
	if(has_configured_map(AS_OPCODES))
		return space_config_vector {
			std::make_pair(AS_PROGRAM, &program_config),
			std::make_pair(AS_OPCODES, &sprogram_config),
			std::make_pair(5, &m_extbus_config),
		};
	else
		return space_config_vector {
			std::make_pair(AS_PROGRAM, &program_config),
			std::make_pair(5, &m_extbus_config),
		};
}

