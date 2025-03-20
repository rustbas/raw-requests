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

#define IF_NAME "wlp0s20f3"

typedef unsigned char  u8;
typedef unsigned short u16;

typedef struct {
  u8  dst[6];
  u8  src[6];
  u16 type;
} eth_header;

typedef struct {
  u16 hw_type;
  u16 proto_type;
  u8  hw_size;
  u8  proto_size;
  u16 opcode;
  u8  sender_mac[6];
  u8  sender_ip[4];
  u8  target_mac[6];
  u8  target_ip[4];
} arp_header;

int main(){
  eth_header eth;
  arp_header arp;
  u8 frame[42];
  u8 rbuf[512];
  struct sockaddr_ll sll = {0};
  ssize_t ret;

  struct ifreq ifr;
  int fd, if_index, i;

  fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (fd == -1)
    return 1;

  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", IF_NAME);

  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
    return 1;
  if_index = ifr.ifr_ifindex;
  if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    return 1;

  memcpy(eth.src,        ifr.ifr_hwaddr.sa_data, 6);
  memcpy(arp.sender_mac, ifr.ifr_hwaddr.sa_data, 6);
  
  printf("Index of %s is %d\n", IF_NAME, if_index);
  printf("MAC Address: ");
  for (i=0; i<6; i++)
    printf("%X%c", eth.src[i], i==5?'\n':':');

  if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    return 1;
  
  /* Adding 2 bytes because of port shift */
  memcpy(arp.sender_ip, ifr.ifr_addr.sa_data+2, 4);
  printf("IP Address:  ");
  for (i=0; i<4; i++)
    printf("%d%c", arp.sender_ip[i], i==3?'\n':'.');

  memset(eth.dst, 0xFF, 6);
  eth.type = htons(0x0806);

  arp.hw_type = htons(0x0001);
  arp.proto_type = htons(0x0800);
  arp.hw_size = 0x06;
  arp.proto_size = 0x04;
  arp.opcode = htons(0x0001);
  memset(arp.target_mac, 0x00, 6);
  arp.target_ip[0] = 192;
  arp.target_ip[1] = 168;
  arp.target_ip[2] = 2;
  arp.target_ip[3] = 1;

  memcpy(frame, &eth, sizeof(eth_header));
  memcpy(frame+sizeof(eth_header), &arp, sizeof(arp_header));

  for (i=0; i<sizeof(frame); i++)
    printf("%02X%c", *(frame+i) & 0xFF, i<sizeof(frame)-1?' ':'\n');

  sll.sll_family   = PF_PACKET;
  sll.sll_ifindex  = if_index;
  sll.sll_protocol = 0x0806;

  if (sendto(fd, frame, sizeof(frame), 0, (struct sockaddr *)&sll, sizeof(sll)) == -1)
    return -1;

  for (;;) {
    if ((ret = recv(fd, rbuf, sizeof(rbuf), 0)) == -1)
      return -1;
    if (memcmp(rbuf+12, &eth.type, 2) == 0)
      {
      for (i=0; i<6; i++)
	printf("%X%c", *(rbuf+22+i), i<5?':':'\n');
      break;
      }
  }
  
  return 0;
}

