#include <net/ethernet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define IF_NAME "wlp0s20f3"

typedef struct {
  u8 dest_mac[6];
  u8 source_mac[6];
  u8 type[2];
} eth_header;

typedef struct {
  u8 version;
  u8 DSF;
  u8 total_length[2];
  u8 identification[2];
  u8 flags;
  u8 ttl;
  u8 protocol;
  u8 checksum[2];
  u8 source_address[4];
  u8 destination_address[4];
} ipv4_header;

typedef struct {
  u8 source_port[2];
  u8 destination_port[2];
  u8 length[2];
  u8 checksum[2];
} udp_header;

typedef struct {
  u8  msg_type;
  u8  hw_type;
  u8  hw_address_len;
  u8  hops;
  u8  transaction_id[4];
  u16 flags;
  u8  sec_elapsed[2];
  u8  old_ip[4];
  u8  new_ip[4];
  u8  next_server_ip[4];
  u8  relay_server_ip[4];
  u8  client_mac[4];
  u8  client_padding[10];
  u8  server_hostname[64];
  u8  boot_file[128];
  u8  magic_cookie[4];
  /* options */
} dhcp_header;

int main() {

  eth_header eth;
  ipv4_header ipv4;
  udp_header udp;
  dhcp_header dhcp;

  memset(&eth,  0x00, sizeof(eth_header));
  memset(&ipv4, 0x00, sizeof(ipv4_header));
  memset(&udp,  0x00, sizeof(udp_header));
  memset(&dhcp, 0x00, sizeof(dhcp_header));

  memset(eth.dest_mac, 0xFF, 6);
  
  /* u8 source_mac[] = {} */
  
  struct ifreq ifr;
  u8 fd, if_index;

  if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    goto error;

  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", IF_NAME);

  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
    goto error;

  if_index = ifr.ifr_ifindex;

  printf("Index of %s is %d\n", IF_NAME, if_index);
  
  /* close(fd); */
  return 0;

 error:
  perror("Error!\n");
  exit(1);
}
