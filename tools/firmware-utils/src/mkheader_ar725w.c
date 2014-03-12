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
 * Build a proper image for the Airlink101 AR725W router
 * and the Asante AWRT-600N router. The resulting image
 * is compatible with the factory firmware's web and TFTP
 * interfaces.
 *
 * To build:
 *  gcc -O2 -o mkheader_ar725w mkheader_ar725w.c -lz
 *
 * Claudio Leite <leitec@staticky.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>

#include <zlib.h> /* for crc32() */

/*
 * The header is in little-endian format. In case
 * we are on a BE host, we need to swap binary
 * values.
 */
#if defined(__linux__)
# include <endian.h>
# ifndef htole32
#  include <byteswap.h>
#  if __BYTE_ORDER == __BIG_ENDIAN
#   define htole32(x) __bswap_32(x)
#  else
#   define htole32(x) (x)
#  endif
# endif
#elif defined(__APPLE__)
# include <libkern/OSByteOrder.h>
# define htole32 OSSwapHostToLittleInt32
#else
# include <sys/endian.h> /* BSD's will have this */
#endif

struct ralink_header {
    uint8_t magic[4];
    uint8_t version[4];
    uint32_t product_id;
    uint32_t imagesz;
    uint32_t checksum;
    uint32_t fast_checksum;
    uint8_t build[4];
    uint8_t lang[4];
};

int main(int argc, char *argv[])
{
    unsigned long res, flen;
    struct ralink_header my_hdr;
    FILE *f, *f_out;
    int use_asante = 0;
    uint8_t *buf;
    uint32_t crc;

    if(argc < 3) {
        printf("usage: mkheader_ar725w <uImage> <webflash image> [--asante]\n");
        printf("  --asante: use magic for Asante AWRT-600N\n");
        exit(-1);
    }

    if(argc == 4)
        if(strcmp(argv[3], "--asante") == 0)
            use_asante = 1;

    printf("Opening %s...\n", argv[1]);

    f = fopen(argv[1], "r");

    fseek(f, 0, SEEK_END);
    flen = (unsigned long)ftell(f);

    printf("  %lu (0x%lX) bytes long\n", flen, flen);

    buf = malloc(flen+32);

    rewind(f);
    res = fread(buf+32, 1, flen, f);
    if(res != flen) {
        perror("Couldn't read entire file: fread()");
        exit(-1);
    }

    fclose(f);

    printf("\nCreating %s...\n", argv[2]);

    memset(&my_hdr, 0, sizeof(struct ralink_header));

    /*
     * magic numbers derived experimentally and from
     * the 'tftpd' binary and Asante firmware .bin
     */
    if(use_asante) {
        printf("  Using Asante AWRT-600N magic\n");
        memcpy(my_hdr.magic, "A600", 4);
        memcpy(my_hdr.version, "1005", 4);
    } else {
        printf("  Using Airlink101 AR725W magic\n");
        memcpy(my_hdr.magic, "GMTK", 4);
        memcpy(my_hdr.version, "1003", 4);
    }

    my_hdr.product_id = htole32(0x03000001);
    my_hdr.imagesz = htole32(flen + 0x20);
    memcpy(my_hdr.build, "01", 2);
    memcpy(my_hdr.lang, "EN", 2);

    memcpy(buf, &my_hdr, 32);

    crc = crc32(0, buf, flen+32);
    printf("  CRC32: %08X\n", crc);

    my_hdr.checksum = htole32(crc);
    memcpy(buf, &my_hdr, 32);

    printf("  Writing...\n");
    f_out = fopen(argv[2], "w");

    fwrite(buf, 1, flen + 32, f_out);

    fclose(f_out);

    free(buf);
    return 0;
}
