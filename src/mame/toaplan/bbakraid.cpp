
#include "emu.h"
#include "truxton2.h"
#include "toaplipt.h"

#include "gp9001.h"

#include "cpu/nec/v25.h"
#include "cpu/z80/z80.h"
#include "cpu/z180/hd647180x.h"
#include "machine/nvram.h"
#include "sound/ymopm.h"
#include "sound/ymopl.h"
#include "sound/ymz280b.h"
#include "speaker.h"



class bbakraid_state : public truxton2_state
{
public:
	bbakraid_state(const machine_config &mconfig, device_type type, const char *tag)
		: truxton2_state(mconfig, type, tag)
	{ }


	void bbakraid(machine_config &config);

	void init_bbakraid();


protected:
private:

	void bbakraid_68k_mem(address_map &map);
	void bbakraid_sound_z80_mem(address_map &map);
	void bbakraid_sound_z80_port(address_map &map);

};

#define XOR(a) WORD_XOR_LE(a)

static const gfx_layout batrider_tx_tilelayout =
{
	8,8,    /* 8x8 characters */
	1024,   /* 1024 characters */
	4,      /* 4 bits per pixel */
	{ STEP4(0,1) },
	{ XOR(0)*4, XOR(1)*4, XOR(2)*4, XOR(3)*4, XOR(4)*4, XOR(5)*4, XOR(6)*4, XOR(7)*4 },
	{ STEP8(0,4*8) },
	8*8*4
};


static GFXDECODE_START( gfx_batrider )
	GFXDECODE_ENTRY( nullptr, 0, batrider_tx_tilelayout, 64*16, 64 )
GFXDECODE_END


void bbakraid_state::bbakraid_68k_mem(address_map &map)
{
	map(0x000000, 0x1fffff).rom();
	// actually 200000 - 20ffff is probably all main RAM, and the text and palette RAM are written via DMA
	map(0x200000, 0x207fff).ram().share(m_mainram);
	map(0x208000, 0x20ffff).ram();
	map(0x300000, 0x33ffff).r(FUNC(truxton2_state::batrider_z80rom_r));
	map(0x400000, 0x40000d).lrw16(
							NAME([this](offs_t offset, u16 mem_mask) { return m_vdp->read(offset ^ (0xc/2), mem_mask); }),
							NAME([this](offs_t offset, u16 data, u16 mem_mask) { m_vdp->write(offset ^ (0xc/2), data, mem_mask); }));
	map(0x500000, 0x500001).portr("IN");
	map(0x500002, 0x500003).portr("SYS-DSW");
	map(0x500004, 0x500005).portr("DSW");
	map(0x500006, 0x500007).r(FUNC(truxton2_state::video_count_r));
	map(0x500009, 0x500009).w(FUNC(truxton2_state::coin_w));
	map(0x500011, 0x500011).r(m_soundlatch[2], FUNC(generic_latch_8_device::read));
	map(0x500013, 0x500013).r(m_soundlatch[3], FUNC(generic_latch_8_device::read));
	map(0x500015, 0x500015).w(FUNC(truxton2_state::batrider_soundlatch_w));
	map(0x500017, 0x500017).w(FUNC(truxton2_state::batrider_soundlatch2_w));
	map(0x500018, 0x500019).r(FUNC(truxton2_state::bbakraid_eeprom_r));
	map(0x50001a, 0x50001b).w(FUNC(truxton2_state::batrider_unknown_sound_w));
	map(0x50001c, 0x50001d).w(FUNC(truxton2_state::batrider_clear_sndirq_w));
	map(0x50001f, 0x50001f).w(FUNC(truxton2_state::bbakraid_eeprom_w));
	map(0x500080, 0x500081).w(FUNC(truxton2_state::batrider_textdata_dma_w));
	map(0x500082, 0x500083).w(FUNC(truxton2_state::batrider_pal_text_dma_w));
	map(0x5000c0, 0x5000cf).w(FUNC(truxton2_state::batrider_objectbank_w)).umask16(0x00ff);
}



void bbakraid_state::bbakraid_sound_z80_mem(address_map &map)
{
	map(0x0000, 0xbfff).rom();     // No banking? ROM only contains code and data up to 0x28DC
	map(0xc000, 0xffff).ram();
}


void bbakraid_state::bbakraid_sound_z80_port(address_map &map)
{
	map.global_mask(0xff);
	map(0x40, 0x40).w(m_soundlatch[2], FUNC(generic_latch_8_device::write));
	map(0x42, 0x42).w(m_soundlatch[3], FUNC(generic_latch_8_device::write));
	map(0x44, 0x44).w(FUNC(truxton2_state::batrider_sndirq_w));
	map(0x46, 0x46).w(FUNC(truxton2_state::batrider_clear_nmi_w));
	map(0x48, 0x48).r(m_soundlatch[0], FUNC(generic_latch_8_device::read));
	map(0x4a, 0x4a).r(m_soundlatch[1], FUNC(generic_latch_8_device::read));
	map(0x80, 0x81).rw("ymz", FUNC(ymz280b_device::read), FUNC(ymz280b_device::write));
}

void bbakraid_state::bbakraid(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 32_MHz_XTAL/2);   // 16MHz, 32MHz Oscillator
	m_maincpu->set_addrmap(AS_PROGRAM, &bbakraid_state::bbakraid_68k_mem);
	m_maincpu->reset_cb().set(FUNC(truxton2_state::toaplan2_reset));

	Z80(config, m_audiocpu, XTAL(32'000'000)/6);     /* 5.3333MHz , 32MHz Oscillator */
	m_audiocpu->set_addrmap(AS_PROGRAM, &bbakraid_state::bbakraid_sound_z80_mem);
	m_audiocpu->set_addrmap(AS_IO, &bbakraid_state::bbakraid_sound_z80_port);
	m_audiocpu->set_periodic_int(FUNC(truxton2_state::bbakraid_snd_interrupt), attotime::from_hz(XTAL(32'000'000) / 6 / 12000)); // sound CPU clock (divider unverified)

	config.set_maximum_quantum(attotime::from_hz(600));

	EEPROM_93C66_8BIT(config, m_eeprom);

	ADDRESS_MAP_BANK(config, m_dma_space, 0);
	m_dma_space->set_addrmap(0, &truxton2_state::batrider_dma_mem);
	m_dma_space->set_endianness(ENDIANNESS_BIG);
	m_dma_space->set_data_width(16);
	m_dma_space->set_addr_width(16);
	m_dma_space->set_stride(0x8000);

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	m_screen->set_raw(27_MHz_XTAL/4, 432, 0, 320, 262, 0, 240);
	//m_screen->set_refresh_hz(60);
	//m_screen->set_size(432, 262);
	//m_screen->set_visarea(0, 319, 0, 239);
	m_screen->set_screen_update(FUNC(truxton2_state::screen_update_truxton2));
	m_screen->screen_vblank().set(FUNC(truxton2_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_batrider);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 0x10000);

	GP9001_VDP(config, m_vdp, 27_MHz_XTAL);
	m_vdp->set_palette(m_palette);
	m_vdp->set_tile_callback(FUNC(truxton2_state::batrider_bank_cb));
	m_vdp->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_1);

	MCFG_VIDEO_START_OVERRIDE(truxton2_state,batrider)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	// these two latches are always written together, via a single move.l instruction
	GENERIC_LATCH_8(config, m_soundlatch[0]);
	GENERIC_LATCH_8(config, m_soundlatch[1]);
	GENERIC_LATCH_8(config, m_soundlatch[2]);
	GENERIC_LATCH_8(config, m_soundlatch[3]);

	YMZ280B(config, "ymz", 16.9344_MHz_XTAL).add_route(ALL_OUTPUTS, "mono", 1.0);
	// IRQ not used ???  Connected to a test pin (TP082)
}

void bbakraid_state::init_bbakraid()
{
	m_sndirq_line = 2;
}

static INPUT_PORTS_START( batrider )
	PORT_START("IN")        // Player Inputs
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8080, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("DSW")       // DSWA and DSWB
	PORT_SERVICE_DIPLOC(0x0001, IP_ACTIVE_HIGH, "SW1:!1")
	PORT_DIPNAME( 0x0002,   0x0000, "Credits to Start" )    PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)    PORT_DIPLOCATION("SW1:!2")
	PORT_DIPSETTING(        0x0000, "1" )                   PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x0002, "2" )                   PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPNAME( 0x0002,   0x0000, "Joystick Mode" )       PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)       PORT_DIPLOCATION("SW1:!2")
	PORT_DIPSETTING(        0x0000, DEF_STR( Normal ) )     PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPSETTING(        0x0002, "90 degrees ACW" )      PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPNAME( 0x001c,   0x0000, DEF_STR( Coin_A ) )     PORT_DIPLOCATION("SW1:!3,!4,!5")
	PORT_DIPSETTING(        0x0018, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(        0x0014, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(        0x0010, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(        0x0004, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(        0x0008, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(        0x000c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(        0x001c, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x00e0,   0x0000, DEF_STR( Coin_B ) )     PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)    PORT_DIPLOCATION("SW1:!6,!7,!8")
	PORT_DIPSETTING(        0x00c0, DEF_STR( 4C_1C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x00a0, DEF_STR( 3C_1C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x0080, DEF_STR( 2C_1C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x0000, DEF_STR( 1C_1C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
//  PORT_DIPSETTING(        0x00e0, DEF_STR( 1C_1C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x0020, DEF_STR( 1C_2C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x0040, DEF_STR( 1C_3C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	PORT_DIPSETTING(        0x0060, DEF_STR( 1C_4C ) )      PORT_CONDITION("DSW", 0x001c, NOTEQUALS, 0x001c)
	// When Coin_A is set to Free_Play, Coin_A becomes Coin_A and Coin_B, and the following dips occur
	PORT_DIPNAME( 0x0020,   0x0000, "Hit Score" )           PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)   PORT_DIPLOCATION("SW1:!6")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )        PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPSETTING(        0x0020, DEF_STR( On ) )         PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPNAME( 0x0040,   0x0000, "Sound Effect" )        PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)   PORT_DIPLOCATION("SW1:!7")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )        PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPSETTING(        0x0040, DEF_STR( On ) )         PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPNAME( 0x0080,   0x0000, "Music" )               PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)   PORT_DIPLOCATION("SW1:!8")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )        PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPSETTING(        0x0080, DEF_STR( On ) )         PORT_CONDITION("DSW", 0x001c, EQUALS, 0x001c)
	PORT_DIPNAME( 0x0300,   0x0000, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:!1,!2")
	PORT_DIPSETTING(        0x0100, DEF_STR( Easy ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( Normal ) )
	PORT_DIPSETTING(        0x0200, DEF_STR( Hard ) )
	PORT_DIPSETTING(        0x0300, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x0c00,   0x0000, "Timer" )               PORT_DIPLOCATION("SW2:!3,!4")
	PORT_DIPSETTING(        0x0c00, DEF_STR( Highest ) )
	PORT_DIPSETTING(        0x0800, DEF_STR( High ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( Normal ) )
	PORT_DIPSETTING(        0x0400, DEF_STR( Low ) )
	PORT_DIPNAME( 0x3000,   0x0000, DEF_STR( Lives ) )      PORT_DIPLOCATION("SW2:!5,!6")
	PORT_DIPSETTING(        0x3000, "1" )
	PORT_DIPSETTING(        0x2000, "2" )
	PORT_DIPSETTING(        0x0000, "3" )
	PORT_DIPSETTING(        0x1000, "4" )
	PORT_DIPNAME( 0xc000,   0x0000, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:!7,!8")
	PORT_DIPSETTING(        0xc000, DEF_STR( None ) )
	PORT_DIPSETTING(        0x8000, "Every 2000k" )
	PORT_DIPSETTING(        0x0000, "Every 1500k" )
	PORT_DIPSETTING(        0x4000, "Every 1000k" )

	PORT_START("SYS-DSW")   // Coin/System and DSWC
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_UNKNOWN)
	TOAPLAN_TEST_SWITCH( 0x0004, IP_ACTIVE_HIGH )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0100,   0x0000, DEF_STR( Flip_Screen ) )    PORT_DIPLOCATION("SW3:!1")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0100, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200,   0x0000, DEF_STR( Demo_Sounds ) )    PORT_DIPLOCATION("SW3:!2")
	PORT_DIPSETTING(        0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400,   0x0000, "Stage Edit" )              PORT_DIPLOCATION("SW3:!3")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0400, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800,   0x0000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW3:!4")
	PORT_DIPSETTING(        0x0800, DEF_STR( No ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x1000,   0x0000, "Invulnerability (Cheat)" )         PORT_DIPLOCATION("SW3:!5")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x1000, DEF_STR( On ) )
	// These dips are shown only when Coin_A is set to Free_Play, but they work in normal play mode too
	PORT_DIPNAME( 0x2000,   0x0000, "Guest Players" )           PORT_DIPLOCATION("SW3:!6")
	PORT_DIPSETTING(        0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000,   0x0000, "Player Select" )           PORT_DIPLOCATION("SW3:!7")
	PORT_DIPSETTING(        0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000,   0x0000, "Special Course" )          PORT_DIPLOCATION("SW3:!8")
	PORT_DIPSETTING(        0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( bbakraid )
	PORT_INCLUDE( batrider )

	PORT_MODIFY("DSW")       // DSWA and DSWB
	PORT_DIPNAME( 0xc000,   0x0000, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:!7,!8")
	PORT_DIPSETTING(        0xc000, DEF_STR( None ) )
	PORT_DIPSETTING(        0x8000, "Every 4000k" )
	PORT_DIPSETTING(        0x4000, "Every 3000k" )
	PORT_DIPSETTING(        0x0000, "Every 2000k" )

	PORT_MODIFY("SYS-DSW")   // Coin/System and DSW-3
	PORT_DIPNAME( 0x2000,   0x0000, "Save Scores" )             PORT_DIPLOCATION("SW3:!6")
	PORT_DIPSETTING(        0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000,   0x0000, DEF_STR( Unused ) )         PORT_DIPLOCATION("SW3:!7")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x4000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000,   0x0000, DEF_STR( Unused ) )         PORT_DIPLOCATION("SW3:!8")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x8000, DEF_STR( On ) )

	PORT_START( "EEPROMOUT" )
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, cs_write)
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, di_write)
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, clk_write)
INPUT_PORTS_END



/*
Battle Bakraid
Raizing/8ing, 1999

PCB Layout
----------

ET68-V99
|-----------------------------------------------------|
|TA8201  16.93MHz     ROM-6                   6264    |
|  YAC516                                             |
|       YMZ280B-F     ROM-7               SND_U0720   |
|                                                     |
| VOL                 ROM-8                 Z80       |
|                                                     |
|                   341256                            |
|                                               93C66 |
|                   341256               XILINX       |
|J                                       XC95108      |
|A                  27MHz     32MHz                   |
|M                                                    |
|M          DIPSW1                      341256  341256|
|A                  XILINX    XILINK                  |
|           DIPSW2  XC95144   XC95108   341256  341256|
|                                                     |
|           DIPSW3                                    |
|                                MACH211    PRG1_U023 |
| TEST_SW            68000                            |
|                                           PRG0_U022 |
|                                                     |
|                                           PRG3_U024 |
|                             L7A0498                 |
|                             GP9001        PRG2_U021 |
| ROM-0       ROM-1           (QFP208)                |
|                                                     |
|                               6264       MN414260   |
| ROM-2       ROM-3                                   |
|                               6264       MN414260   |
|-----------------------------------------------------|
Notes:
      ROM-0 to ROM-3 - 32M DIP42
      ROM-6 to ROM-8 - 32M DIP42 Byte Mode
      68000 clock - 16.000MHz (32/2)
      Z80 clock - 5.33333MHz (32/6)
      VSync - 60Hz
      HSync - 15.39kHz
*/



ROM_START( bbakraid )
	ROM_REGION( 0x200000, "maincpu", 0 )            /* Main 68k code */
	ROM_LOAD16_BYTE( "prg0u022_usa.bin", 0x000000, 0x080000, CRC(95fb2ffd) SHA1(c7f502f3945249573b66226e8bacc6a9bc230693) )
	ROM_LOAD16_BYTE( "prg1u023.new", 0x000001, 0x080000, CRC(4ae9aa64) SHA1(45fdf72141c4c9f24a38d4218c65874799b9c868) )
	ROM_LOAD16_BYTE( "prg2u021.bin", 0x100000, 0x080000, CRC(ffba8656) SHA1(6526bb65fad3384de3f301a7d1095cbf03757433) )
	ROM_LOAD16_BYTE( "prg3u024.bin", 0x100001, 0x080000, CRC(834b8ad6) SHA1(0dd6223bb0749819ad29811eeb04fd08d937abb0) )

	ROM_REGION( 0x20000, "audiocpu", 0 )            /* Sound Z80 code */
	ROM_LOAD( "sndu0720.bin", 0x00000, 0x20000, CRC(e62ab246) SHA1(00d23689dd423ecd4024c58b5903d16e890f1dff) )

	ROM_REGION( 0x1000000, "gp9001", 0 )
	ROM_LOAD( "gfxu0510.bin", 0x000000, 0x400000, CRC(9cca3446) SHA1(1123f8b8bfbe59a2c572cdf61f1ad27ff37f0f0d) )
	ROM_LOAD( "gfxu0512.bin", 0x400000, 0x400000, CRC(a2a281d5) SHA1(d9a6623f9433ad682223f9780c26cd1523ebc5c5) )
	ROM_LOAD( "gfxu0511.bin", 0x800000, 0x400000, CRC(e16472c0) SHA1(6068d679a8b3b65e05acd58a7ce9ead90177049f) )
	ROM_LOAD( "gfxu0513.bin", 0xc00000, 0x400000, CRC(8bb635a0) SHA1(9064f1a2d8bb88ddbca702fb8556d0dfe6a5cadc) )

	ROM_REGION( 0x0c00000, "ymz", 0 )       /* YMZ280B Samples */
	ROM_LOAD( "rom6.829", 0x000000, 0x400000, CRC(8848b4a0) SHA1(e0dce136c5d5a4c1a92b863e57848cd5927d06f1) )
	ROM_LOAD( "rom7.830", 0x400000, 0x400000, CRC(d6224267) SHA1(5c9b7b13effbef9f707811f84bfe50ca85e605e3) )
	ROM_LOAD( "rom8.831", 0x800000, 0x400000, CRC(a101dfb0) SHA1(4b729b0d562e09df35438e9e6b457b8de2690a6e) )

	ROM_REGION( 0x200, "eeprom", 0 )
	ROM_LOAD( "eeprom-bbakraid-new.bin", 0x000, 0x200, CRC(35c9275a) SHA1(1282034adf3c7a24545fd273729867058dc93027) )
ROM_END


ROM_START( bbakraidc )
	ROM_REGION( 0x200000, "maincpu", 0 )            /* Main 68k code */
	ROM_LOAD16_BYTE( "prg0u022_china.bin", 0x000000, 0x080000, CRC(760be084) SHA1(096c8a2336492d7370ae25f3385faebf6e9c3eca) )
	ROM_LOAD16_BYTE( "prg1u023.new", 0x000001, 0x080000, CRC(4ae9aa64) SHA1(45fdf72141c4c9f24a38d4218c65874799b9c868) )
	ROM_LOAD16_BYTE( "prg2u021.bin", 0x100000, 0x080000, CRC(ffba8656) SHA1(6526bb65fad3384de3f301a7d1095cbf03757433) )
	ROM_LOAD16_BYTE( "prg3u024.bin", 0x100001, 0x080000, CRC(834b8ad6) SHA1(0dd6223bb0749819ad29811eeb04fd08d937abb0) )

	ROM_REGION( 0x20000, "audiocpu", 0 )            /* Sound Z80 code */
	ROM_LOAD( "sndu0720.bin", 0x00000, 0x20000, CRC(e62ab246) SHA1(00d23689dd423ecd4024c58b5903d16e890f1dff) )

	ROM_REGION( 0x1000000, "gp9001", 0 )
	ROM_LOAD( "gfxu0510.bin", 0x000000, 0x400000, CRC(9cca3446) SHA1(1123f8b8bfbe59a2c572cdf61f1ad27ff37f0f0d) )
	ROM_LOAD( "gfxu0512.bin", 0x400000, 0x400000, CRC(a2a281d5) SHA1(d9a6623f9433ad682223f9780c26cd1523ebc5c5) )
	ROM_LOAD( "gfxu0511.bin", 0x800000, 0x400000, CRC(e16472c0) SHA1(6068d679a8b3b65e05acd58a7ce9ead90177049f) )
	ROM_LOAD( "gfxu0513.bin", 0xc00000, 0x400000, CRC(8bb635a0) SHA1(9064f1a2d8bb88ddbca702fb8556d0dfe6a5cadc) )

	ROM_REGION( 0x0c00000, "ymz", 0 )       /* YMZ280B Samples */
	ROM_LOAD( "rom6.829", 0x000000, 0x400000, CRC(8848b4a0) SHA1(e0dce136c5d5a4c1a92b863e57848cd5927d06f1) )
	ROM_LOAD( "rom7.830", 0x400000, 0x400000, CRC(d6224267) SHA1(5c9b7b13effbef9f707811f84bfe50ca85e605e3) )
	ROM_LOAD( "rom8.831", 0x800000, 0x400000, CRC(a101dfb0) SHA1(4b729b0d562e09df35438e9e6b457b8de2690a6e) )

	ROM_REGION( 0x200, "eeprom", 0 )
	ROM_LOAD( "eeprom-bbakraid-new.bin", 0x000, 0x200, CRC(35c9275a) SHA1(1282034adf3c7a24545fd273729867058dc93027) )
ROM_END


ROM_START( bbakraidj )
	ROM_REGION( 0x200000, "maincpu", 0 )            /* Main 68k code */
	ROM_LOAD16_BYTE( "prg0u022.new", 0x000000, 0x080000, CRC(fa8d38d3) SHA1(aba91d87a8a62d3fe1139b4437b16e2f844264ad) )
	ROM_LOAD16_BYTE( "prg1u023.new", 0x000001, 0x080000, CRC(4ae9aa64) SHA1(45fdf72141c4c9f24a38d4218c65874799b9c868) )
	ROM_LOAD16_BYTE( "prg2u021.bin", 0x100000, 0x080000, CRC(ffba8656) SHA1(6526bb65fad3384de3f301a7d1095cbf03757433) )
	ROM_LOAD16_BYTE( "prg3u024.bin", 0x100001, 0x080000, CRC(834b8ad6) SHA1(0dd6223bb0749819ad29811eeb04fd08d937abb0) )

	ROM_REGION( 0x20000, "audiocpu", 0 )            /* Sound Z80 code */
	ROM_LOAD( "sndu0720.bin", 0x00000, 0x20000, CRC(e62ab246) SHA1(00d23689dd423ecd4024c58b5903d16e890f1dff) )

	ROM_REGION( 0x1000000, "gp9001", 0 )
	ROM_LOAD( "gfxu0510.bin", 0x000000, 0x400000, CRC(9cca3446) SHA1(1123f8b8bfbe59a2c572cdf61f1ad27ff37f0f0d) )
	ROM_LOAD( "gfxu0512.bin", 0x400000, 0x400000, CRC(a2a281d5) SHA1(d9a6623f9433ad682223f9780c26cd1523ebc5c5) )
	ROM_LOAD( "gfxu0511.bin", 0x800000, 0x400000, CRC(e16472c0) SHA1(6068d679a8b3b65e05acd58a7ce9ead90177049f) )
	ROM_LOAD( "gfxu0513.bin", 0xc00000, 0x400000, CRC(8bb635a0) SHA1(9064f1a2d8bb88ddbca702fb8556d0dfe6a5cadc) )

	ROM_REGION( 0x0c00000, "ymz", 0 )       /* YMZ280B Samples */
	ROM_LOAD( "rom6.829", 0x000000, 0x400000, CRC(8848b4a0) SHA1(e0dce136c5d5a4c1a92b863e57848cd5927d06f1) )
	ROM_LOAD( "rom7.830", 0x400000, 0x400000, CRC(d6224267) SHA1(5c9b7b13effbef9f707811f84bfe50ca85e605e3) )
	ROM_LOAD( "rom8.831", 0x800000, 0x400000, CRC(a101dfb0) SHA1(4b729b0d562e09df35438e9e6b457b8de2690a6e) )

	ROM_REGION( 0x200, "eeprom", 0 )
	ROM_LOAD( "eeprom-bbakraid-new.bin", 0x000, 0x200, CRC(35c9275a) SHA1(1282034adf3c7a24545fd273729867058dc93027) )
ROM_END


ROM_START( bbakraidja )
	ROM_REGION( 0x200000, "maincpu", 0 )            /* Main 68k code */
	ROM_LOAD16_BYTE( "prg0u022.bin", 0x000000, 0x080000, CRC(0dd59512) SHA1(c6a4e6aa49c6ac3b04ae62a0a4cc8084ae048381) )
	ROM_LOAD16_BYTE( "prg1u023.bin", 0x000001, 0x080000, CRC(fecde223) SHA1(eb5ac0eda49b4b0f3d25d8a8bb356e77a453d3a7) )
	ROM_LOAD16_BYTE( "prg2u021.bin", 0x100000, 0x080000, CRC(ffba8656) SHA1(6526bb65fad3384de3f301a7d1095cbf03757433) )
	ROM_LOAD16_BYTE( "prg3u024.bin", 0x100001, 0x080000, CRC(834b8ad6) SHA1(0dd6223bb0749819ad29811eeb04fd08d937abb0) )

	ROM_REGION( 0x20000, "audiocpu", 0 )            /* Sound Z80 code */
	ROM_LOAD( "sndu0720.bin", 0x00000, 0x20000, CRC(e62ab246) SHA1(00d23689dd423ecd4024c58b5903d16e890f1dff) )

	ROM_REGION( 0x1000000, "gp9001", 0 )
	ROM_LOAD( "gfxu0510.bin", 0x000000, 0x400000, CRC(9cca3446) SHA1(1123f8b8bfbe59a2c572cdf61f1ad27ff37f0f0d) )
	ROM_LOAD( "gfxu0512.bin", 0x400000, 0x400000, CRC(a2a281d5) SHA1(d9a6623f9433ad682223f9780c26cd1523ebc5c5) )
	ROM_LOAD( "gfxu0511.bin", 0x800000, 0x400000, CRC(e16472c0) SHA1(6068d679a8b3b65e05acd58a7ce9ead90177049f) )
	ROM_LOAD( "gfxu0513.bin", 0xc00000, 0x400000, CRC(8bb635a0) SHA1(9064f1a2d8bb88ddbca702fb8556d0dfe6a5cadc) )

	ROM_REGION( 0x0c00000, "ymz", 0 )       /* YMZ280B Samples */
	ROM_LOAD( "rom6.829", 0x000000, 0x400000, CRC(8848b4a0) SHA1(e0dce136c5d5a4c1a92b863e57848cd5927d06f1) )
	ROM_LOAD( "rom7.830", 0x400000, 0x400000, CRC(d6224267) SHA1(5c9b7b13effbef9f707811f84bfe50ca85e605e3) )
	ROM_LOAD( "rom8.831", 0x800000, 0x400000, CRC(a101dfb0) SHA1(4b729b0d562e09df35438e9e6b457b8de2690a6e) )

	ROM_REGION( 0x200, "eeprom", 0 )
	ROM_LOAD( "eeprom-bbakraid.bin", 0x000, 0x200, CRC(7f97d347) SHA1(3096c399019924dbb7d6673483f6a011f89467c6) )
ROM_END


// Battle Bakraid
// the 'unlimited' version is a newer revision of the code
GAME( 1999, bbakraid,    0,        bbakraid,   bbakraid,   bbakraid_state, init_bbakraid,   ROT270, "Eighting", "Battle Bakraid - Unlimited Version (USA) (Tue Jun 8 1999)",   MACHINE_SUPPORTS_SAVE )
GAME( 1999, bbakraidc,   bbakraid, bbakraid,   bbakraid,   bbakraid_state, init_bbakraid,   ROT270, "Eighting", "Battle Bakraid - Unlimited Version (China) (Tue Jun 8 1999)", MACHINE_SUPPORTS_SAVE )
GAME( 1999, bbakraidj,   bbakraid, bbakraid,   bbakraid,   bbakraid_state, init_bbakraid,   ROT270, "Eighting", "Battle Bakraid - Unlimited Version (Japan) (Tue Jun 8 1999)", MACHINE_SUPPORTS_SAVE )
// older revision of the code
GAME( 1999, bbakraidja,  bbakraid, bbakraid,   bbakraid,   bbakraid_state, init_bbakraid,   ROT270, "Eighting", "Battle Bakraid (Japan) (Wed Apr 7 1999)", MACHINE_SUPPORTS_SAVE )
