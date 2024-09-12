

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



class probowl_state : public driver_device
{
public:
	probowl_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_dma_space(*this, "dma_space")
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_oki(*this, "oki%u", 1U)
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
		, m_vdp(*this, "gp9001")
		, m_gfxdecode(*this, "gfxdecode")
		, m_shared_ram(*this, "shared_ram")
		, m_mainram(*this, "mainram")
		, m_tx_videoram(*this, "tx_videoram")
		, m_tx_lineselect(*this, "tx_lineselect")
		, m_tx_linescroll(*this, "tx_linescroll")
		, m_tx_gfxram(*this, "tx_gfxram")
		, m_txlayer(*this, "txlayer")
	{ }

	void nprobowl(machine_config &config);

	virtual void device_post_load() override;

protected:
private:
	void nprobowl_68k_mem(address_map &map);

	void batrider_dma_mem(address_map &map);

	void batrider_tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void batrider_textdata_dma_w(u16 data);
	void batrider_pal_text_dma_w(u16 data);

	DECLARE_VIDEO_START(batrider);

	void tx_videoram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	DECLARE_VIDEO_START(toaplan2);
	u32 screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_vblank(int state);
	u32 screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void create_tx_tilemap(int dx = 0, int dx_flipped = 0);

	bitmap_ind8 m_custom_priority_bitmap;
	TILE_GET_INFO_MEMBER(get_text_tile_info);
	tilemap_t *m_tx_tilemap = nullptr;    /* Tilemap for extra-text-layer */

	optional_device<address_map_bank_device> m_dma_space;

	required_device<m68000_base_device> m_maincpu;
	optional_device<cpu_device> m_audiocpu;
	optional_device_array<okim6295_device, 2> m_oki;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<gp9001vdp_device> m_vdp;
	optional_device<gfxdecode_device> m_gfxdecode;
	optional_shared_ptr<u8> m_shared_ram; // 8 bit RAM shared between 68K and sound CPU
	optional_shared_ptr<u16> m_mainram;
	required_shared_ptr<u16> m_tx_videoram;
	optional_shared_ptr<u16> m_tx_lineselect;
	optional_shared_ptr<u16> m_tx_linescroll;
	optional_shared_ptr<u16> m_tx_gfxram;
	optional_device<toaplan2_txlayer_device> m_txlayer;

};



TILE_GET_INFO_MEMBER(probowl_state::get_text_tile_info)
{
	const u16 attrib = m_tx_videoram[tile_index];
	const u32 tile_number = attrib & 0x3ff;
	const u32 color = attrib >> 10;
	tileinfo.set(0,
			tile_number,
			color,
			0);
}


void probowl_state::create_tx_tilemap(int dx, int dx_flipped)
{
	m_tx_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(probowl_state::get_text_tile_info)), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_tx_tilemap->set_scroll_rows(8*32); /* line scrolling */
	m_tx_tilemap->set_scroll_cols(1);
	m_tx_tilemap->set_scrolldx(dx, dx_flipped);
	m_tx_tilemap->set_transparent_pen(0);
}



VIDEO_START_MEMBER(probowl_state,toaplan2)
{
	/* our current VDP implementation needs this bitmap to work with */
	m_screen->register_screen_bitmap(m_custom_priority_bitmap);
	m_vdp->custom_priority_bitmap = &m_custom_priority_bitmap;
}

u32 probowl_state::screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
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


u32 probowl_state::screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp->render_vdp(bitmap, cliprect);

	return 0;
}

void probowl_state::screen_vblank(int state)
{
	// rising edge
	if (state)
	{
		m_vdp->screen_eof();
	}
}


VIDEO_START_MEMBER(probowl_state,batrider)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	m_vdp->disable_sprite_buffer(); // disable buffering on this game

	/* Create the Text tilemap for this game */
	m_gfxdecode->gfx(0)->set_source(reinterpret_cast<u8 *>(m_tx_gfxram.target()));

	create_tx_tilemap(0x1d4, 0x16b);

	/* Has special banking */
	//save_item(NAME(m_gfxrom_bank));
}


static INPUT_PORTS_START( nprobowl )
	PORT_START("IN")        // Player Inputs
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_SERVICE1 )
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Set (Relay)")
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // no effect in test mode
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Gutter L")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Gutter R")
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // no effect in test mode
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Ballout")
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Ballpass")
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Mot Home")
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // no effect in test mode
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // no effect in test mode
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // no effect in test mode
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // no effect in test mode

	PORT_START("DSW")       // SW0323 and SW0324
	PORT_SERVICE_DIPLOC(   0x0001, IP_ACTIVE_HIGH, "SW0323:!1")
	PORT_DIPUNKNOWN_DIPLOC(0x0002, 0x0000, "SW0323:!2")
	PORT_DIPNAME(          0x0004, 0x0004, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW0323:!3")
	PORT_DIPSETTING(       0x0004, DEF_STR( On ) )
	PORT_DIPSETTING(       0x0000, DEF_STR( Off ) )
	PORT_DIPUNKNOWN_DIPLOC(0x0008, 0x0000, "SW0323:!4")
	PORT_DIPNAME(          0x0070, 0x0000, DEF_STR( Coinage ) )  PORT_DIPLOCATION("SW0323:!5,!6,!7")
	PORT_DIPSETTING(       0x0070, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(       0x0060, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(       0x0050, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(       0x0040, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(       0x0000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(       0x0010, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(       0x0020, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(       0x0030, DEF_STR( 1C_4C ) )
	PORT_DIPNAME(          0x0080, 0x0000, DEF_STR( Free_Play ) )  PORT_DIPLOCATION("SW0323:!8")
	PORT_DIPSETTING(       0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(       0x0080, DEF_STR( On ) )
	PORT_DIPUNKNOWN_DIPLOC(0x0100, 0x0000, "SW0324:!1")
	PORT_DIPUNKNOWN_DIPLOC(0x0200, 0x0000, "SW0324:!2")
	PORT_DIPUNKNOWN_DIPLOC(0x0400, 0x0000, "SW0324:!3")
	PORT_DIPUNKNOWN_DIPLOC(0x0800, 0x0000, "SW0324:!4")
	PORT_DIPUNKNOWN_DIPLOC(0x1000, 0x0000, "SW0324:!5")
	PORT_DIPUNKNOWN_DIPLOC(0x2000, 0x0000, "SW0324:!6")
	PORT_DIPUNKNOWN_DIPLOC(0x4000, 0x0000, "SW0324:!7")
	PORT_DIPUNKNOWN_DIPLOC(0x8000, 0x0000, "SW0324:!8")

	PORT_START("UNK")   // ??
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END



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




void probowl_state::device_post_load()
{
	if (m_tx_gfxram != nullptr)
		m_gfxdecode->gfx(0)->mark_all_dirty();
}

void probowl_state::batrider_tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Dynamic GFX decoding for Batrider / Battle Bakraid ***/

	const u16 oldword = m_tx_gfxram[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&m_tx_gfxram[offset]);
		m_gfxdecode->gfx(0)->mark_dirty(offset/16);
	}
}

void probowl_state::batrider_textdata_dma_w(u16 data)
{
	/*** Dynamic Text GFX decoding for Batrider ***/
	/*** Only done once during start-up ***/
	m_dma_space->set_bank(1);
	for (int i = 0; i < (0x8000 >> 1); i++)
	{
		m_dma_space->write16(i, m_mainram[i]);
	}
}

void probowl_state::batrider_pal_text_dma_w(u16 data)
{
	// FIXME: In batrider and bbakraid, the text layer and palette RAM
	// are probably DMA'd from main RAM by writing here at every vblank,
	// rather than being directly accessible to the 68K like the other games
	m_dma_space->set_bank(0);
	for (int i = 0; i < (0x3400 >> 1); i++)
	{
		m_dma_space->write16(i, m_mainram[i]);
	}
}




void probowl_state::nprobowl_68k_mem(address_map &map) // TODO: verify everything, implement oki banking
{
	map(0x000000, 0x0fffff).rom();
	map(0x200000, 0x207fff).ram().share(m_mainram);
	map(0x208000, 0x20ffff).ram();
	map(0x400000, 0x40000d).lrw16(
							NAME([this](offs_t offset, u16 mem_mask) { return m_vdp->read(offset ^ (0xc/2), mem_mask); }),
							NAME([this](offs_t offset, u16 data, u16 mem_mask) { m_vdp->write(offset ^ (0xc/2), data, mem_mask); }));
	map(0x500000, 0x500001).portr("IN");
	map(0x500002, 0x500003).portr("UNK");
	map(0x500004, 0x500005).portr("DSW");
	//map(0x500010, 0x500011).w();
	//map(0x500012, 0x500013).w();
	map(0x500021, 0x500021).w(m_oki[0], FUNC(okim6295_device::write));
	//map(0x500040, 0x500041).w();
	//map(0x500042, 0x500043).w();
	map(0x500060, 0x500061).lr16(NAME([this] () -> u16 { return machine().rand(); })); // TODO: Hack, probably checks something in the mechanical part, verify
	map(0x500080, 0x500081).w(FUNC(probowl_state::batrider_textdata_dma_w));
	map(0x500082, 0x500083).w(FUNC(probowl_state::batrider_pal_text_dma_w));
}


void probowl_state::tx_videoram_w(offs_t offset, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_tx_videoram[offset]);
	if (offset < 64*32)
		m_tx_tilemap->mark_tile_dirty(offset);
}

void probowl_state::tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Line-Scroll RAM for Text Layer ***/
	COMBINE_DATA(&m_tx_linescroll[offset]);

	m_tx_tilemap->set_scrollx(offset, m_tx_linescroll[offset]);
}


void probowl_state::batrider_dma_mem(address_map &map)
{
	map(0x0000, 0x1fff).ram().w(FUNC(probowl_state::tx_videoram_w)).share(m_tx_videoram);
	map(0x2000, 0x2fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x3000, 0x31ff).ram().share(m_tx_lineselect);
	map(0x3200, 0x33ff).ram().w(FUNC(probowl_state::tx_linescroll_w)).share(m_tx_linescroll);
	map(0x3400, 0x7fff).ram();
	map(0x8000, 0xffff).ram().w(FUNC(probowl_state::batrider_tx_gfxram_w)).share(m_tx_gfxram);
}


void probowl_state::nprobowl(machine_config &config)
{
	// basic machine hardware
	M68000(config, m_maincpu, 32_MHz_XTAL / 2);   // 32MHz Oscillator, divisor not verified
	m_maincpu->set_addrmap(AS_PROGRAM, &probowl_state::nprobowl_68k_mem);

	ADDRESS_MAP_BANK(config, m_dma_space, 0);
	m_dma_space->set_addrmap(0, &probowl_state::batrider_dma_mem);
	m_dma_space->set_endianness(ENDIANNESS_BIG);
	m_dma_space->set_data_width(16);
	m_dma_space->set_addr_width(16);
	m_dma_space->set_stride(0x8000);

	// video hardware
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	m_screen->set_raw(27_MHz_XTAL/4, 432, 0, 320, 262, 0, 240);
	//m_screen->set_refresh_hz(60);
	//m_screen->set_size(432, 262);
	//m_screen->set_visarea(0, 319, 0, 239);
	m_screen->set_screen_update(FUNC(probowl_state::screen_update_truxton2));
	m_screen->screen_vblank().set(FUNC(probowl_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_batrider);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 0x10000);

	GP9001_VDP(config, m_vdp, 27_MHz_XTAL);
	m_vdp->set_palette(m_palette);
	m_vdp->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_2);

	MCFG_VIDEO_START_OVERRIDE(probowl_state, batrider)

	// sound hardware
	SPEAKER(config, "mono").front_center();

	OKIM6295(config, m_oki[0], 32_MHz_XTAL/8, okim6295_device::PIN7_HIGH).add_route(ALL_OUTPUTS, "mono", 1.0); // divisor not verified
	// TODO: banking
}




// dedicated PCB marked Pro Bowl
ROM_START( nprobowl )
	ROM_REGION( 0x200000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "newprobowl-prg0-u021", 0x000000, 0x080000, CRC(3a57f122) SHA1(cc361c295f23bc0479ba49eb15de2ec6ca535a56) ) // 11xxxxxxxxxxxxxxxxx = 0xFF
	ROM_LOAD16_BYTE( "newprobowl-prg1-u024", 0x000001, 0x080000, CRC(9e9bb58a) SHA1(3d2159bde418dee5d89e3df9a248b4b1989e6ee9) ) // 11xxxxxxxxxxxxxxxxx = 0xFF

	ROM_REGION( 0x200000, "gp9001", 0 )
	ROM_LOAD16_BYTE( "newprobowl-chr1-u0519", 0x000000, 0x80000, CRC(6700a9bf) SHA1(12a72aa0b91119fbbed994aec702a6869af6f287) )
	ROM_LOAD16_BYTE( "newprobowl-chr0-u0518", 0x000001, 0x80000, CRC(0736cccd) SHA1(5a4b691be1df697fef3847456c0f4bb3466c403f) )
	ROM_LOAD16_BYTE( "newprobowl-chr2-u0520", 0x100000, 0x80000, CRC(e5f6d0b6) SHA1(6e1a4792698be4b478118e8c82edb0cf3e2286f2) )
	ROM_LOAD16_BYTE( "newprobowl-chr3-u0521", 0x100001, 0x80000, CRC(00c21951) SHA1(922abde172fb82b504dce41b95227740f16208a7) )

	ROM_REGION( 0x100000, "oki1", 0 )
	ROM_LOAD( "newprobowl-adpcm0-u0834", 0x00000, 0x80000, CRC(3b40b161) SHA1(ff8ba38dd7e0dadbf72810470e3d9afb1cd983d2) )
	ROM_LOAD( "newprobowl-adpcm1-u0835", 0x80000, 0x80000, CRC(8c191e60) SHA1(f81c2849ffc553d921fc680cd50c2997b834c44a) )
ROM_END

ROM_START( probowl2 ) // identical to New Pro Bowl but for slight mods to the GFX ROMs to display the different title
	ROM_REGION( 0x200000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "probowl2.prg0.u021", 0x000000, 0x080000, CRC(3a57f122) SHA1(cc361c295f23bc0479ba49eb15de2ec6ca535a56) ) // 11xxxxxxxxxxxxxxxxx = 0xFF
	ROM_LOAD16_BYTE( "probowl2.prg1.u024", 0x000001, 0x080000, CRC(9e9bb58a) SHA1(3d2159bde418dee5d89e3df9a248b4b1989e6ee9) ) // 11xxxxxxxxxxxxxxxxx = 0xFF

	ROM_REGION( 0x200000, "gp9001", 0 )
	ROM_LOAD16_BYTE( "probowl2.chr1.u0519", 0x000000, 0x80000, CRC(66a5c9cd) SHA1(b8edc6a66e3929af2a1399e5fb6fc341b9789136) )
	ROM_LOAD16_BYTE( "probowl2.chr0.u0518", 0x000001, 0x80000, CRC(b4439673) SHA1(5ca19d9c3c3cce9485fb9bd5224f0b14abe07f49) )
	ROM_LOAD16_BYTE( "probowl2.chr2.u0520", 0x100000, 0x80000, CRC(e05be3a0) SHA1(922c6f094358cf3df790063e68d5dd4a32d46b06) )
	ROM_LOAD16_BYTE( "probowl2.chr3.u0521", 0x100001, 0x80000, CRC(55e2565f) SHA1(b6a570b736a9b30e26d2a59b53f218ef5cd6f0f6) )

	ROM_REGION( 0x100000, "oki1", 0 )
	ROM_LOAD( "probowl2.adpcm0.u0834", 0x00000, 0x80000, CRC(be2fb43f) SHA1(f3f9bc522f4668852b751f291cef0000bb86b779) )
	ROM_LOAD( "probowl2.adpcm1.u0835", 0x80000, 0x80000, CRC(8c191e60) SHA1(f81c2849ffc553d921fc680cd50c2997b834c44a) )
ROM_END

// dedicated PCB
GAME( 1996, nprobowl,    0,        nprobowl,   nprobowl,   probowl_state, empty_init,      ROT0,   "Zuck / Able Corp", "New Pro Bowl", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_MECHANICAL | MACHINE_SUPPORTS_SAVE ) // bad GFXs, no sound banking, controls, etc
GAME( 1996, probowl2,    nprobowl, nprobowl,   nprobowl,   probowl_state, empty_init,      ROT0,   "Zuck / Able Corp", "Pro Bowl 2",   MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_MECHANICAL | MACHINE_SUPPORTS_SAVE ) // bad GFXs, no sound banking, controls, etc
