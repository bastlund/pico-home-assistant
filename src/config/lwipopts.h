/**
 * lwIP Configuration - Project Specific Settings
 * Based on Raspberry Pi Pico examples
 *
 * Copyright (c) 2024 Peter Westlund
 * Original Pico examples: Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

// Need more memory for TLS
#ifdef MQTT_CERT_INC
#define MEM_SIZE 8000
#endif

// Generally you would define your own explicit list of lwIP options
// (see https://www.nongnu.org/lwip/2_1_x/group__lwip__opts.html)
//
// This example uses a common include to avoid repetition
#include "lwipopts_examples_common.h"

#define MEMP_NUM_SYS_TIMEOUT (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1)

#ifdef MQTT_CERT_INC
#define LWIP_ALTCP 1
#define LWIP_ALTCP_TLS 1
#define LWIP_ALTCP_TLS_MBEDTLS 1
#ifndef NDEBUG
#define ALTCP_MBEDTLS_DEBUG LWIP_DBG_ON
#endif
/* TCP WND must be at least 16 kb to match TLS record size
   or you will get a warning "altcp_tls: TCP_WND is smaller than the RX decrypion buffer, connection
   RX might stall!" */
#undef TCP_WND
#define TCP_WND 16384
#endif // MQTT_CERT_INC

// This defaults to 4
#define MQTT_REQ_MAX_IN_FLIGHT 5

// Increase memory for MQTT operations
#ifndef MEM_SIZE
#define MEM_SIZE 8000
#endif

// MQTT output ring buffer size
#define MQTT_OUTPUT_RINGBUF_SIZE 2048

// TCP settings for better MQTT performance
#undef TCP_SND_BUF
#define TCP_SND_BUF (8 * TCP_MSS)

#undef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE 32

#endif
