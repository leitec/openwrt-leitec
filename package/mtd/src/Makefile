CC = gcc
CFLAGS += -Wall

obj = mtd.o jffs2.o crc32.o
obj.seama = seama.o md5.o
obj.uimage = uimage.o
obj.ar71xx = trx.o
obj.brcm = trx.o
obj.brcm47xx = $(obj.brcm)
obj.brcm63xx = imagetag.o
obj.ramips = $(obj.seama) $(obj.uimage)

ifdef FIS_SUPPORT
  obj += fis.o
endif

mtd: $(obj) $(obj.$(TARGET))
clean:
	rm -f *.o jffs2
