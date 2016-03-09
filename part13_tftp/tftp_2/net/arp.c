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
u8 arp_process(u8 *buf,u32 len)
{
	u32 i;
	ARP_HDR *p =(ARP_HDR *)buf; 
		
		
	if(packet_len < 28)
		return 0;

	switch (HON(p->op))
	{
		case 2: //ARP��Ӧ��
		memcpy(host_ip_addr, p->sipaddr, 4);//��ʱԴ�����PC����
		printf("host ip is :");
		for(i=0; i<4; i++)
		{
			printf("%03d ", host_ip_addr[i]);
		}
		printf("\n \r");
		
		memcpy(host_mac_addr, p->smacaddr, 6);
		for(i=0; i<6; i++)
		{
			printf("%02x ", host_mac_addr);
		}
		
		printf("\n \r");
		break;
		
		case 1:  //�������͵������;����ARP��Ӧ��
		
		memcpy(arp_buf.ethhdr.d_mac, p->smacaddr, 6);//���ڴ������Ϊ�õ����� ��������������������ķ��Ͷ˵�ַ����������MAC��ַ
		memcpy(arp_buf.ethhdr.s_mac, mac_addr, 6);//���ԴMAC��ַ
		arp_buf.ethhdr.frame_type = HON(0x0806); //Э������
		arp_buf.hwtype = HON(1); //Ӳ�����ͣ���̫��Ϊ1
		arp_buf.protocol = HON(0x0800);//���ͷ��߲�Э�����ͣ�������IPЭ��
		arp_buf.hwlen = 6; //��̫����ַ����
		arp_buf.prolen = 4; //Э���ַ����
		arp_buf.op = HON(2); //����Ϊ��Ӧ��
		
		memcpy(arp_buf.smacaddr,mac_addr,6); //���Ͷ���̫��Դ��ַ
		memcpy(arp_buf.sipaddr,ipaddr,4); // ���Ͷ�IP��ַ
		memcpy(arp_buf.dmacaddr,p->dmacaddr,6); //Ŀ����̫����ַ
		memcpy(arp_buf.dipaddr,p->dipaddr,6); //Ŀ��IP��ַ

		packet_len = 14 + 28;
		
		/* ����dm9000���ͺ�������������� */
		dm9000_tx(&arp_buf,packet_len);
		break;
		
	}
}


void udp_process(u8 *buf, u32 len) //�ڰ���UDP�İ��У�ֻ����IP�İ�
{
	UDP_HDR *udp_hdr = (UDP_HDR *)buf;
	
	tftp_process(buf,len,HON(udp_hdr->sport));//ע��Ҫ��¼�˿�
}

void ip_process(u8 *buf, u32 len)
{
	IP_HDR *p = (IP_HDR *)buf;
	
	switch(p->proto)
	{
		case PROTO_UDP: //�����UDP�İ��Ž��д���
			udp_process(buf,len);
			break;
		default:
			break;
	}
}

void net_handle(u8 *buf, u32 len)
{
	ETH_HDR *p = (ETH_HDR *)buf;
	
	switch(HON(p->frame_type))
	{
		case PROTO_ARP:   //arp��
			arp_process(buf,len);
			break;
			
		case PROTO_IP:    //ip��
			ip_process(buf,len);
			break;
		
		default:
			break;
	}
}