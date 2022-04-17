#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include "./include/cli_ser.h"
#include "./include/define_code.h"
#include "./include/des.h"
#include "./include/help_func.h"
#include "./include/rsa.h"

using namespace std;

string des_key = "";

int send_msg_to_server(sockaddr_in server_addr, int fd_skt, int key_aggrement = DATA_EXCHANGE, string msg = "") {
    char client_msg[message_size];
    memset(client_msg, 0, sizeof(client_msg));

    if (key_aggrement == KEY_AGREE_SEND_KEY) { // 密钥协商
        msg = ":" + msg;
        msg = "key" + msg; // 添加头部信息
        for (int i = 0; i < msg.length(); i++) { // string转char[]
            client_msg[i] = msg[i];
        }
        client_msg[msg.size()] = '\0';
    }
    else { // 发送普通对话
        if (des_key == "") {
            printf("client miss des_key!\n");
            return -1;
        }
        int count = 0, sum = 0;
        while((count = read(STDIN_FILENO, client_msg, message_size)) > 0) {
            sum += count;
        }
        if (count == -1 && errno != EAGAIN){
            printf("server send_msg_to_client() read error!\n");
            return -1;
        }
        
        client_msg[sum - 1] = '\0'; // 将末尾多余的\n置为\0

        if (strcmp(client_msg, "quit") == 0) {
            return 2000;
        }
        
        cout << "Send message to <" << inet_ntoa(server_addr.sin_addr) << ">: " << client_msg << endl;

        string encry_res; // 加密结果
        CDesOperate des;
        if (des.encryption(client_msg, des_key, encry_res) != 0) { // 加密
            printf("client send_msg_to_server() err");
            return -1;
        }
        
        encry_res = ":" + encry_res;
        encry_res = "data" + encry_res; // 加密完毕后添加头部信息

        memset(client_msg, '\0', message_size);
        for (int i = 0; i < encry_res.length(); i++) // 加密结果string转char[]
            client_msg[i] = encry_res[i];
        client_msg[encry_res.size()] = '\0';
    }
    
    if (send(fd_skt, client_msg, strlen(client_msg), 0) < 0) { // send，客户端向服务端发消息
        printf("client send_msg_to_server() - send() error!\n");
        return -1;
    }
    return 0;
}

int recv_msg_from_server(sockaddr_in server_addr, int fd_skt, int key_aggrement = DATA_EXCHANGE, string *str_send_msg = NULL) {
    char server_msg[message_size];
    memset(server_msg, 0, sizeof(server_msg));
    int ser_len = recv(fd_skt, server_msg, sizeof(server_msg), 0); // recv，接收服务器发来的消息
    if(ser_len <= 0) { 
        printf("client recv_msg_from_server() - recv() error!\n");
        return -1;
    }
    server_msg[ser_len] = '\0';

    if (key_aggrement == KEY_AGREE_SEND_KEY) {
        *str_send_msg = server_msg;
    }
    else {
        if (des_key == "") {
            printf("client miss des_key!\n");
            return -1;
        }
        string decry_res = "";
        CDesOperate des;
        if (des.decryption(server_msg, des_key, decry_res) != 0) { //解密
            printf("client recv_msg_from_server() err");
            return -1;
        }
        if (key_aggrement == KEY_AGREE_CONFIRM)
            return 0;
        cout << "Receive message from <" << inet_ntoa(server_addr.sin_addr) << ">: " << decry_res << endl;
    }
    return 0;
}

// 秘钥协商：（1）生成随机DES秘钥（2）使用公钥加密DES密钥并发送给服务端
int key_aggrement(sockaddr_in server_addr, int fd_skt) {
    CRsaOperate rsa;
    CDesOperate des;
    des_key = CDesOperate::des_key_gen();
    string str_send_msg;
    if (recv_msg_from_server(server_addr, fd_skt, KEY_AGREE_SEND_KEY, &str_send_msg) != 0) { // 接收服务器的公钥对
        printf("client key_aggrement()-recv_msg_from_server()-1 error!\n");
        return -1;
    }
    int pos = str_send_msg.find(",", 0);
    ull e = str2uint(str_send_msg.substr(0, pos));
    ull n = str2uint(str_send_msg.substr(pos + 1, str_send_msg.size()));
    string encryDesKey = rsa.client_encry(des_key, e, n);
    
    if (send_msg_to_server(server_addr, fd_skt, KEY_AGREE_SEND_KEY, encryDesKey) != 0) { // 给服务器发送加密过的DES秘钥
        printf("client key_aggrement() error!\n");
        return -1;
    }

    if (recv_msg_from_server(server_addr, fd_skt, KEY_AGREE_CONFIRM) != 0) { // 接收服务器的确认DES接收成功
        printf("client key_aggrement()-recv_msg_from_server()-2 error!\ns");
        return -1;
    }
    return 0;
}

int client() {
    cout << "------ client start ------" << endl;
    struct epoll_event events[max_line]; // epoll数据结构

    sockaddr_in server_addr; // 一个将来与套接字绑定的结构体
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = (chat_port); // 从客户端的PORT端口接收服务器信息
    server_addr.sin_family = AF_INET; // 协议簇，AF_INET表示TCP/IP协议   
    cout << "Please input the server address: (input 1 for default server: 127.0.0.1)" << endl;
    char server_address[20];
    cin >> server_address;

    if (strcmp(server_address, "1") == 0) if(inet_pton(AF_INET, default_server, (void *)&server_addr.sin_addr) <= 0) {
        printf("client inet_pton() error!\n");
        return -1;
    }
    else if(inet_pton(AF_INET, default_server, (void *)&server_addr.sin_addr) <= 0) {
        printf("client inet_pton() error!\n");
        return -1;
    }

    int fd_skt = socket(AF_INET, SOCK_STREAM, 0); // socket函数新建套接字fd_skt
    if (fd_skt < 0) {
        printf("client socket() error!\n");
        return -1;
    }

    if (connect(fd_skt, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) { // connect向服务器发起连接请求
	    printf("client connect() error!\n");       
        return -1;
    }
    if (key_aggrement(server_addr, fd_skt) != 0) // 密钥协商过程
        return -1;
    int fd_ep = epoll_create(epoll_size);  // 创建epoll的句柄，fd_ep是epoll的文件描述符
    if (fd_ep < 0) { // 若成功返回一个大于0的值，表示 epoll 实例；出错返回-1
        printf("client epoll_create error!\n");
        return -1;
    }

    struct epoll_event ep_event, ep_input; // 针对监听的fd_skt，创建2个epollevent
    fcntl(fd_skt, F_SETFL, O_NONBLOCK); // 设置非阻塞
    ep_event.data.fd = fd_skt;
    ep_event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, fd_skt, &ep_event) < 0) { // 注册epoll事件
        // 参数3：需要监听的fd，参数4：告诉内核需要监听什么事
        printf("client epoll_ctl-1 error!\n");
        return -1;
    }

    // 给fd_ep绑定监听标准输入的文件描述符（为了实现全双工）
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    ep_input.data.fd  = STDIN_FILENO;
    ep_input.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, STDIN_FILENO, &ep_input) < 0) {
        printf("client epoll_ctl-2 error!\n");
        return -1;
    }

    cout << "Connect Success! \nBegin to chat..." << endl;
    cin.ignore(1024,'\n'); // 去除上一个cin残留在缓冲区的\n
    while(true) {
        int event_num = epoll_wait(fd_ep, events, max_line, epoll_timeout);  
        for (int i = 0; i < event_num; ++i)
            if (events[i].events == EPOLLIN) { // 接收到数据，读socket
                if (events[i].data.fd == STDIN_FILENO) { // 标准输入
                    int code = send_msg_to_server(server_addr, fd_skt); // 给服务器发消息
                    if (code == 2000) { // 客户端选择结束当前对话
                        cout << "------ client end ------" << endl;
                        close(fd_skt); // 服务器会recv err: 0，从而结束连接
                        return 0;
                    }
                }
                else // TCP连接发来的数据
                    recv_msg_from_server(server_addr, fd_skt); // 接收服务器消息 
            }
    }
}
