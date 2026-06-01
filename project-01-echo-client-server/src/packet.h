#pragma once
#ifndef _PROJ_01_PACKET_H_
#define _PROJ_01_PACKET_H_

#include <inttypes.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "utility.h"

#define APP_DATA_BUF_LEN (0x100 - 0x2E)
#define PACKET_LEN_MAX (sizeof(struct ether_frame) - 2)

struct udp_segment {
    uint16_t    src_port;                       // 0x24
    uint16_t    dst_port;                       // 0x28
    uint16_t    len;                            // 0x2A
    uint16_t    checksum;                       // 0x2C
    uint8_t     app_data_buf[APP_DATA_BUF_LEN]; // 0x2E
};

struct inet_datagram {                        
    uint8_t             version_and_hdr_len;    // 0x10
    uint8_t             dscp;                   // 0x11
    uint16_t            len;                    // 0x12
    uint16_t            identification;         // 0x14
    uint16_t            flags_and_offset;       // 0x16
    uint8_t             ttl;                    // 0x18
    uint8_t             protocol;               // 0x19
    uint16_t            hdr_checksum;           // 0x1A
    in_addr_t           src_addr;               // 0x1C - aligned
    in_addr_t           dst_addr;               // 0x20 - aligned
    struct udp_segment  payload_udp;            // 0x24
};

struct ether_frame {                            // Offset
    uint8_t                 _padding[2];        // 0x00
    uint8_t                 dst_mac[6];         // 0x02
    uint8_t                 src_mac[6];         // 0x08
    uint16_t                ethertype;          // 0x0E
    struct inet_datagram    payload_inet;       // 0x10 - multiple of 4
};


void udp_segment_build(struct udp_segment *segment, uint16_t src_port, uint16_t dst_port, const uint8_t *data, size_t data_len);

#endif
