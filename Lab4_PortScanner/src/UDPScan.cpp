#include "UDPScan.h"
void UDPScanHost(struct UDPScanHostThrParam *p)
{
	string HostIP;
	unsigned HostPort,LocalPort,LocalHostIP;

	int UDPSock;
	struct sockaddr_in UDPScanHostAddr,FromHostAddr; 
	int n,FromLen;
	int on,ret;

	struct icmp *icmp;
	struct ipicmphdr hdr;

	struct iphdr *ip; 
    struct udphdr *udp; 
    struct pseudohdr *pseudo; 
    char packet[sizeof(struct iphdr)+sizeof(struct udphdr)];

	char SendBuf[2];
	char RecvBuf[100];

	int HeadLen;
	struct   timeval TimeOut;

	struct timeval TpStart,TpEnd;
	float TimeUse;
	//获得目标主机IP地址
    HostIP = p->HostIP;
	HostPort = p->HostPort;
	LocalPort = p->LocalPort;
	LocalHostIP = p->LocalHostIP;
	//创建套接字UDPSock
	UDPSock=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); 
    if(UDPSock < 0)
	{
		pthread_mutex_lock(&UDPPrintlocker);
		cout<<"Can't creat raw icmp socket !"<<endl;
		pthread_mutex_unlock(&UDPPrintlocker);
	}
    //设置套接字UDPSock选项
	on = 1;
    ret = setsockopt(UDPSock,IPPROTO_IP,IP_HDRINCL,&on,sizeof(on)); 
	if (ret < 0) 
	{
		pthread_mutex_lock(&UDPPrintlocker);
		cout<<"Can't set raw socket !"<<endl;
		pthread_mutex_unlock(&UDPPrintlocker);
	}
	//设置UDP套接字地址
	memset(&UDPScanHostAddr,0,sizeof(UDPScanHostAddr));
	UDPScanHostAddr.sin_family = AF_INET;
	UDPScanHostAddr.sin_addr.s_addr = inet_addr(&HostIP[0]);
	UDPScanHostAddr.sin_port = htons(HostPort); 
	//填充UDP数据包
	memset(packet, 0x00, sizeof(packet)); 
	ip = (struct iphdr *)packet; 
    udp = (struct udphdr *)(packet + sizeof(struct iphdr)); 
    pseudo = (struct pseudohdr *)(packet + sizeof(struct iphdr) - sizeof(struct pseudohdr)); 
	//填充UDP头
	udp->source = htons(LocalPort);                 
    udp->dest = htons(HostPort);           
    udp->len = htons(sizeof(struct udphdr));
    udp->check = 0;  
	//填充UDP伪头部，用于计算校验和
	pseudo->saddr = LocalHostIP; 
    pseudo->daddr = inet_addr(&HostIP[0]); 
    pseudo->useless = 0; 
    pseudo->protocol = IPPROTO_UDP; 
    pseudo->length = udp->len;
	udp->check = in_cksum((u_short *)pseudo,sizeof(struct udphdr)+sizeof(struct pseudohdr));
	//填充IP头
	ip->ihl = 5; 
	ip->version = 4; 
	ip->tos = 0x10; 
	ip->tot_len = sizeof(packet); 
	ip->frag_off = 0; 
	ip->ttl = 69; 
	ip->protocol = IPPROTO_UDP; 
	ip->check = 0; 
	ip->saddr = inet_addr("192.168.1.168"); 
	ip->daddr = inet_addr(&HostIP[0]);
	//发送UDP数据包
	n = sendto(UDPSock, packet, ip->tot_len, 0, (struct sockaddr *)&UDPScanHostAddr, sizeof(UDPScanHostAddr)); 
	if (n<0) 
	{
		pthread_mutex_lock(&UDPPrintlocker);
		cout<<"Send message to Host Failed !"<<endl;
		pthread_mutex_unlock(&UDPPrintlocker);
	}
	//设置套接字UDPSock为非阻塞模式
	if(fcntl(UDPSock, F_SETFL, O_NONBLOCK) == -1) 
	{
		pthread_mutex_lock(&UDPPrintlocker);
        cout<<"Set socket in non-blocked model fail !"<<endl;
		pthread_mutex_unlock(&UDPPrintlocker);
	}
    //接收ICMP相应数据包循环
	gettimeofday(&TpStart,NULL);
	do 
	{
		//接收ICMP数据包
        n = read(UDPSock, (struct ipicmphdr *)&hdr, sizeof(hdr));

		if(n > 0)
		{
			//判断ICMP数据包的源地址是否等于目标主机地址，code字段和
			//type字段的值是否是3
			if((hdr.ip.saddr == inet_addr(&HostIP[0])) && (hdr.icmp.code == 3) && (hdr.icmp.type == 3))
			{
				pthread_mutex_lock(&UDPPrintlocker);
				cout<<"Host: "<<HostIP<<" Port: "<<HostPort<<" closed !"<<endl;
				pthread_mutex_unlock(&UDPPrintlocker);
				break;
			}
		}
		//判断等待时间是否超过3秒
		gettimeofday(&TpEnd,NULL);
		TimeUse=(1000000*(TpEnd.tv_sec-TpStart.tv_sec)+(TpEnd.tv_usec-TpStart.tv_usec))/1000000.0;
		if(TimeUse<3)
		{
			continue;
		}
		else
		{
			pthread_mutex_lock(&UDPPrintlocker);
			cout<<"Host: "<<HostIP<<" Port: "<<HostPort<<" open !"<<endl;
			pthread_mutex_unlock(&UDPPrintlocker);
			break;
		}
	} 
	while(true);
	//关闭套接字
	close(UDPSock);
	delete p;
}
void* Thread_UDPScan(void* param)
{
    struct UDPThrParam *p;

	string HostIP;
	unsigned BeginPort,EndPort,TempPort,LocalPort,LocalHostIP;

	pthread_t subThreadID;
	pthread_attr_t attr;
	int ret;
    
	p=(struct UDPThrParam*)param;
	HostIP = p->HostIP;
	BeginPort = p->BeginPort;
	EndPort = p->EndPort;
	LocalHostIP = p->LocalHostIP;
	//遍历端口，逐次扫描
	LocalPort = 1024;

	for (TempPort=BeginPort;TempPort<=EndPort;TempPort++) 
	{
        UDPScanHostThrParam *pUDPScanHostParam = new UDPScanHostThrParam;
        pUDPScanHostParam->HostIP = HostIP;
		pUDPScanHostParam->HostPort = TempPort;
        pUDPScanHostParam->LocalPort = TempPort + LocalPort;
        pUDPScanHostParam->LocalHostIP = LocalHostIP;
		UDPScanHost(pUDPScanHostParam);
	}
    cout<<"UDP Scan thread exit !"<<endl;
	pthread_exit(NULL);
}

