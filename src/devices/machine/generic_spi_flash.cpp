// license:BSD-3-Clause
// copyright-holders:David Haywood

// HLE-like implementation for SPI Flash ROMs using byte interface rather than SPI signals

#include "emu.h"
#include "generic_spi_flash.h"

#define LOG_SPI (1U << 1)

#define VERBOSE     (0)

#include "logmacro.h"

DEFINE_DEVICE_TYPE(GENERIC_SPI_FLASH, generic_spi_flash_device, "generic_spi_flash", "Generic SPI Flash handling")

generic_spi_flash_device::generic_spi_flash_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, GENERIC_SPI_FLASH, tag, owner, clock)
	, device_nvram_interface(mconfig, *this)
	, m_multibyte_status_read(1)
	, m_multibyte_status_write(1)
{
}

void generic_spi_flash_device::device_start()
{
	save_item(NAME(m_spiaddr));
	save_item(NAME(m_spi_state));
	save_item(NAME(m_spilatch));
	save_item(NAME(m_spidir));
}

void generic_spi_flash_device::device_reset()
{
	m_spiaddr = 0;
	m_spi_state = 0;
	m_spilatch = 0;
	m_spidir = false;
}

void generic_spi_flash_device::write(uint8_t data)
{
	switch (m_spi_state)
	{
	case READY_FOR_COMMAND:
		if (data == 0x01)
		{
			logerror("Set SPI to WRSR, 1 or 2 params required\n");
			m_spi_state = READY_FOR_STATUS_VALUES2;
		}
		else if (data == 0x9f)
		{
			logerror("Set SPI to RDID\n");
			m_spi_state = READY_FOR_RDID2;
			m_spidir = 1;
		}
		else if (data == 0x03)
		{
			logerror("Set SPI to READ (normal) - 3 params needed\n");
			m_spi_state = READY_FOR_ADDRESS2;
		}
		else if (data == 0x05)
		{
			logerror("Set SPI to RDSR\n");
			m_spi_state = READY_FOR_STATUS_READ;
		}
		else if (data == 0x0b)
		{
			logerror("Set SPI to FAST READ (normal) - 4 params needed\n");
			m_spi_state = READY_FOR_HSADDRESS2;
		}
		else if (data == 0x06)
		{
			logerror("Set SPI to WRITE ENABLE\n");
			// write enable
			m_spi_state = READY_FOR_COMMAND;
		}
		else if (data == 0x04)
		{
			logerror("Set SPI to WRITE DISABLE\n");
			// write disable
			m_spi_state = READY_FOR_COMMAND;
		}
		else if (data == 0x02)
		{
			logerror("Set SPI to PAGE PROGRAM\n");
			// page program
			m_spi_state = READY_FOR_WRITEADDRESS2;
		}
		else if (data == 0x20)
		{
			logerror("Set SPI to ERASE 4K SECTOR\n");
			// erase 4k sector
			m_spi_state = READY_FOR_ERASEADDRESS2;
		}
		else if (data == 0x90)
		{
			logerror("Set SPI to REMS\n");
			// REMS - Read Electronic Manufacturer & Device ID
			m_spi_state = REMS_STEP3;
		}
		else if (data == 0xab)
		{
			logerror("Set SPI to RES\n");
			// RES - Read Electronic ID
			m_spi_state = READY_FOR_COMMAND;
		}
		else
		{
			fatalerror("SPI set to unknown mode %02x\n", data);
		}
		break;

	case READY_FOR_STATUS_VALUES2:
		logerror("status step 2\n");
		m_spi_state = READY_FOR_STATUS_VALUES1;
		if (m_multibyte_status_write == 0)
			m_spi_state = READY_FOR_COMMAND;
		break;

	case READY_FOR_STATUS_VALUES1:
		logerror("status step 1\n");
		m_spi_state = READY_FOR_COMMAND;
		break;

	case REMS_STEP3:
		logerror("rems step 3\n");
		m_spi_state = REMS_STEP2;
		break;

	case REMS_STEP2:
		logerror("rems step 2\n");
		m_spi_state = REMS_STEP1;
		break;

	case REMS_STEP1:
		logerror("rems step 1\n");
		m_spi_state = REMS_STEP0;
		break;

	case REMS_STEP0:
		logerror("rems step 0\n");
		m_spi_state = REMS_STEPx;
		break;

	case REMS_STEPx:
		logerror("rems step x\n");
		m_spi_state = READY_FOR_COMMAND;
		break;


	case READY_FOR_WRITEADDRESS2:
		m_spiaddr = (m_spiaddr & 0x00ffff) | (data << 16);
		m_spi_state = READY_FOR_WRITEADDRESS1;
		break;

	case READY_FOR_WRITEADDRESS1:
		m_spiaddr = (m_spiaddr & 0xff00ff) | (data << 8);
		m_spi_state = READY_FOR_WRITEADDRESS0;
		break;

	case READY_FOR_WRITEADDRESS0:
		m_spiaddr = (m_spiaddr & 0xffff00) | (data);
		m_spi_state = READY_FOR_WRITE;
		LOGMASKED(LOG_SPI, "SPI set to page WRITE mode with address %08x\n", m_spiaddr);
		break;

	case READY_FOR_ERASEADDRESS2:
		m_spiaddr = (m_spiaddr & 0x00ffff) | (data << 16);
		m_spi_state = READY_FOR_ERASEADDRESS1;
		break;

	case READY_FOR_ERASEADDRESS1:
		m_spiaddr = (m_spiaddr & 0xff00ff) | (data << 8);
		m_spi_state = READY_FOR_ERASEADDRESS0;
		break;

	case READY_FOR_ERASEADDRESS0:
		m_spiaddr = (m_spiaddr & 0xffff00) | (data);
		m_spi_state = READY_FOR_WRITE;
		LOGMASKED(LOG_SPI, "SPI set to page ERASE mode with address %08x\n", m_spiaddr);
		break;

	case READY_FOR_WRITE:
		LOGMASKED(LOG_SPI, "Write SPI data %02x\n", data);
		m_spiptr[(m_spiaddr++) & (m_length - 1)] = data;
		break;

	case READY_FOR_ADDRESS2:
		m_spiaddr = (m_spiaddr & 0x00ffff) | (data << 16);
		m_spi_state = READY_FOR_ADDRESS1;
		break;

	case READY_FOR_ADDRESS1:
		m_spiaddr = (m_spiaddr & 0xff00ff) | (data << 8);
		m_spi_state = READY_FOR_ADDRESS0;
		break;

	case READY_FOR_ADDRESS0:
		m_spiaddr = (m_spiaddr & 0xffff00) | (data);
		m_spi_state = READY_FOR_READ;
		LOGMASKED(LOG_SPI, "SPI set to READ mode with address %08x\n", m_spiaddr);
		break;

	case READY_FOR_HSADDRESS2:
		m_spiaddr = (m_spiaddr & 0x00ffff) | (data << 16);
		m_spi_state = READY_FOR_HSADDRESS1;
		break;

	case READY_FOR_HSADDRESS1:
		m_spiaddr = (m_spiaddr & 0xff00ff) | (data << 8);
		m_spi_state = READY_FOR_HSADDRESS0;
		break;

	case READY_FOR_HSADDRESS0:
		m_spiaddr = (m_spiaddr & 0xffff00) | (data);
		m_spi_state = READY_FOR_HSDUMMY;
		break;

	case READY_FOR_HSDUMMY:
		m_spi_state = READY_FOR_READ;
		LOGMASKED(LOG_SPI, "SPI set to High Speed READ mode with address %08x\n", m_spiaddr);
		break;

	case READY_FOR_SECTORERASEADDRESS2:
		m_spiaddr = (m_spiaddr & 0x00ffff) | (data << 16);
		m_spi_state = READY_FOR_SECTORERASEADDRESS1;
		break;

	case READY_FOR_SECTORERASEADDRESS1:
		m_spiaddr = (m_spiaddr & 0xff00ff) | (data << 8);
		m_spi_state = READY_FOR_SECTORERASEADDRESS0;
		break;

	case READY_FOR_SECTORERASEADDRESS0:
		m_spiaddr = (m_spiaddr & 0xffff00) | (data);
		LOGMASKED(LOG_SPI, "SPI set to Erase Sector with address %08x\n", m_spiaddr);
		break;

	case READY_FOR_READ:
		m_spilatch = m_spiptr[(m_spiaddr++) & (m_length - 1)];
		logerror("set latch to %02x\n", m_spilatch);
		break;

	case READY_FOR_STATUS_READ:
		m_spilatch = 0x00;
		m_spi_state = READY_FOR_STATUS_READ2;
		if (m_multibyte_status_read == 0)
			m_spi_state = READY_FOR_COMMAND;
		break;

	case READY_FOR_STATUS_READ2:
		m_spilatch = 0x00;
		m_spi_state = READY_FOR_COMMAND;
		break;

	case READY_FOR_RDID2:
		m_spilatch = 0xfe;
		m_spi_state = READY_FOR_RDID1;
		break;

	case READY_FOR_RDID1:
		m_spilatch = 0xfe;
		m_spi_state = READY_FOR_RDID0;
		break;

	case READY_FOR_RDID0:
		m_spilatch = 0x00;
		m_spi_state = READY_FOR_COMMAND;
		break;
	}
}

void generic_spi_flash_device::nvram_default()
{
}

bool generic_spi_flash_device::nvram_read(util::read_stream &file)
{
	if (m_spiptr == nullptr)
	{
		return false;
	}

	auto const [err, actual] = util::read(file, m_spiptr, m_length);
	return !err && (actual == m_length);
}

bool generic_spi_flash_device::nvram_write(util::write_stream &file)
{
	auto const [err, actual] = util::write(file, m_spiptr, m_length);
	return !err;
}

