#include "emu.h"
#include "toaplan2.h"
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


// with text layer
class truxton2_state : public driver_device
{
public:
	truxton2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_oki(*this, "oki%u", 1U)
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
		, m_vdp(*this, "gp9001")
		, m_gfxdecode(*this, "gfxdecode")

		, m_okibank(*this, "okibank")
		, m_soundlatch(*this, "soundlatch%u", 1U)

		, m_shared_ram(*this, "shared_ram")
		, m_mainram(*this, "mainram")
		, m_tx_videoram(*this, "tx_videoram")
		, m_tx_lineselect(*this, "tx_lineselect")
		, m_tx_linescroll(*this, "tx_linescroll")
		, m_tx_gfxram(*this, "tx_gfxram")
		, m_dma_space(*this, "dma_space")
		, m_z80_rom(*this, "audiocpu")
		, m_oki_rom(*this, "oki%u", 1U)
		, m_audiobank(*this, "audiobank")
		, m_raizing_okibank{
			{ *this, "raizing_okibank0_%u", 0U },
			{ *this, "raizing_okibank1_%u", 0U } }
		, m_eeprom(*this, "eeprom")
		, m_eepromout(*this, "EEPROMOUT")
		, m_txlayer(*this, "txlayer")
	{ }


	void init_bgaregga();

	virtual void device_post_load() override;

	required_device<m68000_base_device> m_maincpu;
	optional_device<cpu_device> m_audiocpu;
	optional_device_array<okim6295_device, 2> m_oki;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<gp9001vdp_device> m_vdp;
	optional_device<gfxdecode_device> m_gfxdecode;
	optional_memory_bank m_okibank;
	optional_device_array<generic_latch_8_device, 4> m_soundlatch; // tekipaki, batrider, bgaregga, batsugun

	optional_shared_ptr<u8> m_shared_ram; // 8 bit RAM shared between 68K and sound CPU
	optional_shared_ptr<u16> m_mainram;
	required_shared_ptr<u16> m_tx_videoram;
	optional_shared_ptr<u16> m_tx_lineselect;
	optional_shared_ptr<u16> m_tx_linescroll;
	optional_shared_ptr<u16> m_tx_gfxram;

	optional_device<address_map_bank_device> m_dma_space;
	optional_region_ptr<u8> m_z80_rom;
	optional_region_ptr_array<u8, 2> m_oki_rom;

	optional_memory_bank m_audiobank;
	optional_memory_bank_array<8> m_raizing_okibank[2];

	optional_device<eeprom_serial_93cxx_device> m_eeprom;
	optional_ioport m_eepromout;

	optional_device<toaplan2_txlayer_device> m_txlayer;


	tilemap_t *m_tx_tilemap = nullptr;    /* Tilemap for extra-text-layer */

	void raizing_z80_bankswitch_w(u8 data);
	void raizing_oki_bankswitch_w(offs_t offset, u8 data);
	void tx_videoram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void tx_linescroll_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	void install_raizing_okibank(int chip);

	TILE_GET_INFO_MEMBER(get_text_tile_info);


	u32 screen_update_truxton2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_bootleg(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void create_tx_tilemap(int dx = 0, int dx_flipped = 0);







	u8 shared_ram_r(offs_t offset) { return m_shared_ram[offset]; }
	void shared_ram_w(offs_t offset, u8 data) { m_shared_ram[offset] = data; }
	void coin_w(u8 data);

	DECLARE_VIDEO_START(toaplan2);
	u32 screen_update_toaplan2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_vblank(int state);

	u16 video_count_r();
	void toaplan2_reset(int state);


	bitmap_ind8 m_custom_priority_bitmap;
};
