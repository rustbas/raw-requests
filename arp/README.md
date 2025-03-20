# Address Resolution Protocol

ARP (англ. Address Resolution Protocol — протокол определения адреса)
— протокол в компьютерных сетях, предназначенный для определения
MAC-адреса другого компьютера по известному IP-адресу.

# Checkboxes

## ethernet

- [ ] dst
- [ ] src
- [ ] type
```c
typedef struct {
  u8  dst[6];
  u8  src[6];
  u16 type;
} eth_header;
```

## arp

- [ ] `hw_type`
- [ ] `proto_type`
- [ ] `hw_size`
- [ ] `proto_size`
- [ ] `opcode`
- [ ] `sender_mac`
- [ ] `sender_ip`
- [ ] `target_mac`
- [ ] `target_ip`

```c
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
```
