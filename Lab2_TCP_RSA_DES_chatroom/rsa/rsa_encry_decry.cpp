#include <assert.h>
#include <string>
#include "../include/rsa.h"
#include "../include/help_func.h"
using namespace std;

// 客户端使用服务器的公钥加密DES秘钥
// unsigned short = 16位，本作业中64为需要分四次加密
// 使用服务器公钥，计算 C = (M^e) mod n
// 返回值是4个string类型的16位加密结果，逗号分隔
string CRsaOperate::client_encry(string des_key, ull public_key_e, ull n) {
    // 将8个字母的string类型DesKey转为64位int类型，例如ABCDEFGH
    ull _des_key = 0;
    for (unsigned int i = 0; i < des_key.length(); ++i) {
        _des_key += des_key[i];
        if (i != des_key.length() - 1) {
            _des_key <<= 8;
        }
    }
    
    // 64为int拆成4份，每份16位，得到的M[0] = GH，M[1] = EF，M[2] = CD，M[3] = AB
    unsigned short* p_res = (unsigned short*)&_des_key;
    unsigned short M[4];
    for (int i = 0; i < 4; ++i) {
        M[i] = p_res[i];
    }

    string result;
    // 对每一份执行加密函数，并将4个16位数字转成string，用逗号分隔
    for (int i = 3; i >= 0; --i) {
        string temp = to_string(montgomary(M[i], public_key_e, n));
        result += temp;
        result += ',';
    }
    return result;
}

// 服务器使用私钥解密客户端发来的DES秘钥
// 使用服务器私钥，计算 M = (C^d) mod n 
string CRsaOperate::server_decry(string key_info, ull private_key_d, ull n) {
    // key_info = "AA,BB,CC,DD,"，这些XX是64位的数字
    string des_key = "";
    int pos = 0;
    for (int i = 0; i < 4; i++) {
        string temp = "";
        for (; key_info[pos] != ','; ++pos) {
            temp += key_info[pos];
        }
        ++pos;

        ull Ci = str2uint(temp);
        ull n_res = montgomary(Ci, private_key_d, n);
        unsigned short * p_res = (unsigned short*)&n_res;
        if(p_res[1] != 0 || p_res[2] != 0 || p_res[3] != 0) { // error
            printf("sever server_decry() error!\n");
            return 0;
        }
        else // p_res[0]是16bit数字，可以转成2个字母
            des_key += short2str(p_res[0]);
    }
    
    return des_key;
}