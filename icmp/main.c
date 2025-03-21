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

#define PRINT_DELIM \
  "=========================================================\n"

typedef unsigned char  u8;
typedef unsigned short u16;

/* typedef struct { */
/*   u8  dst[MAC_LEN]; */
/*   u8  src[MAC_LEN]; */
/*   u16 type; */
/* } eth_header; */

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

int main(){

  return 0;
}

