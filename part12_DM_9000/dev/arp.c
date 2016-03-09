#include "arp.h"
#include "DM9000.h"

/* ����ARP����� */
void arp_request()
{
	/* ����ARP����� �������ARP���Ľṹ*/
	memcpy(arp_buf.ethhdr.d_mac, host_mac_addr, 6);//���Ŀ��MAC��ַ 0xff,��ΪĿ��MACδ֪
	memcpy(arp_buf.ethhdr.s_mac, mac_addr, 6);//���ԴMAC��ַ
	arp_buf.ethhdr.frame_type = HON(0x0806);
	arp_buf.hwtype = HON(1);
	arp_buf.protocol = HON(0x0800);
	arp_buf.hwlen = 6;
	arp_buf.prolen = 4;
	arp_buf.op = HON(1);
	memcpy(arp_buf.smacaddr,mac_addr,6);
	memcpy(arp_buf.sipaddr,ipaddr,4);
	memcpy(arp_buf.dipaddr,host_ip_addr,6);

	packet_len = 14 + 28;
	
	/* ����dm9000���ͺ�������������� */
	dm9000_tx(buffer,packet_len);
	
}

/* ����ARPӦ�������ȡmac��ַ��pc�����ģ� */
u8 arp_process()
{
	u32 i;
		
	if(packet_len < 28)
		return 0;

	memcpy(host_ip_addr, arp_buf.sipaddr, 4);//��ʱԴ�����PC����
	printf("host ip is :");
	for(i=0; i<4; i++)
	{
		printf("%03d ", host_ip_addr[i]);
	}
	printf("\n \r");
	
	memcpy(host_mac_addr, arp_buf.smacaddr, 6);
	for(i=0; i<6; i++)
	{
		printf("%02x ", host_mac_addr);
	}
	
	printf("\n \r");
}