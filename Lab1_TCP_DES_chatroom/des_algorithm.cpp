#include "generalheader.h"
// (inverse) init repalcement ip
vector<bool> CDesOperate::init_replacement_IP(vector<bool> input, int type) {
    int* pc_table = init_rep_IP;
    if (type == INIT_REPLACE_IP) {
        pc_table = init_rep_IP;
    }
    else if (type == INVERSE_REPLACE_IP) {
        pc_table = inverse_init_rep_ip;
    }
    vector<bool> result;
    for (int i = 0; i < 64; i++) {
        result.push_back(input[pc_table[i] - 1]);
    }
    return result;
}

//raw data process
vector<vector<bool> > CDesOperate:: encry_str2bool(string text) {
    vector<string> cut_str_8; //cut into groups by 8 letters.
    int group_num = text.size() / 8;
    for (int i = 0; i < group_num; i++) {//press by group
        string temp_text = "";
        for (int j = 0; j < 8; j++) {
            temp_text.push_back(text[8 * i + j]);
        }
        cut_str_8.push_back(temp_text);
    }
    if (text.size() % 8) {//if the last group < 8 
        string temp_text = "";
        for (int i = group_num * 8; i < text.size(); i++) {
            temp_text.push_back(text[i]);
        }
        //fix the last group to 8 letters with '\0'.
        for (int i = temp_text.size(); i < 8; i++) {
            temp_text.push_back('\0');
        }
        cut_str_8.push_back(temp_text);
    }
    //convert each group into 8*8=64bit
    vector<vector<bool> > bin_text;
    for (int i = 0; i < cut_str_8.size(); i++) {
        vector<bool> temp_bin;
        for (int j = 0; j < cut_str_8[i].size(); j++) {
            vector<bool> temp_bin_part = dec2binary(cut_str_8[i][j], 8);
            temp_bin.insert(temp_bin.end(), temp_bin_part.begin(), temp_bin_part.end());
        }
        bin_text.push_back(temp_bin);
    }
    return bin_text;
}

//code data process
vector<vector<bool> > CDesOperate::decry_str2bool(string text) {
    vector<string> cut_str_16; //cut into groups by 16 letters.
    int group_num = text.size() / 16;
    for (int i = 0; i < group_num; i++) {
        string temp_text = "";
        for (int j = 0; j < 16; j++) {
            temp_text.push_back(text[16 * i + j]);
        }
        cut_str_16.push_back(temp_text);
    }
    if (text.size() % 16 != 0) {
        string temp_text = "";
        for (int i = group_num * 16; i < text.size(); i++) {
            temp_text.push_back(text[i]);
        }
        for (int i = temp_text.size(); i < 16; i++) {
            temp_text.push_back('\0');
        }
        cut_str_16.push_back(temp_text);
    }
    //convert each group into 16*4=64bit
    // 1 letter=1 hex=4 bit
    vector<vector<bool> > bin_text;
    for (int i = 0; i < cut_str_16.size(); i++) {
        vector<bool> temp_bin;
        for (int j = 0; j < cut_str_16[i].size(); j++) {
            vector<bool> temp_bin_part = dec2binary(cut_str_16[i][j] - 65, 4);
            temp_bin.insert(temp_bin.end(), temp_bin_part.begin(), temp_bin_part.end());
        }
        bin_text.push_back(temp_bin);
    }
    return bin_text;
}

// key process
vector<bool> CDesOperate::key_str2bool(string key) {
    vector<bool> result;
    for (int i = 0; i < key.size(); i++) {
        vector<bool> temp_res = dec2binary(key[i], 8);
        temp_res = left_shift(1, temp_res);
        result.insert(result.end(), temp_res.begin(), temp_res.end());
    }
    return result;
}