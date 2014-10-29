#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <csp/csp.h>

#include <../src/arch/csp_thread.h>

#include "testlib.h"
#define MY_ADDRESS 1
#define MY_PORT 10

CSP_DEFINE_TASK(task_server) {

	csp_socket_t *sock = csp_socket(CSP_SO_NONE);

	csp_bind(sock, CSP_ANY);

	csp_listen(sock, 100);

	csp_conn_t *conn;
	csp_packet_t *packet;
	bool open = true;
	int timeout = 10000;

	char *data;
	while (open) {
		if ((conn = csp_accept(sock, timeout)) == NULL) {
			//printf("No connection for %d. Connection closed", timeout);
			//printf("no connection\n");
			continue;
		}

		while ((packet = csp_read(conn, 100)) != NULL) {
			switch (csp_conn_dport(conn)) {
				case MY_PORT:
					data = packet->data;
					printf("Packet received: %s\n", data);
					csp_buffer_free(packet);
				default:
					csp_service_handler(conn, packet);
					break;
			}
			if (strcmp(data, "q") == 0) {
				open = false;
				printf("closing connection\n");
			}
		}

		csp_close(conn);
	}
	return CSP_TASK_RETURN;
}

void sendpacket(char *msg) {
	printf("Initialising CSP\r\n");
	csp_buffer_init(10, 300);
	csp_init(MY_ADDRESS);
	csp_route_start_task(500, 1);

	printf("Starting server task\r\n");
	csp_thread_handle_t handle_server;
	csp_thread_create(task_server, (signed char *) "SERVER", 1000, NULL, 0, &handle_server);

	csp_packet_t *packet;
	csp_conn_t *conn;
	packet = csp_buffer_get(10);
	if (packet == NULL) {
		printf("failed to get packet in sendpacket()\n");
		return CSP_TASK_RETURN;
	}
	conn = csp_connect(CSP_PRIO_NORM, MY_ADDRESS, MY_PORT, 1000, CSP_O_NONE);
	if (conn == NULL) {
		printf("Connection failed in sendpacket()\n");
		return CSP_TASK_RETURN;
	}
//	csp_sleep_ms(1000);
	//char *msg = "42!";
	strcpy((char *) packet->data, msg);
	packet->length = strlen(msg);
	if (!csp_send(conn, packet, 1000)) {
		printf("Sending packet from sendpacket() failed\n");
		csp_buffer_free(packet);
	} else {
		printf("Sending packet from sendpacket() succeeded\n");
	}
	csp_close(conn);
	/*while(1) {
		csp_sleep_ms(3000);
	}*/
	//csp_sleep_ms(30);
	return;
}
