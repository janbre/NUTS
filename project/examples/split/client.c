#include <stdio.h>
#include <csp/csp.h>


#include <../../src/arch/csp_thread.h>

#define PORT 10
#define MY_ADDRESS 1

#define SERVER_TIDX 0
#define CLIENT_TIDX 1
#define USART_HANDLE 0

CSP_DEFINE_TASK(task_server) {
	int running = 1;
	csp_socket_t *socket = csp_socket(CSP_SO_NONE);
	csp_conn_t *conn;
	csp_packet_t *packet;
	csp_packet_t *response;

	response = csp_buffer_get(sizeof(csp_packet_t) + 2);
	if (response == NULL) {
		fprintf(stderr, "Could not allocate memory for response packet!\n");
		return CSP_TASK_RETURN;
	}
	response->data[0] = 'O';
	response->data[1] = 'K';
	response->length = 2;

	csp_bind(socket, CSP_ANY);
	csp_listen(socket, 5);

	printf("Server task started\r\n");

	while (running) {
		if ((conn = csp_accept(socket, 10000)) == NULL) {
			continue;
		}

		while ((packet = csp_read(conn, 100)) != NULL) {
			switch (csp_conn_dport(conn)) {
				case PORT:
					if(packet->data[0] == 'q')
						running = 0;
					csp_buffer_free(packet);
					csp_send(conn, response, 1000);
					break;
				default:
					printf("Packet received: %s\n", packet->data);
					csp_service_handler(conn, packet);
					break;
			}
		}
		csp_close(conn);
	}
	csp_buffer_free(response);
	
	return CSP_TASK_RETURN;
}

int main() {
	csp_buffer_init(10, 300);
	csp_init(MY_ADDRESS);
	csp_route_start_task(500,1);
	printf("Starting server task\r\n");
	csp_thread_handle_t handle_server;
	csp_thread_create(task_server, (signed char *) "SERVER", 1000, NULL, 0, &handle_server);
	while(1) {
		csp_sleep_ms(100000);
	}
	return 0;
}
