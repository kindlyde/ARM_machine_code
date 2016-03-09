#include "string.h"
#include "arp.h"

#define SDRAM_KERNEL_START 0x31000000

u8 send_buf[1024];
u8* tftp_down_addr; 
u16 severport = 0;
u16 curblock = 1;

//ip���ĵ�У�麯��
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
	u8 *ptftp  = &send_buf[200];//��ΪIPͷ����UDPͷ�������ռ�
	u32 tftp_len  = 0; //���ݱ���ָ��
	UDP_HDR *udphdr;
	u8 iphdr;//ע��˴�
	
	//������
	ptftp[0] = 0x00;
	ptftp[1] = 0x01;
	tftp_len += 2;
	
	//�ļ���
	sprintf(&ptftp[tftp_len], "%s", filename);
	tftp_len += strlen(filename);
	ptftp[tftp_len] = '\0';
	tftp_len += 1;
	
	//��ʽ
	sprintf(&ptftp[tftp_len], "%s", "octet");
	tftp_len += strlen("octet");
	ptftp[tftp_len] = '\0';
	tftp_len += 1;
	
	//���UDPͷ��
	udphdr = ptftp - sizeof(UDP_HDR); 
	iphdr = ptftp - sizeof(UDP_HDR) + sizeof(ETH_HDR);//�����˽���λ��
	
	//UDPͷ����Ϣ
	udphdr->sport = HON(48915);
	udphdr->dport = HON(69);
	udphdr->len = HON(tftp_len + sizeof(UDP_HDR) - sizeof(IP_HDR));//UDPЭ�鱨���к���IP���ģ��ɲ��䶨������� �ṹ
	udphdr->udpchksum = 0x00;
	
	//IPͷ����Ϣ
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

//tftp����˵�Ӧ��
void tftp_send_ack(u16 blocknum)
{
	u8 *ptftp  = &send_buf[200];//��ΪIPͷ����UDPͷ�������ռ�
	u32 tftp_len  = 0; //���ݱ���ָ��
	UDP_HDR *udphdr;
	u8 iphdr;//ע��˴�
	
	//������---Ӧ���
	ptftp[0] = 0x00;
	ptftp[1] = 0x04;
	tftp_len += 2;
	
	//����
	ptftp[2] = ((blocknum & 0xff00)>>8);
	ptftp[3] = (blocknum&0xff);
	tftp_len += 2;
	
	//���UDPͷ��
	udphdr = ptftp - sizeof(UDP_HDR); 
	iphdr = ptftp - sizeof(UDP_HDR) + sizeof(ETH_HDR);//�����˽���λ��
	
	//UDPͷ����Ϣ
	udphdr->sport = HON(48915);
	udphdr->dport = HON(severport); //��Ϊ��Ӧ������Ӧ�ûظ�����Ӧ�˿ڣ��Ƚ������
	udphdr->len = HON(tftp_len + sizeof(UDP_HDR) - sizeof(IP_HDR));//UDPЭ�鱨���к���IP���ģ��ɲ��䶨������� �ṹ
	udphdr->udpchksum = 0x00;
	
	//IPͷ����Ϣ
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


void tftp_process(u8 *buf, u32 len, u16 port)
{
	u32 i;
	u32 tftp_len; //����tftp���ĳ���
	severport = port;
	
	TFTP_PAK *ptftp = buf + sizeof(UDP_HDR);//ָ��tftp���ݱ��Ĳ���
	
	tftp_len = len -sizeof(UDP_HDR);
	
	if(HON(ptftp->opcode) == 3)//���ݰ��ı��Ϊ3
	{
		if(HON(ptftp->blocknum) == curblock)//��ֹ�ظ����տ����ͬ�����ݰ�
		{
		
			tftp_down_addr = (u8*)(SDRAM_KERNEL_START + (curblock-1)*512);
			for(i = 0; i<(tftp_len - 4); i++)//�鿴tftp���ݰ��ĸ�ʽ֪ǰ���ֽ�Ϊ������Ϳ���
			{
				*(tftp_down_addr + 1) = *(ptftp->data+i);
				
			}
			
			tftp_send_ack(HON(ptftp->blocknum));//���յ�����֮����ظ���Ӧ��
			curblock++;
			
			if((tftp_len-4) < 512)//���һ����һ��С��512 ;��λ��־���ź��ڴ���ʼ��ַ��Ϊ��һ�δ�����׼��
			{
				curblock = 1;
				printf("tftp donwload over!\n\r");
			}
		}
		
	}
	
}


