#include "generalheader.h"

int server() {
    CDesOperate des;// des class

    char client_msg[max_msg_len], server_msg[max_msg_len];

    struct sockaddr_in server_addr; //struct bind with socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = (tcp_port); //set port
    server_addr.sin_family = AF_INET; //use TCP/IP IPv4 protocol
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //1. socket: set up new tcp_socket
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0) {
        printf("server socket error!\n");
        return -1;
    }
    //2. bind socket and server_addr
    if (bind(tcp_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
        printf("server bind error!\n");
        return -1;
    }
    //3. listen tcp_socket
    if (listen(tcp_socket, max_line) < 0) {
        printf("server listen error!\n");
        return -1;
    }
    while(1) {
        cout << "Listening..." << endl;
        struct sockaddr_in client_addr;
        int dec_client_addr[4];
        socklen_t length = sizeof(client_addr);
        int cur_client = accept(tcp_socket, (struct sockaddr*)&client_addr, &length);
        /*
        4. accept link request: program wait here until first shake hand
        recv msg store in 2nd and 3rd params
        */
        if (cur_client < 0) {
            printf("server accept error!\n");
            return -1;
        }
        cout << "server: get connection from:";
        cout << inet_ntoa(client_addr.sin_addr) << endl;
        cout << "port:" << tcp_port << endl;
        cout << "socket:" << cur_client << endl;
        cin.ignore(1024,'\n'); //ignore \n in cin buffer
        while(1) { //chat with current client
            memset(client_msg, 0, sizeof(client_msg));
            memset(server_msg, 0, sizeof(server_msg));
            //5. recv client msg store to client_msg
            int client_msg_len = recv(cur_client, client_msg, sizeof(client_msg), 0);
            if (client_msg_len <= 0) {
                printf("server recv error!\n");
                break;
            }
            client_msg[client_msg_len] = '\0';
            string decry_res = "";
            if (des.decry_operation(client_msg, DES_KEY, decry_res) != 0) { //解密
                printf("decry error!\n");
                return -1;
            }
            cout << "Receive message from <" << inet_ntoa(client_addr.sin_addr) << ">:" << endl;
            cout << decry_res << endl;
            cin.getline(server_msg, sizeof(server_msg));
            if(strcmp(server_msg, "quit") == 0) {
                break;
            }
            cout << "Send message to <" << inet_ntoa(client_addr.sin_addr) << ">:" << endl;
            cout << server_msg << endl;
            string encry_res;
            if (des.encry_operation(server_msg, DES_KEY, encry_res) != 0) { // 加密
                printf("encry error!\n");
                return -1;
            }
            memset(server_msg, '\0', max_msg_len);
            for (int i = 0; i < encry_res.length(); i++) {// string to char
                server_msg[i] = encry_res[i];
            }
            server_msg[encry_res.size()] = '\0';
            //6. send msg to client
            if(send(cur_client, server_msg, strlen(server_msg), 0) <= 0) {
                printf("server send error!\n");
                return -1;
            }
            /*
            recv and send func:
            if return > 0, means the num of bytes send or recv.
            if return = 0, means timeout.
            if return < 0, means error happened.
            */
        }
        close(cur_client);
        cout << "Current client is shut down, waiting for another client? 1 - Yes, 0 - No." << endl;
        bool wait_for_another = false;
        cin >> wait_for_another;
        if (!wait_for_another) {
           break;
        }
    }
    
    cout << "————————server end————————" << endl;
    close(tcp_socket);
    return 0;
}
