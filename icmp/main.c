#include <net/ethernet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netpacket/packet.h>

#define MAC_LEN 6

#define PRINT_DELIM \
  "=========================================================\n"

typedef unsigned char  u8;
typedef unsigned short u16;

typedef struct {
  u8  dst[MAC_LEN];
  u8  src[MAC_LEN];
  u16 type;
} eth_header;

typedef struct {
  /* According to RFC 791 3.1 */
  u8 version_ihl;
  u8 type_of_service;
  u16 total_length;
  u16 identivication;
  u16 flags_fragment_offset;
  u8 ttl;
  u8 protocol;
  u16 header_checksum;
  u8 src_address[4];
  u8 dst_address[4];
} ipv4_header;

typedef struct {
  /* According to RFC 792 */
  u8 type;
  u8 code;
  u16 checksum;
  u16 identifier;
  u16 seq_number;
} icmp_header;

u16 calculate_checksum(ipv4_header *ipv4){

  u16 *data = (u16*) ipv4;

  const size_t data_len = 28 / 2; /* double-bytes size */
  size_t i;
  
  u16 res = 0;

  for (i=0; i<data_len; i++)
    if (i != 5) /* checksum location */
      res += htons(data[i]);
  
  return res;
}

int main(){

  u8 frame[] = {
    0x78, 0x9a, 0x18, 0x33, 0x29, 0x6a,
    0xe0, 0x2b, 0xe9, 0xab, 0xa2, 0x39,
    0x08, 0x00, 0x45, 0x00, 0x00, 0x54,
    0x02, 0x7e, 0x40, 0x00, 0x40, 0x01,
    0xb2, 0xd7, 0xc0, 0xa8, 0x02, 0x02,
    0xc0, 0xa8, 0x02, 0x01, 0x08, 0x00,
    0x3c, 0xa5, 0xb9, 0xac, 0x00, 0x01,
    0xe5, 0x6a, 0xe1, 0x67, 0x00, 0x00,
    0x00, 0x00, 0x70, 0x07, 0x0c, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
    0x36, 0x37};
    
  
  int fd;

  eth_header eth = {
    .dst	= {0x78, 0x9a, 0x18, 0x33, 0x29, 0x6a},
    .src	= {0xe0, 0x2b, 0xe9, 0xab, 0xa2, 0x39},
    .type	= htons(0x0800),
  };

  ipv4_header ipv4 = {
    .version_ihl		= (0x45),
    .type_of_service		= 0x00,
    .total_length		= htons(0x0054),
    .identivication		= htons(0x027e),
    .flags_fragment_offset	= htons(0x4000),
    .ttl			= (0x40),
    .protocol			= (0x01),
    .header_checksum		= htons(0xb2d7), /* TODO: create function to calculate checksum */
    .src_address		= {192, 168, 2, 2},
    .dst_address		= {192, 168, 2, 1},
  };
  
  fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (fd == -1)
    return 1;

  struct sockaddr_ll sll = {0};
  
  /* Create request */
  sll.sll_family   = PF_PACKET;
  sll.sll_ifindex  = 3;
  sll.sll_protocol = 0x0806;

  memcpy(frame, &eth, sizeof(eth_header));


  for (size_t i=sizeof(eth_header);
       i<sizeof(eth_header)+sizeof(ipv4_header);
       i++)
    printf("%02X ", frame[i]);
  printf("\n\n");

  memcpy(frame+sizeof(eth_header), &ipv4, sizeof(ipv4_header));

  for (size_t i=sizeof(eth_header);
       i<sizeof(eth_header)+sizeof(ipv4_header);
       i++)
    printf("%02X ", frame[i]);
  printf("\n");
  
  if (sendto(fd, frame, sizeof(frame), 0, (struct sockaddr *)&sll, sizeof(sll)) == -1)
    return -1;

  printf("%X\n", (calculate_checksum(&ipv4)));
  return 0;
}
