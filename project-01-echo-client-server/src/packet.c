#include "packet.h"

void udp_segment_build(struct udp_segment *segment, uint16_t src_port, uint16_t dst_port, const uint8_t *data, size_t data_len) {
    size_t datagram_len = 8 + data_len;

    segment->src_port = htons(src_port);
    segment->dst_port = htons(dst_port);
    segment->len = htons((uint16_t)datagram_len);
    segment->checksum = 0;
    memcpy(segment->app_data_buf, data, data_len);

    segment->checksum = ~calc_checksum16(segment, datagram_len);
}
