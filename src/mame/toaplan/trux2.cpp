

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



class trux2_state : public truxton2_state
{
public:
	trux2_state(const machine_config &mconfig, device_type type, const char *tag)
		: truxton2_state(mconfig, type, tag)
	{ }

	void truxton2(machine_config &config);

protected:
private:

	void truxton2_68k_mem(address_map &map);
	void tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	DECLARE_VIDEO_START(truxton2);

};

VIDEO_START_MEMBER(trux2_state,truxton2)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	/* Create the Text tilemap for this game */
	m_gfxdecode->gfx(0)->set_source(reinterpret_cast<u8 *>(m_tx_gfxram.target()));

	create_tx_tilemap(0x1d5, 0x16a);
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


static INPUT_PORTS_START( truxton2 )
	PORT_INCLUDE( toaplan2_3b )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("Fast Scrolling (Cheat)")

	PORT_MODIFY("DSWA")
	PORT_DIPNAME( 0x0001,   0x0000, "Rapid Fire" )              PORT_DIPLOCATION("SW1:!1")
	PORT_DIPSETTING(        0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( On ) )
	// Various features on bit mask 0x000e - see above
	TOAPLAN_COINAGE_DUAL_LOC( JMPR, 0x0f, 0x02, SW1 )

	PORT_MODIFY("DSWB")
	// Difficulty on bit mask 0x0003 - see above
	PORT_DIPNAME( 0x000c,   0x0000, DEF_STR( Bonus_Life ) )     PORT_DIPLOCATION("SW2:!3,!4")
	PORT_DIPSETTING(        0x000c, "200k only" )
	PORT_DIPSETTING(        0x0008, "100k only" )
	PORT_DIPSETTING(        0x0004, "100k and 250k" )
	PORT_DIPSETTING(        0x0000, "70k and 200k" )
	PORT_DIPNAME( 0x0030,   0x0000, DEF_STR( Lives ) )          PORT_DIPLOCATION("SW2:!5,!6")
	PORT_DIPSETTING(        0x0030, "2" )
	PORT_DIPSETTING(        0x0000, "3" )
	PORT_DIPSETTING(        0x0020, "4" )
	PORT_DIPSETTING(        0x0010, "5" )
	PORT_DIPNAME( 0x0040,   0x0000, "Invulnerability (Cheat)" ) PORT_DIPLOCATION("SW2:!7")
	PORT_DIPSETTING(        0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(        0x0040, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080,   0x0000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:!8")
	PORT_DIPSETTING(        0x0080, DEF_STR( No ) )
	PORT_DIPSETTING(        0x0000, DEF_STR( Yes ) )

	PORT_START("JMPR")
	PORT_CONFNAME( 0x000f,  0x0002, DEF_STR( Region ) ) //PORT_CONFLOCATION("JP:!4,!3,!2,!1")
	PORT_CONFSETTING(       0x0002, DEF_STR( Europe ) )
	PORT_CONFSETTING(       0x0001, DEF_STR( USA ) )
	PORT_CONFSETTING(       0x0000, DEF_STR( Japan ) )
	PORT_CONFSETTING(       0x0003, DEF_STR( Hong_Kong ) )
	PORT_CONFSETTING(       0x0004, DEF_STR( Korea ) )
	PORT_CONFSETTING(       0x0005, DEF_STR( Taiwan ) )
	PORT_CONFSETTING(       0x0006, "Southeast Asia (Charterfield)" )
	PORT_CONFSETTING(       0x0007, "USA (Romstar, Inc.)" )
	PORT_CONFSETTING(       0x0008, "Hong Kong (Honest Trading Co.)" )
	PORT_CONFSETTING(       0x0009, "Korea (JC Trading Corp.)" )
	PORT_CONFSETTING(       0x000a, "USA (Fabtek)" )
//  PORT_CONFSETTING(        0x000b, DEF_STR( Unused ) )
//  PORT_CONFSETTING(        0x000c, DEF_STR( Unused ) )
//  PORT_CONFSETTING(        0x000d, DEF_STR( Unused ) )
//  PORT_CONFSETTING(        0x000e, DEF_STR( Unused ) )
	PORT_CONFSETTING(       0x000f, "Japan (Taito Corp.)" )
INPUT_PORTS_END





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

void trux2_state::tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Dynamic GFX decoding for Truxton 2 / FixEight ***/

	const u16 oldword = m_tx_gfxram[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&m_tx_gfxram[offset]);
		m_gfxdecode->gfx(0)->mark_dirty(offset/32);
	}
}



void trux2_state::truxton2_68k_mem(address_map &map)
{
	map(0x000000, 0x07ffff).rom();
	map(0x100000, 0x10ffff).ram();
	map(0x200000, 0x20000d).rw(m_vdp, FUNC(gp9001vdp_device::read), FUNC(gp9001vdp_device::write));
	map(0x300000, 0x300fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x400000, 0x401fff).ram().w(FUNC(truxton2_state::tx_videoram_w)).share(m_tx_videoram);
	map(0x402000, 0x402fff).ram().share(m_tx_lineselect);
	map(0x403000, 0x4031ff).ram().w(FUNC(truxton2_state::tx_linescroll_w)).share(m_tx_linescroll);
	map(0x403200, 0x403fff).ram();
	map(0x500000, 0x50ffff).ram().w(FUNC(trux2_state::tx_gfxram_w)).share(m_tx_gfxram);
	map(0x600000, 0x600001).r(FUNC(truxton2_state::video_count_r));
	map(0x700000, 0x700001).portr("DSWA");
	map(0x700002, 0x700003).portr("DSWB");
	map(0x700004, 0x700005).portr("JMPR");
	map(0x700006, 0x700007).portr("IN1");
	map(0x700008, 0x700009).portr("IN2");
	map(0x70000a, 0x70000b).portr("SYS");
	map(0x700011, 0x700011).rw(m_oki[0], FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0x700014, 0x700017).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write)).umask16(0x00ff);
	map(0x70001f, 0x70001f).w(FUNC(truxton2_state::coin_w));
}


void trux2_state::truxton2(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 16_MHz_XTAL);         /* verified on pcb */
	m_maincpu->set_addrmap(AS_PROGRAM, &trux2_state::truxton2_68k_mem);
	m_maincpu->reset_cb().set(FUNC(truxton2_state::toaplan2_reset));

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	m_screen->set_raw(27_MHz_XTAL/4, 432, 0, 320, 262, 0, 240);
	m_screen->set_screen_update(FUNC(truxton2_state::screen_update_truxton2));
	m_screen->screen_vblank().set(FUNC(truxton2_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_truxton2);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 0x10000);

	GP9001_VDP(config, m_vdp, 27_MHz_XTAL);
	m_vdp->set_palette(m_palette);
	m_vdp->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_2);

	MCFG_VIDEO_START_OVERRIDE(trux2_state,truxton2)

	/* sound hardware */
#ifdef TRUXTON2_STEREO  // music data is stereo...
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	YM2151(config, "ymsnd", 27_MHz_XTAL/8).add_route(0, "lspeaker", 1.0).add_route(1, "rspeaker", 1.0);

	OKIM6295(config, m_oki[0], 16_MHz_XTAL/4, okim6295_device::PIN7_LOW);
	m_oki[0]->add_route(ALL_OUTPUTS, "lspeaker", 1.0);
	m_oki[0]->add_route(ALL_OUTPUTS, "rspeaker", 1.0);
#else   // ...but the hardware is mono
	SPEAKER(config, "mono").front_center();

	YM2151(config, "ymsnd", 27_MHz_XTAL/8).add_route(ALL_OUTPUTS, "mono", 1.0); // verified on PCB

	OKIM6295(config, m_oki[0], 16_MHz_XTAL/4, okim6295_device::PIN7_LOW); // verified on PCB
	m_oki[0]->add_route(ALL_OUTPUTS, "mono", 1.0);
#endif
}


ROM_START( truxton2 )
	ROM_REGION( 0x080000, "maincpu", 0 )            /* Main 68K code */
	/* program ROM is byte swapped ! */
	ROM_LOAD16_WORD( "tp024_1.bin", 0x000000, 0x080000, CRC(f5cfe6ee) SHA1(30979888a4cd6500244117748f28386a7e20a169) )

	ROM_REGION( 0x200000, "gp9001", 0 )
	ROM_LOAD( "tp024_4.bin", 0x000000, 0x100000, CRC(805c449e) SHA1(fdf985344145bd320b88b9b0c25e73066c9b2ada) )
	ROM_LOAD( "tp024_3.bin", 0x100000, 0x100000, CRC(47587164) SHA1(bac493e2d5507286b984957b289c929335d27eaa) )

	ROM_REGION( 0x80000, "oki1", 0 )         /* ADPCM Samples */
	ROM_LOAD( "tp024_2.bin", 0x00000, 0x80000, CRC(f2f6cae4) SHA1(bb4e8c36531bed97ced4696ca12fd40ede2531aa) )
ROM_END


GAME( 1992, truxton2,    0,        truxton2,     truxton2,   trux2_state, empty_init,    ROT270, "Toaplan",         "Truxton II / Tatsujin Oh",  MACHINE_SUPPORTS_SAVE )
