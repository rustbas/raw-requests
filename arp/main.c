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

#define PROTOCOL_TYPE_SHIFT  22
#define REQUEST_TYPE_SHIFT   12
#define PORT_SHIFT           2
#define IF_NAME              "wlp0s20f3"
#define MAC_LEN              6
#define IP_LEN               4

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
  u16 hw_type;
  u16 proto_type;
  u8  hw_size;
  u8  proto_size;
  u16 opcode;
  u8  sender_mac[MAC_LEN];
  u8  sender_ip[IP_LEN];
  u8  target_mac[MAC_LEN];
  u8  target_ip[IP_LEN];
} arp_header;

int main(){
  struct sockaddr_ll sll = {0};
  struct ifreq ifr;
  
  int fd, if_index, i;
  eth_header eth;
  arp_header arp;
  u8 frame[sizeof(eth_header) + sizeof(arp_header)];
  u8 rbuf[512];
  ssize_t ret;

  /* Ethernet header fill */
  eth.type = htons(0x0806);  
  memset(eth.dst, 0xFF, MAC_LEN); /* Broadcasing MAC-address */

  /* ARP header fill */
  arp.hw_type = htons(0x0001);
  arp.proto_type = htons(0x0800);
  arp.hw_size = 0x06;
  arp.proto_size = 0x04;
  arp.opcode = htons(0x0001);
  memset(arp.target_mac, 0x00, MAC_LEN);
  /* Router IP */
  arp.target_ip[0] = 192;
  arp.target_ip[1] = 168;
  arp.target_ip[2] = 2;
  arp.target_ip[3] = 1;

  printf("Target IP:   ");
  for (i=0; i<IP_LEN; i++)
    printf("%d%c", arp.target_ip[i], i==3?'\n':'.');    
  printf(PRINT_DELIM);
  
  /* Get socket descriptor */
  fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (fd == -1)
    return 1;

  /* Get interface data */
  snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", IF_NAME);

  if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
    return 1;
  if_index = ifr.ifr_ifindex;
  if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
    return 1;
  
  memcpy(eth.src,        ifr.ifr_hwaddr.sa_data, MAC_LEN);
  memcpy(arp.sender_mac, ifr.ifr_hwaddr.sa_data, MAC_LEN);
  
  if (ioctl(fd, SIOCGIFADDR, &ifr) < 0)
    return 1;
  
  /* Adding 2 bytes because of port shift */
  memcpy(arp.sender_ip, ifr.ifr_addr.sa_data+PORT_SHIFT, IP_LEN);

  /* Print interface data */
  printf("Index of %s is %d\n", IF_NAME, if_index);
  printf("MAC Address: ");
  for (i=0; i<MAC_LEN; i++)
    printf("%X%c", eth.src[i], i==5?'\n':':');

  printf("IP Address:  ");
  for (i=0; i<IP_LEN; i++)
    printf("%d%c", arp.sender_ip[i], i==3?'\n':'.');

  /* Create frame */
  memcpy(frame, &eth, sizeof(eth_header));
  memcpy(frame+sizeof(eth_header), &arp, sizeof(arp_header));
  
  printf(PRINT_DELIM);


  
  /* Print frame per byte */
  printf("Frame:       ");
  for (i=0; (long unsigned int)i<sizeof(frame); i++)
    printf("%02X%c",
	   *(frame+i) & 0xFF,
	   ((long unsigned int)i<sizeof(frame)-1)?' ':'\n');

  printf(PRINT_DELIM);
  
  /* Create request */
  sll.sll_family   = PF_PACKET;
  sll.sll_ifindex  = if_index;
  sll.sll_protocol = 0x0806;

  /* Send request */
  if (sendto(fd, frame, sizeof(frame), 0, (struct sockaddr *)&sll, sizeof(sll)) == -1)
    return -1;
  
  /* Receive answer */
  for (;;) {
    /* Get all answers */
    if ((ret = recv(fd, rbuf, sizeof(rbuf), 0)) == -1)
      return -1;
    /* Filter only ARP answers */
    if (memcmp(rbuf+REQUEST_TYPE_SHIFT, &eth.type, 2) == 0) {
      /* Print Answer MAC address */
      printf("Target MAC:  ");
      for (i=0; i<MAC_LEN; i++)
	printf("%X%c", *(rbuf+PROTOCOL_TYPE_SHIFT+i), i<5?':':'\n');
      break;
      }
  }
  return 0;
}

