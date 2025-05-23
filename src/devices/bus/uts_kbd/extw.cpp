// license:BSD-3-Clause
// copyright-holders:AJR
/***************************************************************************

    Sperry Univac UTS Expanded Typewriter Keyboard

    This is the default keyboard for the UTS 10 and UTS 20, though the
    latter's manual claims that "SYS MODE" and "WS MODE" have no function.

    User beware: the - and / keys are swapped from their respective
    positions on most keyboards.

    The LED is located on the LOCK key, but also responds to either shift
    key being pressed (which also releases the shift lock).

    The keyboard controller is a custom 48-pin DIP marked:

        P60515
        L7340660
        20-90039-000
      © INTEL '84

    This is likely a hybrid ASIC integrating a high-speed 8035-like core
    with at least one of the custom drivers/receivers present in other
    Keytronic keyboards.

    There seems to be a slight timing error in the transmit routine which
    causes data bits to be shifted out one 64th of a bit time too quickly.

***************************************************************************/

#include "emu.h"
#include "bus/uts_kbd/extw.h"

#include "cpu/mcs48/mcs48.h"


DEFINE_DEVICE_TYPE(UTS_EXTW_KEYBOARD, uts_extw_keyboard_device, "uts_extw", "UTS Expanded Typewriter Keyboard (F4725-25)")

uts_extw_keyboard_device::uts_extw_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, UTS_EXTW_KEYBOARD, tag, owner, clock)
	, device_uts_keyboard_interface(mconfig, *this)
	, m_keys(*this, "KEY%X", 0U)
	, m_shift_led(*this, "shift_led")
	, m_p1_output(0xff)
	, m_p2_output(0xff)
	, m_shift_register(0xff)
	, m_ready_line(true)
	, m_prog_line(true)
{
}

void uts_extw_keyboard_device::device_start()
{
	m_shift_led.resolve();

	save_item(NAME(m_p1_output));
	save_item(NAME(m_p2_output));
	save_item(NAME(m_shift_register));
	save_item(NAME(m_ready_line));
	save_item(NAME(m_prog_line));
}

void uts_extw_keyboard_device::ready_w(int state)
{
	m_ready_line = bool(state);
}

u8 uts_extw_keyboard_device::p1_r()
{
	return m_ready_line ? 0xf7 : 0xff;
}

void uts_extw_keyboard_device::p1_w(u8 data)
{
	write_rxd(!BIT(data, 2));

	if (!BIT(m_p1_output, 0) && BIT(data, 0))
		m_shift_register = (m_shift_register >> 1) | (m_shift_register & 0x01) << 7;

	// P11 = hysteresis control?

	m_p1_output = data;
}

void uts_extw_keyboard_device::p2_w(u8 data)
{
	m_shift_led = !BIT(data, 4);
	m_p2_output = data;
}

void uts_extw_keyboard_device::t0_clock(u32 clk)
{
}

int uts_extw_keyboard_device::t1_r()
{
	return BIT(m_shift_register, 7);
}

void uts_extw_keyboard_device::prog_w(int state)
{
	if (!m_prog_line && state)
	{
		u8 n = m_p2_output & 0x0f;
		m_shift_register = m_keys[n]->read();
	}

	m_prog_line = bool(state);
}

static INPUT_PORTS_START(uts_extw)
	PORT_START("KEY0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E  F15") PORT_CHAR('e') PORT_CHAR('E') PORT_CHAR(UCHAR_MAMEKEY(F15)) PORT_CODE(KEYCODE_E)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('3') PORT_CHAR('#') PORT_CHAR(UCHAR_MAMEKEY(F3)) PORT_CODE(KEYCODE_3)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('2') PORT_CHAR('"') PORT_CHAR(UCHAR_MAMEKEY(F2)) PORT_CODE(KEYCODE_2)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W  F14") PORT_CHAR('w') PORT_CHAR('W') PORT_CHAR(UCHAR_MAMEKEY(F14)) PORT_CODE(KEYCODE_W)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S") PORT_CHAR('s') PORT_CHAR('S') PORT_CODE(KEYCODE_S)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A") PORT_CHAR('a') PORT_CHAR('A') PORT_CODE(KEYCODE_A)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X") PORT_CHAR('x') PORT_CHAR('X') PORT_CODE(KEYCODE_X)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z") PORT_CHAR('z') PORT_CHAR('Z') PORT_CODE(KEYCODE_Z)

	PORT_START("KEY1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Tab Back") PORT_CODE(KEYCODE_RCONTROL) // ⇤
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(0x08) PORT_CODE(KEYCODE_BACKSPACE) // doesn't work on uts10?
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('^') PORT_CHAR('~') PORT_CHAR(UCHAR_MAMEKEY(F12)) PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("[  {  Ws Mode") PORT_CHAR('[') PORT_CHAR('{') PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(']') PORT_CHAR('}') PORT_CODE(KEYCODE_BACKSLASH) // between : and return key
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(':') PORT_CHAR('*') PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(0x0d) PORT_CODE(KEYCODE_ENTER) // ↵
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Right Shift") PORT_CODE(KEYCODE_RSHIFT)

	PORT_START("KEY2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("@  \\  Sys Mode") PORT_CHAR('@') PORT_CHAR('\\') PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('/') PORT_CHAR('?') PORT_CHAR(UCHAR_MAMEKEY(F11)) PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('0') PORT_CHAR('=') PORT_CHAR(UCHAR_MAMEKEY(F10)) PORT_CODE(KEYCODE_0)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P  F22") PORT_CHAR('p') PORT_CHAR('P') PORT_CODE(KEYCODE_P)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(';') PORT_CHAR('+') PORT_CODE(KEYCODE_COLON)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",  ,") PORT_CHAR(',') PORT_CODE(KEYCODE_COMMA)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('-') PORT_CHAR('_') PORT_CODE(KEYCODE_SLASH)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".  .") PORT_CHAR('.') PORT_CODE(KEYCODE_STOP)

	PORT_START("KEY3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U  F19") PORT_CHAR('u') PORT_CHAR('U') PORT_CHAR(UCHAR_MAMEKEY(F19)) PORT_CODE(KEYCODE_U)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('7') PORT_CHAR('\'') PORT_CHAR(UCHAR_MAMEKEY(F7)) PORT_CODE(KEYCODE_7)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('6') PORT_CHAR('&') PORT_CHAR(UCHAR_MAMEKEY(F6)) PORT_CODE(KEYCODE_6)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y  F18") PORT_CHAR('y') PORT_CHAR('Y') PORT_CHAR(UCHAR_MAMEKEY(F18)) PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H") PORT_CHAR('h') PORT_CHAR('H') PORT_CODE(KEYCODE_H)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G") PORT_CHAR('g') PORT_CHAR('G') PORT_CODE(KEYCODE_G)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N") PORT_CHAR('n') PORT_CHAR('N') PORT_CODE(KEYCODE_N)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B") PORT_CHAR('b') PORT_CHAR('B') PORT_CODE(KEYCODE_B)

	PORT_START("KEY4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(u8"SOE \u25b6  Tab Set") PORT_CODE(KEYCODE_LALT) // ▶
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Insert in Line  Insert in Disp") PORT_CHAR(UCHAR_MAMEKEY(INSERT)) PORT_CODE(KEYCODE_INSERT)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Delete in Line  Delete in Disp") PORT_CHAR(UCHAR_MAMEKEY(DEL)) PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Cursor to Home") PORT_CHAR(UCHAR_MAMEKEY(HOME)) PORT_CODE(KEYCODE_HOME) // ↖
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(UP)) PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(LEFT)) PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(RIGHT)) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(DOWN)) PORT_CODE(KEYCODE_DOWN)

	PORT_START("KEY5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(5_PAD)) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(4_PAD)) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(7_PAD)) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(1_PAD)) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(2_PAD)) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(8_PAD)) PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(0_PAD)) PORT_CODE(KEYCODE_0_PAD)

	PORT_START("KEY6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(PLUS_PAD)) PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(6_PAD)) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(9_PAD)) PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(3_PAD)) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(MINUS_PAD)) PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(DEL_PAD)) PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(TAB_PAD)) PORT_CODE(KEYCODE_ENTER_PAD) // ⇥

	PORT_START("KEY7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q  F13") PORT_CHAR('q') PORT_CHAR('Q') PORT_CHAR(UCHAR_MAMEKEY(F13)) PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('1') PORT_CHAR('!') PORT_CHAR(UCHAR_MAMEKEY(F1)) PORT_CODE(KEYCODE_1)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('|') PORT_CHAR('`') PORT_CODE(KEYCODE_TILDE)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Tab Forward") PORT_CHAR(0x09) PORT_CODE(KEYCODE_TAB) // ⇥
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Fctn") PORT_CHAR(UCHAR_SHIFT_2) PORT_CODE(KEYCODE_LCONTROL)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Lock") PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK)) PORT_CODE(KEYCODE_CAPSLOCK)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('<') PORT_CHAR('>') PORT_CODE(KEYCODE_BACKSLASH2)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left Shift") PORT_CHAR(UCHAR_SHIFT_1) PORT_CODE(KEYCODE_LSHIFT)

	PORT_START("KEY8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FF  Disp 1-2") PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Clr Chg") PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FCC Locate") PORT_CODE(KEYCODE_F5)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("LF  Hang Up") PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Line Dup") PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Delete Line  Insert Line") PORT_CODE(KEYCODE_ESC)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Erase EOL  Erase EOD") PORT_CODE(KEYCODE_END)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Erase to EOF  Erase Display") PORT_CODE(KEYCODE_PGDN)

	PORT_START("KEY9")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T  F17") PORT_CHAR('t') PORT_CHAR('T') PORT_CHAR(UCHAR_MAMEKEY(F17)) PORT_CODE(KEYCODE_T)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('5') PORT_CHAR('%') PORT_CHAR(UCHAR_MAMEKEY(F5)) PORT_CODE(KEYCODE_5)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('4') PORT_CHAR('$') PORT_CHAR(UCHAR_MAMEKEY(F4)) PORT_CODE(KEYCODE_4)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R  F16") PORT_CHAR('r') PORT_CHAR('R') PORT_CHAR(UCHAR_MAMEKEY(F16)) PORT_CODE(KEYCODE_R)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F") PORT_CHAR('f') PORT_CHAR('F') PORT_CODE(KEYCODE_F)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D") PORT_CHAR('d') PORT_CHAR('D') PORT_CODE(KEYCODE_D)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V") PORT_CHAR('v') PORT_CHAR('V') PORT_CODE(KEYCODE_V)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C") PORT_CHAR('c') PORT_CHAR('C') PORT_CODE(KEYCODE_C)

	PORT_START("KEYA")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Msg Wait  Unlock") PORT_CODE(KEYCODE_F11)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FCC Clear") PORT_CODE(KEYCODE_F6)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Xmit") PORT_CODE(KEYCODE_F12)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Print  Ctl Page") PORT_CODE(KEYCODE_F10)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FCC Gen") PORT_CODE(KEYCODE_F8)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FCC Enable") PORT_CODE(KEYCODE_F7)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Xfer  Char Erase") PORT_CODE(KEYCODE_F9)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(' ') PORT_CODE(KEYCODE_SPACE)

	PORT_START("KEYB")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O  F21") PORT_CHAR('o') PORT_CHAR('O') PORT_CODE(KEYCODE_O)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('9') PORT_CHAR(')') PORT_CHAR(UCHAR_MAMEKEY(F9)) PORT_CODE(KEYCODE_9)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('8') PORT_CHAR('(') PORT_CHAR(UCHAR_MAMEKEY(F8)) PORT_CODE(KEYCODE_8)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I  F20") PORT_CHAR('i') PORT_CHAR('I') PORT_CHAR(UCHAR_MAMEKEY(F20)) PORT_CODE(KEYCODE_I)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K") PORT_CHAR('k') PORT_CHAR('K') PORT_CODE(KEYCODE_K)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L") PORT_CHAR('l') PORT_CHAR('L') PORT_CODE(KEYCODE_L)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J") PORT_CHAR('j') PORT_CHAR('J') PORT_CODE(KEYCODE_J)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M") PORT_CHAR('m') PORT_CHAR('M') PORT_CODE(KEYCODE_M)

	PORT_START("KEYC")
	PORT_BIT(0xff, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEYD")
	PORT_BIT(0xff, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEYE")
	PORT_BIT(0xff, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEYF")
	PORT_DIPNAME(0x0f, 0x0f, "National Layout") // stored as a 4-bit value even though only 7 of the first 8 are valid
	PORT_DIPSETTING(0x0f, "USA/UK English")
	PORT_DIPSETTING(0x0d, "German (QWERTZ)")
	PORT_DIPSETTING(0x0c, "European A")
	PORT_DIPSETTING(0x0b, "European B") // Swedish/Finnish?
	PORT_DIPSETTING(0x0a, "European C")
	PORT_DIPSETTING(0x09, "European D") // Spanish?
	PORT_DIPSETTING(0x08, "European E")
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

ioport_constructor uts_extw_keyboard_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(uts_extw);
}

void uts_extw_keyboard_device::prog_map(address_map &map)
{
	map(0x000, 0x7ff).rom().region("kbdc", 0);
}

void uts_extw_keyboard_device::ext_map(address_map &map)
{
	map(0x00, 0xff).nopr(); // read only once, at reset time
}

void uts_extw_keyboard_device::device_add_mconfig(machine_config &config)
{
	mcs48_cpu_device &kbdc(I8035(config, "kbdc", 9.216_MHz_XTAL)); // not a standard 8035; see notes
	kbdc.set_addrmap(AS_PROGRAM, &uts_extw_keyboard_device::prog_map);
	kbdc.set_addrmap(AS_IO, &uts_extw_keyboard_device::ext_map);
	kbdc.p1_in_cb().set(FUNC(uts_extw_keyboard_device::p1_r));
	kbdc.p1_out_cb().set(FUNC(uts_extw_keyboard_device::p1_w));
	kbdc.p2_out_cb().set(FUNC(uts_extw_keyboard_device::p2_w));
	kbdc.set_t0_clk_cb(FUNC(uts_extw_keyboard_device::t0_clock));
	kbdc.t1_in_cb().set(FUNC(uts_extw_keyboard_device::t1_r));
	kbdc.prog_out_cb().set(FUNC(uts_extw_keyboard_device::prog_w));
}

ROM_START(uts_extw)
	ROM_REGION(0x800, "kbdc", 0)
	ROM_LOAD("2716264.bin", 0x000, 0x800, CRC(75e188aa) SHA1(a6486576525f7eec617fd7f9db469063f8c357fc)) // "© 1985 Keytronic"
ROM_END

const tiny_rom_entry *uts_extw_keyboard_device::device_rom_region() const
{
	return ROM_NAME(uts_extw);
}
