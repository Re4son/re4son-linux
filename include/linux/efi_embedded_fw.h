/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_EFI_EMBEDDED_FW_H
#define _LINUX_EFI_EMBEDDED_FW_H

#include <linux/mod_devicetable.h>

struct efi_embedded_fw_desc {
	const char *name;
	u8 prefix[8];
	u32 length;
	u32 crc;
};

int efi_get_embedded_fw(const char *name, void **dat, size_t *sz, size_t msize);

#endif
