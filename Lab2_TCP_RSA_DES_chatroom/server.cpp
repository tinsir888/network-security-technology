#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include "./include/cli_ser.h"
#include "./include/define_code.h"
#include "./include/des.h"
#include "./include/rsa.h"

using namespace std;

struct client_info {
    int fd;
    sockaddr_in* client_addr;
    RSA_key_pair key_pair;
    string des_key = "";
    bool key_aggrement = true;
};

client_info *cinfo;

int send_msg_to_client(client_info *cinfo, int key_aggrement = DATA_EXCHANGE, string key = "") {
    char server_msg[message_size];
    memset(server_msg, 0, sizeof(server_msg));
    
    if (key_aggrement == KEY_AGREE_SEND_KEY) { // 发送RSA公钥对
        for (int j = 0; j < key.size(); ++j) // 加密结果string转char[]
            server_msg[j] = key[j];
        server_msg[key.size()] = '\0';
    }
    else { // 发送普通数据信息
        if (cinfo->des_key == "") {
            printf("server miss des_key!\n");
            return -1;//DES key 信息丢失
        }
        if (key_aggrement == DATA_EXCHANGE) {
            int count = 0, sum = 0;
            while((count = read(STDIN_FILENO, server_msg, message_size)) > 0) {
                sum += count;
            }
            if (count == -1 && errno != EAGAIN){
                printf("server send_msg_to_client() read error!\n");
                return -1; //发送信息给客户端读取错误
            }
            server_msg[sum - 1] = '\0'; // 将末尾多余的 '\n' 置为 '\0'
            cout << "Send message to <" << inet_ntoa(cinfo->client_addr->sin_addr);
            cout << ">: " << server_msg << endl;
        }
        else strcpy(server_msg, "GetKeySuccess");
        // 秘钥协商结束，服务器给客户端发送协商成功的信息
        if (strcmp(server_msg, "quit") == 0)
            return 2000;//输入了 quit，退出

        string encry_res; // 加密结果
        CDesOperate des;
        if (des.encryption(server_msg, cinfo->des_key, encry_res) != 0) { // 加密
            printf("encry error!\n");
            return -1; //加密错误
        }
        memset(server_msg, '\0', message_size);
        for (int j = 0; j < encry_res.length(); ++j) { // string 2 char[]
            server_msg[j] = encry_res[j];
        }
        server_msg[encry_res.size()] = '\0';
    }

    if (send(cinfo->fd, server_msg, strlen(server_msg), 0) <= 0) { 
        // send()将服务器的消息发给客户端
        printf("server send error!\n");
        return -1;
    }
    
    return 0;
}

int recv_msg_from_client(client_info *cinfo) {
    char* client_addr = inet_ntoa(cinfo->client_addr->sin_addr);

    char client_msg[message_size];
    memset(client_msg, 0, sizeof(client_msg));
    int cli_len = 0, count = 0;
    while((count = recv(cinfo->fd, client_msg, sizeof(client_msg), 0)) > 0)
        // recv将收到的信息存在client_msg中
        // ET模式（边缘触发），需要一次将缓存中的数据通过while循环读取完毕
        cli_len += count;
    client_msg[cli_len] = '\0';

    int key_aggrement = DATA_EXCHANGE;    
    // 截取消息头部，判断是密钥协商阶段还是互通消息阶段，并截掉头部
    if (client_msg[0] == 'k' && client_msg[1] == 'e' && client_msg[2] == 'y') {
        key_aggrement = KEY_AGREE_SEND_KEY; 
        strncpy(client_msg, client_msg + 4, sizeof(client_msg));
        CRsaOperate rsa;
        cinfo->des_key = rsa.server_decry(client_msg, cinfo->key_pair.private_key_d, cinfo->key_pair.n);
    }
    else if (client_msg[0] == 'd' && client_msg[1] == 'a' && client_msg[2] == 't' && client_msg[3] == 'a') {
        strncpy(client_msg, client_msg + 5, sizeof(client_msg));
    }
    else {
        //printf("server recv_msg_from_client() header format error!\n");
        return -1;
    }

    if (key_aggrement == DATA_EXCHANGE) { // 客户端和服务器端对话
        if (cinfo->des_key == "") {
            printf("server miss des_key!\n");
            return -1;// 服务器端丢失 DES 密钥
        }
        string decry_res = "";
        CDesOperate des;
        if (des.decryption(client_msg, cinfo->des_key, decry_res) != 0) { //解密
            printf("decry error!\n");
            return -1;
        }
        cout << "Receive message from <" << client_addr << ">: " << decry_res << endl;
    }
    return 0;
}

int server() {
    struct epoll_event events[max_line]; // epoll数据结构

    struct sockaddr_in server_addr; // 一个将来与套接字绑定的结构体
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = chat_port; // 从服务器的PORT端口接收客户端信息
    server_addr.sin_family = AF_INET; // 协议簇，AF_INET表示TCP/IP协议
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 指定接收的信息来自于某个IP，这里随意

    int fd_skt = socket(AF_INET, SOCK_STREAM, 0); // socket函数新建套接字fd_skt
    if (fd_skt < 0) {
        printf("server socket error!\n");
        return -1;
    }
     
    int fd_ep = epoll_create(epoll_size);  // 创建epoll的句柄，fd_ep是epoll的文件描述符
    if (fd_ep < 0) { // 若成功返回一个大于0的值，表示 epoll 实例；出错返回-1
        printf("server epoll_create error!\n");
        return -1;
    }

    struct epoll_event ep_event, ep_input; // 针对监听的fd_skt，创建2个epollevent
    fcntl(fd_skt, F_SETFL, O_NONBLOCK); // 设置非阻塞
    ep_event.data.fd = fd_skt;
    ep_event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, fd_skt, &ep_event) < 0) { // 注册epoll事件
        // 参数3：需要监听的fd，参数4：告诉内核需要监听什么事
        printf("server epoll_ctl-1 error!\n");
        return -1;
    }

    // 给fd_ep绑定监听标准输入的文件描述符（为了实现全双工）
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    ep_input.data.fd  = STDIN_FILENO;
    ep_input.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, STDIN_FILENO, &ep_input) < 0) {
        printf("server epoll_ctl-2 error!\n");
        return -1;
    }

    if (bind(fd_skt, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) { //bind绑定套接字与serverAddr
        printf("server bind error!\n");
        return -1;
    }
    
    if (listen(fd_skt, max_line) < 0) { // listen监听套接字fd_skt
        printf("server listen error!\n");
        return -1;
    }
    cout << "------ server start ------" << endl;
    cout << "Listening..." << endl;
    cin.ignore(1024,'\n');
    while(1) {
        int event_num = epoll_wait(fd_ep, events, max_line, epoll_timeout);   
        //参数2：epoll将发生的事件复制到events数组中。events不可以是空指针，内核只负责把数据复制到数组中，不会在用户态中分配内存，效率很高。
        //参数3: 返回的events的最大个数，如果最大个数大于实际触发的个数，则下次epoll_wait的时候仍然可以返回
        //返回值：大于0表示事件的个数；等于0表示超时；小于0表示出错。
        
        for (int i = 0; i < event_num; ++i) {
            if (events[i].data.fd == fd_skt) { // 有新的连接
                sockaddr_in* client_addr = new sockaddr_in();
                socklen_t length = sizeof(client_addr);
                int fd_client = accept(fd_skt, (struct sockaddr*)client_addr, &length);
                // accept接收连接请求：程序在此阻塞直至产生第一次握手
                // 接收到的信息存于第二第三个参数中
                // 返回值是新的文件描述符，用于后续read/recv和write/send调用
                if (fd_client < 0) {
                    printf("server accept err");
                    continue;
                }
                // 生成RSA公钥/私钥对
                RSA_key_pair key_pair;
                CRsaOperate rsa;
                while(1) // 可能生成失败（由于采用随机数生成），循环直到生成成功
                    if(rsa.RSA_key_pair_gen(key_pair) == 0)
                        break;

                cinfo = new client_info();
                cinfo->fd = fd_client;
                cinfo->client_addr = client_addr;
                cinfo->key_pair = key_pair;

                fcntl(fd_client, F_SETFL, O_NONBLOCK);
                struct epoll_event new_client_event;
                new_client_event.events = EPOLLIN | EPOLLET;
                new_client_event.data.ptr = cinfo; // new_client_event.data是union类型
                
                // 注册epoll事件，让epoll监听这个客户端发来的消息
                if (epoll_ctl(fd_ep, EPOLL_CTL_ADD, fd_client, &new_client_event) < 0) { 
                    printf("server key_aggrement() epoll_ctl-1 error!\n");
                    return -1;
                }

                // 将公钥对发给客户端
                if (send_msg_to_client(cinfo, KEY_AGREE_SEND_KEY, key_pair.en) != 0) {
                    printf("server key_aggrement() send_msg_to_client-1 error!\n");
                    return -1;
                }

                cout << "server: got connection from " << inet_ntoa((*client_addr).sin_addr)
                     << ", port " << chat_port
                     << ", socket " << fd_client << endl;
            }
            else if (events[i].events == EPOLLIN) { // 接收到数据，读socket
                if (events[i].data.fd == STDIN_FILENO) { // 标准输入
                    int key_aggrement = DATA_EXCHANGE;
                    if (int code = send_msg_to_client(cinfo, key_aggrement) != 0) { // 给客户端发消息
                        if (code == 2000) { // 服务器选择结束当前对话
                            close(cinfo->fd);
                        }
                        continue;
                    }
                }
                else { // TCP连接发来的数据
                    client_info *cinfo = (client_info*)events[i].data.ptr;
                    if (cinfo == NULL) {
                        printf("server cinfo NULL!\n");
                        continue;
                    }
                    if (cinfo->key_aggrement) { // 密钥协商阶段
                        if (recv_msg_from_client(cinfo) != 0) { // 收到使用公钥加密的DES密钥
                            printf("server recv_msg_from_client error!\n");
                            return -1;
                        }
                        if (send_msg_to_client(cinfo, KEY_AGREE_CONFIRM) != 0) {
                            // 将成功得到DES秘钥的消息发给客户端
                            printf("server send_msg_to_client-2 error!\n");
                            return -1;
                        }
                        cinfo->key_aggrement = false; // 密钥协商结束
                        cout << "Begin to chat..." << endl;
                    }
                    else // 普通数据交互阶段
                        recv_msg_from_client(cinfo); // 接收客户端消息
                    
                }
            }
        }
    }
    cout << "------ server end ------" << endl;
    close(fd_skt);
    return 0;
}