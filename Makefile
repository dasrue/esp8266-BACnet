BAC_STK = BACnet_Stack
BAC_SRC = $(BAC_STK)/src
BAC_HNDLR = $(BAC_STK)/demo/handler
BAC_OBJ = $(BAC_STK)/demo/object

CC = xtensa-lx106-elf-gcc
SIZE = xtensa-lx106-elf-size
CFLAGS = -I. -I$(BAC_STK)/include -I$(BAC_OBJ) -Iinclude -mlongcalls -DICACHE_FLASH
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld
PROJNAME = test

SRCS = 	src/main.c \
	src/net.c \
	src/bip-init.c \
	$(BAC_SRC)/apdu.c \
	$(BAC_SRC)/bip.c \
	$(BAC_SRC)/bacdcode.c \
	$(BAC_SRC)/bacerror.c \
	$(BAC_SRC)/bvlc.c \
	$(BAC_SRC)/iam.c \
	$(BAC_SRC)/bacint.c \
	$(BAC_SRC)/npdu.c \
	$(BAC_SRC)/tsm.c \
	$(BAC_SRC)/dcc.c \
	$(BAC_SRC)/bacstr.c \
	$(BAC_SRC)/bacaddr.c \
	$(BAC_SRC)/debug.c \
	$(BAC_SRC)/bacreal.c \
	$(BAC_HNDLR)/h_npdu.c
	

OBJS = ${SRCS:.c=.o}

$(PROJNAME)-0x00000.bin: $(PROJNAME)
	esptool.py elf2image $^

$(PROJNAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)
	$(SIZE) $@

flash: $(PROJNAME)-0x00000.bin
	esptool.py write_flash 0 $(PROJNAME)-0x00000.bin 0x10000 $(PROJNAME)-0x10000.bin

clean:
	rm -rf $(PROJNAME) *.o src/*.o $(BAC_SRC)/*.o $(BAC_HNDLR)/*.o $(BAC_OBJ)/*.o $(PROJNAME)-0x00000.bin $(PROJNAME)-0x10000.bin
