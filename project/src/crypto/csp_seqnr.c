/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdint.h>
#include <string.h>

/* CSP includes */
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include "csp_seqnr.h"

#include "../arch/csp_thread.h"
#include "../arch/csp_semaphore.h"

#ifdef CSP_USE_SEQNR

/* Global seqnr. counter */
/* Initialize large enough so that we don't wrap arround in the forseable lifetime of the satellite. */
uint16_t csp_seqnr_rx[CSP_ID_HOST_MAX];
csp_mutex_t csp_seqnr_mutex_rx;

uint16_t csp_seqnr_tx[CSP_ID_HOST_MAX];
csp_mutex_t csp_seqnr_mutex_tx;

uint8_t csp_seqnr_errorcount[CSP_ID_HOST_MAX];

uint16_t csp_seqnr_get_rx(uint8_t host){
    return csp_seqnr_rx[host];
}

uint16_t csp_seqnr_get_tx(uint8_t host){
    return csp_seqnr_tx[host];
}

void csp_seqnr_set_rx(uint8_t host, uint16_t new_seqnr){
    if(csp_mutex_lock(&csp_seqnr_mutex_rx, 1000) == CSP_MUTEX_OK){
	csp_seqnr_rx[host] = new_seqnr;
	csp_mutex_unlock(&csp_seqnr_mutex_rx);
    }
}

void csp_seqnr_set_tx(uint8_t host, uint16_t new_seqnr){
    if(csp_mutex_lock(&csp_seqnr_mutex_tx, 1000) == CSP_MUTEX_OK){
        csp_seqnr_tx[host] = new_seqnr;
        csp_mutex_unlock(&csp_seqnr_mutex_tx);
    }
}

void csp_seqnr_increase_rx(uint8_t host) {
    if(csp_mutex_lock(&csp_seqnr_mutex_rx, 1000) == CSP_MUTEX_OK){
	csp_seqnr_rx[host] +=1;
        csp_mutex_unlock(&csp_seqnr_mutex_rx);
    }
}

void csp_seqnr_increase_tx(uint8_t host) {
    if(csp_mutex_lock(&csp_seqnr_mutex_tx, 1000) == CSP_MUTEX_OK){
        csp_seqnr_tx[host] +=1;
        csp_mutex_unlock(&csp_seqnr_mutex_tx);	
    }
}

int csp_seqnr_append(csp_packet_t * packet){

	if (packet == NULL)
		return CSP_ERR_INVAL;

	/* Make room for the sequence number (dst, src, len) */
	uint16_t hton_seqnr = csp_hton16(csp_seqnr_get_tx(packet->id.dst));
	
	memcpy(&packet->data[packet->length], &hton_seqnr, CSP_SEQUENCE_NUMBER_LENGTH);
	packet->length += CSP_SEQUENCE_NUMBER_LENGTH;

	csp_seqnr_increase_tx(packet->id.dst);

	return CSP_ERR_NONE;
}

/* Read seqnr from packet and check if > than global_counter + CSP_SEQUENCE_THRESHOLD */
int csp_seqnr_verify(csp_packet_t * packet){
	
    uint16_t sequence_number;
    memcpy(&sequence_number, &packet->data[packet->length - CSP_SEQUENCE_NUMBER_LENGTH], CSP_SEQUENCE_NUMBER_LENGTH);
    sequence_number = csp_ntoh16(sequence_number);
    printf("Verifying sequence number: PACKET = 0x%02X, COUNTER = 0x%02X \r\n", sequence_number, csp_seqnr_get_rx(packet->id.src));

    // Strip seqnr
    packet->length -= CSP_SEQUENCE_NUMBER_LENGTH;
    
    if(sequence_number >= csp_seqnr_get_rx(packet->id.src) && sequence_number < csp_seqnr_get_rx(packet->id.src) + CSP_SEQUENCE_THRESHOLD){
	/* Successful verification, reset error counter */
	csp_seqnr_errorcount[packet->id.src] = 0;
	csp_seqnr_increase_rx(packet->id.src);
	return CSP_ERR_NONE;		
    }

    /* Verification failed, checking to see if resync required or just increase counter */
    csp_seqnr_errorcount[packet->id.src] += 1;
    if(csp_seqnr_errorcount[packet->id.src] >= CSP_SEQUENCE_RESYNC_THRESHOLD){
	csp_seqnr_resync_init(packet);
	csp_seqnr_errorcount[packet->id.src] = 0;
    }

    return CSP_ERR_SEQNR;	
}
 
CSP_DEFINE_TASK(resync_thread){
    
    printf("Starting resynchronization\r\n");
    csp_packet_t * invalid_packet = param;
    
    // Trying to connect unathenticated to notify other party of HMAC/SEQNR verification errors
    csp_conn_t * resync_conn = csp_connect(CSP_PRIO_HIGH, invalid_packet->id.src , CSP_SEQNR_RESYNC, 1000, CSP_O_NONE);
    if(resync_conn == NULL){
        csp_log_error("Could not connect to resync port\r\n");
	//	return CSP_ERR_RESYNC;
	csp_thread_exit();
    }

    // Creating a packet to send
    csp_packet_t * packet = csp_buffer_get(100);
    if(packet == NULL){
	csp_log_error("Could not get buffer for resync packet\r\n");
	csp_close(resync_conn);
	//	return CSP_ERR_RESYNC;
	csp_thread_exit();
    }
    
    // Setting local sequence numbers as data
    uint16_t hton_rx_seqnr = csp_hton16(csp_seqnr_get_rx(invalid_packet->id.src));
    uint16_t hton_tx_seqnr = csp_hton16(csp_seqnr_get_tx(invalid_packet->id.src));
    memcpy(&packet->data[0], &hton_rx_seqnr, CSP_SEQUENCE_NUMBER_LENGTH);
    memcpy(&packet->data[CSP_SEQUENCE_NUMBER_LENGTH], &hton_tx_seqnr, CSP_SEQUENCE_NUMBER_LENGTH);
    packet->length = 2 * CSP_SEQUENCE_NUMBER_LENGTH;
    
    // Sending with HIGH priority
    if(csp_send_prio(CSP_PRIO_HIGH, resync_conn, packet, 1000) != 1){
	csp_log_error("Could not send resync packet\r\n");
	csp_buffer_free(packet);
	csp_close(resync_conn);
	//	return CSP_ERR_RESYNC;
	csp_thread_exit();
    }
    csp_buffer_free(packet);
    
    // Wait for response
    csp_packet_t * response_packet = csp_buffer_get(100);
    if(response_packet == NULL){
        csp_log_error("Could not get buffer for resync response packet\r\n");
        csp_close(resync_conn);
	//        return CSP_ERR_RESYNC;
	csp_thread_exit();
    }

    response_packet = csp_read(resync_conn, 100000);
    if(response_packet == NULL){
	csp_log_error("No response to resync packet\r\n");
	csp_buffer_free(response_packet);
        csp_close(resync_conn);
	//	return CSP_ERR_RESYNC;
	csp_thread_exit();
    }
 
    printf("Received a RESYNC RESPONSE packet: Length = %u, RAW = 0x", response_packet->length);
    int i;
    for(i = 0; i < response_packet->length; i++){
	printf("%02X", response_packet->data[i]);
    }
    printf("\r\n");
    
    uint16_t rx_response;
    uint16_t tx_response;
    memcpy(&rx_response, &response_packet->data[0], CSP_SEQUENCE_NUMBER_LENGTH);
    memcpy(&tx_response, &response_packet->data[CSP_SEQUENCE_NUMBER_LENGTH], CSP_SEQUENCE_NUMBER_LENGTH);
    rx_response = csp_ntoh16(rx_response);
    tx_response = csp_ntoh16(tx_response);

    // Set local seqnr to value in response ONLY if it is smaller, to prevent resetting by malicious mallards
    if(rx_response >= csp_seqnr_get_rx(response_packet->id.src)){
	csp_seqnr_set_rx(response_packet->id.src, rx_response);
    }
    if(tx_response >= csp_seqnr_get_tx(response_packet->id.src)){
	csp_seqnr_set_tx(response_packet->id.src, tx_response);
    }

    // Cleaning up
    csp_buffer_free(response_packet);
    csp_close(resync_conn);

    //    return CSP_ERR_NONE;
    csp_thread_exit();
}

csp_thread_handle_t csp_seqnr_resync_init(csp_packet_t * invalid_packet){
	
	csp_thread_handle_t resync_thread_handle;
	csp_thread_create(resync_thread, (signed  char *) "Resynch thread", 1000, invalid_packet, 0, &resync_thread_handle);
	return resync_thread_handle;

}

int csp_seqnr_resync_respond(csp_conn_t * resync_conn, csp_packet_t * resync_packet){

    uint16_t remote_rx_sequence_number;
    uint16_t remote_tx_sequence_number;
    memcpy(&remote_rx_sequence_number, &resync_packet->data[0], CSP_SEQUENCE_NUMBER_LENGTH);
    memcpy(&remote_tx_sequence_number, &resync_packet->data[CSP_SEQUENCE_NUMBER_LENGTH], CSP_SEQUENCE_NUMBER_LENGTH);
    remote_rx_sequence_number = csp_ntoh16(remote_rx_sequence_number);
    remote_tx_sequence_number = csp_ntoh16(remote_tx_sequence_number);

    uint16_t rx_sequence_number_to_send = (remote_rx_sequence_number > csp_seqnr_get_tx(resync_packet->id.src)) ? remote_rx_sequence_number : csp_seqnr_get_tx(resync_packet->id.src);
    uint16_t tx_sequence_number_to_send = (remote_tx_sequence_number > csp_seqnr_get_rx(resync_packet->id.src)) ? remote_tx_sequence_number : csp_seqnr_get_rx(resync_packet->id.src);
    csp_seqnr_set_rx(resync_packet->id.src, tx_sequence_number_to_send);
    csp_seqnr_set_tx(resync_packet->id.src, rx_sequence_number_to_send);
    
    printf("Preparing to send RESYNC RESPONSE: %u %u\r\n", tx_sequence_number_to_send, rx_sequence_number_to_send);
    
    // Creating a packet to send
    csp_packet_t * packet = csp_buffer_get(100);
    if(packet == NULL){
        csp_log_error("Could not get buffer for resync response packet\r\n");
        csp_close(resync_conn);
	return CSP_ERR_RESYNC;
    }

    // Setting highest sequence number as data
    rx_sequence_number_to_send = csp_hton16(rx_sequence_number_to_send);
    tx_sequence_number_to_send = csp_hton16(tx_sequence_number_to_send);
    memcpy(&packet->data[0], &rx_sequence_number_to_send, CSP_SEQUENCE_NUMBER_LENGTH);
    memcpy(&packet->data[CSP_SEQUENCE_NUMBER_LENGTH], &tx_sequence_number_to_send, CSP_SEQUENCE_NUMBER_LENGTH);
    packet->length = 2 * CSP_SEQUENCE_NUMBER_LENGTH;

    printf("Sending a RESYNC RESPONSE packet: Length = %u, RAW = 0x", packet->length);
    int i;
    for(i = 0; i < packet->length; i++){
        printf("%02X", packet->data[i]);
    }
    printf("\r\n");

    if(csp_send_prio(CSP_PRIO_HIGH, resync_conn, packet, 1000) != 1){
        csp_log_error("Could not send resync response packet\r\n");
	csp_buffer_free(packet);
        csp_close(resync_conn);
	return CSP_ERR_RESYNC;
    }

    // Cleaning up
    csp_buffer_free(packet);
    csp_close(resync_conn);

    return CSP_ERR_NONE;
}

CSP_DEFINE_TASK(resync_listen_task){
    // Need to listen with no options on CSP_SEQNR_RESYNC incase other wants to tell me we're out of sync
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, CSP_SEQNR_RESYNC);
    csp_listen(sock, 10);

    csp_conn_t * conn;
    csp_packet_t * packet;

    while(true){
        if((conn = csp_accept(sock, 10000)) != NULL){
            while ((packet = csp_read(conn, 100)) != NULL) {
                switch (csp_conn_dport(conn)) {
                case CSP_SEQNR_RESYNC:
		    csp_service_handler(conn, packet);
                    break;
                default:
		    csp_buffer_free(packet);
                    break;
                }
            }
            csp_close(conn);
        }
    }

    return CSP_TASK_RETURN;
}

void csp_seqnr_init(){
    csp_mutex_create(&csp_seqnr_mutex_rx);
    csp_mutex_create(&csp_seqnr_mutex_tx);

    printf("Starting resync_listen_task\r\n");
    csp_thread_handle_t resync_handle;
    csp_thread_create(resync_listen_task, (signed char *) "Resync", 1000, NULL, 0, &resync_handle);
}

#endif // CSP_USE_SEQNR
