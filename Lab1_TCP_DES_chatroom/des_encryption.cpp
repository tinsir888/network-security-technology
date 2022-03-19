#include "generalheader.h"

int CDesOperate::encry_operation(string raw_text, string key, string& encry_res) {
    if (key.size() != 8) {
        cout << "error: key size isn't 8!" << endl;
        return -1;
    }
    vector<bool> bin_key = key_str2bool(key); // key:string2vector<bool>
    vector< vector<bool> > sub_key = gen_subkey(bin_key);//generate subkey
    vector< vector<bool> > bin_raw_text = encry_str2bool(raw_text);
    vector< vector<bool> > bin_code_text; //binary cipher code
    for (int i = 0; i < bin_raw_text.size(); i++) { //encry by group
        vector<bool> temp_res = encry_process(bin_raw_text[i], sub_key);
        bin_code_text.push_back(temp_res);
    }
    //cipher code by bit by group 2 hex then +65 to ASCII.
    encry_res = "";
    int dec2hex[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                      0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
    for (int i = 0; i < bin_code_text.size(); i++) {
        int letter_assign = 0;
        while (letter_assign < 64) {
            vector<bool> bin_cipher_letter(bin_code_text[i].begin() + letter_assign, 
                                           bin_code_text[i].begin() + letter_assign + 4);
            int decipher_letter_dec = binary2dec(bin_cipher_letter);
            encry_res += dec2hex[decipher_letter_dec] + 65;
            letter_assign += 4;
        }
    }
    return 0;
}

vector<bool> CDesOperate:: encry_process(vector<bool> input, vector<vector<bool> > sub_key) {
    //step 1: initial replacement IP
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
    //step 2: 16 times iteration
    //each iteration：l_i = r_(i-1)， r_i = f(r_(i-1), subkey) XOR l_(i-1)
    for (int i = 0; i < 16; i++) {
        vector<bool> f_func_res = f_func(step1_r, sub_key[i]);
        vector<bool> xor_res = XOR(f_func_res, step1_l);
        step1_l = step1_r;
        step1_r = xor_res;
    }
    //step 3: inverse initial replacement IP
    step3 = step1_l;
    step3.insert(step3.end(), step1_r.begin(), step1_r.end());
    step3 = init_replacement_IP(step3, INVERSE_REPLACE_IP);
    return step3;
}