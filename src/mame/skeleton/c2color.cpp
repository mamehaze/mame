// license:BSD-3-Clause
// copyright-holders:David Haywood
/******************************************************************************

    basic information
    https://gbatemp.net/threads/the-c2-color-game-console-an-obscure-chinese-handheld.509320/

    "The C2 is a glorious console with a D-Pad, Local 2.4GHz WiFi, Cartridge slot, A, B, and C buttons,
     and has micro usb power! Don't be fooled though, there is no lithium battery, so you have to put in
     3 AA batteries if you don't want to play with it tethered to a charger.

     It comes with a built in game based on the roco kingdom characters.

     In addition, there is a slot on the side of the console allowing cards to be swiped through. Those
     cards can add characters to the game. The console scans the barcode and a new character or item appears in the game for you to use.

     The C2 comes with 9 holographic game cards that will melt your eyes."

    also includes a link to the following video
    https://www.youtube.com/watch?v=D3XO4aTZEko

    TODO:
    identify CPU type - It's an i8051 derived CPU, and seems to be "Mars Semiconductor Corp" related, there is a MARS-PCCAM string
                        amongst other things, this is a known USB identifier for the "Discovery Kids Digital Camera"
                        Possibly a MR97327B, which is listed as RISC-51 in places, but little information can be found in English
    - Dump the internal boot ROM; its initial SPI-to-DRAM load is substituted below.
    - Establish CPU/peripheral clocks, DMA/codec timing and the remaining interrupt sources.
    - Identify the companion and implement commands beyond the boot challenge.
    - Complete LCD/OSD palettes, display latching, JPEG formats and audio controls.
    - Barcode reader, radio, USB and cartridge boot selection are not implemented.
    - Flash erase/program behaviour depends on the incomplete generic SPI flash model.

*******************************************************************************/

#include "emu.h"
#include "bus/c2color/slot.h"
#include "bus/c2color/carts.h"

#include "machine/generic_spi_flash.h"
#include "sound/dac.h"

#include "rendutil.h"
#include "screen.h"
#include "softlist_dev.h"
#include "speaker.h"

#include "ioprocs.h"

#include "c2color_companion.h"
#include "c2color_cpu.h"

#include <vector>

#define LOG_REGS (1U << 1)
#define LOG_DMA  (1U << 2)
#define LOG_SPI  (1U << 3)

#define VERBOSE (LOG_REGS)
#include "logmacro.h"


namespace {

class c2_color_state : public driver_device
{
public:
	c2_color_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_cart(*this, "cartslot")
		, m_cart_region(nullptr)
		, m_screen(*this, "screen")
		, m_flash(*this, "flash%u", 1U)
		, m_xram(*this, "xram")

		, m_dram_dword_out(*this, "dram_dword_out")
		, m_dram_dword_out2(*this, "dram_dword_out2")
	    , m_dram_dword_in(*this, "dram_dword_in")
	    , m_dram_dword_in2(*this, "dram_dword_in2")
	    , m_jpeg_dest(*this, "jpeg_dest")
	    , m_jpeg_src(*this, "jpeg_src")
	    , m_jpeg_len(*this, "jpeg_len")
	    , m_render_base(*this, "render_base")
	    , m_render_unk(*this, "render_unk")
	    , m_render_overlay(*this, "render_overlay")
	    , m_render_mask(*this, "render_mask")
	    , m_overlay_width(*this, "overlay_width")
	    , m_overlay_height(*this, "overlay_height")
	    , m_overlay_x(*this, "overlay_x")
	    , m_overlay_y(*this, "overlay_y")

		, m_companion(*this, "companion")
		, m_dac(*this, "dac")
		, m_buttons(*this, "BUTTONS")
		, m_battery(*this, "BATTERY")
	{ }

	void c2_color(machine_config &config);

private:
	virtual void machine_start() override ATTR_COLD;
	virtual void machine_reset() override ATTR_COLD;

	u32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void clear_state();
	u8 code_r(offs_t offset);
	u8 io_r(offs_t offset);
	void io_w(offs_t offset, u8 data);
	u8 &reg(u16 address) { return m_regs[address - 0x2000]; }
	u32 reg32(u16 address) const;
	void spi_select();
	u8 spi_exchange(u8 data);
	void dma(unsigned channel);
	u8 dma_r(u8 source, u32 address);
	void dma_w(u8 destination, u32 address, u8 data);
	void dram_access(u8 data);
	void jpeg_decode();
	void audio_control();
	void update_irq();

	void io_2002_w(u8 data);
	void io_2064_w(u8 data);
	void io_20ad_w(u8 previous, u8 data);
	void io_2185_w(u8 previous, u8 data);
	void io_21c0_w(u8 data);
	void io_2402_w(u8 data);
	u8 io_229b_r();
	void io_229b_w(u8 data);
	void io_229d_w(u8 data);

	u8 io_2400_r();

	u8 m_2400_value;
	u8 m_2402_value;
	u8 m_229b_value;
	u8 m_2405_value;

	u8 io_246d_r();
	void io_246d_w(u8 data);
	u8 io_2405_r();
	void io_2405_w(u8 data);
	void io_2400_w(u8 data);
	void write_unk_reg(u16 address, u8 data);
	u8 read_unk_reg(u16 address);

	void c2_dma_channel_w(int channel, offs_t offset, u8 data);
	u8 c2_dma_channel_r(int channel, offs_t offset);


	u8 c2_dma_channel0_r(offs_t offset);
	void c2_dma_channel0_w(offs_t offset, u8 data);
	u8 c2_dma_channel1_r(offs_t offset);
	void c2_dma_channel1_w(offs_t offset, u8 data);

	u32 get_32(u8* rgn);
	u32 get_24(u8* rgn);
	u16 get_16(u8* rgn);
	u8 get_8(u8* rgn);

	TIMER_CALLBACK_MEMBER(audio_tick);

	void prog_map(address_map &map) ATTR_COLD;
	void ext_map(address_map &map) ATTR_COLD;

	required_device<c2_color_cpu_device> m_maincpu;
	required_device<c2color_cartslot_device> m_cart;
	memory_region *m_cart_region;
	required_device<screen_device> m_screen;
	required_device_array<generic_spi_flash_device, 2> m_flash;
	required_shared_ptr<u8> m_xram;

	required_shared_ptr<u8> m_dram_dword_out;
	required_shared_ptr<u8> m_dram_dword_out2;
	required_shared_ptr<u8> m_dram_dword_in;
	required_shared_ptr<u8> m_dram_dword_in2;
	required_shared_ptr<u8> m_jpeg_dest;
	required_shared_ptr<u8> m_jpeg_src;
	required_shared_ptr<u8> m_jpeg_len;
	required_shared_ptr<u8> m_render_base;
	required_shared_ptr<u8> m_render_unk;
	required_shared_ptr<u8> m_render_overlay;
	required_shared_ptr<u8> m_render_mask;
	required_shared_ptr<u8> m_overlay_width;
	required_shared_ptr<u8> m_overlay_height;
	required_shared_ptr<u8> m_overlay_x;
	required_shared_ptr<u8> m_overlay_y;

	required_device<c2_color_companion_device> m_companion;
	required_device<dac_16bit_r2r_device> m_dac;
	required_ioport m_buttons;
	required_ioport m_battery;
	u8 m_companion_sda;
	emu_timer *m_audio_timer;
	u32 m_audio_address;
	u32 m_audio_remaining;
	bool m_audio_enabled;

	static constexpr u32 DRAM_SIZE = 0x200000;
	std::unique_ptr<u8[]> m_dram;
	std::unique_ptr<u16[]> m_osd_code;
	std::unique_ptr<u8[]> m_osd_attr;
	bool m_lcd_sleep = true;
	bool m_lcd_on = false;
	std::unique_ptr<u8[]> m_flash_data[2];
	u8 m_regs[0x600];

	struct dma_channel
	{
		u8 m_dma_trigger;
		u8 m_dma_count[4];
		u8 m_dma_source_addr[4];
		u8 m_dma_source;
		u8 m_dma_dest_addr[4];
		u8 m_dma_dest;

		u8 m_dma_fill[4];
		u8 m_dma_fill_pos;
	};

	dma_channel m_dma_channel[2];



	s8 m_spi_selected;
	u8 m_quant[2][128];
	u8 m_quant_pos;
};

u32 c2_color_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(rgb_t::black(), cliprect);
	if (m_lcd_sleep || !m_lcd_on)
		return 0;

	u32 const base = get_24(m_render_base);
	u32 const font = (u32(reg(0x21a4)) | (u32(reg(0x21a5)) << 8)) << 9;
	u16 const columns = reg(0x2186);
	u16 const rows = reg(0x2187);

	u32 const overlay = get_32(m_render_overlay);
	u32 const mask = get_32(m_render_mask);
	u16 const overlay_width = get_16(m_overlay_width);
	u16 const overlay_height = get_16(m_overlay_height);
	u16 const overlay_x = get_16(m_overlay_x);
	u16 const overlay_y = get_16(m_overlay_y);
	for (int y = cliprect.min_y; y <= cliprect.max_y; ++y)
	{
		for (int x = cliprect.min_x; x <= cliprect.max_x; ++x)
		{
			u32 const address = base + (y * screen.visible_area().width() + x) * 2;
			u16 const pixel = m_dram[address & (DRAM_SIZE - 1)] | (u16(m_dram[(address + 1) & (DRAM_SIZE - 1)]) << 8);
			bitmap.pix(y, x) = rgb_t(pal5bit(pixel >> 11), pal6bit(pixel >> 5), pal5bit(pixel));
		}
	}

	// A second RGB565 plane has a separate packed, LSB-first opacity mask.
	// DMA constructs the plane in DRAM; the display controller composites it.
	// TODO: Configuration latch timing, signed positions and non-byte-aligned widths.
	if (BIT(get_8(m_render_unk), 0) && overlay_width && overlay_height)
	{
		rectangle area(overlay_x, overlay_x + overlay_width - 1, overlay_y, overlay_y + overlay_height - 1);
		area &= cliprect;
		for (int y = area.min_y; y <= area.max_y; ++y)
		{
			for (int x = area.min_x; x <= area.max_x; ++x)
			{
				u32 const index = (y - overlay_y) * overlay_width + x - overlay_x;
				if (BIT(m_dram[(mask + (index >> 3)) & (DRAM_SIZE - 1)], index & 7))
				{
					u32 const address = overlay + index * 2;
					u16 const pixel = m_dram[address & (DRAM_SIZE - 1)] | (u16(m_dram[(address + 1) & (DRAM_SIZE - 1)]) << 8);
					bitmap.pix(y, x) = rgb_t(pal5bit(pixel >> 11), pal6bit(pixel >> 5), pal5bit(pixel));
				}
			}
		}
	}

	for (int y = cliprect.min_y; y <= cliprect.max_y; ++y)
	{
		for (int x = cliprect.min_x; x <= cliprect.max_x; ++x)
		{
			if (BIT(reg(0x2185), 0) && x / 16 < columns && y / 20 < rows)
			{
				u16 const cell = (y / 20) * columns + x / 16;
				u32 const glyph = font + m_osd_code[cell] * 80 + (y % 20) * 4 + (x % 16) / 4;
				u8 const ink = BIT(m_dram[glyph & (DRAM_SIZE - 1)], (x & 3) * 2, 2);
				// TODO: Decode the OSD palette, attributes and blending controls.
				if (ink)
					bitmap.pix(y, x) = rgb_t(ink * 85, ink * 85, ink * 85);
			}
		}
	}
	return 0;
}

void c2_color_state::clear_state()
{
	m_companion_sda = 1;
	std::fill(std::begin(m_regs), std::end(m_regs), 0);
	std::fill(std::begin(m_dma_channel[0].m_dma_fill), std::end(m_dma_channel[0].m_dma_fill), 0);
	std::fill(std::begin(m_dma_channel[1].m_dma_fill), std::end(m_dma_channel[1].m_dma_fill), 0);

	for (int i = 0; i < 2; i++)
	{
		m_dma_channel[i].m_dma_trigger = 0;
		m_dma_channel[i].m_dma_source = 0;
		m_dma_channel[i].m_dma_dest = 0;
		m_dma_channel[i].m_dma_fill_pos = 0;

		for (int j = 0; j < 4; j++)
		{
			m_dma_channel[i].m_dma_count[j] = 0;
			m_dma_channel[i].m_dma_source_addr[j] = 0;
			m_dma_channel[i].m_dma_dest_addr[j] = 0;
			m_dma_channel[i].m_dma_fill[j] = 0;
		}
	}

	m_spi_selected = -1;

	for (auto& table : m_quant)
		std::fill(std::begin(table), std::end(table), 0);
	m_quant_pos = 0;

	m_lcd_sleep = true;
	m_lcd_on = false;
	m_audio_address = m_audio_remaining = 0;
	m_audio_enabled = false;
}

void c2_color_state::machine_start()
{
	m_dram = std::make_unique<u8[]>(DRAM_SIZE);
	m_osd_code = std::make_unique<u16[]>(0x10000);
	m_osd_attr = std::make_unique<u8[]>(0x10000);

	clear_state();

	save_pointer(NAME(m_dram), DRAM_SIZE);
	save_pointer(NAME(m_osd_code), 0x10000);
	save_pointer(NAME(m_osd_attr), 0x10000);
	save_item(NAME(m_lcd_sleep));
	save_item(NAME(m_lcd_on));
	save_item(NAME(m_regs));
	save_item(NAME(m_dma_channel[0].m_dma_fill));
	save_item(NAME(m_dma_channel[0].m_dma_fill_pos));
	save_item(NAME(m_spi_selected));
	save_item(NAME(m_quant));
	save_item(NAME(m_quant_pos));
	save_item(NAME(m_companion_sda));
	save_item(NAME(m_audio_address));
	save_item(NAME(m_audio_remaining));
	save_item(NAME(m_audio_enabled));

	save_item(NAME(m_2400_value));
	save_item(NAME(m_2402_value));
	save_item(NAME(m_229b_value));
	save_item(NAME(m_2405_value));


	m_audio_timer = timer_alloc(FUNC(c2_color_state::audio_tick), this);
	machine().save().register_postload(save_prepost_delegate(FUNC(c2_color_state::update_irq), this));

	for (unsigned i = 0; i != 2; ++i)
	{
		memory_region *const region = i == 2 ? m_cart_region : memregion(i ? "spi2" : "spi1");
		u32 const length = region ? region->bytes() : 1;
		m_flash_data[i] = std::make_unique<u8[]>(length);
		if (region)
			std::copy_n(region->base(), length, m_flash_data[i].get());
		else
			m_flash_data[i][0] = 0xff;
		m_flash[i]->set_rom_ptr(m_flash_data[i].get());
		m_flash[i]->set_rom_size(length);
		save_pointer(NAME(m_flash_data[i]), length, i);
	}
}

void c2_color_state::machine_reset()
{
	std::fill_n(&m_xram[0], 0x2000, 0);
	std::fill_n(m_dram.get(), DRAM_SIZE, 0);
	std::fill_n(m_osd_code.get(), 0x10000, 0);
	std::fill_n(m_osd_attr.get(), 0x10000, 0);
	clear_state();
	m_audio_timer->adjust(attotime::never);
	m_dac->write(0x8000);
	reg(0x2144) = 1;
	reg(0x2152) = 0x20;
	reg(0x2042) = 0x10;
	reg(0x2002) = 3;
	update_irq();

	m_2400_value = 0;
	m_2402_value = 0;
	m_229b_value = 0;
	m_2405_value = 0;

	// The internal boot ROM is undumped.  Substitute its initial load of the
	// built-in firmware into DRAM, skipping the SPI image's 32-byte header.
	// The header specifies a 0x40000-byte initial load.
	// Subsequent resource transfers use the emulated SPI and DMA controllers.
	std::copy_n(m_flash_data[0].get() + 0x20, 0x40000, m_dram.get());
}

u8 c2_color_state::code_r(offs_t offset)
{
	if (BIT(reg(0x2141), 0) && offset >= 0x4000 && offset < 0x4200)
		return m_xram[offset - 0x4000];
	u32 const address = offset < 0x8000 ? offset : (u32(reg(0x2144)) << 15) | (offset & 0x7fff);
	return m_dram[address & (DRAM_SIZE - 1)];
}

u32 c2_color_state::get_32(u8* rgn) { return u32(rgn[0]) | (u32(rgn[1]) << 8) | (u32(rgn[2]) << 16) | (u32(rgn[3]) << 24); }
u32 c2_color_state::get_24(u8* rgn) { return u32(rgn[0]) | (u32(rgn[1]) << 8) | (u32(rgn[2]) << 16); }
u16 c2_color_state::get_16(u8* rgn) { return u16(rgn[0]) | (u16(rgn[1]) << 8); }
u8 c2_color_state::get_8(u8* rgn) { return u8(rgn[0]); }

u32 c2_color_state::reg32(u16 address) const
{
	u8 const *const bytes = &m_regs[address - 0x2000];
	return u32(bytes[0]) | (u32(bytes[1]) << 8) | (u32(bytes[2]) << 16) | (u32(bytes[3]) << 24);
}

void c2_color_state::spi_select()
{
	// The first flash uses an active-high select; the other two are active-low.
	s8 const selected = BIT(reg(0x2155), 5) ? 0 : !BIT(reg(0x2152), 5) ? 1 : !BIT(reg(0x2042), 4) ? 2 : -1;
	if (selected == m_spi_selected)
		return;
	for (unsigned i = 0; i != 2; ++i)
		m_flash[i]->cs_w(selected != int(i));
	m_spi_selected = selected;
	LOGMASKED(LOG_SPI, "%s: SPI select %d\n", machine().describe_context(), selected);
}

u8 c2_color_state::spi_exchange(u8 data)
{
	if (m_spi_selected < 0 || (m_spi_selected == 2 && !m_cart_region))
		return 0xff;
	m_flash[m_spi_selected]->write(data);
	return m_flash[m_spi_selected]->read();
}

u8 c2_color_state::dma_r(u8 source, u32 address)
{
	switch (source)
	{
	case 0:
		return m_xram[address & 0x1fff];
	case 2:
	case 3:
		return m_dram[address & (DRAM_SIZE - 1)];
	case 9:
		return spi_exchange(0xff);
	default:
		return 0xff;
	}
}

void c2_color_state::dma_w(u8 destination, u32 address, u8 data)
{
	switch (destination)
	{
	case 0:
		m_xram[address & 0x1fff] = data;
		break;
	case 2:
	case 3:
		m_dram[address & (DRAM_SIZE - 1)] = data;
		break;
	}
}

void c2_color_state::dma(unsigned channel)
{
	u8 const source = m_dma_channel[channel].m_dma_source & 0x0f;
	u8 const destination = m_dma_channel[channel].m_dma_dest & 0x0f;
	u32 const source_address = get_32(m_dma_channel[channel].m_dma_source_addr);
	u32 const destination_address = get_32(m_dma_channel[channel].m_dma_dest_addr);
	u32 const count = get_32(m_dma_channel[channel].m_dma_count);
	bool const fill = BIT(m_dma_channel[channel].m_dma_trigger, 1);
	LOGMASKED(LOG_DMA, "%s: DMA %u %x:%08x -> %x:%08x, %08x bytes%s\n", machine().describe_context(), channel,
		source, source_address, destination, destination_address, count, fill ? " (fill)" : "");

	// The observed transfers fit within DRAM.  Other modes, peripheral targets
	// and transfer timing still need investigation.
	if (count > DRAM_SIZE || (!fill && source != 0 && source != 2 && source != 3 && source != 9)
		|| (destination != 0 && destination != 2 && destination != 3))
	{
		LOGMASKED(LOG_DMA, "%s: unsupported DMA transfer\n", machine().describe_context());
		return;
	}
	for (u32 i = 0; i != count; ++i)
	{
		dma_w(destination, destination_address + i, fill ? m_dma_channel[channel].m_dma_fill[i & 3] : dma_r(source, source_address + i));
	}

	reg(0x2147) |= 0x40 << channel;
	if (destination == 2 || destination == 3)
	{
		reg(0x2147) |= 0x01;
		reg(0x2148) |= 0x40;
	}
	update_irq();
}

void c2_color_state::dram_access(u8 data)
{
	// Firmware first writes 03, then requests a four-byte read or write.
	if (data == 0x07)
	{
		u32 const address = get_32(m_dram_dword_out);
		for (unsigned i = 0; i != 4; ++i)
			m_dram[(address + i) & (DRAM_SIZE - 1)] = m_dram_dword_out[i];
		m_2405_value |= 0x08;
	}
	else if (data == 0x13)
	{
		u32 const address = get_32(m_dram_dword_in);
		for (unsigned i = 0; i != 4; ++i)
			m_dram_dword_in[i] = m_dram[(address + i) & (DRAM_SIZE - 1)];
		m_2405_value |= 0x20;
	}
}

void c2_color_state::jpeg_decode()
{
	u16 const width = reg32(0x2024) & 0xffff;
	u16 const height = reg32(0x202a) & 0xffff;
	u32 const source = get_32(m_jpeg_src);
	u32 const destination = get_24(m_jpeg_dest);
	u32 const length = get_24(m_jpeg_len);

	if (!width || !height || u32(width) * height > DRAM_SIZE / 2 || !length || length > DRAM_SIZE)
		return;

	// The hardware receives quantisation tables and a baseline 4:2:2 scan
	// separately.  Supply the JPEG framing expected by the existing decoder;
	// libjpeg supplies the standard Huffman tables when DHT is omitted.
	// TODO: Establish whether DRAM holds RGB565 or YUV422 converted on display.
	std::vector<u8> stream{ 0xff, 0xd8, 0xff, 0xdb, 0x00, 0x84 };
	for (unsigned table = 0; table != 2; ++table)
	{
		stream.push_back(table);
		for (unsigned i = 0; i != 64; ++i)
			stream.push_back(m_quant[table][i * 2]);
	}
	u8 const header[] = { 0xff, 0xc0, 0x00, 0x11, 0x08, u8(height >> 8), u8(height), u8(width >> 8), u8(width), 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f, 0x00 };
	stream.insert(stream.end(), std::begin(header), std::end(header));

	for (u32 i = 0; i != length; ++i)
		stream.push_back(m_dram[(source + i) & (DRAM_SIZE - 1)]);

	stream.push_back(0xff);
	stream.push_back(0xd9);
	bitmap_argb32 decoded;
	auto input = util::ram_read(stream.data(), stream.size());
	if (input)
		render_load_jpeg(decoded, *input);
	if (!decoded.valid())
		return;

	for (u32 y = 0; y != decoded.height(); ++y)
	{
		for (u32 x = 0; x != decoded.width(); ++x)
		{
			rgb_t const color(decoded.pix(y, x));
			u16 const pixel = ((color.r() >> 3) << 11) | ((color.g() >> 2) << 5) | (color.b() >> 3);
			u32 const address = destination + (y * width + x) * 2;
			m_dram[address & (DRAM_SIZE - 1)] = pixel;
			m_dram[(address + 1) & (DRAM_SIZE - 1)] = pixel >> 8;
		}
	}
	reg(0x2148) |= 0x30;
	update_irq();
}

void c2_color_state::update_irq()
{
	m_maincpu->set_input_line(MCS51_INT0_LINE, ((reg(0x2147) & reg(0x2145)) | (reg(0x2148) & reg(0x2146))) ? ASSERT_LINE : CLEAR_LINE);
	m_maincpu->set_input_line(MCS51_INT1_LINE, ((reg(0x214d) & reg(0x214b)) | (reg(0x214e) & reg(0x214c))) ? ASSERT_LINE : CLEAR_LINE);
}

void c2_color_state::audio_control()
{
	bool const enabled = BIT(reg(0x208c), 0) && BIT(reg(0x2097), 1) && BIT(reg(0x246d), 1);
	if (enabled && !m_audio_enabled)
	{
		// Addresses and lengths are in 16-bit samples.  The observed setting
		// (20ab bits 2:1 clear) plays signed little-endian mono PCM at 8 kHz.
		// TODO: Other rates, formats, volume and output filtering.
		m_audio_address = (reg32(0x20a5) & 0xffffff) * 2;
		m_audio_remaining = reg32(0x2099) & 0xffffff;
		m_audio_timer->adjust(attotime::zero, 0, attotime::from_hz(8000));
	}
	else if (!enabled)
	{
		m_audio_timer->adjust(attotime::never);
		m_dac->write(0x8000);
	}
	m_audio_enabled = enabled;
}

TIMER_CALLBACK_MEMBER(c2_color_state::audio_tick)
{
	if (m_audio_remaining)
	{
		u16 const sample = m_dram[m_audio_address & (DRAM_SIZE - 1)]
			| (u16(m_dram[(m_audio_address + 1) & (DRAM_SIZE - 1)]) << 8);
		m_dac->write(sample ^ 0x8000);
		m_audio_address += 2;
		--m_audio_remaining;
	}
	else
	{
		m_dac->write(0x8000);
		m_audio_timer->adjust(attotime::never);
		reg(0x214e) |= 0x10;
		update_irq();
	}
}

u8 c2_color_state::io_r(offs_t offset)
{
	u16 const address = 0x2000 + offset;
	u8 data = reg(address);
	switch (address)
	{
// 2000 region

	case 0x2002: data = (data & ~2) | ((BIT(data, 1) && m_companion_sda) ? 2 : 0); break;
	case 0x2004: break; // unknown
	case 0x200a: break; // unknown
	case 0x200b: break; // unknown
	case 0x203a: break; // unknown
	case 0x203b: break; // unknown
	case 0x203c: break; // unknown
	case 0x203f: break; // unknown

	case 0x2040: break; // unknown
	case 0x2041: break; // unknown
	case 0x2042: break; // unknown
	case 0x204b: break; // unknown
	case 0x204c: break; // unknown

	case 0x2051: break; // unknown

	case 0x2053: data = (data & 0x03) | (m_buttons->read() & 0xfc); break;

	case 0x205a: break; // unknown
	case 0x205b: break; // unknown
	case 0x205c: break; // unknown

	case 0x205f: break; // Timer
	case 0x2060: break; // Timer
	case 0x2061: break; // Timer
	case 0x2062: break; // Timer

	case 0x2064: break; // Timer
	case 0x2065: break; // unknown
	case 0x2067: break; // unknown

	case 0x208d: break; // unknown
	case 0x2097: break; // unknown

	case 0x20ac: break; // unknown ADC?
	case 0x20ab: break; // unknown
	case 0x20ad: break; // unknown ADC?
	case 0x20ae: break; // unknown ADC?
	case 0x20af: break; // unknown ADC?

// 2100 region

	case 0x2140: break; // unknown
	case 0x2141: break; // unknown XRAM control
	case 0x2142: break; // unknown
	case 0x2145: break; // unknown IRQ 
	case 0x2146: break; // unknown IRQ
	case 0x2147: break; // unknown IRQ
	case 0x2148: break; // unknown IRQ
	case 0x2149: break; // unknown IRQ
	case 0x214b: break; // unknown IRQ
	case 0x214c: break; // unknown IRQ
	case 0x214e: break; // unknown IRQ

	case 0x2151: break; // unknown
	case 0x2152: data = (data & 0x7f) | (BIT(m_buttons->read(), 0) ? 0x80 : 0); break;
	case 0x2155: break; // unknown SPI
	case 0x2156: data |= 0x18; /* SPI transmit / receive ready; transfers currently complete immediately. */ break;
	case 0x2158: break; // unknown
	case 0x215c: break; // unknown
	case 0x215d: break; // unknown

	case 0x2184: break; // unknown
	case 0x2185: break; // unknown
	case 0x2188: break; // unknown
	case 0x218a: break; // unknown
	case 0x218b: break; // unknown
	case 0x218c: break; // unknown

	case 0x21c7: break; // unknown

// 2200 region



	default:
	{
		if (!machine().side_effects_disabled())
			LOGMASKED(LOG_REGS, "%s: read %04x = %02x\n", machine().describe_context(), address, data);
	}
	break;

	}

	return data;
}

void c2_color_state::io_2002_w(u8 data)
{
	if (!BIT(data, 0))
		m_companion->scl_write(0);
	m_companion->sda_write(BIT(data, 1));
	if (BIT(data, 0))
		m_companion->scl_write(1);
}

void c2_color_state::io_2064_w(u8 data)
{
	if (BIT(data, 1))
	{
		// The millisecond unit is inferred from the LCD Sleep Out delay.
		// TODO: Identify the timer clock and divider controls.
		u32 const ticks = machine().time().as_ticks(1000);
		for (unsigned i = 0; i != 4; ++i)
			reg(0x205f + i) = ticks >> (8 * i);
		reg(0x2064) &= ~0x02;
	}
}

void c2_color_state::io_20ad_w(u8 previous, u8 data)
{
	reg(0x20ad) = (data & ~9) | (previous & 8);
	if (BIT(data, 0))
	{
		// Channel 0 measures the batteries.  Voltage scaling, other inputs
		// and conversion timing are unknown; use representative raw levels.
		u16 const sample = (reg(0x20ac) & 3) == 0 ? m_battery->read() : 0;
		reg(0x20ae) = sample >> 8;
		reg(0x20af) = sample;
		reg(0x20ad) |= 8;
	}
}

void c2_color_state::io_2185_w(u8 previous, u8 data)
{
	// The two table-write strobes are armed separately before both go high.
	if ((data & 6) == 6)
	{
		if (!(previous & 2))
			m_osd_code[reg32(0x219d) & 0xffff] = reg32(0x219f) & 0xffff;
		if (!(previous & 4))
			m_osd_attr[reg32(0x21a1) & 0xffff] = reg(0x21a3);
		reg(0x2185) &= ~6;
	}
}

void c2_color_state::io_21c0_w(u8 data)
{
	switch (data)
	{
	case 0x10: m_lcd_sleep = true; break;
	case 0x11: m_lcd_sleep = false; break;
	case 0x28: m_lcd_on = false; break;
	case 0x29: m_lcd_on = true; break;
	}
}

void c2_color_state::io_2402_w(u8 data)
{
	u8 previous = m_2402_value;
	m_2402_value = data;

	if ((data & 0x18) == 0x18 && (previous & 0x18) != 0x18)
		jpeg_decode();
}

u8 c2_color_state::io_229b_r()
{
	return m_229b_value;
}

void c2_color_state::io_229b_w(u8 data)
{
	u8 previous = m_229b_value;
	m_229b_value = data;

	if (BIT(previous, 3) && !BIT(data, 3))
		m_quant_pos = 0;
}

void c2_color_state::io_229d_w(u8 data)
{
	m_quant[BIT(m_229b_value, 2) ? 0 : 1][m_quant_pos++ & 0x7f] = data;
}



void c2_color_state::io_w(offs_t offset, u8 data)
{
	u16 const address = 0x2000 + offset;
	u8 const previous = reg(address);
	reg(address) = data;
	switch (address)
	{
// 2000 region

	case 0x2002: io_2002_w(data); break;
	case 0x2004: break; // unknown
	case 0x200a: break; // unknown
	case 0x200b: break; // unknown

	case 0x2024: case 0x2025: case 0x2026: case 0x2027: break; // JPEG width
	case 0x202a: case 0x202b: case 0x202c: case 0x202d: break; // JPEG height

	case 0x2028: break; // unknown
	case 0x2029: break; // unknown
	case 0x202e: break; // unknown
	case 0x202f: break; // unknown

	case 0x203a: break; // unknown
	case 0x203b: break; // unknown
	case 0x203c: break; // unknown
	case 0x203f: break; // unknown
	case 0x2040: break; // unknown

	case 0x2041: break;
	case 0x2042: spi_select(); break;

	case 0x204b: break;
	case 0x204c: break;

	case 0x2051: break;
	case 0x2053: break;
	case 0x205b: break;
	case 0x205c: break;

	case 0x205f: case 0x2060: case 0x2061: case 0x2062: break; // Timer Val
	case 0x2064: io_2064_w(data); break; // Timer?
	case 0x2065: break; // unknown
	case 0x2066: break;
	case 0x2067: break;

	case 0x208c: audio_control(); break;
	case 0x208d: break;

	case 0x2093: break; // unknown
	case 0x2097: audio_control(); break;
	case 0x2099: case 0x209a: case 0x209b: break; //Audio remaining related
	case 0x20a5: case 0x20a6: case 0x20a7: break; // Audio address related

	case 0x20ab: break; // unknown
	case 0x20ac: break; // -ADC ?
	case 0x20ad: io_20ad_w(previous, data); break;
	case 0x20ae: break; // -ADC ?
	case 0x20af: break; // -ADC ?

	case 0x20b6: break; // unknown
	case 0x20b7: break; // unknown
	case 0x20b8: break; // unknown
	case 0x20b9: break; // unknown

// 2100 region

	case 0x2140: break; // unknown
	case 0x2141: break; // unknown  XRAM control
	case 0x2142: break; // unknown

	case 0x2144: break; // RAM access address upper
	case 0x2145: update_irq(); break;
	case 0x2146: update_irq(); break;
	case 0x2147: break; // IRQ related (DMA)
	case 0x2148: break; // IRQ related (JPEG decoding, DMA)
	case 0x2149: reg(0x2147) &= ~data; update_irq(); break;
	case 0x214a: reg(0x2148) &= ~data; update_irq(); break;
	case 0x214b: update_irq(); break;
	case 0x214c: update_irq(); break;
	case 0x214d: break; // IRQ related
	case 0x214e: break; // IRQ related (audio)
	case 0x214f: reg(0x214d) &= ~data; update_irq(); break;
	case 0x2150: reg(0x214e) &= ~data; update_irq(); break;
	case 0x2151: break; // unknown

	case 0x2152: spi_select(); break;
	case 0x2154: break; // unknown
	case 0x2155: spi_select(); break;
	case 0x2156: break; // unknown

	case 0x2157: spi_exchange(data); break; /* Bit 6 of 2155 also enables a debug output stream on this port,  With no flash selected those bytes do not enter a flash command parser. */
	case 0x2158: reg(address) = spi_exchange(data);	break;
	case 0x215c: break; // unknown
	case 0x215d: break; // unknown

	case 0x2184: break; // unknown
	case 0x2185: io_2185_w(previous, data); break;
	case 0x2186: break; // render columns
	case 0x2187: break; // render rows
	case 0x2188: break; // unknown
	case 0x218a: break; // unknown
	case 0x218b: break; // unknown
	case 0x218c: break; // unknown
	case 0x218d: break; // unknown
	case 0x218e: break; // unknown
	case 0x218f: break; // unknown
	case 0x2190: break; // unknown
	case 0x2191: break; // unknown
	case 0x2192: break; // unknown
	case 0x2193: break; // unknown
	case 0x2194: break; // unknown
	case 0x2195: break; // unknown
	case 0x2196: break; // unknown
	case 0x2197: break; // unknown
	case 0x2198: break; // unknown
	case 0x2199: break; // unknown
	case 0x219a: break; // unknown
	case 0x219b: break; // unknown
	case 0x219c: break; // unknown
	case 0x219d: case 0x219e: break; // render OSD related
	case 0x219f: case 0x21a0: break; // render OSD related
	case 0x21a1: case 0x21a2: break; // render OSD related
	case 0x21a3: break; // render OSD related
	case 0x21a4: case 0x21a5: break; // render font
	case 0x21a6: break; // unknown
	case 0x21a7: break; // unknown
	case 0x21a8: break; // unknown
	case 0x21a9: break; // unknown
	case 0x21aa: break; // unknown
	case 0x21ab: break; // unknown
	case 0x21ac: break; // unknown
	case 0x21ad: break; // unknown
	case 0x21ae: break; // unknown
	case 0x21af: break; // unknown
	case 0x21b0: break; // unknown
	case 0x21b1: break; // unknown
	case 0x21b2: break; // unknown
	case 0x21b3: break; // unknown
	case 0x21b4: break; // unknown
	case 0x21b5: break; // unknown
	case 0x21b6: break; // unknown
	case 0x21b7: break; // unknown
	case 0x21b8: break; // unknown
	case 0x21b9: break; // unknown
	case 0x21ba: break; // unknown
	case 0x21bb: break; // unknown

	case 0x21bf: break; // unknown
	case 0x21c0: io_21c0_w(data); break;
	case 0x21c7: break; // unknown

	default:
		LOGMASKED(LOG_REGS, "%s: write %04x = %02x\n", machine().describe_context(), address, data); break;

	}
}

void c2_color_state::prog_map(address_map &map)
{
	map(0x0000, 0xffff).r(FUNC(c2_color_state::code_r));
}

u8 c2_color_state::io_2400_r()
{
	u8 data = m_2400_value;
	return (data & 0x3f) | (BIT(data, 2) ? 0x80 : 0x40); /* DRAM stop / resume acknowledgement. */
}

u8 c2_color_state::io_246d_r()
{
	return read_unk_reg(0x246d);
}

void c2_color_state::io_246d_w(u8 data)
{
	write_unk_reg(0x246d, data);
	audio_control();
}

u8 c2_color_state::io_2405_r()
{
	return m_2405_value;
}

void c2_color_state::io_2405_w(u8 data)
{
	m_2405_value = data;
	dram_access(data);
}

void c2_color_state::io_2400_w(u8 data)
{
	m_2400_value = data;
}

void c2_color_state::write_unk_reg(u16 address, u8 data)
{
	address -= 0x2000;
	m_regs[address] = data;
}

u8 c2_color_state::read_unk_reg(u16 address)
{
	address -= 0x2000;
	return m_regs[address];
}

u8 c2_color_state::c2_dma_channel_r(int channel, offs_t offset)
{
	if (offset == 0x26)
	{
		// read
		return 0x00;
	}
	// not seen read
	return 0x00;
}

void c2_color_state::c2_dma_channel_w(int channel, offs_t offset, u8 data)
{
	if (offset == 0x00)
	{
		m_dma_channel[channel].m_dma_trigger = data;
		if (data == 2)
			m_dma_channel[channel].m_dma_fill_pos = 0;
		if (BIT(data, 0))
			dma(channel);
	}
	else if (offset < 0x05)
	{
		int realoffset = offset - 0x01;
		m_dma_channel[channel].m_dma_count[realoffset] = data;
	}
	else if (offset < 0x0d)
	{
		// unknown
	}
	else if (offset == 0xd)
	{
		m_dma_channel[channel].m_dma_fill[m_dma_channel[channel].m_dma_fill_pos++ & 3] = data;
	}
	else if (offset < 0x0f)
	{
		// unknown
	}
	else if (offset == 0x10)
	{
		m_dma_channel[channel].m_dma_source = data;
	}
	else if (offset < 0x12)
	{
		// unknown
	}
	else if (offset < 0x16)
	{
		int realoffset = offset - 0x12;
		m_dma_channel[channel].m_dma_source_addr[realoffset] = data;
	}
	else if (offset < 0x25)
	{
		// unknown
	}
	else if (offset == 0x25)
	{
		m_dma_channel[channel].m_dma_dest = data;
	}
	else if (offset == 0x26)
	{
		// unknown
	}
	else if (offset < 0x2b)
	{
		int realoffset = offset - 0x27;
		m_dma_channel[channel].m_dma_dest_addr[realoffset] = data;
	}
	else
	{
		// unknown
	}
}

u8 c2_color_state::c2_dma_channel0_r(offs_t offset)
{
	return c2_dma_channel_r(0, offset);
}

void c2_color_state::c2_dma_channel0_w(offs_t offset, u8 data)
{
	c2_dma_channel_w(0, offset, data);
}

u8 c2_color_state::c2_dma_channel1_r(offs_t offset)
{
	return c2_dma_channel_r(1, offset);
}

void c2_color_state::c2_dma_channel1_w(offs_t offset, u8 data)
{
	c2_dma_channel_w(1, offset, data);
}

void c2_color_state::ext_map(address_map &map)
{
	map(0x0000, 0x1fff).ram().share("xram");
	map(0x2000, 0x21ff).rw(FUNC(c2_color_state::io_r), FUNC(c2_color_state::io_w));

	//////////////////////////////////////////////
	// 0x2200 region
	//////////////////////////////////////////////

	map(0x2200, 0x2239).rw(FUNC(c2_color_state::c2_dma_channel0_r), FUNC(c2_color_state::c2_dma_channel0_w));
	map(0x223a, 0x2273).rw(FUNC(c2_color_state::c2_dma_channel1_r), FUNC(c2_color_state::c2_dma_channel1_w));

	map(0x229a, 0x229a).nopw();
	map(0x229b, 0x229b).rw(FUNC(c2_color_state::io_229b_r), FUNC(c2_color_state::io_229b_w));

	map(0x229d, 0x229d).w(FUNC(c2_color_state::io_229d_w));

	map(0x22a1, 0x22a1).ram();

	//////////////////////////////////////////////
	// 0x2300 region
	//////////////////////////////////////////////

	map(0x2345, 0x2345).ram();
	map(0x2436, 0x243c).nopw();
	map(0x234d, 0x234d).ram();

	//////////////////////////////////////////////
	// 0x2400 region
	//////////////////////////////////////////////

	map(0x2400, 0x2400).rw(FUNC(c2_color_state::io_2400_r), FUNC(c2_color_state::io_2400_w));

	map(0x2402, 0x2402).w(FUNC(c2_color_state::io_2402_w));

	map(0x2403, 0x2404).ram();
	map(0x2405, 0x2405).rw(FUNC(c2_color_state::io_2405_r), FUNC(c2_color_state::io_2405_w));
	map(0x2406, 0x2407).nopw();

	map(0x2429, 0x242c).ram().share("dram_dword_out2");
	map(0x242d, 0x2430).ram().share("dram_dword_out");

	map(0x2431, 0x2434).ram().share("dram_dword_in2");
	map(0x2435, 0x2438).ram().share("dram_dword_in");

	map(0x2446, 0x2449).nopw();

	map(0x244a, 0x244c).ram().share("jpeg_dest");
	map(0x244d, 0x244d).ram();
	map(0x244e, 0x2451).ram().share("jpeg_src");
	map(0x2452, 0x2454).ram().share("jpeg_len");
	map(0x2455, 0x2455).ram();

	map(0x2456, 0x245f).nopw();

	map(0x2460, 0x2462).ram().share("render_base");

	map(0x2464, 0x2466).nopw();

	map(0x2468, 0x246a).nopw();

	map(0x246c, 0x246c).nopw();

	map(0x246d, 0x246d).rw(FUNC(c2_color_state::io_246d_r), FUNC(c2_color_state::io_246d_w));

	map(0x246e, 0x246e).ram().share("render_unk");

	map(0x246f, 0x2472).ram().share("render_overlay");
	map(0x2473, 0x2476).ram().share("render_mask");
	map(0x2477, 0x2478).ram().share("overlay_width");
	map(0x2479, 0x247a).ram().share("overlay_height");
	map(0x247b, 0x247c).ram().share("overlay_x");
	map(0x247d, 0x247e).ram().share("overlay_y");

	map(0x24a4, 0x24a4).ram();

	//////////////////////////////////////////////
	// 0x2500 region
	//////////////////////////////////////////////

	map(0x2541, 0x2541).ram();
	map(0x2542, 0x2546).nopw();

	map(0x254a, 0x254a).ram();

	map(0x256b, 0x256e).nopw();

	map(0x25e3, 0x25e6).nopw();
}

static INPUT_PORTS_START( c2_color )
	PORT_START("BUTTONS")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Button C")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Button A")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Button B")

	PORT_START("BATTERY")
	PORT_CONFNAME(0x0fff, 0x0200, "Battery")
	PORT_CONFSETTING(0x0200, "Full")
	PORT_CONFSETTING(0x0100, "Low")
	PORT_CONFSETTING(0x0040, "Empty")
INPUT_PORTS_END

void c2_color_state::c2_color(machine_config &config)
{
	C2_COLOR_CPU(config, m_maincpu, 24'000'000); // exact type and clock / opcode cycle counts unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &c2_color_state::prog_map);
	m_maincpu->set_addrmap(AS_DATA, &c2_color_state::ext_map);

	SCREEN(config, m_screen);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(0));
	m_screen->set_size(320, 240);
	m_screen->set_visarea_full();
	m_screen->set_screen_update(FUNC(c2_color_state::screen_update));

	SPEAKER(config, "speaker").front_center();
	DAC_16BIT_R2R(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 1.0);

	GENERIC_SPI_FLASH(config, m_flash[0]);
	GENERIC_SPI_FLASH(config, m_flash[1]);

	C2_COLOR_COMPANION(config, m_companion);
	m_companion->sda_callback().set([this] (int state) { m_companion_sda = state; });

	C2COLOR_CARTSLOT(config, "cartslot", c2color_plain_slot);

	SOFTWARE_LIST(config, "cart_list").set_original("c2color_cart");
}

ROM_START( c2color )
	ROM_REGION( 0x4000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bootloader", 0x0000, 0x4000, NO_DUMP )

	// As with the cartridges, each of these has a 0x20 byte header before the i8051
	// code starts.  This suggests it is unlikely the game runs directly from the SPI
	// ROM and more likely a bootloader copies the code into RAM.
	// The Mainboard has a 2MByte DRAM on it

	// This, the larger of the 2 ROMs contains unique code, it appears to be the base
	// game, and system functions.  It also has some 16-bit signed PCM samples.
	ROM_REGION( 0x800000, "spi1", ROMREGION_ERASEFF )
	ROM_LOAD( "spi.u7", 0x000000, 0x800000, CRC(6a4d2cd2) SHA1(46e109bbd5db206911716919ad13efc080cbdf34) )

	// The smaller ROM is much more similar to the cartridges (actually identical up
	// until the first MRDB resource block at 0x26000 aside from a few bytes in the
	// 0x20 header, and the game number at the 0x20000 mark being 0)
	//
	// This ROM also has a 2nd MRDB resource block, whereas the cartridges only have
	// a single block
	//
	// The code still contains a lot of generic 'firmware' like functions, but it is
	// unclear if any of the code is used, or if these ROMs are used more like skins
	// for the base game, accessing the resource table only
	//
	// The MRDB tables index IMG0 resources containing quantisation tables and
	// baseline JPEG scans.  No sound effects or non-graphical resources have
	// been identified in this flash.

	ROM_REGION( 0x400000, "spi2", ROMREGION_ERASEFF )
	ROM_LOAD( "spi.u16", 0x000000, 0x400000, CRC(9101b02a) SHA1(8c31e7641f4667bd8d5d7cc991cd5976828a0628) )
ROM_END

} // anonymous namespace


//    year, name,         parent,  compat, machine,      input,        class,              init,       company,  fullname,                             flags
CONS( 201?, c2color,      0,       0,      c2_color,   c2_color, c2_color_state, empty_init, "Baiyi Animation", "C2 Color (China)", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS | MACHINE_NOT_WORKING )
