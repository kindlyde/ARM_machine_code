#ifndef _arp_h_
#define _arp_h_

#define HON(n) (((n&0xff)<<8) | ((n&0xff00)>>8))

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct eth_hdr
{
	u8 d_mac[6]; //��̫��Ŀ�ĵ�ַ
	u8 s_mac[6]; //��̫��Դ��ַ
	u16 frame_type; // ֡����
}ETH_HDR; 


typedef struct arp_hdr 
{
	ETH_HDR ethhdr;
	u16 hwtype; //Ӳ������
	u16 protocol; //Э������
	u8  hwlen; //Ӳ����ַ����
	u8  prolen; //Э���ַ����
	u16 op; //�������ͣ���������Ӧ���λ
	u8 smacaddr[6]; //���Ͷ���̫����ַ
	u8 sipaddr[4]; //���Ͷ�IP��ַ
	u8 dmacaddr[6]; //Ŀ����̫����ַ
	u8 dipaddr[4]; //Ŀ��IP��ַ
	
}ARP_HDR;

ARP_HDR arp_buf;

extern u8 host_mac_addr[6];
extern u8 ipaddr[4];
extern u8 host_ip_addr[4];
extern u8 *buffer;
extern u8 mac_addr[6];
extern u16 packet_len;
#endif