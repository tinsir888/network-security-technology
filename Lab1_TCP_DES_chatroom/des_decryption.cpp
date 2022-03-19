#include "generalheader.h"

int CDesOperate::decry_operation(string cipherText, string key, string& decry_res) {
    if (key.size() != 8) {
        cout << "error: key size isn't 8!" << endl;
        return -1;
    }
    vector<bool> bin_key = key_str2bool(key);
    vector< vector<bool> > sub_key = gen_subkey(bin_key);
    vector< vector<bool> > bin_code_text = decry_str2bool(cipherText); 
    vector< vector<bool> > bin_raw_text;
    for (int i = 0; i < bin_code_text.size(); i++) {
        vector<bool> temp_res = decry_process(bin_code_text[i], sub_key);
        bin_raw_text.push_back(temp_res);
    }
    //64 = 8*8
    for (int i = 0; i < bin_raw_text.size(); i++) {
        string temp_res = "";
        int letter_assign = 0;
        while (letter_assign < 64) {
            vector<bool> cuttedBin(bin_raw_text[i].begin() + letter_assign,
                                   bin_raw_text[i].begin() + letter_assign + 8);
            temp_res += binary2dec(cuttedBin);
            letter_assign += 8;
        }
        decry_res += temp_res;
    }
    return 0;
}

vector<bool> CDesOperate:: decry_process(vector<bool> input, vector<vector<bool> > sub_key) {
    //step 1: inital replacement IP
    vector<bool> temp_step1 = init_replacement_IP(input, INIT_REPLACE_IP);
    //map<string, vector<bool> > step1;
    vector<bool> step1_l,step1_r;
    vector<bool> temp_l, temp_r;
    vector<bool> step3;
    for (int i = 0; i < 32; i++) {
        temp_l.push_back(temp_step1[i]);
        temp_r.push_back(temp_step1[i + 32]);
    }
    step1_l = temp_l;
    step1_r = temp_r;
    //step 2: 16 time iteration
    //each iteration: r_i = l_(i-1)，l_i = f(l_(i-1), subkey) XOR r_(i-1) 
    for (int i = 0; i < 16; i++) {
        vector<bool> f_func_res = f_func(step1_l, sub_key[15 - i]);//reverse as 15-i here.
        vector<bool> xor_res = XOR(f_func_res, step1_r);
        step1_r = step1_l;
        step1_l = xor_res;
    }

    //step 3: inverse inital replacement IP
    step3 = step1_l;
    step3.insert(step3.end(), step1_r.begin(), step1_r.end());
    step3 = init_replacement_IP(step3, INVERSE_REPLACE_IP);
    return step3;
}