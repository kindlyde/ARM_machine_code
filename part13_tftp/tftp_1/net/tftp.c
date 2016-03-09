#include "string.h"
#include "arp.h"

u8 send_buf[1024];

//ip报文的校验函数
u16 checksum(u8 *ptr, int len)
{
    u32 sum = 0;
    u16 *p = (u16 *)ptr;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }
    
    if(len == 1)
        sum += *(u8 *)p;
    
    while(sum>>16)
        sum = (sum&0xffff) + (sum>>16);
    
    return (u16)((~sum)&0xffff);
}

void tftp_send_request(const char *filename)
{
	u8 *ptftp  = &send_buf[200];//先为IP头部、UDP头部留出空间
	u32 tftp_len  = 0; //数据报文指针
	UDP_HDR *udphdr;
	u8 iphdr;//注意此处
	
	//操作码
	ptftp[0] = 0x00;
	ptftp[1] = 0x01;
	tftp_len += 2;
	
	//文件名
	sprintf(&ptftp[tftp_len], "%s", filename);
	tftp_len += strlen(filename);
	ptftp[tftp_len] = '\0';
	tftp_len += 1;
	
	//格式
	sprintf(&ptftp[tftp_len], "%s", "octet");
	tftp_len += strlen("octet");
	ptftp[tftp_len] = '\0';
	tftp_len += 1;
	
	//添加UDP头部
	udphdr = ptftp - sizeof(UDP_HDR); 
	iphdr = ptftp - sizeof(UDP_HDR) + sizeof(ETH_HDR);//还需了解其位置
	
	//UDP头部信息
	udphdr->sport = HON(48915);
	udphdr->dport = HON(69);
	udphdr->len = HON(tftp_len + sizeof(UDP_HDR) - sizeof(IP_HDR));//UDP协议报文中含有IP报文，可查其定义的数据 结构
	udphdr->udpchksum = 0x00;
	
	//IP头部信息
	udphdr->iphdr.vhl = 0x45;
	udphdr->iphdr.tos = 0x00;
	udphdr->iphdr.len = HON(tftp_len + sizeof(UDP_HDR) - sizeof(ETH_HDR));
	udphdr->iphdr.ipid = HON(0X00);
	udphdr->iphdr.ipoffset = HON(0x4000);
	udphdr->iphdr.ttl = 0xff;
	udphdr->iphdr.proto = 17;
	memcpy(udphdr->iphdr.srcipaddr, ipaddr, 4);
	memcpy(udphdr->iphdr.destipaddr, host_ip_addr, 4);
	udphdr->iphdr.ipchksum = 0;
	udphdr->iphdr.ipchksum = checksum(iphdr,20);

	memcpy(udphdr->iphdr.ethhdr.s_mac, mac_addr, 6);
	memcpy(udphdr->iphdr.ethhdr.d_mac, host_mac_addr, 6);
	udphdr->iphdr.ethhdr.frame_type = HON(PROTO_IP);
	
	dm9000_tx((u32 *)udphdr, sizeof(UDP_HDR) + tftp_len);
}