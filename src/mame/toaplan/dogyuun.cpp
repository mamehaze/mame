#include "emu.h"
#include "toaplan2.h"
#include "toaplipt.h"

#include "cpu/nec/v25.h"
#include "cpu/z80/z80.h"
#include "cpu/z180/hd647180x.h"
#include "machine/nvram.h"
#include "sound/ymopm.h"
#include "sound/ymopl.h"
#include "sound/ymz280b.h"
#include "speaker.h"

class dogyuun_state : public toaplan2_state
{
public:
	dogyuun_state(const machine_config &mconfig, device_type type, const char *tag)
		: toaplan2_state(mconfig, type, tag)
	{ }

	void dogyuun(machine_config &config);
	void dogyuunto(machine_config &config);

	void init_dogyuunto();
	void init_dogyuun();

protected:
private:
	void dogyuun_68k_mem(address_map &map);
	void dogyuunto_68k_mem(address_map &map);
	void dogyuunto_sound_z80_mem(address_map &map);
	void dogyuun_v25_mem(address_map &map);


	u32 screen_update_dogyuun(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void sound_reset_w(u8 data);
	void coin_w(u8 data);
	void coin_sound_reset_w(u8 data);

	u8 shared_ram_r(offs_t offset) { return m_shared_ram[offset]; }
	void shared_ram_w(offs_t offset, u8 data) { m_shared_ram[offset] = data; }

	DECLARE_VIDEO_START(toaplan2);
	void screen_vblank(int state);


};

constexpr unsigned toaplan2_state::T2PALETTE_LENGTH;


VIDEO_START_MEMBER(dogyuun_state,toaplan2)
{
	/* our current VDP implementation needs this bitmap to work with */
	m_screen->register_screen_bitmap(m_custom_priority_bitmap);

	if (m_vdp[0] != nullptr)
	{
		m_secondary_render_bitmap.reset();
		m_vdp[0]->custom_priority_bitmap = &m_custom_priority_bitmap;
	}

	if (m_vdp[1] != nullptr)
	{
		m_screen->register_screen_bitmap(m_secondary_render_bitmap);
		m_vdp[1]->custom_priority_bitmap = &m_custom_priority_bitmap;
	}
}


void dogyuun_state::screen_vblank(int state)
{
	// rising edge
	if (state)
	{
		if (m_vdp[0]) m_vdp[0]->screen_eof();
		if (m_vdp[1]) m_vdp[1]->screen_eof();
	}
}


u32 dogyuun_state::screen_update_dogyuun(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);

	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp[1]->render_vdp(bitmap, cliprect);

	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp[0]->render_vdp(bitmap, cliprect);
	

	return 0;
}




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


static INPUT_PORTS_START( toaplan2_3b )
	PORT_INCLUDE( toaplan2_2b )

	PORT_MODIFY("IN1")
	TOAPLAN_JOY_UDLR_3_BUTTONS( 1 )

	PORT_MODIFY("IN2")
	TOAPLAN_JOY_UDLR_3_BUTTONS( 2 )
INPUT_PORTS_END



static INPUT_PORTS_START( dogyuun )
	PORT_INCLUDE( toaplan2_3b )

	PORT_MODIFY("DSWA")
	PORT_DIPNAME( 0x0001,   0x0000, DEF_STR( Free_Play) )       PORT_DIPLOCATION("SW1:!1")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0001, DEF_STR( On ) )
	// Various features on bit mask 0x000e - see above
	TOAPLAN_COINAGE_DUAL_LOC( JMPR, 0x8000, 0x8000, SW1 )

	PORT_MODIFY("DSWB")
	// Difficulty on bit mask 0x0003 - see above
	PORT_DIPNAME( 0x000c,   0x0000, DEF_STR( Bonus_Life ) )     PORT_DIPLOCATION("SW2:!3,!4")
	PORT_DIPSETTING(        0x000c, DEF_STR( None ) )
	PORT_DIPSETTING(        0x0008, "400k only" )
	PORT_DIPSETTING(        0x0000, "200k only" )
	PORT_DIPSETTING(        0x0004, "200k, 400k and 600k" )
	PORT_DIPNAME( 0x0030,   0x0000, DEF_STR( Lives ) )          PORT_DIPLOCATION("SW2:!5,!6")
	PORT_DIPSETTING(        0x0030, "1" )
	PORT_DIPSETTING(        0x0020, "2" )
	PORT_DIPSETTING(        0x0000, "3" )
	PORT_DIPSETTING(        0x0010, "5" )
	PORT_DIPNAME( 0x0040,   0x0000, "Invulnerability (Cheat)" )         PORT_DIPLOCATION("SW2:!7")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0040, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080,   0x0000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:!8")
	PORT_DIPSETTING(        0x0080, DEF_STR( No ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( Yes ) )

	PORT_START("JMPR")
	// Bit Mask 0x8000 is used here to signify European Coinage for MAME purposes - not read on the real board!
	// "No speedups": all speedup items in game are replaced with bombs
	PORT_CONFNAME( 0x80f0,  0x8030, DEF_STR( Region ) ) //PORT_CONFLOCATION("JP:!4,!3,!2,!1,FAKE:!1")
	PORT_CONFSETTING(       0x8030, DEF_STR( Europe ) )
	PORT_CONFSETTING(       0x0010, DEF_STR( USA ) )
	PORT_CONFSETTING(       0x0020, "USA (Atari Games Corp.)" )
	PORT_CONFSETTING(       0x0000, DEF_STR( Japan ) )
	PORT_CONFSETTING(       0x0040, "Hong Kong (Charterfield); no speedups" )
	PORT_CONFSETTING(       0x0050, "Korea (Unite Trading); no speedups" )
	PORT_CONFSETTING(       0x0060, "Taiwan; no speedups" )
	PORT_CONFSETTING(       0x0070, "USA; no speedups" )
	PORT_CONFSETTING(       0x0080, "Southeast Asia (Charterfield); no speedups" )
	PORT_CONFSETTING(       0x0090, "Hong Kong (Charterfield)" )
	PORT_CONFSETTING(       0x00a0, "Korea (Unite Trading)" )
	PORT_CONFSETTING(       0x00b0, DEF_STR( Taiwan ) )
	PORT_CONFSETTING(       0x00c0, "USA (Atari Games Corp.); no speedups" )
	PORT_CONFSETTING(       0x00d0, "Southeast Asia (Charterfield)" )
	PORT_CONFSETTING(       0x80e0, "Europe; no speedups" )
	PORT_CONFSETTING(       0x00f0, "Japan (Taito Corp.)" )
INPUT_PORTS_END


static INPUT_PORTS_START( dogyuuna )
	PORT_INCLUDE( dogyuun )

	PORT_MODIFY("DSWA")
	TOAPLAN_COINAGE_DUAL_LOC( JMPR, 0xf0, 0x30, SW1 )

	PORT_MODIFY("JMPR")
	// "No speedups": all speedup items in game are replaced with bombs
	PORT_CONFNAME( 0x00f0,  0x0030, DEF_STR( Region ) ) //PORT_CONFLOCATION("JP:!4,!3,!2,!1")
	PORT_CONFSETTING(       0x0030, DEF_STR( Europe ) )
	PORT_CONFSETTING(       0x0010, DEF_STR( USA ) )
	PORT_CONFSETTING(       0x0020, "USA (Atari Games Corp.)" )
	PORT_CONFSETTING(       0x0000, DEF_STR( Japan ) )
	PORT_CONFSETTING(       0x0040, "Hong Kong (Charterfield); no speedups" )
	PORT_CONFSETTING(       0x0050, "Korea (Unite Trading); no speedups" )
	PORT_CONFSETTING(       0x0060, "Taiwan; no speedups" )
//  PORT_CONFSETTING(        0x0070, "Taiwan (Licensed to ???????); no speedups" )
	PORT_CONFSETTING(       0x0080, "Southeast Asia (Charterfield); no speedups" )
	PORT_CONFSETTING(       0x0090, "Hong Kong (Charterfield)" )
	PORT_CONFSETTING(       0x00a0, "Korea (Unite Trading)" )
	PORT_CONFSETTING(       0x00b0, DEF_STR( Taiwan ) )
//  PORT_CONFSETTING(        0x00c0, "Taiwan (Licensed to ???????)" )
	PORT_CONFSETTING(       0x00d0, "Southeast Asia (Charterfield)" )
//  PORT_CONFSETTING(        0x00e0, DEF_STR( Unused ) )
	PORT_CONFSETTING(       0x00f0, "Japan (Taito Corp.)" )
INPUT_PORTS_END


static INPUT_PORTS_START( dogyuunt )
	PORT_INCLUDE( dogyuun )

	PORT_MODIFY("DSWA")
	TOAPLAN_COINAGE_DUAL_LOC( JMPR, 0xf0, 0x20, SW1 )

	PORT_MODIFY("JMPR")
	PORT_CONFNAME( 0x00f0,  0x0020, DEF_STR( Region ) ) //PORT_CONFLOCATION("JP:!4,!3,!2,!1")
	PORT_CONFSETTING(       0x0020, DEF_STR( Europe ) )
	PORT_CONFSETTING(       0x0010, DEF_STR( USA ) )
	PORT_CONFSETTING(       0x0000, DEF_STR( Japan ) )
	PORT_CONFSETTING(       0x0030, DEF_STR( Hong_Kong ) )
	PORT_CONFSETTING(       0x0040, DEF_STR( Korea ) )
	PORT_CONFSETTING(       0x0050, DEF_STR( Taiwan ) )
	PORT_CONFSETTING(       0x0060, "Southeast Asia (Charterfield)" )
	PORT_CONFSETTING(       0x0070, "USA (Romstar, Inc.)" )
	PORT_CONFSETTING(       0x0080, "Hong Kong (Honest Trading Co.)" )
	PORT_CONFSETTING(       0x0090, "Korea (JC Trading Corp.)" )
	PORT_CONFSETTING(       0x00a0, "USA (Fabtek)" )
//  PORT_CONFSETTING(        0x00b0, DEF_STR( Unused ) )
//  PORT_CONFSETTING(        0x00c0, DEF_STR( Unused ) )
//  PORT_CONFSETTING(        0x00d0, DEF_STR( Unused ) )
//  PORT_CONFSETTING(        0x00e0, DEF_STR( Unused ) )
	PORT_CONFSETTING(       0x00f0, "Japan (Taito Corp.)" )
INPUT_PORTS_END

void dogyuun_state::sound_reset_w(u8 data)
{
	m_audiocpu->set_input_line(INPUT_LINE_RESET, (data & m_sound_reset_bit) ? CLEAR_LINE : ASSERT_LINE);
}


void dogyuun_state::coin_w(u8 data)
{
	/* +----------------+------ Bits 7-5 not used ------+--------------+ */
	/* | Coin Lockout 2 | Coin Lockout 1 | Coin Count 2 | Coin Count 1 | */
	/* |     Bit 3      |     Bit 2      |     Bit 1    |     Bit 0    | */

	if (data & 0x0f)
	{
		machine().bookkeeping().coin_lockout_w(0, BIT(~data, 2));
		machine().bookkeeping().coin_lockout_w(1, BIT(~data, 3));
		machine().bookkeeping().coin_counter_w(0, BIT( data, 0));
		machine().bookkeeping().coin_counter_w(1, BIT( data, 1));
	}
	else
	{
		machine().bookkeeping().coin_lockout_global_w(1);    // Lock all coin slots
	}
	if (data & 0xf0)
	{
		logerror("Writing unknown upper bits (%02x) to coin control\n",data);
	}
}

void dogyuun_state::coin_sound_reset_w(u8 data)
{
	logerror("coin_sound_reset_w %02x\n",data);

	coin_w(data & ~m_sound_reset_bit);
	sound_reset_w(data & m_sound_reset_bit);
}


void dogyuun_state::dogyuun_68k_mem(address_map &map)
{
	map(0x000000, 0x07ffff).rom();
	map(0x100000, 0x103fff).ram();
	map(0x200010, 0x200011).portr("IN1");
	map(0x200014, 0x200015).portr("IN2");
	map(0x200018, 0x200019).portr("SYS");
	map(0x20001d, 0x20001d).w(FUNC(dogyuun_state::coin_sound_reset_w)); // Coin count/lock + v25 reset line
	map(0x210000, 0x21ffff).rw(FUNC(dogyuun_state::shared_ram_r), FUNC(dogyuun_state::shared_ram_w)).umask16(0x00ff);
	map(0x300000, 0x30000d).rw(m_vdp[0], FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x400000, 0x400fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x500000, 0x50000d).rw(m_vdp[1], FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x700000, 0x700001).r(FUNC(toaplan2_state::video_count_r));         // test bit 8
}


void dogyuun_state::dogyuunto_68k_mem(address_map &map)
{
	map(0x000000, 0x07ffff).rom();
	map(0x100000, 0x103fff).ram();
	map(0x218000, 0x218fff).rw(FUNC(dogyuun_state::shared_ram_r), FUNC(dogyuun_state::shared_ram_w)).umask16(0x00ff); // reads the same area as the finished game on startup, but then uses only this part
	map(0x21c01d, 0x21c01d).w(FUNC(dogyuun_state::coin_sound_reset_w)); // Coin count/lock + Z80 reset line
	map(0x21c020, 0x21c021).portr("IN1");
	map(0x21c024, 0x21c025).portr("IN2");
	map(0x21c028, 0x21c029).portr("SYS");
	map(0x21c02c, 0x21c02d).portr("DSWA");
	map(0x21c030, 0x21c031).portr("DSWB");
	map(0x300000, 0x30000d).rw(m_vdp[0], FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x400000, 0x400fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x500000, 0x50000d).rw(m_vdp[1], FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x700000, 0x700001).r(FUNC(toaplan2_state::video_count_r));         // test bit 8
}

void dogyuun_state::dogyuunto_sound_z80_mem(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0xc000, 0xc7ff).ram().share(m_shared_ram);
	map(0xe000, 0xe001).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0xe004, 0xe004).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
}



/* probably dogyuun, vfive and kbash use the same decryption table;
those 3 games have been seen with the NITRO905 chip, other alias are
ts002mach for dogyuun, ts004dash for kbash and ts007spy for vfive */

static const u8 nitro_decryption_table[256] = {
	0x1b,0x56,0x75,0x88,0x8c,0x06,0x58,0x72, 0x83,0x86,0x36,0x1a,0x5f,0xd3,0x8c,0xe9, /* 00 */
	/* *//* *//* *//* *//* *//* *//* *//* */ /* *//* *//* *//* *//* *//* *//* *//* */
	0x22,0x0f,0x03,0x2a,0xeb,0x2a,0xf9,0x0f, 0xa4,0xbd,0x75,0xf3,0x4f,0x53,0x8e,0xfe, /* 10 */
	/*W*//*W*//*r*//*W*//*r*//*W*//*W*//*r*/ /*r*//*a*//*r*//*r*//*r*//*W*//*x*//*r*/
	0x87,0xe8,0xb1,0x8d,0x36,0xb5,0x43,0x73, 0x2a,0x5b,0xf9,0x02,0x24,0x8a,0x03,0x80, /* 20 */
	/*a*//*r*//*r*//*r*//*x*//*r*//*r*//*r*/ /*W*//*r*//*W*//*W*//*a*//*r*//*a*//*r*/
	0x86,0x8b,0xd1,0x3e,0x8d,0x3e,0x58,0xfb, 0xc3,0x79,0xbd,0xb7,0x8a,0xe8,0x0f,0x81, /* 30 */
	/*a*//*a*//*a*//*a*//*r*//*r*//*a*//*r*/ /*r*//*a*//*a*//*W*//*r*//*r*//*r*//*r*/
	0xb7,0xd0,0x8b,0xeb,0xff,0xb8,0x90,0x8b, 0x5e,0xa2,0x90,0xc1,0xab,0xb4,0x80,0x59, /* 40 */
	/*r*//*r*//*a*//*r*//*a*//*x*/     /*a*/ /*W*//*W*/          /*r*//*W*//*r*//*a*/
	0x87,0x72,0xb5,0xbd,0xb0,0x88,0x50,0x0f, 0xfe,0xd2,0xc3,0x90,0x8a,0x90,0xf9,0x75, /* 50 */
	/*W*//*a*//*a*//*r*//*r*//*a*//*a*//*a*/ /*r*//*W*//*r*/     /*r*/     /*W*//*r*/
	0x1a,0xb3,0x74,0x0a,0x68,0x24,0xbb,0x90, 0x75,0x47,0xfe,0x2c,0xbe,0xc3,0x88,0xd2, /* 60 */
	/*W*//*r*//*a*//*r*//*a*//*a*//*W*/      /*r*//*a*//*r*//*W*//*W*//*a*//*r*//*a*/
	0x3e,0xc1,0x8c,0x33,0x0f,0x4f,0x8b,0x90, 0xb9,0x1e,0xff,0xa2,0x3e,0x22,0xbe,0x57, /* 70 */
	/*r*//*W*//*r*//*r*//*a*/     /*a*/      /*r*//*r*//*a*//*r*//*a*//*W*//*r*//*a*/
	0x81,0x3a,0xf6,0x88,0xeb,0xb1,0x89,0x8a, 0x32,0x80,0x0f,0xb1,0x48,0xc3,0x68,0x72, /* 80 */
	/*r*//*r*//*r*//*r*//*a*//*W*//*a*//*r*/ /*r*//*r*//*r*//*a*//*x*//*a*//*a*//*r*/
	0x53,0x02,0xc0,0x02,0xe8,0xb4,0x74,0xbc, 0x90,0x58,0x0a,0xf3,0x75,0xc6,0x90,0xe8, /* 90 */
	/*a*//*W*//*r*//*W*//*r*//*r*//*r*//*x*/      /*a*//*r*//*r*//*r*//*x*/     /*r*/
	0x26,0x50,0xfc,0x8c,0x06,0xb1,0xc3,0xd1, 0xeb,0x83,0xa4,0xbf,0x26,0x4b,0x46,0xfe, /* a0 */
	/*r*//*a*//*a*//*r*/     /*a*//*r*//*W*/ /*a*//*r*//*r*//*r*//*r*//*W*//*a*//*r*/
	0xe2,0x89,0xb3,0x88,0x03,0x56,0x0f,0x38, 0xbb,0x0c,0x90,0x0f,0x07,0x8a,0x8a,0x33, /* b0 */
	/*r*//*a*//*W*//*r*//*a*//*W*//*r*//*W*/ /*W*//*W*/     /*a*//*r*//*r*//*r*//*x*/
	0xfe,0xf9,0xb1,0xa0,0x45,0x36,0x22,0x5e, 0x8a,0xbe,0xc6,0xea,0x3c,0xb2,0x1e,0xe8, /* c0 */
	/*r*//*W*//*r*//*r*//*r*//*r*//*W*//*r*/ /*r*//*W*//*x*//*x*//*r*//*?*//*r*//*r*/
	0x90,0xeb,0x55,0xf6,0x8a,0xb0,0x5d,0xc0, 0xbb,0x8d,0xf6,0xd0,0xd1,0x88,0x4d,0x90, /* d0 */
			/*a*//*r*//*r*//*a*//*a*//*r*//*W*/ /*x*//*r*//*r*//*a*//*W*//*r*//*W*/
	0x51,0x51,0x74,0xbd,0x32,0xd1,0xc6,0xd2, 0x53,0xc7,0xab,0x36,0x50,0xe9,0x33,0xb3, /* e0 */
	/*r*//*a*//*r*//*r*//*r*//*W*/     /*a*/ /*r*//*x*//*r*//*r*//*W*//*a*//*r*//*W*/
	0x2e,0x05,0x88,0x59,0x74,0x74,0x22,0x8e, 0x8a,0x8a,0x36,0x08,0x0f,0x45,0x90,0x2e, /* f0 */
	/*r*//*W*//*r*//*r*//*a*//*a*//*W*//*x*/ /*r*//*r*//*x*//*a*//*r*//*a*/     /*r*/
};

/*
dogyuun
a5272 cd

kbash

vfive
a4849 cd

*/


void dogyuun_state::dogyuun_v25_mem(address_map &map)
{
	map(0x00000, 0x00001).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x00004, 0x00004).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0x80000, 0x87fff).mirror(0x78000).ram().share(m_shared_ram);
}

void dogyuun_state::dogyuun(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 25_MHz_XTAL/2);           /* verified on pcb */
	m_maincpu->set_addrmap(AS_PROGRAM, &dogyuun_state::dogyuun_68k_mem);
	m_maincpu->reset_cb().set(FUNC(toaplan2_state::toaplan2_reset));

	v25_device &audiocpu(V25(config, m_audiocpu, 25_MHz_XTAL/2));         /* NEC V25 type Toaplan marked CPU ??? */
	audiocpu.set_addrmap(AS_PROGRAM, &dogyuun_state::dogyuun_v25_mem);
	audiocpu.set_decryption_table(nitro_decryption_table);
	audiocpu.pt_in_cb().set_ioport("DSWB").exor(0xff);
	audiocpu.p0_in_cb().set_ioport("DSWA").exor(0xff);
	audiocpu.p1_in_cb().set_ioport("JMPR").exor(0xff);
	audiocpu.p2_out_cb().set_nop();  // bit 0 is FAULT according to kbash schematic

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	m_screen->set_raw(27_MHz_XTAL/4, 432, 0, 320, 262, 0, 240);
	m_screen->set_screen_update(FUNC(dogyuun_state::screen_update_dogyuun));
	m_screen->screen_vblank().set(FUNC(dogyuun_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, T2PALETTE_LENGTH);

	GP9001_VDP(config, m_vdp[0], 27_MHz_XTAL);
	m_vdp[0]->set_palette(m_palette);
	m_vdp[0]->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_4);

	GP9001_VDP(config, m_vdp[1], 27_MHz_XTAL);
	m_vdp[1]->set_palette(m_palette);

	MCFG_VIDEO_START_OVERRIDE(dogyuun_state,toaplan2)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	YM2151(config, "ymsnd", 27_MHz_XTAL/8).add_route(ALL_OUTPUTS, "mono", 0.5); // verified on pcb

	OKIM6295(config, m_oki[0], 25_MHz_XTAL/24, okim6295_device::PIN7_HIGH); // verified on PCB
	m_oki[0]->add_route(ALL_OUTPUTS, "mono", 0.5);
}


void dogyuun_state::dogyuunto(machine_config &config)
{
	dogyuun(config);

	m_maincpu->set_addrmap(AS_PROGRAM, &dogyuun_state::dogyuunto_68k_mem);
	m_maincpu->set_clock(24_MHz_XTAL / 2); // 24 MHz instead of 25

	z80_device &audiocpu(Z80(config.replace(), "audiocpu", 27_MHz_XTAL / 8)); // guessed divisor
	audiocpu.set_addrmap(AS_PROGRAM, &dogyuun_state::dogyuunto_sound_z80_mem);

	m_oki[0]->set_clock(1.056_MHz_XTAL); // blue resonator 1056J
}

void dogyuun_state::init_dogyuunto()
{
	m_sound_reset_bit = 0x10;
}

void dogyuun_state::init_dogyuun()
{
	m_sound_reset_bit = 0x20;
}



ROM_START( dogyuun )
	ROM_REGION( 0x080000, "maincpu", 0 )            /* Main 68K code */
	ROM_LOAD16_WORD_SWAP( "tp022_01.r16", 0x000000, 0x080000, CRC(79eb2429) SHA1(088c5ed0ed77557ab71f52cafe35028e3648ae1e) )

	/* Secondary CPU is a Toaplan marked chip, (TS-002-MACH  TOA PLAN) */
	/* It's a NEC V25 (PLCC94) (encrypted program uploaded by main CPU) */

	ROM_REGION( 0x200000, "gp9001_0", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_3.w92", 0x000000, 0x100000, CRC(191b595f) SHA1(89344946daa18087cc83f92027cf5da659b1c7a5) )
	ROM_LOAD16_WORD_SWAP( "tp022_4.w93", 0x100000, 0x100000, CRC(d58d29ca) SHA1(90d142fef37764ef817347a2bed77892a288a077) )

	ROM_REGION( 0x400000, "gp9001_1", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_5.w16", 0x000000, 0x200000, CRC(d4c1db45) SHA1(f5655467149ba737128c2f54c9c6cdaca6e4c35c) )
	ROM_LOAD16_WORD_SWAP( "tp022_6.w17", 0x200000, 0x200000, CRC(d48dc74f) SHA1(081b5a00a2ff2bd82b98b30aab3cb5b6ae1014d5) )

	ROM_REGION( 0x40000, "oki1", 0 )     /* ADPCM Samples */
	ROM_LOAD( "tp022_2.w30", 0x00000, 0x40000, CRC(043271b3) SHA1(c7eaa929e55dd956579b824ea9d20a1d0129a925) )
ROM_END


ROM_START( dogyuuna )
	ROM_REGION( 0x080000, "maincpu", 0 )            /* Main 68K code */
	ROM_LOAD16_WORD_SWAP( "01.u64", 0x000000, 0x080000, CRC(fe5bd7f4) SHA1(9c725466112a514c9ed0fb074422d291c175c3f4) )

	/* Secondary CPU is a Toaplan marked chip, (TS-002-MACH  TOA PLAN) */
	/* It's a NEC V25 (PLCC94) (encrypted program uploaded by main CPU) */

	ROM_REGION( 0x200000, "gp9001_0", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_3.w92", 0x000000, 0x100000, CRC(191b595f) SHA1(89344946daa18087cc83f92027cf5da659b1c7a5) )
	ROM_LOAD16_WORD_SWAP( "tp022_4.w93", 0x100000, 0x100000, CRC(d58d29ca) SHA1(90d142fef37764ef817347a2bed77892a288a077) )

	ROM_REGION( 0x400000, "gp9001_1", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_5.w16", 0x000000, 0x200000, CRC(d4c1db45) SHA1(f5655467149ba737128c2f54c9c6cdaca6e4c35c) )
	ROM_LOAD16_WORD_SWAP( "tp022_6.w17", 0x200000, 0x200000, CRC(d48dc74f) SHA1(081b5a00a2ff2bd82b98b30aab3cb5b6ae1014d5) )

	ROM_REGION( 0x40000, "oki1", 0 )     /* ADPCM Samples */
	ROM_LOAD( "tp022_2.w30", 0x00000, 0x40000, CRC(043271b3) SHA1(c7eaa929e55dd956579b824ea9d20a1d0129a925) )
ROM_END


// found on a standard TP-022-1 PCB, main CPU ROM had a MR sticker. It's a little closer to the location test than to the released versions (i.e region configuration).
ROM_START( dogyuunb )
	ROM_REGION( 0x080000, "maincpu", 0 )            // Main 68K code
	ROM_LOAD16_WORD_SWAP( "mr.u64", 0x000000, 0x080000, CRC(4dc258dc) SHA1(ac2783030a8367a20bfad282942c2aa383156291) ) // M27C4002

	// Secondary CPU is a Toaplan marked chip, (TS-002-MACH  TOA PLAN)
	// It's a NEC V25 (PLCC94) (encrypted program uploaded by main CPU)

	ROM_REGION( 0x200000, "gp9001_0", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_3.w92", 0x000000, 0x100000, CRC(191b595f) SHA1(89344946daa18087cc83f92027cf5da659b1c7a5) )
	ROM_LOAD16_WORD_SWAP( "tp022_4.w93", 0x100000, 0x100000, CRC(d58d29ca) SHA1(90d142fef37764ef817347a2bed77892a288a077) )

	ROM_REGION( 0x400000, "gp9001_1", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_5.w16", 0x000000, 0x200000, CRC(d4c1db45) SHA1(f5655467149ba737128c2f54c9c6cdaca6e4c35c) )
	ROM_LOAD16_WORD_SWAP( "tp022_6.w17", 0x200000, 0x200000, CRC(d48dc74f) SHA1(081b5a00a2ff2bd82b98b30aab3cb5b6ae1014d5) )

	ROM_REGION( 0x40000, "oki1", 0 )
	ROM_LOAD( "tp022_2.w30", 0x00000, 0x40000, CRC(043271b3) SHA1(c7eaa929e55dd956579b824ea9d20a1d0129a925) )
ROM_END


ROM_START( dogyuunt )
	ROM_REGION( 0x080000, "maincpu", 0 )            /* Main 68K code */
	ROM_LOAD16_WORD_SWAP( "sample10.9.u64.bin", 0x000000, 0x080000, CRC(585f5016) SHA1(18d57843f33a560a3bb4b6aef176f7ef795b742d) )

	/* Secondary CPU is a Toaplan marked chip, (TS-002-MACH  TOA PLAN) */
	/* It's a NEC V25 (PLCC94) (encrypted program uploaded by main CPU) */

	ROM_REGION( 0x200000, "gp9001_0", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_3.w92", 0x000000, 0x100000, CRC(191b595f) SHA1(89344946daa18087cc83f92027cf5da659b1c7a5) )
	ROM_LOAD16_WORD_SWAP( "tp022_4.w93", 0x100000, 0x100000, CRC(d58d29ca) SHA1(90d142fef37764ef817347a2bed77892a288a077) )

	ROM_REGION( 0x400000, "gp9001_1", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_5.w16", 0x000000, 0x200000, CRC(d4c1db45) SHA1(f5655467149ba737128c2f54c9c6cdaca6e4c35c) )
	ROM_LOAD16_WORD_SWAP( "tp022_6.w17", 0x200000, 0x200000, CRC(d48dc74f) SHA1(081b5a00a2ff2bd82b98b30aab3cb5b6ae1014d5) )

	ROM_REGION( 0x40000, "oki1", 0 )     /* ADPCM Samples */
	ROM_LOAD( "tp022_2.w30", 0x00000, 0x40000, CRC(043271b3) SHA1(c7eaa929e55dd956579b824ea9d20a1d0129a925) )
ROM_END


/*
This set came on a TX-022 PCB (different from the final version, TP-022).
Seems the game is always in 'debug mode' according to the test menu (dip has no effect). Still, it has no invicibility effect.
Couldn't find a read for the region settings jumpers.
Hardware differences according to the dumper:
* Two GCUs have nearly identical sections copy-pasted, one above the other.
* Toaplan HK-1000 ceramic module is used for inputs; the final implements the logic separately.
* Inputs mapped in the main CPU address space.
* No NEC V25 sound CPU; instead, there is a Z80 with its own ROM.
* Sound amp pinout is reversed; mounted to the underside as a bodge fix (this is how I received it).
* Board is marked "TX-022" instead of "TP-022". A halfway point from the development code "GX-022"
*/
ROM_START( dogyuunto )
	ROM_REGION( 0x080000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "8-25.u11", 0x000000, 0x080000, CRC(4d3c952f) SHA1(194f3065c513238921047ead8b425c3d0538b9a7) ) // real hand-written label is '8/25'

	ROM_REGION( 0x08000, "audiocpu", 0 )
	ROM_LOAD( "u25", 0x00000, 0x08000, CRC(41a34a7e) SHA1(c4f7833249436fd064c7088c9776d12dee4a7d39) ) // only had a white label

	// the GP9001 mask ROMs were not dumped for this set, but the ROM codes match so they are believed identical
	ROM_REGION( 0x200000, "gp9001_0", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_3.w92", 0x000000, 0x100000, CRC(191b595f) SHA1(89344946daa18087cc83f92027cf5da659b1c7a5) )
	ROM_LOAD16_WORD_SWAP( "tp022_4.w93", 0x100000, 0x100000, CRC(d58d29ca) SHA1(90d142fef37764ef817347a2bed77892a288a077) )

	ROM_REGION( 0x400000, "gp9001_1", 0 )
	ROM_LOAD16_WORD_SWAP( "tp022_5.w16", 0x000000, 0x200000, CRC(d4c1db45) SHA1(f5655467149ba737128c2f54c9c6cdaca6e4c35c) )
	ROM_LOAD16_WORD_SWAP( "tp022_6.w17", 0x200000, 0x200000, CRC(d48dc74f) SHA1(081b5a00a2ff2bd82b98b30aab3cb5b6ae1014d5) )

	// this may have some corruption (only 24, apparently random, bytes differ from the standard ROM), however preserve it for now until it has been verified.
	ROM_REGION( 0x40000, "oki1", 0 )
	ROM_LOAD( "2m.u29", 0x00000, 0x40000, CRC(5e7a77d8) SHA1(da6beb5e8e015965ff42fd52f5aa0c0ae5bcee4f) ) // '2M' hand-written
ROM_END

GAME( 1992, dogyuun,     0,        dogyuun,      dogyuun,    dogyuun_state, init_dogyuun,  ROT270, "Toaplan",         "Dogyuun",                           MACHINE_SUPPORTS_SAVE )
GAME( 1992, dogyuuna,    dogyuun,  dogyuun,      dogyuuna,   dogyuun_state, init_dogyuun,  ROT270, "Toaplan",         "Dogyuun (older set)",               MACHINE_SUPPORTS_SAVE )
GAME( 1992, dogyuunb,    dogyuun,  dogyuun,      dogyuunt,   dogyuun_state, init_dogyuun,  ROT270, "Toaplan",         "Dogyuun (oldest set)",              MACHINE_SUPPORTS_SAVE ) // maybe a newer location test version, instead
GAME( 1992, dogyuunt,    dogyuun,  dogyuun,      dogyuunt,   dogyuun_state, init_dogyuun,  ROT270, "Toaplan",         "Dogyuun (10/9/1992 location test)", MACHINE_SUPPORTS_SAVE )
GAME( 1992, dogyuunto,   dogyuun,  dogyuunto,    dogyuunt,   dogyuun_state, init_dogyuunto, ROT270, "Toaplan",         "Dogyuun (8/25/1992 location test)", MACHINE_SUPPORTS_SAVE )
