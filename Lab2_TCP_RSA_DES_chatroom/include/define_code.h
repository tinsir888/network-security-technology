#ifndef _DEFINE_CODE_H_
#define _DEFINE_CODE_H_
const static int epoll_size = 256;
const static int max_line = 20;
const static int chat_port = 13245;
const static int message_size = 4096;
#define default_server "127.0.0.1"
const static int epoll_timeout = -1;
typedef unsigned long long ull;
const static int DATA_EXCHANGE = 100; // 不处于密钥协商状态，处于数据交换状态
const static int KEY_AGREE_SEND_KEY = 101; // 密钥协商状态中发送公钥对
const static int KEY_AGREE_CONFIRM = 102; // 密钥协商状态中确认成功接收DES秘钥
const static int init_replace_ip = 1021; // 初始置换IP
const static int inverse_replace_ip = 1022; // 逆初始置换IP
#endif