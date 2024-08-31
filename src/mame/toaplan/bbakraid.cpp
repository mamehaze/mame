
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

	virtual void device_post_load() override;

protected:
	virtual void machine_start() override;

private:

	void bbakraid_68k_mem(address_map &map);
	void bbakraid_sound_z80_mem(address_map &map);
	void bbakraid_sound_z80_port(address_map &map);

	u16 bbakraid_eeprom_r();
	void bbakraid_eeprom_w(u8 data);

	INTERRUPT_GEN_MEMBER(bbakraid_snd_interrupt);


	void batrider_bank_cb(u8 layer, u32 &code);

	void batrider_dma_mem(address_map &map);

	u16 batrider_z80rom_r(offs_t offset);
	void batrider_soundlatch_w(u8 data);
	void batrider_soundlatch2_w(u8 data);
	void batrider_unknown_sound_w(u16 data);
	void batrider_clear_sndirq_w(u16 data);
	void batrider_sndirq_w(u8 data);
	void batrider_clear_nmi_w(u8 data);

	void batrider_objectbank_w(offs_t offset, u8 data);

	u8 m_sndirq_line = 0;        /* IRQ4 for batrider, IRQ2 for bbakraid */
	u8 m_z80_busreq = 0;
	u16 m_gfxrom_bank[8]{};       /* Batrider object bank */

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
	u16 video_count_r();
	bitmap_ind8 m_custom_priority_bitmap;
	void toaplan2_reset(int state);
	TILE_GET_INFO_MEMBER(get_text_tile_info);
	void coin_w(u8 data);
	tilemap_t *m_tx_tilemap = nullptr;    /* Tilemap for extra-text-layer */

};


TILE_GET_INFO_MEMBER(bbakraid_state::get_text_tile_info)
{
	const u16 attrib = m_tx_videoram[tile_index];
	const u32 tile_number = attrib & 0x3ff;
	const u32 color = attrib >> 10;
	tileinfo.set(0,
			tile_number,
			color,
			0);
}


void bbakraid_state::toaplan2_reset(int state)
{
	if (m_audiocpu != nullptr)
		m_audiocpu->pulse_input_line(INPUT_LINE_RESET, attotime::zero);
}

u16 bbakraid_state::video_count_r()
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

void bbakraid_state::create_tx_tilemap(int dx, int dx_flipped)
{
	m_tx_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(bbakraid_state::get_text_tile_info)), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_tx_tilemap->set_scroll_rows(8*32); /* line scrolling */
	m_tx_tilemap->set_scroll_cols(1);
	m_tx_tilemap->set_scrolldx(dx, dx_flipped);
	m_tx_tilemap->set_transparent_pen(0);
}


VIDEO_START_MEMBER(bbakraid_state,toaplan2)
{
	/* our current VDP implementation needs this bitmap to work with */
	m_screen->register_screen_bitmap(m_custom_priority_bitmap);
	m_vdp->custom_priority_bitmap = &m_custom_priority_bitmap;
}

u32 bbakraid_state::screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
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


u32 bbakraid_state::screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	m_custom_priority_bitmap.fill(0, cliprect);
	m_vdp->render_vdp(bitmap, cliprect);

	return 0;
}

void bbakraid_state::screen_vblank(int state)
{
	// rising edge
	if (state)
	{
		m_vdp->screen_eof();
	}
}




void bbakraid_state::device_post_load()
{
	if (m_tx_gfxram != nullptr)
		m_gfxdecode->gfx(0)->mark_all_dirty();
}

VIDEO_START_MEMBER(bbakraid_state,batrider)
{
	VIDEO_START_CALL_MEMBER(toaplan2);

	m_vdp->disable_sprite_buffer(); // disable buffering on this game

	/* Create the Text tilemap for this game */
	m_gfxdecode->gfx(0)->set_source(reinterpret_cast<u8 *>(m_tx_gfxram.target()));

	create_tx_tilemap(0x1d4, 0x16b);

	/* Has special banking */
	save_item(NAME(m_gfxrom_bank));
}


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

void bbakraid_state::machine_start()
{
	save_item(NAME(m_z80_busreq));
}

u16 bbakraid_state::bbakraid_eeprom_r()
{
	// Bit 0x01 returns the status of BUSAK from the Z80.
	// BUSRQ is activated via bit 0x10 on the EEPROM write port.
	// These accesses are made when the 68K wants to read the Z80
	// ROM code. Failure to return the correct status incurrs a Sound Error.

	u8 data;
	data  = ((m_eeprom->do_read() & 0x01) << 4);
	data |= ((m_z80_busreq >> 4) & 0x01);   // Loop BUSRQ to BUSAK

	return data;
}


void bbakraid_state::bbakraid_eeprom_w(u8 data)
{
	if (data & ~0x1f)
		logerror("CPU #0 PC:%06X - Unknown EEPROM data being written %02X\n",m_maincpu->pc(),data);

	m_eepromout->write(data, 0xff);

	m_z80_busreq = data & 0x10; // see bbakraid_eeprom_r above
}



u16 bbakraid_state::batrider_z80rom_r(offs_t offset)
{
	return m_z80_rom[offset];
}

// these two latches are always written together, via a single move.l instruction
void bbakraid_state::batrider_soundlatch_w(u8 data)
{
	m_soundlatch[0]->write(data & 0xff);
	m_audiocpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}


void bbakraid_state::batrider_soundlatch2_w(u8 data)
{
	m_soundlatch[1]->write(data & 0xff);
	m_audiocpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}

void bbakraid_state::batrider_unknown_sound_w(u16 data)
{
	// the 68K writes here when it wants a sound acknowledge IRQ from the Z80
	// for bbakraid this is on every sound command; for batrider, only on certain commands
}


void bbakraid_state::batrider_clear_sndirq_w(u16 data)
{
	// not sure whether this is correct
	// the 68K writes here during the sound IRQ handler, and nowhere else...
	m_maincpu->set_input_line(m_sndirq_line, CLEAR_LINE);
}


void bbakraid_state::batrider_sndirq_w(u8 data)
{
	// if batrider_clear_sndirq_w() is correct, should this be ASSERT_LINE?
	m_maincpu->set_input_line(m_sndirq_line, HOLD_LINE);
}


void bbakraid_state::batrider_clear_nmi_w(u8 data)
{
	m_audiocpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}

void bbakraid_state::batrider_objectbank_w(offs_t offset, u8 data)
{
	data &= 0xf;
	if (m_gfxrom_bank[offset] != data)
	{
		m_gfxrom_bank[offset] = data;
		m_vdp->set_dirty();
	}
}


void bbakraid_state::batrider_tx_gfxram_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Dynamic GFX decoding for Batrider / Battle Bakraid ***/

	const u16 oldword = m_tx_gfxram[offset];

	if (oldword != data)
	{
		COMBINE_DATA(&m_tx_gfxram[offset]);
		m_gfxdecode->gfx(0)->mark_dirty(offset/16);
	}
}

void bbakraid_state::batrider_textdata_dma_w(u16 data)
{
	/*** Dynamic Text GFX decoding for Batrider ***/
	/*** Only done once during start-up ***/
	m_dma_space->set_bank(1);
	for (int i = 0; i < (0x8000 >> 1); i++)
	{
		m_dma_space->write16(i, m_mainram[i]);
	}
}

void bbakraid_state::batrider_pal_text_dma_w(u16 data)
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

void bbakraid_state::coin_w(u8 data)
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



void bbakraid_state::bbakraid_68k_mem(address_map &map)
{
	map(0x000000, 0x1fffff).rom();
	// actually 200000 - 20ffff is probably all main RAM, and the text and palette RAM are written via DMA
	map(0x200000, 0x207fff).ram().share(m_mainram);
	map(0x208000, 0x20ffff).ram();
	map(0x300000, 0x33ffff).r(FUNC(bbakraid_state::batrider_z80rom_r));
	map(0x400000, 0x40000d).lrw16(
							NAME([this](offs_t offset, u16 mem_mask) { return m_vdp->read(offset ^ (0xc/2), mem_mask); }),
							NAME([this](offs_t offset, u16 data, u16 mem_mask) { m_vdp->write(offset ^ (0xc/2), data, mem_mask); }));
	map(0x500000, 0x500001).portr("IN");
	map(0x500002, 0x500003).portr("SYS-DSW");
	map(0x500004, 0x500005).portr("DSW");
	map(0x500006, 0x500007).r(FUNC(bbakraid_state::video_count_r));
	map(0x500009, 0x500009).w(FUNC(bbakraid_state::coin_w));
	map(0x500011, 0x500011).r(m_soundlatch[2], FUNC(generic_latch_8_device::read));
	map(0x500013, 0x500013).r(m_soundlatch[3], FUNC(generic_latch_8_device::read));
	map(0x500015, 0x500015).w(FUNC(bbakraid_state::batrider_soundlatch_w));
	map(0x500017, 0x500017).w(FUNC(bbakraid_state::batrider_soundlatch2_w));
	map(0x500018, 0x500019).r(FUNC(bbakraid_state::bbakraid_eeprom_r));
	map(0x50001a, 0x50001b).w(FUNC(bbakraid_state::batrider_unknown_sound_w));
	map(0x50001c, 0x50001d).w(FUNC(bbakraid_state::batrider_clear_sndirq_w));
	map(0x50001f, 0x50001f).w(FUNC(bbakraid_state::bbakraid_eeprom_w));
	map(0x500080, 0x500081).w(FUNC(bbakraid_state::batrider_textdata_dma_w));
	map(0x500082, 0x500083).w(FUNC(bbakraid_state::batrider_pal_text_dma_w));
	map(0x5000c0, 0x5000cf).w(FUNC(bbakraid_state::batrider_objectbank_w)).umask16(0x00ff);
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
	map(0x44, 0x44).w(FUNC(bbakraid_state::batrider_sndirq_w));
	map(0x46, 0x46).w(FUNC(bbakraid_state::batrider_clear_nmi_w));
	map(0x48, 0x48).r(m_soundlatch[0], FUNC(generic_latch_8_device::read));
	map(0x4a, 0x4a).r(m_soundlatch[1], FUNC(generic_latch_8_device::read));
	map(0x80, 0x81).rw("ymz", FUNC(ymz280b_device::read), FUNC(ymz280b_device::write));
}

INTERRUPT_GEN_MEMBER(bbakraid_state::bbakraid_snd_interrupt)
{
	device.execute().set_input_line(0, HOLD_LINE);
}


void bbakraid_state::batrider_bank_cb(u8 layer, u32 &code)
{
	code = (m_gfxrom_bank[code >> 15] << 15) | (code & 0x7fff);
}


void bbakraid_state::tx_videoram_w(offs_t offset, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_tx_videoram[offset]);
	if (offset < 64*32)
		m_tx_tilemap->mark_tile_dirty(offset);
}

void bbakraid_state::tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask)
{
	/*** Line-Scroll RAM for Text Layer ***/
	COMBINE_DATA(&m_tx_linescroll[offset]);

	m_tx_tilemap->set_scrollx(offset, m_tx_linescroll[offset]);
}


void bbakraid_state::batrider_dma_mem(address_map &map)
{
	map(0x0000, 0x1fff).ram().w(FUNC(bbakraid_state::tx_videoram_w)).share(m_tx_videoram);
	map(0x2000, 0x2fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x3000, 0x31ff).ram().share(m_tx_lineselect);
	map(0x3200, 0x33ff).ram().w(FUNC(bbakraid_state::tx_linescroll_w)).share(m_tx_linescroll);
	map(0x3400, 0x7fff).ram();
	map(0x8000, 0xffff).ram().w(FUNC(bbakraid_state::batrider_tx_gfxram_w)).share(m_tx_gfxram);
}


void bbakraid_state::bbakraid(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 32_MHz_XTAL/2);   // 16MHz, 32MHz Oscillator
	m_maincpu->set_addrmap(AS_PROGRAM, &bbakraid_state::bbakraid_68k_mem);
	m_maincpu->reset_cb().set(FUNC(bbakraid_state::toaplan2_reset));

	Z80(config, m_audiocpu, XTAL(32'000'000)/6);     /* 5.3333MHz , 32MHz Oscillator */
	m_audiocpu->set_addrmap(AS_PROGRAM, &bbakraid_state::bbakraid_sound_z80_mem);
	m_audiocpu->set_addrmap(AS_IO, &bbakraid_state::bbakraid_sound_z80_port);
	m_audiocpu->set_periodic_int(FUNC(bbakraid_state::bbakraid_snd_interrupt), attotime::from_hz(XTAL(32'000'000) / 6 / 12000)); // sound CPU clock (divider unverified)

	config.set_maximum_quantum(attotime::from_hz(600));

	EEPROM_93C66_8BIT(config, m_eeprom);

	ADDRESS_MAP_BANK(config, m_dma_space, 0);
	m_dma_space->set_addrmap(0, &bbakraid_state::batrider_dma_mem);
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
	m_screen->set_screen_update(FUNC(bbakraid_state::screen_update_truxton2));
	m_screen->screen_vblank().set(FUNC(bbakraid_state::screen_vblank));
	m_screen->set_palette(m_palette);

	toaplan2_screen_device& t2screen(TOAPLAN2_SCREEN(config, "t2screen", 27_MHz_XTAL / 4));
	t2screen.set_screen(m_screen);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_batrider);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 0x10000);

	GP9001_VDP(config, m_vdp, 27_MHz_XTAL);
	m_vdp->set_palette(m_palette);
	m_vdp->set_tile_callback(FUNC(bbakraid_state::batrider_bank_cb));
	m_vdp->vint_out_cb().set_inputline(m_maincpu, M68K_IRQ_1);

	MCFG_VIDEO_START_OVERRIDE(bbakraid_state,batrider)

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
