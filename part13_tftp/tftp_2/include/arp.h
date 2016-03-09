#ifndef _arp_h_
#define _arp_h_

#define HON(n) (((n&0xff)<<8) | ((n&0xff00)>>8))
#define PROTO_ARP (0x0806)
#define PROTO_IP (0x0800)
#define PROTO_UDP (0x11)


typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct eth_hdr
{
	u8 d_mac[6]; //以太网目的地址
	u8 s_mac[6]; //以太网源地址
	u16 frame_type; // 协议类型
}ETH_HDR; 

//ARP协议报文
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

//ip协议报文--共20字节
typedef struct ip_hdr 
{
	ETH_HDR ethhdr;
	u8 vhl; //版本、数据长度
	u8 tos; //服务级别
	u16  len; //报文长度
	u16  ipid; //标示
	u16 ipoffset; //标志、分段偏移
	u8 ttl; //生存时间
	u8 proto; //用户协议
	u16 ipchksum; //报头检验和
	u8 srcipaddr[4]; //源IP地址
	u8 destipaddr[4];//目的IP地址	
}IP_HDR;

//UDP协议报文
typedef struct udp_hdr 
{
	IP_HDR iphdr;
	u16 sport; //源端口号
	u16 dport; //目的端口号
	u16  len; //报文长度
	u16 udpchksum; //报头检验和
	
}UDP_HDR;


//tftp数据包
typedef struct tftp_package
{
	u16 opcode;
	u16 blocknum;
	u8 data[0];
	
}TFTP_PAK;


extern u8 host_mac_addr[6];
extern u8 ipaddr[4];
extern u8 host_ip_addr[4];
extern u8 mac_addr[6];
extern u16 packet_len;
extern u8 buffer[1500];
#endif