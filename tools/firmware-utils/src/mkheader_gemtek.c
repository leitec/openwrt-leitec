/*
 * Copyright (C) 2014  Claudio Leite <leitec@staticky.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * Builds a proper flash image for routers using some Gemtek
 * OEM boards. These include the Airlink101 AR725W, the
 * Asante SmartHub 600 (AWRT-600N), and Linksys WRT110.
 *
 * The resulting image is compatible with the factory firmware
 * web upgrade and TFTP interface.
 *
 * To build:
 *  gcc -O2 -o mkheader_gemtek mkheader_gemtek.c -lz
 *
 * Claudio Leite <leitec@staticky.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <zlib.h> /* for crc32() */

/*
 * The header is in little-endian format. In case
 * we are on a BE host, we need to swap binary
 * values.
 */
#if defined(__linux__)
# include <endian.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define le32 OSSwapHostToLittleInt32
#else
# include <sys/endian.h> /* BSD's should have this */
#endif

#ifndef __APPLE__
# if __BYTE_ORDER == __BIG_ENDIAN
#  define le32(x) (((x & 0xFF000000) >> 24) | \
                   ((x & 0x00FF0000) >> 8)  | \
                   ((x & 0x0000FF00) << 8)  | \
                   ((x & 0x000000FF) << 24))
# else
#  define le32(x) (x)
# endif
#endif

struct gemtek_header {
    uint8_t magic[4];
    uint8_t version[4];
    uint32_t product_id;
    uint32_t imagesz;
    uint32_t checksum;
    uint32_t fast_checksum;
    uint8_t build[4];
    uint8_t lang[4];
};

struct machines {
    char *desc;
    uint32_t maxsize;
    struct gemtek_header header;
};

struct machines mach_def[] = {
    { "Airlink101 AR725W", 0x340000,
      { "GMTK", "1003", le32(0x03000001), 0, 0,
          0, "01\0\0",  "EN\0\0" }},
    { "Asante AWRT-1600N", 0x340000,
      { "A600", "1005", le32(0x03000001), 0, 0,
          0, "01\0\0",  "EN\0\0" }},
    { "Linksys WRT110",    0x340000,
      { "GMTK", "1007", le32(0x03040001), 0, 0,
          0, "2\0\0\0", "EN\0\0" }},
    { 0 }
};

int main(int argc, char *argv[])
{
    unsigned long res, flen;
    struct gemtek_header my_hdr;
    FILE *f, *f_out;
    int image_type = 0, index;
    uint8_t *buf;
    uint32_t crc;

    if(argc < 3) {
        fprintf(stderr, "mkheader_gemtek <uImage> <webflash image> [machine ID]\n");
        fprintf(stderr, "  where [machine ID] is one of:\n");
        for(index = 0; mach_def[index].desc != 0; index++) {
            fprintf(stderr, "    %2d  %s", index, mach_def[index].desc);
            if(index == 0)
                fprintf(stderr, " (default)\n");
            else
                fprintf(stderr, "\n");
        }

        exit(-1);
    }

    if(argc == 4) {
        image_type = atoi(argv[3]);
    }

    printf("Opening %s...\n", argv[1]);

    f = fopen(argv[1], "r");

    fseek(f, 0, SEEK_END);
    flen = (unsigned long)ftell(f);

    printf("  %lu (0x%lX) bytes long\n", flen, flen);

    if(flen > mach_def[image_type].maxsize) {
        fprintf(stderr, "\nERROR: image exceeds maximum compatible size\n");
        fclose(f);
        exit(-1);
    }

    buf = malloc(flen+32);

    rewind(f);
    res = fread(buf+32, 1, flen, f);
    if(res != flen) {
        perror("Couldn't read entire file: fread()");
        fclose(f);
        exit(-1);
    }

    fclose(f);

    printf("\nCreating %s...\n", argv[2]);

    memcpy(&my_hdr, &mach_def[image_type].header, sizeof(struct gemtek_header));

    printf("  Using %s magic\n", mach_def[image_type].desc);

    my_hdr.imagesz = le32(flen + 0x20);
    memcpy(my_hdr.lang, "EN", 2);

    memcpy(buf, &my_hdr, 32);

    crc = crc32(0, buf, flen+32);
    printf("  CRC32: %08X\n", crc);

    my_hdr.checksum = le32(crc);
    memcpy(buf, &my_hdr, 32);

    printf("  Writing...\n");
    f_out = fopen(argv[2], "w");

    fwrite(buf, 1, flen + 32, f_out);

    fclose(f_out);

    free(buf);
    return 0;
}
