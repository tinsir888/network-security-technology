#include "TCPConnectScan.h"

void* Thread_TCPconnectHost(void* param)
{
	//变量定义
	struct TCPConHostThrParam *p;
	string HostIP;
	unsigned HostPort;

    int ConSock;
	struct sockaddr_in HostAddr; 
	int ret;
	//获得目标主机的IP地址和扫描端口号
    p=(struct TCPConHostThrParam*)param;
    HostIP = p->HostIP;
	HostPort = p->HostPort;
    //创建流套接字
	ConSock = socket(AF_INET,SOCK_STREAM,0);
	if(ConSock < 0)
	{
		pthread_mutex_lock(&TCPConPrintlocker);
		cout<<"Create TCP connect Socket failed! "<<endl;
		pthread_mutex_unlock(&TCPConPrintlocker);
	}
    //设置连接主机地址
	memset(&HostAddr,0,sizeof(HostAddr));
	HostAddr.sin_family = AF_INET;
	HostAddr.sin_addr.s_addr = inet_addr(&HostIP[0]);
	HostAddr.sin_port = htons(HostPort);
	//连接目标主机
    ret = connect(ConSock,(struct sockaddr*)&HostAddr,sizeof(HostAddr));
	if(ret==-1)//端口关闭
	{
		pthread_mutex_lock(&TCPConPrintlocker);
	    cout<<"Host: "<<HostIP<<" Port: "<<HostPort<<" closed ! "<<endl;
		pthread_mutex_unlock(&TCPConPrintlocker);
	}
	else//端口打开
	{
		pthread_mutex_lock(&TCPConPrintlocker);
        cout<<"Host: "<<HostIP<<" Port: "<<HostPort<<" open ! "<<endl;
		pthread_mutex_unlock(&TCPConPrintlocker);
	}
	//退出吸纳成
	delete p;
    close(ConSock);//关闭套接字
	
	pthread_mutex_lock(&TCPConScanlocker);
        TCPConThrdNum--;
	pthread_mutex_unlock(&TCPConScanlocker);

}
void* Thread_TCPconnectScan(void* param)
{
	//变量定义
	//获得扫描的目标主机IP，起始端口， 终止端口
    struct TCPConThrParam *p;
	string HostIP;
	unsigned BeginPort,EndPort,TempPort;
	
	pthread_t subThreadID;
	pthread_attr_t attr;
	int ret;
	p=(struct TCPConThrParam*)param;
	HostIP = p->HostIP;
	BeginPort = p->BeginPort;
	EndPort = p->EndPort;
    TCPConThrdNum = 0;
	for (TempPort=BeginPort;TempPort<=EndPort;TempPort++) 
	{
		//设置子线程参数
        TCPConHostThrParam *pConHostParam = new TCPConHostThrParam;
        pConHostParam->HostIP = HostIP;
		pConHostParam->HostPort = TempPort;
		//子线程设为分离状态
		pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
        //创建connect目标主机指定的端口子线程
		ret=pthread_create(&subThreadID,&attr,Thread_TCPconnectHost,pConHostParam);
		if (ret==-1) 
		{
			cout<<"Can't create the TCP connect Host thread !"<<endl;
		}
		pthread_attr_destroy(&attr);
		
		pthread_mutex_lock(&TCPConScanlocker);
        TCPConThrdNum++;
		pthread_mutex_unlock(&TCPConScanlocker);

		while (TCPConThrdNum>100)
		{
			sleep(3);
		}
	}
	//等待所有子线程数为0
	while (TCPConThrdNum != 0)
	{
		sleep(1);
	}
    cout<<"TCP Connect Scan thread exit !"<<endl;
	pthread_exit(NULL);
}
