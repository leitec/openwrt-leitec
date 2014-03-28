/*
 * uimage.c
 *
 * Fix the uImage header on a Linksys WRT160N v2.
 *
 * The WRT160Nv2's header format is a uImage of a TRX
 * image. In order to allow initial flashing from the
 * original web interface, the generated OpenWRT image
 * must include the TRX image of both the kernel and
 * rootfs, and the uImage must reflect the size/checksum
 * of the entire thing.
 *
 * The issue comes after the first boot, when jffs2
 * formatting changes the very end of the rootfs
 * part still within the TRX image size. To work around
 * this, we use this kludge at firstboot that recreates
 * the uImage header using only the kernel partition
 * (the only part the bootloader cares about) and recalculates
 * the CRCs based on that.
 *
 * Copyright (C) 2014 Claudio Leite <leitec@staticky.com>
 *
 * Based on the seama fixup code:
 *   Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Based on the trx fixup code:
 *   Copyright (C) 2005 Mike Baker
 *   Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#include "mtd.h"
#include "image.h"
#include "crc32.h"

ssize_t pread(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

int
uimage_fix_crc32(char *buf, size_t len)
{
	image_header_t *uhdr;
	char *data;
        size_t newsize;
        uint32_t newhcrc, newdcrc;

	if (len < sizeof(struct image_header))
		return -1;

	uhdr = (struct image_header *) buf;
	if (uhdr->ih_magic != htonl(IH_MAGIC)) {
		fprintf(stderr, "no uImage header found\n");
		return -1;
	}

        if(ntohl(uhdr->ih_size) <= len - sizeof(struct image_header)) {
            if(quiet < 2)
                fprintf(stderr, "header doesn't need fixing, already contained within partition\n");

            return -1;
        }

        newsize = len - sizeof(struct image_header);
        data = buf + sizeof(struct image_header);
        newdcrc = crc32buf(data, newsize) ^ 0xFFFFFFFFL;

        if(uhdr->ih_dcrc == htonl(newdcrc) &&
           uhdr->ih_size == htonl(newsize)) {
            if(quiet < 2)
                fprintf(stderr, "the header is already fixed\n");

            return -1;
        }

        uhdr->ih_size = htonl(newsize);
        uhdr->ih_dcrc = htonl(newdcrc);
        uhdr->ih_hcrc = 0;

        newhcrc = crc32buf(buf, sizeof(struct image_header)) ^ 0xFFFFFFFFL;
        uhdr->ih_hcrc = htonl(newhcrc);

	if (quiet < 2) {
		fprintf(stderr, "new size: %u, new data CRC32: %04X\n",
                        newsize, newdcrc);
                fprintf(stderr, "new header CRC32: %04X\n", newhcrc);
	}

	return 0;
}

int
mtd_fixuimage(const char *mtd, size_t offset)
{
	int fd;
	char *buf;
	ssize_t res;
	size_t block_offset;

	if (quiet < 2)
		fprintf(stderr, "Trying to fix uImage header in %s at 0x%x...\n",
			mtd, offset);

	block_offset = offset & ~(erasesize - 1);
	offset -= block_offset;

	fd = mtd_check_open(mtd);
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device: %s\n", mtd);
		exit(1);
	}

	if (block_offset + erasesize > mtdsize) {
		fprintf(stderr, "Offset too large, device size 0x%x\n",
			mtdsize);
		exit(1);
	}

	buf = malloc(mtdsize);
	if (!buf) {
		perror("malloc");
		exit(1);
	}

	res = pread(fd, buf, mtdsize, block_offset);
	if (res != mtdsize) {
		perror("pread");
		exit(1);
	}

	if (uimage_fix_crc32(buf, mtdsize))
		goto out;

	if (mtd_erase_block(fd, block_offset)) {
		fprintf(stderr, "Can't erease block at 0x%x (%s)\n",
			block_offset, strerror(errno));
		exit(1);
	}

	if (quiet < 2)
		fprintf(stderr, "Rewriting block at 0x%x\n", block_offset);

	if (pwrite(fd, buf, erasesize, block_offset) != erasesize) {
		fprintf(stderr, "Error writing block (%s)\n", strerror(errno));
		exit(1);
	}

	if (quiet < 2)
		fprintf(stderr, "Done.\n");

out:
	close (fd);
	sync();

	return 0;
}

