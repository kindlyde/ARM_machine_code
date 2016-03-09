#include "arp.h"
#include "DM9000.h"

/* 发送ARP请求包 */
void arp_request()
{
	/* 构成ARP请求包 ，并填充ARP包的结构*/
	memcpy(arp_buf.ethhdr.d_mac, host_mac_addr, 6);//填充目的MAC地址 0xff,因为目的MAC未知
	memcpy(arp_buf.ethhdr.s_mac, mac_addr, 6);//填充源MAC地址
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
	
	/* 调用dm9000发送函数，发送请求包 */
	dm9000_tx(buffer,packet_len);
	
}

/* 解析ARP应答包，提取mac地址（pc主机的） */
u8 arp_process(u8 *buf,u32 len)
{
	u32 i;
	ARP_HDR *p =(ARP_HDR *)buf; 
		
		
	if(packet_len < 28)
		return 0;

	switch (HON(p->op))
	{
		case 2: //ARP响应包
		memcpy(host_ip_addr, p->sipaddr, 4);//此时源则代表PC主机
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
		
		case 1:  //主机发送的请求包;则发送ARP响应包
		
		memcpy(arp_buf.ethhdr.d_mac, p->smacaddr, 6);//由于此种情况为得到主机 的请求包；这在请求包里的发送端地址就是主机的MAC地址
		memcpy(arp_buf.ethhdr.s_mac, mac_addr, 6);//填充源MAC地址
		arp_buf.ethhdr.frame_type = HON(0x0806); //协议类型
		arp_buf.hwtype = HON(1); //硬件类型：以太网为1
		arp_buf.protocol = HON(0x0800);//发送方高层协议类型，这里是IP协议
		arp_buf.hwlen = 6; //以太网地址长度
		arp_buf.prolen = 4; //协议地址长度
		arp_buf.op = HON(2); //现在为响应包
		
		memcpy(arp_buf.smacaddr,mac_addr,6); //发送端以太网源地址
		memcpy(arp_buf.sipaddr,ipaddr,4); // 发送端IP地址
		memcpy(arp_buf.dmacaddr,p->dmacaddr,6); //目的以太网地址
		memcpy(arp_buf.dipaddr,p->dipaddr,6); //目的IP地址

		packet_len = 14 + 28;
		
		/* 调用dm9000发送函数，发送请求包 */
		dm9000_tx(&arp_buf,packet_len);
		break;
		
	}
}


void udp_process(u8 *buf, u32 len) //在包含UDP的包中，只处理IP的包
{
	UDP_HDR *udp_hdr = (UDP_HDR *)buf;
	
	tftp_process(buf,len,HON(udp_hdr->sport));//注意要记录端口
}

void ip_process(u8 *buf, u32 len)
{
	IP_HDR *p = (IP_HDR *)buf;
	
	switch(p->proto)
	{
		case PROTO_UDP: //如果是UDP的包才进行处理
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
		case PROTO_ARP:   //arp包
			arp_process(buf,len);
			break;
			
		case PROTO_IP:    //ip包
			ip_process(buf,len);
			break;
		
		default:
			break;
	}
}