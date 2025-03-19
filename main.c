#include <net/ethernet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define IF_NAME "wlp0s20f3"

typedef struct {
  u8 dest_mac[6];
  u8 source_mac[6];
  u8 type[2];
} eth_packet;

int main() {

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
