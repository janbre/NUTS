#include <stdio.h>
#include <stdlib.h>

#include <../../src/arch/csp_thread.h>

#define MY_ADDRESS 1
#define MY_PORT 10

CSP_DEFINE_TASK(task_client) {
	csp_packet_t *packet;
	csp_conn_t *conn;

	while (1) {
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
		} else {
			printf("Packet sent\n");
		}	
		csp_close(conn);
	}
	return CSP_TASK_RETURN;
}

int main() {
	csp_buffer_init(10, 300);
	csp_init(MY_ADDRESS);
	csp_route_start_task(500,1);

	printf("Starting client task\r\n");
	csp_thread_handle_t handle_client;
	csp_thread_create(task_client, (signed char *) "CLIENT", 1000, NULL, 0, &handle_client);
	
	while(1) {
		csp_sleep_ms(100000);
	}		
	return 0;
}
