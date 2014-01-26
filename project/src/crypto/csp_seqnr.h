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

#ifndef _CSP_HMAC_H_
#define _CSP_HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "../arch/csp_thread.h"

#define CSP_SEQUENCE_NUMBER_LENGTH 2 /* DO NOT CHANGE THIS WITHOUT CHANGING TO SUITABLE VARIABLE TYPES */
#define CSP_SEQUENCE_THRESHOLD 1
#define CSP_SEQUENCE_RESYNC_THRESHOLD 2

/**
 * Initialize the seqnr environment
 */
void csp_seqnr_init();

/**
 * Set/get the sequence number to a specific value
 * 
 * @param the host for set it for
 * @param the value to set it to
 */
void csp_seqnr_set_rx(uint8_t host, uint16_t new_seqnr);
void csp_seqnr_set_tx(uint8_t host, uint16_t new_seqnr);

uint16_t csp_seqnr_get_rx(uint8_t host);
uint16_t csp_seqnr_get_tx(uint8_t host);


/**
 * Increase sequence number for a specific host
 *
 * @param the host
 */
void csp_seqnr_increase_rx(uint8_t host);
void csp_seqnr_increase_tx(uint8_t host);

/**
 * Append sequence number to the packet
 *
 *@param packet Pointer to packet
 *@return + on success, -1 on failure
 */
int csp_seqnr_append(csp_packet_t * packet);

/**
 * Verify that the sequence number actually is increasing.
 * 
 *
 * @param packet Pointer to packet
 * @return 0 if the sequence number is within the CSP_SEQUENCE_NUMBER_JUMP threshold. -1 if failure 
 */
int csp_seqnr_verify(csp_packet_t * packet);

/**
* If the sewuence number gets out of synch on either the groundstation (GS) or in the satellite (S), this methow will bring it back to a consistent state.
* @param csp_conn_t the connection used to send the resynch-commands over. 
* @return 0 if the opreration is successfull, != 0 otherwise.
*/
csp_thread_handle_t csp_seqnr_resync_init(csp_packet_t * invalid_packet);

int csp_seqnr_resync_respond(csp_conn_t * resync_conn, csp_packet_t * resync_packet);


#endif // _CSP_HMAC_H_
