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
    csp_listen(sock, 10);

    /* Pointer to current connection and packet */
    csp_conn_t *conn;
    csp_packet_t *packet;
	//fp = fopen("/home/alex/Pictures/received.jpg", "w");

    /* Process incoming connections */
    while(1) {

        /* Wait for connection, 10000 ms timeout */
        if ((conn = csp_accept(sock, 10000)) == NULL)
            continue;

        /* Read packets. Timeout is 100 ms */
        while ((packet = csp_read(conn, 100)) != NULL) {
            switch(csp_conn_dport(conn)) {
                case MY_PORT:
                    /* Process packet here */
                    printf("Packet received on MY_PORT: %s\r\n", (char *) packet->data);
                    printf("Length of packet:%d \n", packet->length);
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

    return CSP_TASK_RETURN;

}

CSP_DEFINE_TASK(task_client) {

    csp_packet_t * packet;
    csp_conn_t * conn;

    FILE *fp;
    //fp = fopen("/home/alex/git/NUTS/project/examples/largefile.txt", "r");
	fp = fopen("/home/alex/Pictures/appflow.jpg", "r");
    if(!fp) {
        fprintf(stderr, "Could not open file\n");
        exit(EXIT_FAILURE);
    }
    char *msg;
    char buff[255];

    while(1) {

       
        /**
         * Try ping
         */

        csp_sleep_ms(100);

        int result = csp_ping(MY_ADDRESS, 100, 100, CSP_O_NONE);
        printf("Ping result %d [ms]\r\n", result);

        csp_sleep_ms(100);

        /**
         * Try data packet to server
         */

        /* Get packet buffer for data */
        packet = csp_buffer_get(10);
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

        //fgets(buff, 55, (FILE*)fp);
        int bytes_read = fread(buff, sizeof(unsigned char), 255, (FILE*)fp);
        printf("Bytes read %d\n", bytes_read);
        /* Copy dummy data to packet */
        msg = "TThree rings for the Elven kings under the sky\nSeven for the Dwarf Lords in their halls of stone\nNine for mortal men doomed to die\nOne for the Dark Lord on his dark throne in the land of Mordor where the shadows lie\nOne ring to rule them all\nOne ring to find them\nOne ring to bring them all, and in the darkness bind them\nIn the land of Mordor where the shadows lie\nThree rings for the Elven kings under the sky\nSeven for the Dwarf Lords in their halls of stone\nNine for mortal men doomed to die\nOne for the Dark Lord on his dark throne in the land of Mordor where the shadows lie\nOne ring to rule them all\nOne ring to find them\nOne ring to bring them all, and in the darkness bind them\nIn the land of Mordor where the shadows lie.";
        msg = buff;
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
    }

    return CSP_TASK_RETURN;
}

int main(int argc, char * argv[]) {

    /**
     * Initialise CSP,
     * No physical interfaces are initialised in this example,
     * so only the loopback interface is registered
     */

    /* Init buffer system with 10 packets of maximum 300 bytes each */
    printf("Initialising CSP\r\n");
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

    /**
     * Initialise example threads, using pthreads
     */

    /* Server */
    printf("Starting Server task\r\n");
    csp_thread_handle_t handle_server;
    csp_thread_create(task_server, (signed char *) "SERVER", 1000, NULL, 0, &handle_server);

    /* Client */
    printf("Starting Client task\r\n");
    csp_thread_handle_t handle_client;
    csp_thread_create(task_client, (signed char *) "CLIENT", 1000, NULL, 0, &handle_client);

    /* Wait for execution to end (ctl+c) */
    while(1) {
        csp_sleep_ms(100000);
    }

    return 0;

}
