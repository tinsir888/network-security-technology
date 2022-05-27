#include "TCPSYNScan.h"
void* Thread_TCPSYNHost(void* param)
{
	//cout<<"create a Thread_TCPSYNHost!"<<endl;
	struct TCPSYNHostThrParam *p;
	string HostIP;
	unsigned HostPort,LocalPort,LocalHostIP;
	int SynSock;
	int len; 
	char sendbuf[8192]; 
	char recvbuf[8192];
	struct sockaddr_in SYNScanHostAddr;
	//获得目标主机的IP地址和扫描端口号，以及本机的IP地址和端口
    p=(struct TCPSYNHostThrParam*)param;
    HostIP = p->HostIP;
	HostPort = p->HostPort;
	LocalPort = p->LocalPort;
	LocalHostIP = p->LocalHostIP;
	//设置TCP SYN扫描的套接字地址
	memset(&SYNScanHostAddr,0,sizeof(SYNScanHostAddr));
	SYNScanHostAddr.sin_family = AF_INET;
	SYNScanHostAddr.sin_addr.s_addr = inet_addr(&HostIP[0]);
	SYNScanHostAddr.sin_port = htons(HostPort);  
	//创建套接字
	SynSock=socket(PF_INET, SOCK_RAW, IPPROTO_TCP); 
	if ( SynSock<0 ) 
	{
		pthread_mutex_lock(&TCPSynPrintlocker);
		cout<<"Can't creat raw socket !"<<endl;
		pthread_mutex_unlock(&TCPSynPrintlocker);
	}
	//填充TCP SYN数据包
    struct pseudohdr *ptcph=(struct pseudohdr*)sendbuf; 
    struct tcphdr *tcph=(struct tcphdr*)(sendbuf+sizeof(struct pseudohdr)); 
	//填充TCP伪头部，用于计算校验和
	ptcph->saddr = LocalHostIP; 
    ptcph->daddr = inet_addr(&HostIP[0]); 
    ptcph->useless = 0; 
    ptcph->protocol = IPPROTO_TCP; 
    ptcph->length = htons(sizeof(struct tcphdr));
	//填充TCP头
    tcph->th_sport=htons(LocalPort);  
    tcph->th_dport=htons(HostPort); 
    tcph->th_seq=htonl(123456); 
    tcph->th_ack=0; 
    tcph->th_x2=0; 
    tcph->th_off=5; 
    tcph->th_flags=TH_SYN;//TCP头flags字段的SYN位置1
    tcph->th_win=htons(65535); 
    tcph->th_sum=0; 
    tcph->th_urp=0; 
	tcph->th_sum=in_cksum((unsigned short*)ptcph, 20+12); 
    //发送TCP SYN数据包
	len=sendto(SynSock, tcph, 20, 0, (struct sockaddr *)&SYNScanHostAddr, sizeof(SYNScanHostAddr)); 
	if( len < 0) 
	{
		pthread_mutex_lock(&TCPSynPrintlocker);
		cout<<"Send TCP SYN Packet error !"<<endl;
		pthread_mutex_unlock(&TCPSynPrintlocker);		
	}
	//接收目标主机的TCP响应数据包
	len=read(SynSock, recvbuf, 8192); 
	if(len <= 0)
	{
		pthread_mutex_lock(&TCPSynPrintlocker);
		cout<<"Read TCP SYN Packet error !"<<endl;
		pthread_mutex_unlock(&TCPSynPrintlocker);
	}
	else
	{
		//判断响应数据包的源地址是否等于目标主机地址，目的地址是否等于本机
		//IP地址，源端口是否等于被扫描端口，目的端口是否等于本机端口号
		struct ip *iph=(struct ip *)recvbuf; 
        int i=iph->ip_hl*4; 
        struct tcphdr *tcph=(struct tcphdr *)&recvbuf[i]; 

		string SrcIP = inet_ntoa(iph->ip_src);       //source IP in TCP response packet
		string DstIP = inet_ntoa(iph->ip_dst);       //destination IP in TCP response packet
		struct in_addr in_LocalhostIP;
		in_LocalhostIP.s_addr = LocalHostIP;
		string LocalIP = inet_ntoa(in_LocalhostIP);  //LocalhostIP

		unsigned SrcPort = ntohs(tcph->th_sport);    //source port in TCP response packet
		unsigned DstPort = ntohs(tcph->th_dport);    //destination port in TCP response packet
		if(HostIP == SrcIP && LocalIP == DstIP && SrcPort == HostPort && DstPort == LocalPort)
		{
			if (tcph->th_flags == TH_SYN || tcph->th_flags == TH_ACK) //judge whether is SYN|ACK
			{  
				pthread_mutex_lock(&TCPSynPrintlocker);
				cout<<"Host: "<<SrcIP<<" Port: "<<ntohs(tcph->th_sport)<<" closed !"<<endl;
				pthread_mutex_unlock(&TCPSynPrintlocker);
			}
			if (tcph->th_flags == TH_RST) //judge whether is RST
			{
				pthread_mutex_lock(&TCPSynPrintlocker);
				cout<<"Host: "<<SrcIP<<" Port: "<<ntohs(tcph->th_sport)<<" open !"<<endl;
				pthread_mutex_unlock(&TCPSynPrintlocker);
			}		
		}

	}
	delete p;
	close(SynSock);
	pthread_mutex_lock(&TCPSynScanlocker);
    TCPSynThrdNum--;
	pthread_mutex_unlock(&TCPSynScanlocker);
}

//==============================================================================
void* Thread_TCPSynScan(void* param)
{
    struct TCPSYNThrParam *p;
	string HostIP;
	unsigned BeginPort,EndPort,TempPort,LocalPort,LocalHostIP;
	pthread_t listenThreadID,subThreadID;
	pthread_attr_t attr,lattr;
	int ret;
	//获得目标主机的IP地址和扫描的起始端口号，终止端口号，以及本机的IP地址
	p=(struct TCPSYNThrParam*)param;
	HostIP = p->HostIP;
	BeginPort = p->BeginPort;
	EndPort = p->EndPort;
	LocalHostIP = p->LocalHostIP;
	//循环遍历扫描端口
	TCPSynThrdNum = 0;
	LocalPort = 1024;

	for (TempPort=BeginPort;TempPort<=EndPort;TempPort++) 
	{
		//设置子线程参数
        struct TCPSYNHostThrParam *pTCPSYNHostParam = new TCPSYNHostThrParam;
        pTCPSYNHostParam->HostIP = HostIP;
		pTCPSYNHostParam->HostPort = TempPort;
        pTCPSYNHostParam->LocalPort = TempPort + LocalPort;
		pTCPSYNHostParam->LocalHostIP = LocalHostIP;
		//将子线程设置为分离状态
		pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		//创建子线程
		ret=pthread_create(&subThreadID,&attr,Thread_TCPSYNHost,pTCPSYNHostParam);
		if (ret==-1) 
		{
			cout<<"Can't create the TCP SYN Scan Host thread !"<<endl;
		}
		pthread_attr_destroy(&attr);
		pthread_mutex_lock(&TCPSynScanlocker);
        TCPSynThrdNum++;
		pthread_mutex_unlock(&TCPSynScanlocker);
		while (TCPSynThrdNum>100)
		{
			sleep(3);
		}
	}
	while (TCPSynThrdNum != 0)
	{
		sleep(1);
	}

    cout<<"TCP SYN scan thread exit !"<<endl;
	pthread_exit(NULL);
}

