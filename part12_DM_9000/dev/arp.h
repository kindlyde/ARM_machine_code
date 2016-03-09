#ifndef _arp_h_
#define _arp_h_

#define HON(n) (((n&0xff)<<8) | ((n&0xff00)>>8))

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct eth_hdr
{
	u8 d_mac[6]; //以太网目的地址
	u8 s_mac[6]; //以太网源地址
	u16 frame_type; // 帧类型
}ETH_HDR; 


typedef struct arp_hdr 
{
	ETH_HDR ethhdr;
	u16 hwtype; //硬件类型
	u16 protocol; //协议类型
	u8  hwlen; //硬件地址长度
	u8  prolen; //协议地址长度
	u16 op; //操作类型，区分请求、应答包位
	u8 smacaddr[6]; //发送端以太网地址
	u8 sipaddr[4]; //发送端IP地址
	u8 dmacaddr[6]; //目的以太网地址
	u8 dipaddr[4]; //目的IP地址
	
}ARP_HDR;

ARP_HDR arp_buf;

extern u8 host_mac_addr[6];
extern u8 ipaddr[4];
extern u8 host_ip_addr[4];
extern u8 *buffer;
extern u8 mac_addr[6];
extern u16 packet_len;
#endif