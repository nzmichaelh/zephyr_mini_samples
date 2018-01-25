/*
 * Copyright (c) 2018 Google LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <fs.h>
#include <kernel.h>
#include <stdio.h>

static int test(int idx)
{
	int err;
	char name[MAX_FILE_NAME + 1];
	fs_file_t fp;
	static const char msg[] = "A little Zephyr demo.\0";
	char buf[sizeof(msg)];

	/* Note that NFFS needs an absolute path */
	sprintf(name, "/%d.txt", idx);

	/* Delete the old file, if any */
	fs_unlink(name);

	/* Open and write */
	err = fs_open(&fp, name);
	if (err != 0) {
		printk("%s: error %d while opening for write\n", name, err);
		return err;
	}

	err = fs_write(&fp, msg, sizeof(msg));
	if (err < 0) {
		printk("%s: error %d while writing\n", name, err);
		return err;
	}

	if (err != sizeof(msg)) {
		printk("%s: tried to write %d bytes, only wrote %d\n", name,
		       sizeof(msg), err);
		return err;
	}

	/* Close the file and read the data back */
	err = fs_close(&fp);
	if (err != 0) {
		printk("%s: error %d while closing after write\n", name, err);
		return err;
	}

	err = fs_open(&fp, name);
	if (err != 0) {
		printk("%s: error %d while opening for read\n", name, err);
		return err;
	}

	err = fs_read(&fp, buf, sizeof(buf));
	if (err < 0) {
		printk("%s: error %d while reading\n", name, err);
		return err;
	}

	if (err != sizeof(msg)) {
		printk("%s: tried to read %d bytes, only got %d\n", name,
		       sizeof(msg), err);
		return err;
	}

	printk("%s: wrote then read \"%s\"\n", name, buf);

	/* Close the file */
	err = fs_close(&fp);
	if (err != 0) {
		printk("%s: error %d while closing after read\n", name, err);
		return err;
	}

	return 0;
}

void main(void)
{
	printk("nffs sample - create, write, and read files.\n");

	if (device_get_binding(CONFIG_FS_NFFS_FLASH_DEV_NAME) == NULL) {
		printk("warning: flash device %s not found\n",
		       CONFIG_FS_NFFS_FLASH_DEV_NAME);
	}

	for (;;) {
		int i;

		for (i = 0; i < 10; i++) {
			test(i);
		}

		k_sleep(K_SECONDS(1));
	}
}
