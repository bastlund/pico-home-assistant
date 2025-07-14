/**
 * Pico W Ping Network Utility
 * Based on Raspberry Pi Pico examples
 * 
 * Copyright (c) 2024 Peter Westlund
 * Original Pico examples: Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* Standard library includes */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/raw.h"

/* Network configuration constants */
#define PING_TARGET_IP_STR      "192.168.86.1"      /* Default ping target */
#define PING_TIMEOUT_MS         5000                /* Ping timeout in milliseconds */
#define PING_INTERVAL_MS        10000               /* Interval between pings */
#define PING_SIZE               (sizeof(struct icmp_echo_hdr) + 32)  /* Ping packet size */

/* Global variables */
static uint16_t ping_seq_num = 0;
static uint32_t ping_start_time = 0;
static struct raw_pcb *ping_pcb = NULL;
static ip_addr_t ping_target_ip;
static bool ping_in_flight = false;

/**
 * Callback function for receiving ping responses
 * 
 * @param arg User argument (unused)
 * @param pcb Protocol Control Block
 * @param p Received packet buffer
 * @param addr Source IP address
 * @return 1 if packet was consumed, 0 otherwise
 */
static u8_t ping_recv_cb(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);

    struct icmp_echo_hdr *iecho;

    if ((p->len >= (sizeof(struct icmp_echo_hdr))) && 
        pbuf_header(p, -(s16_t)PBUF_IP_HLEN) == 0) {
        
        iecho = (struct icmp_echo_hdr *)p->payload;

        if ((iecho->type == ICMP_ER) && (iecho->id == 0xABCD)) {
            printf("Ping-svar från %s: seq=%d tid=%ldms\n",
                   ipaddr_ntoa(addr), iecho->seqno, 
                   time_us_64() / 1000 - ping_start_time);
            ping_in_flight = false;
            pbuf_free(p);
            return 1;
        }
    }
    
    pbuf_free(p);
    return 0;
}

// --- Initiera ping-funktionaliteten ---
static void ping_init(void) {
    ipaddr_aton(PING_TARGET_IP_STR, &ping_target_ip);
    ping_pcb = raw_new(IP_PROTO_ICMP); // Korrekt konstant
    LWIP_ASSERT("ping_init: raw_pcb != NULL", ping_pcb != NULL);
    raw_recv(ping_pcb, ping_recv_cb, NULL);
    raw_bind(ping_pcb, IP_ANY_TYPE);
}

// --- Skicka en ping-begäran ---
static void ping_send(void) {
    struct pbuf *p = pbuf_alloc(PBUF_IP, PING_SIZE, PBUF_RAM);
    LWIP_ASSERT("ping_send: pbuf_alloc failed", p != NULL);

    if (p != NULL) {
        struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)p->payload;

        // **KORRIGERAT: Fyll i ICMP-huvudet manuellt.**
        // Fält för ICMP Echo Request (typ 8, kod 0)
        iecho->type = ICMP_ECHO;     // Typ: 8 (Echo Request)
        iecho->code = 0;             // Kod: 0
        iecho->id = 0xABCD;          // Identifierare
        iecho->seqno = ++ping_seq_num; // Sekvensnummer

        // Lägg till lite dummy-data efter ICMP-huvudet
        for (int i = 0; i < PING_SIZE - sizeof(struct icmp_echo_hdr); i++) {
            ((u8_t *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (u8_t)i;
        }

        // Beräkna checksumma (checksumman måste vara noll innan beräkning)
        iecho->chksum = 0;
        iecho->chksum = inet_chksum(iecho, PING_SIZE);

        ping_start_time = time_us_64() / 1000;

        err_t err = raw_sendto(ping_pcb, p, &ping_target_ip);

        if (err == ERR_OK) {
            printf("Skickar ping %s (seq=%d)\n", PING_TARGET_IP_STR, ping_seq_num);
            ping_in_flight = true;
        } else {
            printf("Fel vid sändning av ping: %d\n", err);
            pbuf_free(p); // Frigör endast om sändningen misslyckas
            ping_in_flight = false;
        }
    }
}

// --- Main-funktionen ---
int main(void)
{
    stdio_init_all();

    /* Give stdio time to initialize */
    sleep_ms(100);

    /* Display version information */
    printf("=== Pico W Network Ping Utility ===\n");
    printf("Version: %s\n", PROJECT_VERSION_FULL);
    printf("Base version: %s\n", PROJECT_VERSION);
    printf("Build info: Major=%d, Minor=%d, Patch=%d\n", 
           PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH);
    printf("====================================\n");
    
    /* Flush output to ensure it's displayed */
    fflush(stdout);

    printf("Startar WiFi-anslutning...\n");

    if (cyw43_arch_init()) {
        printf("WiFi-initialisering misslyckades!\n");
        return 1;
    }

    cyw43_arch_enable_sta_mode();

    printf("Ansluter till WiFi \"%s\"...\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Misslyckades med att ansluta till WiFi! Kontrollera SSID/Lösenord.\n");
        cyw43_arch_deinit();
        return 1;
    }
    printf("Ansluten till WiFi!\n");

    struct netif *netif = &cyw43_state.netif[CYW43_ITF_STA];
    if (netif_is_up(netif) && netif_is_link_up(netif)) {
        printf("Pico W IP-adress: %s\n", ipaddr_ntoa(&netif->ip_addr));
        printf("Nätmask: %s\n", ipaddr_ntoa(&netif->netmask));
        printf("Gateway: %s\n", ipaddr_ntoa(&netif->gw));
    } else {
        printf("Pico W:s nätverksgränssnitt är inte uppe eller länken är nere (DHCP misslyckades?).\n");
    }

    ping_init();

    uint64_t next_ping_time_us = time_us_64();

    while (true) {
        cyw43_arch_poll();

        if (ping_in_flight && (time_us_64() / 1000 - ping_start_time > PING_TIMEOUT_MS)) {
            printf("Ping till %s (seq=%d) timeout!\n", PING_TARGET_IP_STR, ping_seq_num);
            ping_in_flight = false;
        }

        if (!ping_in_flight && time_us_64() >= next_ping_time_us) {
            ping_send();
            next_ping_time_us = time_us_64() + (PING_INTERVAL_MS * 1000);
        }

        sleep_ms(1);
    }

    cyw43_arch_deinit();
    return 0;
}