#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <csp/csp.h>

#include "../src/arch/csp_thread.h"

#define MY_ADDRESS  1   // Address of local CSP node
#define MY_PORT     10  // Port to send test traffic to

CSP_DEFINE_TASK(task_server) {

    /* Create socket without any socket options
     * TODO: Test with CRC32 required           */
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);

    /* Bind all ports to socket */
    csp_bind(sock, CSP_ANY);

    /* Create 10 connections backlog queue */
    csp_listen(sock, 100);

    /* Pointer to current connection and packet */
    csp_conn_t *conn;
    csp_packet_t *packet;
	FILE *fr;
	//fr = fopen("/home/alex/fjfjfj.pdf", "wb");
	fr = fopen("/home/alex/appflow.jpg", "wb");
	//fr = fopen("/home/alex/fjfjfj.txt", "w");
	char *data;
	char data_string[255];
	bool c = true;
    /* Process incoming connections */
    while(c) {

        /* Wait for connection, 10000 ms timeout */
        if ((conn = csp_accept(sock, 10000)) == NULL) {
			printf("no connection");
            continue;
		}

        /* Read packets. Timeout is 100 ms */
        while ((packet = csp_read(conn, 100)) != NULL) {
            switch(csp_conn_dport(conn)) {
                case MY_PORT:
					data = packet->data;
					printf("length of received packet: %d\n", strlen(data));
					strncpy(data_string, data, 255);
					if (strcmp(data, "xffxx") == 0) {
						return CSP_TASK_RETURN;
					}
					//printf((char *) packet->data);
					fwrite(data,1,packet->length,fr);
					//fprintf(fr, data_string);
                    csp_buffer_free(packet);
            default:
                    /* Let the service handler reply pings, buffer use
                     * etc */
                    csp_service_handler(conn, packet);
                    break;
            }
        }

        /* Close current connection and handle next */
        csp_close(conn);

    }
	fclose(fr);
    return CSP_TASK_RETURN;

}

CSP_DEFINE_TASK(task_client) {

    csp_packet_t * packet;
    csp_conn_t * conn;

    FILE *fp;
	fp = fopen("/home/alex/Pictures/appflow.jpg", "rb");
	//fp = fopen("/home/alex/Documents/IT.pdf", "rb");
	//fp = fopen("/home/alex/cinnamon.css", "r");
    if(!fp) {
        fprintf(stderr, "Could not open file\n");
        exit(EXIT_FAILURE);
    }
    char *msg;
    char buff[255];
    int numr;

    while(feof(fp)==0) {
	    if((numr = fread(buff,1,255,fp)) != 255) {
	        printf("Failed to read data!");
	    } else {
            csp_sleep_ms(100);


            /**
             * Try data packet to server
             */

            /* Get packet buffer for data */
            packet = csp_buffer_get(500);
            printf("numr: %d\n", numr);
            if (packet == NULL) {
                /* Could not get buffer element */
               printf("Failed to get buffer element\n");
               return CSP_TASK_RETURN;
            }

            /* Connect to host HOST, port PORT, with regular UDP-like
             * protocol and 1000 ms timeout */
            conn = csp_connect(CSP_PRIO_NORM, MY_ADDRESS, MY_PORT, 1000, CSP_O_NONE);
            if (conn == NULL) {
                /* Connect failed */
                printf("Connection failed\n");
                /* Remember to free packet buffer */
                csp_buffer_free(packet);
                return CSP_TASK_RETURN;
            }

            /* Copy dummy data to packet */
            //msg = buff;
            //printf("Length of msg: %d\n", strlen(msg));
            //strcpy((char *) packet->data, msg);
		    /*int array[255];
		    for (int i = 0; i < 255; i++) {
		        array[i] = 'h';
		    }*/
		    
		    strcpy((char *) packet->data, buff);
		    
		  /*  for (int i = 0; i < 255; i++) {
		        packet->data[i] = buff[i];
		        printf("data[i]: %c", buff[i]);
		    } */
            

            /* Set packet length */
            packet->length = numr;

            /* Send packet */
            if (!csp_send(conn, packet, 1000)) {
                /* Send failed */
                printf("Send failed\n");
                csp_buffer_free(packet);
            }

            /* Close connection */
            csp_close(conn);
        }
    }
	csp_sleep_ms(199);
	conn = csp_connect(CSP_PRIO_NORM, MY_ADDRESS, MY_PORT, 1000, CSP_O_NONE);
        if (conn == NULL) {
            /* Connect failed */
            printf("Connection failed\n");
            /* Remember to free packet buffer */
            csp_buffer_free(packet);
            return CSP_TASK_RETURN;
        }

        /* Copy dummy data to packet */
        msg = "xffxx";
        strcpy((char *) packet->data, msg);

        /* Set packet length */
        packet->length = strlen(msg);

        /* Send packet */
        if (!csp_send(conn, packet, 1000)) {
            /* Send failed */
            printf("Send failed\n");
            csp_buffer_free(packet);
        }

        /* Close connection */
        csp_close(conn);

    return CSP_TASK_RETURN;
}

int main(int argc, char * argv[]) {

    /* Init buffer system with 10 packets of maximum 300 bytes each */
    csp_buffer_init(2, 800);

    /* Init CSP with address MY_ADDRESS */
    csp_init(MY_ADDRESS);

    /* Start router task with 500 word stack, OS task priority 1 */
    csp_route_start_task(500, 1);

    /* Enable debug output from CSP */
    if ((argc > 1) && (strcmp(argv[1], "-v") == 0)) {
        printf("Debug eneabled\r\n");
        csp_debug_toggle_level(4);

        printf("Conn table\r\n");
        csp_conn_print_table();

        printf("Route table\r\n");
        csp_route_print_table();

        printf("Interfaces\r\n");
        csp_route_print_interfaces();

    }
    
	int iret1, iret2;

    csp_thread_handle_t handle_server;
    iret1 = csp_thread_create(task_server, (signed char *) "SERVER", 1000, NULL, 0, &handle_server);

    csp_thread_handle_t handle_client;
    iret2 = csp_thread_create(task_client, (signed char *) "CLIENT", 1000, NULL, 0, &handle_client);

	pthread_join(handle_client, NULL);
	printf("Client thread returns: %d\n", iret2);
	pthread_join(handle_server, NULL);
	printf("Server thread returns: %d\n", iret1);


    return 0;

}
