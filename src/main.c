#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "ip_addr.h"
#include "espconn.h"
#include "user_interface.h"
#include "bip.h"
#include "net.h"
#include <string.h>

// ESP-12 modules have LED on GPIO2. Change to another GPIO
// for other boards.
static const int pin = 14;
static volatile os_timer_t some_timer;
static struct espconn testConn;
static const char sendString[] = "Hello World!";

void some_timerfunc(void *arg)
{
  //Do blinky stuff
  if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
  {
    // set gpio low
    gpio_output_set(0, (1 << pin), 0, 0);
  }
  else
  {
    // set gpio high
    gpio_output_set((1 << pin), 0, 0, 0);
  }
  /*
	testConn.proto.udp->remote_port = 2567;
	uint8_t remoteIP[] = {192,168,1,132};
	os_memcpy(testConn.proto.udp->remote_ip, remoteIP, 4);
	espconn_sendto(&testConn,sendString,strlen(sendString)+1);*/
}

void ICACHE_FLASH_ATTR user_init()
{
	// init gpio subsytem
	gpio_init();

	// configure UART TXD to be GPIO1, set as output
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	gpio_output_set(0, 0, (1 << pin), 0);
	gpio_output_set(0, 0, (1 << 12), 0);
	if(wifi_get_opmode()!=0x01)
		wifi_set_opmode(0x01);		// Make sure we are in station mode.
	user_set_station_config();
	
	esp_udp testUDPinfo;
	testUDPinfo.remote_port = 2567;
	testUDPinfo.local_port = 2567;
	uint8_t remoteIP[] = {192,168,1,132};
	os_memcpy(testUDPinfo.remote_ip, remoteIP, 4);
	testConn.type = ESPCONN_UDP;
	testConn.proto.udp = &testUDPinfo;
	espconn_create(&testConn);
  // setup timer (500ms, repeating)
	setupHandlers();
	bip_init(0);
	os_printf("Ready for battle!\n");
	os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);
	os_timer_arm(&some_timer, 500, 1);
}
