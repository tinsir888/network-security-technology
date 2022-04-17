#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "../include/des.h"
#include "../include/define_code.h"
#include "../include/help_func.h"
using namespace std;

int CDesOperate::encryption(string raw_data, string key, string& encry_res) {
    if (key.size() != 8) {
        cout << "err: key size isn't 8" << endl;
        return -1;// des 密钥长度不为 8
    }

    vector<bool> bin_key = key_process(key); // key预处理，string转vector<bool>
    vector< vector<bool> > sub_key = make_key(bin_key); // 生成子秘钥

    vector< vector<bool> > bin_raw_data = encry_data_proc(raw_data);
    vector< vector<bool> > bin_cipher_res; //二进制结果

    for (int i = 0; i < bin_raw_data.size(); i++) { // 分组处理
        vector<bool> temp = encry_proc(bin_raw_data[i], sub_key);
        bin_cipher_res.push_back(temp);
    }

    //将2进制的加密结果分组，4个一组，转为16进制，再+65转为好输出的ascii码
    encry_res = "";
    int dec2hex[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 
                      0x05, 0x06, 0x07, 0x08, 0x09, 
                      0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f }; 
                      // 十进制转十六进制
    for (int i = 0; i < bin_cipher_res.size(); i++) {
        int start = 0;
        while (start < 64) {
            vector<bool> bin_cipher(bin_cipher_res[i].begin() + start, bin_cipher_res[i].begin() + start + 4);
            int dec_cipher = bin2dec(bin_cipher);
            encry_res += dec2hex[dec_cipher] + 65;
            start += 4;
        }
    }
    return 0;
}

vector<bool> CDesOperate:: encry_proc(vector<bool> input, vector<vector<bool> > sub_key) {
    // 步骤1: 初始置换IP
    vector<bool> temp_step_1 = init_replacement_IP(input, init_replace_ip);
    map<string, vector<bool> > step1;
    vector<bool> temp_l, temp_r;
    for (int i = 0; i < 32; i++) {
        temp_l.push_back(temp_step_1[i]);
        temp_r.push_back(temp_step_1[i + 32]);
    }
    step1["left"] = temp_l;
    step1["right"] = temp_r;
    // 步骤2: 16轮迭代
    // 每一轮：左 = 右， 右 = f(右, subkey)^左 
    for (int i = 0; i < 16; i++) {
        vector<bool> f_func_res = f_function(step1["right"], sub_key[i]);
        vector<bool> xor_res = XOR(f_func_res, step1["left"]);

        step1["left"] = step1["right"];
        step1["right"] = xor_res;
    }

    // 步骤3: 逆初始置换IP
    vector<bool> step3 = step1["left"];
    step3.insert(step3.end(), step1["right"].begin(), step1["right"].end());
    step3 = init_replacement_IP(step3, inverse_replace_ip);
    return step3;
}