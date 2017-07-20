CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld
PROJNAME = test

$(PROJNAME)-0x00000.bin: $(PROJNAME)
	esptool.py elf2image $^

$(PROJNAME): $(PROJNAME).o

$(PROJNAME).o: $(PROJNAME).c

flash: $(PROJNAME)-0x00000.bin
	esptool.py write_flash 0 $(PROJNAME)-0x00000.bin 0x10000 $(PROJNAME)-0x10000.bin

clean:
	rm -f $(PROJNAME) $(PROJNAME).o $(PROJNAME)-0x00000.bin $(PROJNAME)-0x10000.bin
