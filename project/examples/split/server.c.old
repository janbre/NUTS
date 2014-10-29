#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_kiss.h>

#include "../../src/arch/csp_thread.h"

#define MY_ADDRESS  1			// Address of local CSP node
#define REMOTE_ADDRESS 2
#define MY_PORT	10			// Port to send test traffic to

CSP_DEFINE_TASK(server) {

	/* HMAC SETUP */
	csp_hmac_set_key("Testkey", 7);
	/*HMAC SETUP */

	csp_socket_t *sock = csp_socket(CSP_SO_RDPREQ | CSP_SO_SEQNR | CSP_SO_HMACREQ);	

	csp_bind(sock, MY_PORT);
	csp_listen(sock, 200);

	printf("Listening \r\n");

	csp_conn_t *conn;
	csp_packet_t *packet;

	while (1) {

		if ((conn = csp_accept(sock, 10000)) == NULL)
		  continue;

		while ((packet = csp_read(conn, 1000)) != NULL) {
		  switch (csp_conn_dport(conn)) {
		  case MY_PORT:
		    /* Process packet here */
		      printf("Received a packet: Length = %u, DATA = %.*s \r\n", packet->length, packet->length, packet->data);
		    csp_buffer_free(packet);
		    break;
		  default:
		    csp_service_handler(conn, packet);
		    printf("Service request received (%s) \r\n");
		    break;
		  }
		}
		csp_close(conn);
	}	
	return;
}

int main(int argc, char * argv[]) {

	printf("Initialising CSP\r\n");
	csp_buffer_init(100, 3100);
	csp_init(MY_ADDRESS);

	struct usart_conf conf;
	conf.device = "/dev/ttyS0";
	printf("Using '%s' as the serial port\r\n", conf.device);
        conf.baudrate = 1200;
        usart_init(&conf);
        csp_kiss_init(usart_putstr, usart_insert);
        usart_set_callback(csp_kiss_rx);

        csp_route_set(REMOTE_ADDRESS, &csp_if_kiss, CSP_NODE_MAC);
	csp_route_start_task(500, 1);
	csp_seqnr_init(0);

	if ((argc > 1) && (strcmp(argv[1], "-v") == 0)) {
		printf("Debug enabed\r\n");
		csp_debug_toggle_level(4);

		printf("Conn table\r\n");
		csp_conn_print_table();

		printf("Route table\r\n");
		csp_route_print_table();

		printf("Interfaces\r\n");
		csp_route_print_interfaces();
	}

	printf("Starting server task\r\n");
        csp_thread_handle_t server_handle;
        csp_thread_create(server, (signed char *) "Server", 1000, NULL, 0, &server_handle);

	while(1) {
		csp_sleep_ms(100000);
	}

	return 0;
}
