// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_MACHINE_GENERIC_SPI_FLASH_H
#define MAME_MACHINE_GENERIC_SPI_FLASH_H

#pragma once

class generic_spi_flash_device : public device_t,
							  public device_nvram_interface
{
public:
	generic_spi_flash_device(const machine_config& mconfig, const char* tag, device_t* owner, uint32_t clock);

	void set_rom_ptr(uint8_t* rom) { m_spiptr = rom; }
	void set_rom_size(size_t size) { m_length = size; }

	uint8_t read()
	{
		logerror("returning %02x\n", m_spilatch);
		return m_spilatch;
	}

	void set_ready()
	{
		m_spi_state = READY_FOR_COMMAND;
	}

	void dir_w(int state)
	{
		// was previously hooked up for Monon, but SPI doesn't have a direction pin
		// and it isn't important, must be something internal to the AX208
	}

	void write(uint8_t data);

	void set_single_byte_status_read() { m_multibyte_status_read = 0; };
	void set_single_byte_status_writes() { m_multibyte_status_write = 0; };

protected:
	// device-level overrides
	virtual void device_start() override ATTR_COLD;
	virtual void device_reset() override ATTR_COLD;

	// device_nvram_interface overrides
	virtual void nvram_default() override;
	virtual bool nvram_read(util::read_stream &file) override;
	virtual bool nvram_write(util::write_stream &file) override;

private:
	enum
	{
		READY_FOR_COMMAND = 0x00,

		READY_FOR_ADDRESS2 = 0x01,
		READY_FOR_ADDRESS1 = 0x02,
		READY_FOR_ADDRESS0 = 0x03,

		READY_FOR_HSADDRESS2 = 0x04,
		READY_FOR_HSADDRESS1 = 0x05,
		READY_FOR_HSADDRESS0 = 0x06,
		READY_FOR_HSDUMMY = 0x07,

		READY_FOR_WRITEADDRESS2 = 0x08,
		READY_FOR_WRITEADDRESS1 = 0x09,
		READY_FOR_WRITEADDRESS0 = 0x0a,

		READY_FOR_SECTORERASEADDRESS2 = 0x0b,
		READY_FOR_SECTORERASEADDRESS1 = 0x0c,
		READY_FOR_SECTORERASEADDRESS0 = 0x0d,

		READY_FOR_WRITE = 0x0e,

		READY_FOR_READ = 0x0f,

		READY_FOR_STATUS_READ = 0x10,
		READY_FOR_STATUS_READ2 = 0x11,

		REMS_STEP3 = 0x12,
		REMS_STEP2 = 0x13,
		REMS_STEP1 = 0x14,
		REMS_STEP0 = 0x15,
		REMS_STEPx = 0x16,

		READY_FOR_ERASEADDRESS2 = 0x17,
		READY_FOR_ERASEADDRESS1 = 0x18,
		READY_FOR_ERASEADDRESS0 = 0x19,

		READY_FOR_STATUS_VALUES2 = 0x1a,
		READY_FOR_STATUS_VALUES1 = 0x1b,

		READY_FOR_RDID2 = 0x1c,
		READY_FOR_RDID1 = 0x1d,
		READY_FOR_RDID0 = 0x1e,
	};

	uint32_t m_spiaddr;
	uint8_t m_spi_state;
	uint8_t m_spilatch;
	bool m_spidir;

	// config
	uint8_t* m_spiptr;
	size_t m_length;
	uint8_t m_multibyte_status_read;
	uint8_t m_multibyte_status_write;
};

DECLARE_DEVICE_TYPE(GENERIC_SPI_FLASH, generic_spi_flash_device)

#endif // MAME_MACHINE_GENERIC_SPI_FLASH_H
