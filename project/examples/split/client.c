#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_kiss.h>

#include "../../src/arch/csp_thread.h"

#define MY_ADDRESS	2			// Address of local CSP node
#define REMOTE_ADDRESS  1
#define MY_PORT		10			// Port to send test traffic to

CSP_DEFINE_TASK(send_hello_world_task) {

    csp_packet_t * packet;
    csp_conn_t * conn;
    csp_hmac_set_key("Testkey", 7);

    while (true) {

	/* Get packet buffer for data */
	packet = csp_buffer_get(300);
	if (packet == NULL) {
	    printf("Failed to get buffer element\n");
	    return;
	}

	/* Connect to host HOST, port PORT with regular UDP-like protocol and 1000 ms timeout */
	conn = csp_connect(CSP_PRIO_NORM, REMOTE_ADDRESS, MY_PORT, 1000, CSP_O_SEQNR | CSP_O_HMAC | CSP_O_RDP);
	if (conn == NULL) {
	    printf("Connection failed\n");
	    csp_buffer_free(packet);
	    continue;
	}

	for(int k = 0; k < 4; k++){
	  /* Copy dummy data to packet */
	  //	  char *msg = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin at ipsum lorem, at malesuada urna. Aliquam et laoreet purus. Maecenas pellentesque, arcu sit amet posuere tristique, quam nisi ornare massa, vel tempus magna enim quis volutpat.";
	    char *msg = "Hello world";
	  strcpy((char *) packet->data, msg);
	  packet->length = strlen(msg);

	  /* Send packet */
	  printf("Sending a packet: Length = %u, DATA = %s \r\n", packet->length, (char *) packet->data);

	  if (!csp_send(conn, packet, 1000)) {
	    /* Send failed */
	    printf("Send failed\n");
	    csp_buffer_free(packet);
	  }
	  csp_sleep_ms(300);

	}
	/* Close connection */
	csp_close(conn);

	printf("Sleeping 6000 ms \r\n");
	csp_sleep_ms(6000);
	printf("Done sleeping\r\n");
    }

    return CSP_TASK_RETURN;
}

int main(int argc, char * argv[]) {

	printf("Initialising CSP\r\n");
	csp_buffer_init(100, 1000);
	csp_init(MY_ADDRESS);

	struct usart_conf conf;
	conf.device = "/dev/ttyS0";
	conf.baudrate = 500000;
	printf("Using '%s' as the serial port \r\n", conf.device);
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

	printf("Starting send_hello_world_task\r\n");
        csp_thread_handle_t send_hello_world_handle;
        csp_thread_create(send_hello_world_task, (signed char *) "Send hello world", 1000, NULL, 0, &send_hello_world_handle);

	while(1) {
		csp_sleep_ms(100000);
	}

	return 0;
}
