#include <iostream>
#include <string>
#include <vector>
#include "define_code.h"
using namespace std;

struct RSA_key_pair {
    ull public_key_e;
    ull private_key_d;
    ull n;
    string en;
};

class CRsaOperate {
public:
    int RSA_key_pair_gen(RSA_key_pair &key_pair); // 服务器端生成RSA公私钥对
    string client_encry(string des_key, ull public_key_e, ull n); // 客户端使用服务器的公钥加密DES秘钥
    string server_decry(string key_info, ull private_key_d, ull n); // 服务器使用私钥解密客户端发来的DES秘钥
    
private:
    ull montgomary(ull a, ull q, ull n); // 计算(a^q)%n，蒙哥马利快速幂算法
    ull rand_prime(char bits); // 质数生成函数
    ull phi(ull n); // phi(x)，欧拉函数
    bool miller_rabin_prime_test(ull n); // 米勒罗宾素数测试
    bool miller_rabin(ull n, int time);// 重复调用RabinMillerKnl()判别某数n是否是质数
    bool is_relatively_prime(ull p, ull q); // 判断两个参数是否互质
};