
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



class fixeight_state : public truxton2_state
{
public:
	fixeight_state(const machine_config &mconfig, device_type type, const char *tag)
		: truxton2_state(mconfig, type, tag)
	{ }

	void fixeight(machine_config &config);
	void fixeightbl(machine_config &config);

	void init_fixeight();
	void init_fixeightbl();

protected:
private:

	void fixeight_68k_mem(address_map &map);
	void fixeightbl_68k_mem(address_map &map);
	void fixeight_v25_mem(address_map &map);
	void cpu_space_fixeightbl_map(address_map &map);
	void fixeightbl_oki_bankswitch_w(u8 data);
	void fixeightbl_oki(address_map &map);

	void sound_reset_w(u8 data);

	u8 m_sound_reset_bit = 0; /* 0x20 for dogyuun/batsugun, 0x10 for vfive, 0x08 for fixeight */
	void tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	DECLARE_VIDEO_START(truxton2);

	DECLARE_VIDEO_START(fixeightbl);

	void tx_videoram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	u8 shared_ram_r(offs_t offset) { return m_shared_ram[offset]; }
	void shared_ram_w(offs_t offset, u8 data) { m_shared_ram[offset] = data; }


	DECLARE_VIDEO_START(toaplan2);
	u32 screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_vblank(int state);
	u32 screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_bootleg(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void create_tx_tilemap(int dx = 0, int dx_flipped = 0);
	u16 video_count_r();

	bitmap_ind8 m_custom_priority_bitmap;

};


u16 fixeight_state::video_count_r()
{
	/* +---------+---------+--------+---------------------------+ */
	/* | /H-Sync | /V-Sync | /Blank |       Scanline Count      | */
	/* | Bit 15  | Bit 14  | Bit 8  |  Bit 7-0 (count from #EF) | */
	/* +---------+---------+--------+---------------------------+ */
	/*************** Control Signals are active low ***************/

	int vpos = m_screen->vpos();

	u16 video_status = 0xff00;    // Set signals inactive

	vpos = (vpos + 15) % 262;

	if (!m_vdp->hsync_r())
		video_status &= ~0x8000;
	if (!m_vdp->vsync_r())
		video_status &= ~0x4000;
	if (!m_vdp->fblank_r())
		video_status &= ~0x0100;
	if (vpos < 256)
		video_status |= (vpos & 0xff);
	else
		video_status |= 0xff;

//  logerror("VC: vpos=%04x hpos=%04x VBL=%04x\n",vpos,hpos,m_screen->vblank());

	return video_status;
}


void fixeight_state::create_tx_tilemap(int dx, int dx_flipped)
{
	m_tx_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(truxton2_state::get_text_tile_info)), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_tx_tilemap->set_scroll_rows(8*32); /* line scrolling */
	m_tx_tilemap->set_scroll_cols(1);
	m_tx_tilemap->set_scrolldx(dx, dx_flipped);
	m_tx_tilemap->set_transparent_pen(0);
}


/* fixeightbl and bgareggabl do not use the lineselect or linescroll tables */
u32 fixeight_state::screen_update_bootleg(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	screen_update_toaplan2(screen, bitmap, cliprect);
	m_tx_tilemap->draw(screen, bitmap, cliprect, 0);
	return 0;
}



VIDEO_START_MEMBER(fixeight_state,toaplan2)
{
	/* our current VDP implementation needs this bitmap to work with */
	m_screen->register_screen_bitmap(m_custom_priority_bitmap);
	m_vdp->custom_priority_bitmap = &m_custom_priority_bitmap;
}


u32 fixeight_state::screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp->render_vdp(bitmap, cliprect);

	return 0;
}

void fixeight_state::screen_vblank(int state)
{
	// rising edge
	if (state)
	{
		m_vdp->screen_eof();
	}
}


VIDEO_START_MEMBER(fixeight_state,truxton2)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	m_gfxdecode->gfx(0)->set_source(reinterpret_cast<u8 *>(m_tx_gfxram.target()));

	create_tx_tilemap(0x1d5, 0x16a);
}



VIDEO_START_MEMBER(fixeight_state,fixeightbl)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	create_tx_tilemap();

	/* This bootleg has additional layer offsets on the VDP */
	m_vdp->set_tm_extra_offsets(0, -0x1d6 - 26, -0x1ef - 15, 0, 0);
	m_vdp->set_tm_extra_offsets(1, -0x1d8 - 22, -0x1ef - 15, 0, 0);
	m_vdp->set_tm_extra_offsets(2, -0x1da - 18, -0x1ef - 15, 0, 0);
	m_vdp->set_sp_extra_offsets(8/*-0x1cc - 64*/, 8/*-0x1ef - 128*/, 0, 0);

	m_vdp->init_scroll_regs();
}


static INPUT_PORTS_START( fixeight )
	// The Suicide buttons are technically P1 and P2 Button 3, but we hook
	// them up as IPT_OTHER so each player has the same number of buttons.
	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("P1 Suicide (Cheat)")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Unknown/Unused

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("P2 Suicide (Cheat)")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Unknown/Unused

	PORT_START("IN3")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_START3 )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_MEMORY_RESET ) PORT_NAME("Region Reset")
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Unknown/Unused

	PORT_START("SYS")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_SERVICE_NO_TOGGLE(0x0004, IP_ACTIVE_HIGH)  // Service input is a pushbutton marked 'Test SW'
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Unknown/Unused

	PORT_START("EEPROM")
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, cs_write)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, clk_write)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, di_write)
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_READ_LINE_DEVICE_MEMBER("eeprom", eeprom_serial_93cxx_device, do_read)
INPUT_PORTS_END


static INPUT_PORTS_START( toaplan2_2b )
	PORT_START("IN1")
	TOAPLAN_JOY_UDLR_2_BUTTONS( 1 )

	PORT_START("IN2")
	TOAPLAN_JOY_UDLR_2_BUTTONS( 2 )

	PORT_START("SYS")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_TILT )
	TOAPLAN_TEST_SWITCH( 0x04, IP_ACTIVE_HIGH )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("DSWA")
	TOAPLAN_MACHINE_NO_COCKTAIL_LOC(SW1)
	// Coinage on bit mask 0x00f0
	PORT_BIT( 0x00f0, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Modified below

	PORT_START("DSWB")
	TOAPLAN_DIFFICULTY_LOC(SW2)
	// Per-game features on bit mask 0x00fc
	PORT_BIT( 0x00fc, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Modified below
INPUT_PORTS_END

static INPUT_PORTS_START( fixeightbl )
	PORT_INCLUDE( toaplan2_2b )

	PORT_MODIFY("SYS")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_SERVICE_NO_TOGGLE(0x0004, IP_ACTIVE_HIGH)  // Service input is a pushbutton marked 'Test SW'

	PORT_START("IN3")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(3) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_START3 )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // Unknown/Unused

	PORT_MODIFY("DSWA")
	PORT_DIPNAME( 0x0001,   0x0000, "Maximum Players" )     PORT_DIPLOCATION("SW1:!1")
	PORT_DIPSETTING(        0x0000, "2" )
	PORT_DIPSETTING(        0x0001, "3" )
	PORT_DIPNAME( 0x0002,   0x0000, DEF_STR( Unused ) )     PORT_DIPLOCATION("SW1:!2")  // This video HW doesn't support flip screen
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004,   0x0004, "Shooting Style" )      PORT_DIPLOCATION("SW1:!3")
	PORT_DIPSETTING(        0x0004, "Semi-Auto" )
	PORT_DIPSETTING(        0x0000, "Full-Auto" )
	// Various features on bit mask 0x0008 - see above
	TOAPLAN_COINAGE_JAPAN_LOC(SW1)

	PORT_MODIFY("DSWB")
	// Difficulty on bit mask 0x0003 - see above
	PORT_DIPNAME( 0x000c,   0x0000, DEF_STR( Bonus_Life ) )     PORT_DIPLOCATION("SW2:!3,!4")
	PORT_DIPSETTING(        0x000c, DEF_STR( None ) )
	PORT_DIPSETTING(        0x0000, "500k and every 500k" )
	PORT_DIPSETTING(        0x0008, "300k only" )
	PORT_DIPSETTING(        0x0004, "300k and every 300k" )
	PORT_DIPNAME( 0x0030,   0x0000, DEF_STR( Lives ) )          PORT_DIPLOCATION("SW2:!5,!6")
	PORT_DIPSETTING(        0x0030, "1" )
	PORT_DIPSETTING(        0x0020, "2" )
	PORT_DIPSETTING(        0x0000, "3" )
	PORT_DIPSETTING(        0x0010, "5" )
	PORT_DIPNAME( 0x0040,   0x0000, "Invulnerability (Cheat)" ) PORT_DIPLOCATION("SW2:!7")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0040, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080,   0x0000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:!8")
	PORT_DIPSETTING(        0x0080, DEF_STR( No ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( Yes ) )
INPUT_PORTS_END


/* x = modified to match batsugun 'unencrypted' code - '?' likewise, but not so sure about them */
/* e = opcodes used in the EEPROM service routine */
/* this one seems more different to the other tables */
static const u8 ts001turbo_decryption_table[256] = {
	0x90,0x05,0x57,0x5f,0xfe,0x4f,0xbd,0x36, 0x80,0x8b,0x8a,0x0a,0x89,0x90,0x47,0x80, /* 00 */
			/*r*//*r*//*r*//*r*//*r*//*r*//*r*/ /*r*//*r*//*r*//*r*//*r*/     /*r*//*r*/
	0x22,0x90,0x90,0x5d,0x81,0x3c,0xb5,0x83, 0x68,0xff,0x75,0x75,0x8d,0x5b,0x8a,0x38, /* 10 */
	/*r*/          /*r*//*r*//*r*//*r*//*r*/ /*r*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x8b,0xeb,0xd2,0x0a,0xb4,0xc7,0x46,0xd1, 0x0a,0x53,0xbd,0x77,0x22,0xff,0x1f,0x03, /* 20 */
	/*a*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/ /*r*//*r*//*r*//*e*//*r*//*r*//*?*//*r*/
	0xfb,0x45,0xc3,0x02,0x90,0x0f,0xa3,0x02, 0x0f,0xb7,0x90,0x24,0xc6,0xeb,0x1b,0x32, /* 30 */
	/*r*//*r*//*r*//*r*/     /*r*//*e*//*r*/ /*r*//*r*/     /*r*//*r*//*r*//*r*//*r*/
	0x8d,0xb9,0xfe,0x08,0x88,0x90,0x8a,0x8a, 0x75,0x8a,0xbd,0x58,0xfe,0x51,0x1e,0x8b, /* 40 */
	/*r*//*r*//*r*//*r*//*r*/     /*r*//*r*/ /*r*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x0f,0x22,0xf6,0x90,0xc3,0x36,0x03,0x8d, 0xbb,0x16,0xbc,0x90,0x0f,0x5e,0xf9,0x2e, /* 50 */
	/*r*//*r*//*r*/     /*r*//*r*//*r*//*r*/ /*r*//*?*//*r*/     /*r*//*r*//*r*//*r*/
	0x90,0x90,0x59,0x90,0xbb,0x1a,0x0c,0x8d, 0x89,0x72,0x83,0xa4,0xc3,0xb3,0x8b,0xe9, /* 60 */
				/*r*/     /*r*//*r*//*r*//*r*/ /*a*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x81,0x43,0xa0,0x2c,0x0f,0x55,0xf3,0x36, 0xb0,0x59,0xe8,0x03,0x26,0xe9,0x22,0xb0, /* 70 */
	/*r*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/ /*r*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x90,0x8e,0x24,0x8a,0xd0,0x3e,0xc3,0x3a, 0x90,0x79,0x57,0x16,0x88,0x86,0x24,0x74, /* 80 */
			/*r*//*r*//*r*//*r*//*r*//*r*//*r*/      /*a*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x33,0xc3,0x53,0xb8,0xab,0x75,0x90,0x90, 0x8e,0xb1,0xe9,0x5d,0xf9,0x02,0x3c,0x90, /* 90 */
	/*x*//*r*//*r*//*r*//*r*//*r*/           /*r*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x80,0xd3,0x89,0xe8,0x90,0x90,0x2a,0x74, 0x90,0x5f,0xf6,0x88,0x4f,0x56,0x8c,0x03, /* a0 */
	/*r*//*a*//*r*//*r*/          /*r*//*r*/      /*r*//*r*//*r*//*r*//*r*//*r*//*r*/
	0x47,0xa1,0x88,0x90,0x03,0xfe,0x90,0xfc, 0x2a,0x90,0x33,0x07,0xb1,0x50,0x0f,0x3e, /* b0 */
	/*r*//*e*//*r*/     /*r*//*r*/     /*r*/ /*r*/     /*r*//*r*//*r*//*r*//*r*//*r*/
	0xbd,0x4d,0xf3,0xbf,0x59,0xd2,0xea,0xc6, 0x2a,0x74,0x72,0xe2,0x3e,0x2e,0x90,0x2e, /* c0 */
	/*r*//*r*//*r*//*r*//*r*//*a*//*x*//*r*/ /*r*//*r*//*r*//*r*//*r*//*r*/     /*r*/
	0x2e,0x73,0x88,0x72,0x45,0x5d,0xc1,0xb9, 0x32,0x38,0x88,0xc1,0xa0,0x06,0x45,0x90, /* d0 */
	/*r*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/ /*r*//*r*//*r*//*r*//*a*//*r*//*r*/
	0x90,0x86,0x4b,0x87,0x90,0x8a,0x3b,0xab, 0x33,0xbe,0x90,0x32,0xbd,0xc7,0xb2,0x80, /* e0 */
			/*r*//*r*//*r*/     /*r*//*?*//*r*/ /*r*//*r*/     /*r*//*r*//*r*//*?*//*r*/
	0x0f,0x75,0xc0,0xb9,0x07,0x74,0x3e,0xa2, 0x8a,0x48,0x3e,0x8d,0xeb,0x90,0xfe,0x90, /* f0 */
	/*r*//*r*//*r*//*r*//*r*//*r*//*r*//*r*/ /*r*//*x*//*r*//*r*//*r*/     /*r*/
};

#define XOR(a) WORD_XOR_LE(a)
#define LOC(x) (x+XOR(0))

static const gfx_layout truxton2_tx_tilelayout =
{
	8,8,    /* 8x8 characters */
	1024,   /* 1024 characters */
	4,      /* 4 bits per pixel */
	{ STEP4(0,1) },
	{ LOC(0)*4, LOC(1)*4, LOC(4)*4, LOC(5)*4, LOC(8)*4, LOC(9)*4, LOC(12)*4, LOC(13)*4 },
	{ STEP8(0,8*8) },
	8*8*8
};


static GFXDECODE_START( gfx_truxton2 )
	GFXDECODE_ENTRY( nullptr, 0, truxton2_tx_tilelayout, 64*16, 64 )
GFXDECODE_END

static GFXDECODE_START( gfx_textrom )
	GFXDECODE_ENTRY( "text", 0, gfx_8x8x4_packed_msb, 64*16, 64 )
GFXDECODE_END


void fixeight_state::sound_reset_w(u8 data)
{
	m_audiocpu->set_input_line(INPUT_LINE_RESET, (data & m_sound_reset_bit) ? CLEAR_LINE : ASSERT_LINE);
}

void fixeight_state::tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Dynamic GFX decoding for Truxton 2 / FixEight ***/

	const u16 oldword = m_tx_gfxram[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&m_tx_gfxram[offset]);
		m_gfxdecode->gfx(0)->mark_dirty(offset/32);
	}
}


void fixeight_state::tx_videoram_w(offs_t offset, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_tx_videoram[offset]);
	if (offset < 64*32)
		m_tx_tilemap->mark_tile_dirty(offset);
}

void fixeight_state::tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Line-Scroll RAM for Text Layer ***/
	COMBINE_DATA(&m_tx_linescroll[offset]);

	m_tx_tilemap->set_scrollx(offset, m_tx_linescroll[offset]);
}

u32 fixeight_state::screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	screen_update_toaplan2(screen, bitmap, cliprect);
	rectangle clip = cliprect;
	m_tx_tilemap->set_flip(m_tx_lineselect[0] & 0x8000 ? 0 : TILEMAP_FLIPX);
	for (int y = cliprect.min_y; y <= cliprect.max_y; y++)
	{
		clip.min_y = clip.max_y = y;
		m_tx_tilemap->set_scrolly(0, m_tx_lineselect[y] - y);
		m_tx_tilemap->draw(screen, bitmap, clip, 0);
	}
	return 0;
}


void fixeight_state::fixeight_68k_mem(address_map &map)
{
	map(0x000000, 0x07ffff).rom();
	map(0x100000, 0x103fff).ram();
	map(0x200000, 0x200001).portr("IN1");
	map(0x200004, 0x200005).portr("IN2");
	map(0x200008, 0x200009).portr("IN3");
	map(0x200010, 0x200011).portr("SYS");
	map(0x20001d, 0x20001d).w(FUNC(truxton2_state::coin_w));
	map(0x280000, 0x28ffff).rw(FUNC(fixeight_state::shared_ram_r), FUNC(fixeight_state::shared_ram_w)).umask16(0x00ff);
	map(0x300000, 0x30000d).rw(m_vdp, FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x400000, 0x400fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x500000, 0x501fff).ram().w(FUNC(fixeight_state::tx_videoram_w)).share(m_tx_videoram);
	map(0x502000, 0x5021ff).ram().share(m_tx_lineselect);
	map(0x503000, 0x5031ff).ram().w(FUNC(fixeight_state::tx_linescroll_w)).share(m_tx_linescroll);
	map(0x600000, 0x60ffff).ram().w(FUNC(fixeight_state::tx_gfxram_w)).share(m_tx_gfxram);
	map(0x700000, 0x700001).w(FUNC(fixeight_state::sound_reset_w)).umask16(0x00ff).cswidth(16);
	map(0x800000, 0x800001).r(FUNC(fixeight_state::video_count_r));
}


void fixeight_state::fixeightbl_68k_mem(address_map &map)
{
	map(0x000000, 0x0fffff).rom();     // 0-7ffff ?
	map(0x100000, 0x10ffff).ram();     // 100000-107fff  105000-105xxx  106000-106xxx  108000 - related to sound ?
	map(0x200000, 0x200001).portr("IN1");
	map(0x200004, 0x200005).portr("IN2");
	map(0x200008, 0x200009).portr("IN3");
	map(0x20000c, 0x20000d).portr("DSWB");
	map(0x200010, 0x200011).portr("SYS");
	map(0x200015, 0x200015).w(FUNC(fixeight_state::fixeightbl_oki_bankswitch_w));  // Sound banking. Code at $4084c, $5070
	map(0x200019, 0x200019).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0x20001c, 0x20001d).portr("DSWA");
	map(0x300000, 0x30000d).rw(m_vdp, FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x400000, 0x400fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x500000, 0x501fff).ram().w(FUNC(fixeight_state::tx_videoram_w)).share(m_tx_videoram);
	map(0x700000, 0x700001).r(FUNC(fixeight_state::video_count_r));
	map(0x800000, 0x87ffff).rom().region("maincpu", 0x80000);
}

void fixeight_state::fixeight_v25_mem(address_map &map)
{
	map(0x00000, 0x00000).portr("IN1");
	map(0x00002, 0x00002).portr("IN2");
	map(0x00004, 0x00004).portr("IN3");
	map(0x0000a, 0x0000b).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x0000c, 0x0000c).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0x80000, 0x87fff).mirror(0x78000).ram().share(m_shared_ram);
}

void fixeight_state::fixeight(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 16_MHz_XTAL);         // verified on PCB
	m_maincpu->set_addrmap(AS_PROGRAM, &fixeight_state::fixeight_68k_mem);
	m_maincpu->reset_cb().set(FUNC(truxton2_state::toaplan2_reset));

	v25_device &audiocpu(V25(config, m_audiocpu, 16_MHz_XTAL));           // NEC V25 type Toaplan marked CPU ???
	audiocpu.set_addrmap(AS_PROGRAM, &fixeight_state::fixeight_v25_mem);
	audiocpu.set_decryption_table(ts001turbo_decryption_table);
	audiocpu.p0_in_cb().set_ioport("EEPROM");
	audiocpu.p0_out_cb().set_ioport("EEPROM");

	EEPROM_93C46_16BIT(config, m_eeprom);

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	m_screen->set_raw(27_MHz_XTAL/4, 432, 0, 320, 262, 0, 240); // verified on PCB
	m_screen->set_screen_update(FUNC(fixeight_state::screen_update_truxton2));
	m_screen->screen_vblank().set(FUNC(fixeight_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_truxton2);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 0x10000);

	GP9001_VDP(config, m_vdp, 27_MHz_XTAL);
	m_vdp->set_palette(m_palette);
	m_vdp->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_4);

	MCFG_VIDEO_START_OVERRIDE(fixeight_state,truxton2)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	YM2151(config, "ymsnd", 27_MHz_XTAL/8).add_route(ALL_OUTPUTS, "mono", 0.5); /* verified on pcb */

	OKIM6295(config, m_oki[0], 16_MHz_XTAL/16, okim6295_device::PIN7_HIGH); /* verified on pcb */
	m_oki[0]->add_route(ALL_OUTPUTS, "mono", 0.5);
}

void fixeight_state::cpu_space_fixeightbl_map(address_map &map)
{
	map(0xfffff0, 0xffffff).m(m_maincpu, FUNC(m68000_base_device::autovectors_map));
	map(0xfffff5, 0xfffff5).lr8(NAME([this] () { m_maincpu->set_input_line(M68K_IRQ_2, CLEAR_LINE); return m68000_device::autovector(2); }));
}

void fixeight_state::fixeightbl_oki_bankswitch_w(u8 data)
{
	data &= 7;
	if (data <= 4) m_okibank->set_entry(data);
}

void fixeight_state::fixeightbl_oki(address_map &map)
{
	map(0x00000, 0x2ffff).rom();
	map(0x30000, 0x3ffff).bankr(m_okibank);
}


void fixeight_state::fixeightbl(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, XTAL(10'000'000));         /* 10MHz Oscillator */
	m_maincpu->set_addrmap(AS_PROGRAM, &fixeight_state::fixeightbl_68k_mem);
	m_maincpu->set_addrmap(m68000_base_device::AS_CPU_SPACE, &fixeight_state::cpu_space_fixeightbl_map);
	m_maincpu->reset_cb().set(FUNC(truxton2_state::toaplan2_reset));

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	m_screen->set_raw(27_MHz_XTAL/4, 432, 0, 320, 262, 0, 240);
	//m_screen->set_refresh_hz(60);
	//m_screen->set_size(432, 262);
	//m_screen->set_visarea(0, 319, 0, 239);
	m_screen->set_screen_update(FUNC(fixeight_state::screen_update_bootleg));
	m_screen->screen_vblank().set(FUNC(fixeight_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_textrom);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 0x10000);

	GP9001_VDP(config, m_vdp, 27_MHz_XTAL);
	m_vdp->set_palette(m_palette);
	m_vdp->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_2, ASSERT_LINE);

	MCFG_VIDEO_START_OVERRIDE(fixeight_state,fixeightbl)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	OKIM6295(config, m_oki[0], 14_MHz_XTAL/16, okim6295_device::PIN7_LOW);
	m_oki[0]->add_route(ALL_OUTPUTS, "mono", 1.0);
	m_oki[0]->set_addrmap(0, &fixeight_state::fixeightbl_oki);
}


void fixeight_state::init_fixeight()
{
	m_sound_reset_bit = 0x08;
}

void fixeight_state::init_fixeightbl()
{
	u8 *ROM = memregion("oki1")->base();

	m_okibank->configure_entries(0, 5, &ROM[0x30000], 0x10000);
}



#define ROMS_FIXEIGHT \
	ROM_REGION( 0x080000, "maincpu", 0 ) \
	ROM_LOAD16_WORD_SWAP( "tp-026-1", 0x000000, 0x080000, CRC(f7b1746a) SHA1(0bbea6f111b818bc9b9b2060af4fe900f37cf7f9) ) \
	ROM_REGION( 0x400000, "gp9001", 0 ) \
	ROM_LOAD( "tp-026-3", 0x000000, 0x200000, CRC(e5578d98) SHA1(280d2b716d955e767d311fc9596823852435b6d7) ) \
	ROM_LOAD( "tp-026-4", 0x200000, 0x200000, CRC(b760cb53) SHA1(bc9c5e49e45cdda0f774be0038aa4deb21d4d285) ) \
	ROM_REGION( 0x40000, "oki1", 0 ) \
	ROM_LOAD( "tp-026-2", 0x00000, 0x40000, CRC(85063f1f) SHA1(1bf4d77494de421c98f6273b9876e60d827a6826) )

// note you may need to byteswap these EEPROM files to reprogram the original chip, this is the same for many supported in MAME.

ROM_START( fixeightkt )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightkt.nv", 0x00, 0x80, CRC(08fa73ba) SHA1(b7761d3dd3f4485e55c8ef2cf1a840ca771ee2fc) )
ROM_END

ROM_START( fixeightk )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightk.nv", 0x00, 0x80, CRC(cac91c6f) SHA1(55b284f081753d60abff63493094322756b7f0c5) )
ROM_END

ROM_START( fixeightht )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightht.nv", 0x00, 0x80, CRC(57edaa51) SHA1(b8d50e82590b8cbbbcafec5f9cfbc91e4c286db5) )
ROM_END

ROM_START( fixeighth )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeighth.nv", 0x00, 0x80, CRC(95dec584) SHA1(1c309074b51da5a5263dee00403296946e41067b) )
ROM_END

ROM_START( fixeighttwt )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeighttwt.nv", 0x00, 0x80, CRC(b6d5c06c) SHA1(7fda380ac6835a983c57d093ccad7bd76893c9ba))
ROM_END

ROM_START( fixeighttw )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeighttw.nv", 0x00, 0x80, CRC(74e6afb9) SHA1(87bdc95eb0d2d54375de2c622557d503e14154be))
ROM_END

ROM_START( fixeightat )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightat.nv", 0x00, 0x80,CRC(e9c21987) SHA1(7f699e38deb84902ed62b857a3d2b4e3ea1475bb) )
ROM_END

ROM_START( fixeighta )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeighta.nv", 0x00, 0x80, CRC(2bf17652) SHA1(4ec6f188e63610d258cd6b2432d2200d61d80bed))
ROM_END

ROM_START( fixeightt )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightt.nv", 0x00, 0x80, CRC(c0da4a05) SHA1(3686161244e3e8be0e2fdb5fc5c24e39a7aeba85) )
ROM_END

ROM_START( fixeight )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeight.nv", 0x00, 0x80, CRC(02e925d0) SHA1(5839d10aceff84916ea99e9c6afcdc90eef7468b) )
ROM_END

ROM_START( fixeightut )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightut.nv", 0x00, 0x80, CRC(9fcd93ee) SHA1(4f2750f09d9b8ff358a2fd6c7a4a8ba6de67017a) )
ROM_END

ROM_START( fixeightu )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightu.nv", 0x00, 0x80, CRC(5dfefc3b) SHA1(5203525c58e2ae10575af2e277a5696bd64c5b60) )
ROM_END

ROM_START( fixeightj )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightj.nv", 0x00, 0x80, CRC(21e22038) SHA1(29fb10061e62799bb5e4171e144daac49f0cdf06) )
ROM_END

ROM_START( fixeightjt )
	ROMS_FIXEIGHT
	ROM_REGION16_BE( 0x80, "eeprom", 0 )
	ROM_LOAD( "fixeightjt.nv", 0x00, 0x80, CRC(e3d14fed) SHA1(ee4982ef195240c5eaa5005ca1d591901fb01b47) )
ROM_END


/*
Fix Eight (bootleg)
Toaplan, 1992

PCB Layout
----------

|--------------------------------------------|
|   1.BIN        PAL               14MHz  PAL|
|   M6295        PAL                         |
|   PAL     6116 4.BIN          681000 681000|
|           6116                             |
|           6116                681000 681000|
|J          6116        PAL                  |
|A                             PAL           |
|M                                           |
|M   62256  62256              PAL           |
|A   2.BIN  3.BIN       PAL                  |
|                       PAL                  |
|       68000           PAL                  |
| DSW2        |------|  5.BIN                |
| DSW1   6264 |TPC   |                       |
| 3.579545MHz |1020  |  6.BIN                |
| 10MHz  6264 |------|  7.BIN                |
|--------------------------------------------|
Notes:
      68000 clock at 10.000MHz
      M6295 clock at 875kHz [14M/16]. Sample rate = 875000 / 165
      VSync at 60Hz
      6116  - 2k   x8 SRAM (x4)
      6264  - 8k   x8 SRAM (x2)
      62256 - 32k  x8 SRAM (x2)
      681000- 128k x8 SRAM (x4)
*/


ROM_START( fixeightbl )
	ROM_REGION( 0x100000, "maincpu", 0 )            /* Main 68K code */
	ROM_LOAD16_BYTE( "3.bin", 0x000000, 0x80000, CRC(cc77d4b4) SHA1(4d3376cbae13d90c6314d8bb9236c2183fc6253c) )
	ROM_LOAD16_BYTE( "2.bin", 0x000001, 0x80000, CRC(ed715488) SHA1(37be9bc8ff6b54a1f660d89469c6c2da6301e9cd) )

	ROM_REGION( 0x400000, "gp9001", 0 )
	ROM_LOAD( "tp-026-3", 0x000000, 0x200000, CRC(e5578d98) SHA1(280d2b716d955e767d311fc9596823852435b6d7) )
	ROM_LOAD( "tp-026-4", 0x200000, 0x200000, CRC(b760cb53) SHA1(bc9c5e49e45cdda0f774be0038aa4deb21d4d285) )

	ROM_REGION( 0x08000, "text", 0)
	ROM_LOAD( "4.bin", 0x00000, 0x08000, CRC(a6aca465) SHA1(2b331faeee1832e0adc5218254a99d66331862c6) )

	ROM_REGION( 0x80000, "oki1", 0 )         /* ADPCM Samples */
	ROM_LOAD( "1.bin", 0x00000, 0x80000, CRC(888f19ac) SHA1(d2f4f8b7be7a0fdb95baa0af8930e50e2f875c05) )

	ROM_REGION( 0x8000, "user1", 0 )            /* ??? Some sort of table  - same as in pipibibsbl */
	ROM_LOAD( "5.bin", 0x0000, 0x8000, CRC(456dd16e) SHA1(84779ee64d3ea33ba1ba4dee39b504a81c6811a1) )
ROM_END



// region is in eeprom (and also requires correct return value from a v25 mapped address??)
GAME( 1992, fixeight,    0,        fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight (Europe)",                                          MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightk,   fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight (Korea)",                                           MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeighth,   fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight (Hong Kong)",                                       MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeighttw,  fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight (Taiwan)",                                          MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeighta,   fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight (Southeast Asia)",                                  MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightu,   fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight (USA)",                                             MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightj,   fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan",                 "FixEight - Jigoku no Eiyuu Densetsu (Japan)",                MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightt,   fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight (Europe, Taito license)",                           MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightkt,  fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight (Korea, Taito license)",                            MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightht,  fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight (Hong Kong, Taito license)",                        MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeighttwt, fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight (Taiwan, Taito license)",                           MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightat,  fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight (Southeast Asia, Taito license)",                   MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightut,  fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight (USA, Taito license)",                              MACHINE_SUPPORTS_SAVE )
GAME( 1992, fixeightjt,  fixeight, fixeight,   fixeight,   fixeight_state, init_fixeight,   ROT270, "Toaplan (Taito license)", "FixEight - Jigoku no Eiyuu Densetsu (Japan, Taito license)", MACHINE_SUPPORTS_SAVE )

GAME( 1992, fixeightbl,  fixeight, fixeightbl, fixeightbl, fixeight_state, init_fixeightbl, ROT270, "bootleg", "FixEight (Korea, bootleg)", MACHINE_SUPPORTS_SAVE )
