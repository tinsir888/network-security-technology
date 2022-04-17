#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "../include/des.h"
#include "../include/define_code.h"
#include "../include/help_func.h"
using namespace std;

int CDesOperate::decryption(string cipherText, string key, string& decry_res) {
    if (key.size() != 8) {
        cout << "err: key size isn't 8" << endl;
        return -1;
    }

    vector<bool> bin_key = key_process(key); // key预处理，string转vector<bool>
    vector< vector<bool> > sub_key = make_key(bin_key); // 生成子秘钥

    vector< vector<bool> > bin_cipher_res = decry_data_proc(cipherText); 
    vector< vector<bool> > bin_raw_data; //明文二进制结果

    for (int i = 0; i < bin_cipher_res.size(); i++) { // 分组处理
        vector<bool> temp = decryption_proc(bin_cipher_res[i], sub_key);
        bin_raw_data.push_back(temp);
    }
    
    // 2进制结果分组，64 = 8 * 8
    for (int i = 0; i < bin_raw_data.size(); i++) {
        string temp = "";
        int start = 0;
        while (start < 64) {
            vector<bool> div_bin(bin_raw_data[i].begin() + start, bin_raw_data[i].begin() + start + 8);
            temp += bin2dec(div_bin);
            start += 8;
        }
        decry_res += temp;
    }
    
    return 0;
}

vector<bool> CDesOperate:: decryption_proc(vector<bool> input, vector<vector<bool> > sub_key) {
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
    // 每一轮：右 = 左，左 = f(左, subkey)^右 
    for (int i = 0; i < 16; i++) {
        vector<bool> f_func_res = f_function(step1["left"], sub_key[15 - i]);
        vector<bool> xor_res = XOR(f_func_res, step1["right"]);

        step1["right"] = step1["left"];
        step1["left"] = xor_res;
    }

    // 步骤3: 逆初始置换IP
    vector<bool> step3 = step1["left"];
    step3.insert(step3.end(), step1["right"].begin(), step1["right"].end());
    step3 = init_replacement_IP(step3, inverse_replace_ip);
    return step3;
}