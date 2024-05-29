/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/logging/log.h>
#include <psa/crypto.h>
#include <psa/internal_trusted_storage.h>

LOG_MODULE_REGISTER(psa_its, LOG_LEVEL_DBG);

int main(void)
{
	LOG_INF("Starting PSA ITS example...");

	psa_status_t status = psa_crypto_init();
	if (status) {
		LOG_INF("psa_crypto_init failed");
		return 1;
	}

	{
		LOG_INF("Verify that reading the wrong UID will fail");

		psa_storage_uid_t non_existent_uid = 0x5EB01234; // It's just a uint64_t

		// Read from the start of the entry
		uint32_t data_offset = 0;

		// Read 4 bytes
		uint32_t data_length = 4;

		// Write the result from psa_its_get to p_data
		uint8_t p_data[4];

		// Number of bytes written to p_data
		size_t p_data_length = 0x5EB0;

		status = psa_its_get(non_existent_uid, data_offset, data_length,
							 p_data, &p_data_length);

		if(status == 0) {
			LOG_ERR("Expected psa_its_get with the wrong UID to fail, but it succeeded.");
			return 1;
		}

		LOG_INF("psa_its_get correctly returned an error code when we intentionally read the wrong UID");
	}

	{
		LOG_INF("Write to ITS and then check that we can read it back");

		psa_storage_uid_t new_uid = 0x5EB00000;

		// Data to be written to ITS
		uint8_t p_data_write[1] = {42};

		// permission flags for ITS entry
		psa_storage_create_flags_t create_flags = PSA_STORAGE_FLAG_NONE;

		status = psa_its_set(new_uid, sizeof(p_data_write), p_data_write, create_flags);

		if(status) {
			LOG_ERR("Unable to write to ITS");
			return 1;
		}

		// Write the 'new_uid' entry in ITS to this buffer
		uint8_t p_data_read[1];

		// Read from the start of the entry
		uint32_t data_offset = 0;

		// Read 1 byte
		uint32_t data_length = 1;

		// Number of bytes written to p_data
		size_t p_data_length = 0xc0ffe;

		status = psa_its_get(new_uid, data_offset, data_length,
							 p_data_read, &p_data_length);
		if(status) {
			LOG_ERR("Unable to read back from ITS");
			return 1;
		}

		if(p_data_length != 1) {
			LOG_ERR("Unable to read the correct amount of bytes from ITS");
		}

		if(p_data_read[0] != 42) {
			LOG_ERR("Read the wrong value back from ITS");
		}

		LOG_INF("Successfully wrote to ITS and read back what was written");
	}


	LOG_INF("Example finished successfully!");

	return 0;
}
