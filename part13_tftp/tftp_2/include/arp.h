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
	u8 d_mac[6]; //��̫��Ŀ�ĵ�ַ
	u8 s_mac[6]; //��̫��Դ��ַ
	u16 frame_type; // Э������
}ETH_HDR; 

//ARPЭ�鱨��
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

//ipЭ�鱨��--��20�ֽ�
typedef struct ip_hdr 
{
	ETH_HDR ethhdr;
	u8 vhl; //�汾�����ݳ���
	u8 tos; //���񼶱�
	u16  len; //���ĳ���
	u16  ipid; //��ʾ
	u16 ipoffset; //��־���ֶ�ƫ��
	u8 ttl; //����ʱ��
	u8 proto; //�û�Э��
	u16 ipchksum; //��ͷ�����
	u8 srcipaddr[4]; //ԴIP��ַ
	u8 destipaddr[4];//Ŀ��IP��ַ	
}IP_HDR;

//UDPЭ�鱨��
typedef struct udp_hdr 
{
	IP_HDR iphdr;
	u16 sport; //Դ�˿ں�
	u16 dport; //Ŀ�Ķ˿ں�
	u16  len; //���ĳ���
	u16 udpchksum; //��ͷ�����
	
}UDP_HDR;


//tftp���ݰ�
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