#include "TCPFINScan.h"

void* Thread_TCPFINHost(void* param)
{
	struct TCPFINHostThrParam *p;
	string HostIP,SrcIP,DstIP,LocalIP;
	unsigned HostPort,LocalPort,SrcPort,DstPort,LocalHostIP;

	struct sockaddr_in FINScanHostAddr,FromAddr,FinRevAddr;
	struct in_addr in_LocalhostIP;

	int FinSock,FinRevSock;
	int len,FromAddrLen; 

	char sendbuf[8192]; 
	char recvbuf[8192];
    
	struct timeval TpStart,TpEnd;
	float TimeUse;
	//获得目标主机的IP地址和扫描端口号，以及本机的IP地址和端口
    p=(struct TCPFINHostThrParam*)param;
    HostIP = p->HostIP;
	HostPort = p->HostPort;
	LocalPort = p->LocalPort;
	LocalHostIP = p->LocalHostIP;
	//设置TCP SYN扫描的套接字地址
	memset(&FINScanHostAddr,0,sizeof(FINScanHostAddr));
	FINScanHostAddr.sin_family = AF_INET;
	FINScanHostAddr.sin_addr.s_addr = inet_addr(&HostIP[0]);
	FINScanHostAddr.sin_port = htons(HostPort); 
    //创建套接字
	FinSock=socket(PF_INET, SOCK_RAW, IPPROTO_TCP); 
	if ( FinSock<0 ) 
	{
		pthread_mutex_lock(&TCPFinPrintlocker);
		cout<<"Can't creat raw socket !"<<endl;
		pthread_mutex_unlock(&TCPFinPrintlocker);
	}
	FinRevSock=socket(PF_INET, SOCK_RAW, IPPROTO_TCP); 
	if ( FinRevSock<0 ) 
	{
		pthread_mutex_lock(&TCPFinPrintlocker);
		cout<<"Can't creat raw socket !"<<endl;
		pthread_mutex_unlock(&TCPFinPrintlocker);
	}
    //填充TCP伪头部，用于计算校验和
    struct pseudohdr *ptcph=(struct pseudohdr*)sendbuf; 
    struct tcphdr *tcph=(struct tcphdr*)(sendbuf+sizeof(struct pseudohdr)); 

	ptcph->saddr = LocalHostIP; 
    ptcph->daddr = inet_addr(&HostIP[0]); 
    ptcph->useless = 0; 
    ptcph->protocol = IPPROTO_TCP; 
    ptcph->length = htons(sizeof(struct tcphdr));


    tcph->th_sport=htons(LocalPort);  
    tcph->th_dport=htons(HostPort); 
    tcph->th_seq=htonl(123456); 
    tcph->th_ack=0; 
    tcph->th_x2=0; 
    tcph->th_off=5; 
    tcph->th_flags=TH_FIN; 
    tcph->th_win=htons(65535); 
    tcph->th_sum=0; 
    tcph->th_urp=0; 
	tcph->th_sum=in_cksum((unsigned short*)ptcph, 20+12); 
    //发送TCP FIN数据包
	len=sendto(FinSock, tcph, 20, 0, (struct sockaddr *)&FINScanHostAddr, sizeof(FINScanHostAddr)); 
	if( len < 0) 
	{
		pthread_mutex_lock(&TCPFinPrintlocker);
		cout<<"Send TCP FIN Packet error !"<<endl;
		pthread_mutex_unlock(&TCPFinPrintlocker);		
	}
	//接收目标主机的TCP响应数据包
	if(fcntl(FinRevSock, F_SETFL, O_NONBLOCK) == -1) 
	{
		pthread_mutex_lock(&TCPFinPrintlocker);
        cout<<"Set socket in non-blocked model fail !"<<endl;
		pthread_mutex_unlock(&TCPFinPrintlocker);
	}
	
    FromAddrLen = sizeof(struct sockaddr_in);
	gettimeofday(&TpStart,NULL);
	do 
	{
		len = recvfrom(FinRevSock,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&FromAddr,(socklen_t*)&FromAddrLen);
		if(len > 0)
		{
			SrcIP = inet_ntoa(FromAddr.sin_addr);
			//判断响应数据包的源地址是否等于目标主机地址，目的地址是否等于本机
			//IP地址，源端口是否等于被扫描端口，目的端口是否等于本机端口号
			if(SrcIP == HostIP)
			{
				struct ip *iph=(struct ip *)recvbuf; 
				int i=iph->ip_hl*4; 
				struct tcphdr *tcph=(struct tcphdr *)&recvbuf[i]; 
				SrcIP = inet_ntoa(iph->ip_src);       //source IP in TCP response packet
				DstIP = inet_ntoa(iph->ip_dst);       //destination IP in TCP response packet
				in_LocalhostIP.s_addr = LocalHostIP;
				LocalIP = inet_ntoa(in_LocalhostIP);  //LocalhostIP
				unsigned SrcPort = ntohs(tcph->th_sport);    //source port in TCP response packet
				unsigned DstPort = ntohs(tcph->th_dport);    //destination port in TCP response packet
				if(HostIP == SrcIP && LocalIP == DstIP && SrcPort == HostPort && DstPort == LocalPort)
				{
					if (tcph->th_flags == 0x14) 
					{  
						pthread_mutex_lock(&TCPFinPrintlocker);
						cout<<"Host: "<<SrcIP<<" Port: "<<ntohs(tcph->th_sport)<<" closed !"<<endl;
						pthread_mutex_unlock(&TCPFinPrintlocker);
					}				
				}
				break;
			}
		}
		gettimeofday(&TpEnd,NULL);
		TimeUse=(1000000*(TpEnd.tv_sec-TpStart.tv_sec)+(TpEnd.tv_usec-TpStart.tv_usec))/1000000.0;
		if(TimeUse<5)
			continue;
		else
		{
			pthread_mutex_lock(&TCPFinPrintlocker);
			cout<<"Host: "<<HostIP<<" Port: "<<HostPort<<" open !"<<endl;
			pthread_mutex_unlock(&TCPFinPrintlocker);
			break;
		}
	}
    while(true);
    	delete p;
	close(FinSock);
	close(FinRevSock);
	pthread_mutex_lock(&TCPFinScanlocker);
    TCPFinThrdNum--;
	pthread_mutex_unlock(&TCPFinScanlocker);
}
void* Thread_TCPFinScan(void* param)
{
    struct TCPFINThrParam *p;

	string HostIP;
	unsigned BeginPort,EndPort,TempPort,LocalPort,LocalHostIP;

	pthread_t listenThreadID,subThreadID;
	pthread_attr_t attr,lattr;
	int ret;
	//获得目标主机的IP地址和扫描的起始端口号，终止端口号，以及本机的IP地址
	p=(struct TCPFINThrParam*)param;
	HostIP = p->HostIP;
	BeginPort = p->BeginPort;
	EndPort = p->EndPort;
	LocalHostIP = p->LocalHostIP;
	//循环遍历扫描端口
	TCPFinThrdNum = 0;
	LocalPort = 1024;

	for (TempPort=BeginPort;TempPort<=EndPort;TempPort++) 
	{
		//设置子线程参数
        struct TCPFINHostThrParam *pTCPFINHostParam = new TCPFINHostThrParam;
        pTCPFINHostParam->HostIP = HostIP;
		pTCPFINHostParam->HostPort = TempPort;
        pTCPFINHostParam->LocalPort = TempPort + LocalPort;
		pTCPFINHostParam->LocalHostIP = LocalHostIP;
		//将子线程设置为分离状态
		pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		//创建子线程
		ret=pthread_create(&subThreadID,&attr,Thread_TCPFINHost,pTCPFINHostParam);
		if (ret==-1) 
			cout<<"Can't create the TCP FIN Scan Host thread !"<<endl;
		pthread_attr_destroy(&attr);
		pthread_mutex_lock(&TCPFinScanlocker);
        TCPFinThrdNum++;
		pthread_mutex_unlock(&TCPFinScanlocker);
		while (TCPFinThrdNum>100)
			sleep(3);
	}
	while (TCPFinThrdNum != 0)
		sleep(1);
    cout<<"TCP FIN scan thread exit !"<<endl;
	pthread_exit(NULL);
}

