#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include "../include/des.h"
#include "../include/define_code.h"
#include "../include/help_func.h"
using namespace std;

// 生成随机DES秘钥，8个字母的string
string CDesOperate::des_key_gen() {
    string des_key = "";
    srand((unsigned)time(NULL));
    for (int i = 0; i < 8; ++i) {
        char temp = 65 + rand() % 26;
        des_key += temp;
    }
    return des_key;
}

// 初始置换IP，逆初始置换IP
vector<bool> CDesOperate::init_replacement_IP(vector<bool> input, int type) {
    int* pc_table = pc_first;
    if (type == init_replace_ip)
        pc_table = pc_first;
    else if (type == inverse_replace_ip) 
        pc_table = pc_last;
    vector<bool> res;
    for (int i = 0; i < 64; i++)
        res.push_back(input[pc_table[i] - 1]);

    return res;
}

// 明文数据处理
vector<vector<bool> > CDesOperate:: encry_data_proc(string text) {
    vector<string> div_text; // 8个字母为一组的切割
    int gpno = text.size() / 8;
    for (int i = 0; i < gpno; i++) { // 按组压入
        string tmp_txt = "";
        for (int j = 0; j < 8; j++) {
            tmp_txt.push_back(text[8 * i + j]);
        }
        div_text.push_back(tmp_txt);
    }
    if (text.size() % 8 != 0) { // 末尾有小于8个字符
        string tmp_txt = "";
        for (int i = gpno * 8; i < text.size(); i++) {
            tmp_txt.push_back(text[i]);
        }
        // 将最后一组string补全为8个字符 
        for (int i = tmp_txt.size(); i < 8; i++) {
            tmp_txt.push_back('\0');
        }
        div_text.push_back(tmp_txt);
    }

    // 将每一组的8个字符转为8*8=64bit的二进制
    vector<vector<bool> > bin_txt;
    for (int i = 0; i < div_text.size(); i++) {
        vector<bool> tmp_bin;
        for (int j = 0; j < div_text[i].size(); j++) {
            vector<bool> tmp_bin_part = dec2bin(div_text[i][j], 8);
            tmp_bin.insert(tmp_bin.end(), tmp_bin_part.begin(), tmp_bin_part.end());
        }
        bin_txt.push_back(tmp_bin);
    }
    return bin_txt;
}

// 密文数据处理
vector<vector<bool> > CDesOperate::decry_data_proc(string text) {
    vector<string> div_text; // 16个字母为一组的切割
    int gpno = text.size() / 16;
    for (int i = 0; i < gpno; i++) { // 按组压入
        string tmp_txt = "";
        for (int j = 0; j < 16; j++) {
            tmp_txt.push_back(text[16 * i + j]);
        }
        div_text.push_back(tmp_txt);
    }
    if (text.size() % 16 != 0) { // 末尾有小于16个字符
        string tmp_txt = "";
        for (int i = gpno * 16; i < text.size(); i++) {
            tmp_txt.push_back(text[i]);
        }
        // 将最后一组string补全为16个字符
        for (int i = tmp_txt.size(); i < 16; i++) {
            tmp_txt.push_back('\0');
        }
        div_text.push_back(tmp_txt);
    }
    // 将每一组的8个字符转为16*4=64bit的二进制
    // 1个字母 → 1个16进制 → 4bit
    vector<vector<bool> > bin_txt;
    for (int i = 0; i < div_text.size(); i++) {
        vector<bool> tmp_bin;
        for (int j = 0; j < div_text[i].size(); j++) {
            vector<bool> tmp_bin_part = dec2bin(div_text[i][j] - 65, 4);
            tmp_bin.insert(tmp_bin.end(), tmp_bin_part.begin(), tmp_bin_part.end());
        }
        bin_txt.push_back(tmp_bin);
    }
    return bin_txt;
}

// key的处理
vector<bool> CDesOperate::key_process(string key) {
    vector<bool> result;
    for (int i = 0; i < key.size(); i++) {
        vector<bool> temp = dec2bin(key[i], 8);
        temp = left_shift(1, temp);
        result.insert(result.end(), temp.begin(), temp.end());
    }
    return result;
}