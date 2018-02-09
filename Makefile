BAC_STK = BACnet_Stack
BAC_SRC = $(BAC_STK)/src
BAC_HNDLR = $(BAC_STK)/demo/handler
BAC_OBJ = $(BAC_STK)/demo/object
HTTP = libesphttpd
ESP_SDK = /home/Brad/esp-open-sdk/ESP8266_NONOS_SDK-2.1.0-18-g61248df
ESP_COMPORT = /dev/ttyS3
ESP_BAUD = 1152000

CC = xtensa-lx106-elf-gcc
SIZE = xtensa-lx106-elf-size
CFLAGS = -I. -I$(BAC_STK)/include -I$(BAC_OBJ) -Iinclude -Iesp-httpd/extras/httpd -Iesp-httpd/examples/http_server/fsdata -I$(ESP_SDK)/driver_lib/include -I$(HTTP)/include -I$(HTTP)/espfs -mlongcalls -std=c99 -DICACHE_FLASH -DHTTPD_MAX_CONNECTIONS=4 -D__ets__ -g
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lcirom -L $(ESP_SDK)/lib -lgcc
LDFLAGS = -Teagle.app.v6.ld -Wl,-Map,Test.map -Wl,--cref -g
PROJNAME = test

SRCS = 	src/main.c \
	src/bip-init.c \
	src/dht22.c \
	src/hdc1080.c \
	src/htu21d.c \
	src/httpd_setup.c \
	src/net.c \
	src/nvmem.c \
	src/uart_console.c \
	src/driver/uart.c \
	src/driver/i2c_master.c \
	$(BAC_SRC)/abort.c \
	$(BAC_SRC)/address.c \
	$(BAC_SRC)/apdu.c \
	$(BAC_SRC)/bacaddr.c \
	$(BAC_SRC)/bacapp.c \
	$(BAC_SRC)/bacdcode.c \
	$(BAC_SRC)/bacerror.c \
	$(BAC_SRC)/bacint.c \
	$(BAC_SRC)/bacdevobjpropref.c \
	$(BAC_SRC)/bacreal.c \
	$(BAC_SRC)/bacstr.c \
	$(BAC_SRC)/bip.c \
	$(BAC_SRC)/bvlc.c \
	$(BAC_SRC)/cov.c \
	$(BAC_SRC)/dcc.c \
	$(BAC_SRC)/debug.c \
	$(BAC_SRC)/iam.c \
	$(BAC_SRC)/npdu.c \
	$(BAC_SRC)/memcopy.c \
	$(BAC_SRC)/proplist.c \
	$(BAC_SRC)/reject.c \
	$(BAC_SRC)/rp.c \
	$(BAC_SRC)/rpm.c \
	$(BAC_SRC)/tsm.c \
	$(BAC_SRC)/whois.c \
	$(BAC_SRC)/wp.c \
	$(BAC_SRC)/version.c \
	$(BAC_HNDLR)/h_cov.c \
	$(BAC_HNDLR)/h_npdu.c \
	$(BAC_HNDLR)/h_whois.c \
	$(BAC_HNDLR)/h_rp.c \
	$(BAC_HNDLR)/h_rpm.c \
	$(BAC_HNDLR)/h_wp.c \
	$(BAC_HNDLR)/txbuf.c \
	$(BAC_HNDLR)/s_iam.c \
	$(BAC_OBJ)/device.c \
	$(BAC_OBJ)/ai.c \
	$(BAC_OBJ)/bo.c \
	$(HTTP)/core/httpd.c \
	$(HTTP)/core/httpd-nonos.c \
	$(HTTP)/core/httpdespfs.c \
	$(HTTP)/espfs/espfs.c
	
	

OBJS = ${SRCS:.c=.o}

$(PROJNAME)-0x00000.bin: $(PROJNAME)
	esptool.py elf2image $^

$(PROJNAME): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS) $(LDFLAGS)
	$(SIZE) $@

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

flash: $(PROJNAME)-0x00000.bin
	esptool.py -p $(ESP_COMPORT) -b $(ESP_BAUD) write_flash --flash_mode dio 0x00000 $(PROJNAME)-0x00000.bin 0x10000 $(PROJNAME)-0x10000.bin

clean:
	rm -rf $(PROJNAME) *.o src/*.o $(BAC_SRC)/*.o $(BAC_HNDLR)/*.o $(BAC_OBJ)/*.o $(PROJNAME)-0x00000.bin $(PROJNAME)-0x10000.bin
