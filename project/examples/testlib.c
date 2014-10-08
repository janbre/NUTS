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

	char *data;
	while (1) {
		if ((conn = csp_accept(sock, 10000)) == NULL) {
			printf("no connection\n");
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
		}

		csp_close(conn);
	}
	return CSP_TASK_RETURN;
}

CSP_DEFINE_TASK(task_client) {
	csp_packet_t *packet;
	csp_conn_t *conn;


	while(1) {
		csp_sleep_ms(1000);
		int result = csp_ping(MY_ADDRESS, 100, 100, CSP_O_NONE);
		printf("Ping result %d [ms]\r\n", result);
		csp_sleep_ms(1000);

		packet = csp_buffer_get(10);
		if (packet == NULL) {
			printf("Failed to get buffer element\n");
			return CSP_TASK_RETURN;
		}

		conn = csp_connect(CSP_PRIO_NORM, MY_ADDRESS, MY_PORT, 1000, CSP_O_NONE);
		if(conn == NULL) {
			printf("Connection failed\n");
			csp_buffer_free(packet);
			return CSP_TASK_RETURN;
		}

		char *msg = "Hello world";
		strcpy((char *) packet->data, msg);

		packet->length = strlen(msg);

		if (!csp_send(conn, packet, 1000)) {
			printf("Send failed\n");
			csp_buffer_free(packet);
		}

		csp_close(conn);
	}
	return CSP_TASK_RETURN;
}

int sendpacket() {
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
	char *msg = "42!";
	strcpy((char *) packet->data, msg);
	packet->length = strlen(msg);
	if (!csp_send(conn, packet, 1000)) {
		printf("Sending packet from sendpacket() failed\n");
		csp_buffer_free(packet);
	}
	csp_close(conn);
	return;
}

int main(int argc, char * argv[]) {
	printf("Initialising CSP\r\n");
	csp_buffer_init(10, 300);
	csp_init(MY_ADDRESS);
	csp_route_start_task(500, 1);
	printf(argv[1]);

	printf("Starting server task\r\n");
	csp_thread_handle_t handle_server;
	csp_thread_create(task_server, (signed char *) "SERVER", 1000, NULL, 0, &handle_server);

	printf("Starting client task\r\n");
//	csp_thread_handle_t handle_client;
//	csp_thread_create(task_client, (signed char *) "CLIENT", 1000, NULL, 0, &handle_client);

/*	while(1) {
		csp_sleep_ms(1000);
		csp_packet_t *packet;
		csp_conn_t *conn;
		packet = csp_buffer_get(10);
		if (packet == NULL) {
			printf("Failed to get buffer element\n");
			return CSP_TASK_RETURN;
		}

		conn = csp_connect(CSP_PRIO_NORM, MY_ADDRESS, MY_PORT, 1000, CSP_O_NONE);
		if(conn == NULL) {
			printf("Connection failed\n");
			csp_buffer_free(packet);
			return CSP_TASK_RETURN;
		}

		char *msg = "Hello world";
		msg = argc;
		strcpy((char *) packet->data, msg);

		packet->length = strlen(msg);

		if (!csp_send(conn, packet, 1000)) {
			printf("Send failed\n");
			csp_buffer_free(packet);
		}

		csp_close(conn);
	}*/
	sendpacket();
	while(1){
		csp_sleep_ms(100000);
	}
	return 0;
}
