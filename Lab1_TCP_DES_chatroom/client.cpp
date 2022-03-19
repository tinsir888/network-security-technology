#include "generalheader.h"
int client() {
    CDesOperate des; // class des
    char client_msg[max_msg_len], server_msg[max_msg_len];

    struct sockaddr_in server_addr; //struct bind with socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = (tcp_port);//set port
    server_addr.sin_family = AF_INET;//use TCP/IP IPv4 protocol 
    cout << "Please input the server address: (type 1 connect to local server)" << endl;
    string input_saddr;
    cin >> input_saddr;
    if (strcmp(input_saddr.c_str(), "1") == 0) {
        input_saddr=default_server_addr;
        if(inet_pton(AF_INET, default_server_addr, (void *)&server_addr.sin_addr) <= 0) {
            printf("client inet_pton error!\n");
            return -1;
        }
    }
    else {
        if(inet_pton(AF_INET, input_saddr.c_str(), (void *)&server_addr.sin_addr) <= 0) {
            printf("client inet_pton error!\n");
            return -1;
        }
    }
    //1. socket: set up new tcp_socket
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0) {
        printf("client socket error!\n");
        return -1;
    }
    //2. connect request to server
    if (connect(tcp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	    printf("client connect error!\n");       
        return -1;
    }
    cout << "Connect Success!" << endl << "Begin to chat with server: " << inet_ntoa(server_addr.sin_addr) << "..." << endl;
    cin.ignore(1024,'\n');//ignore \n in cin buffer
    while(1) {
        memset(client_msg, 0, sizeof(client_msg));
        memset(server_msg, 0, sizeof(server_msg));
	    cin.getline(client_msg, sizeof(client_msg));
        if (strcmp(client_msg, "quit") == 0) {
            break;
        }
        cout << "Send message to <" << inet_ntoa(server_addr.sin_addr) << ">:" << endl;
        cout << client_msg << endl;
        string encry_res;
        if (des.encry_operation(client_msg, DES_KEY, encry_res) != 0) {
            printf("encry error!\n");
            return -1;
        }
        memset(client_msg, '\0', max_msg_len);
        for (int i = 0; i < encry_res.length(); i++) {
            client_msg[i] = encry_res[i];
        }
        client_msg[encry_res.size()] = '\0';
        //3. send msg to server
        if (send(tcp_socket, client_msg, strlen(client_msg), 0) < 0) {
            printf("client send error!\n");
            return -1;
        }
        int server_msg_len = recv(tcp_socket, server_msg, sizeof(server_msg),0);
        //recv msg from server
        if(server_msg_len <= 0) { 
            printf("client recv error!\n");
            break;
        }
        server_msg[server_msg_len] = '\0';
        string decry_res = "";
        if (des.decry_operation(server_msg, DES_KEY, decry_res) != 0) { //解密
            printf("decry error!\n");
            return -1;
        }

        cout << "Receive message from <" << inet_ntoa(server_addr.sin_addr) << ">:" << endl; 
        cout << decry_res << endl;
    }
    cout << "————————client end————————" << endl;
    close(tcp_socket);//recv err: SUCCESS, end connection
    return 1;
}
