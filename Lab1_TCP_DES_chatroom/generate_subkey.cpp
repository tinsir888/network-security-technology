#include "generalheader.h"

//generate 16 subkeys
vector< vector<bool> > CDesOperate::gen_subkey(vector<bool> init_key) {
    //step 0: even-odd verify
    for (int i = 0; i < 8; i++) {
        int cnt = 0;//cnt refers to num of bit is 1 in init key.
        for (int j = 0; j < 7; j++) {
            if (init_key[i * 8 + j]) {
                cnt++;
            }
        }
        //if there is even number of 1, change the last digit to 1
        if (cnt & 1 == 0) {
            init_key[i * 8 + 7] = true;
        } //if there is odd number of 1, change the last digit to 0
        else {
            init_key[i * 8 + 7] = false;
        }
    }
    /*
    step 1: replace select PC-1 64bit -> 56bit valid
    64 = 8bit odd-even verify + left 28bit + right 28bit, int = 4byte = 4*8bit
    the 8th、16th、24th、32nd、40th、48th、56th、64th bit are odd-even verify bit
    others are valid bits.
    */
    vector< vector<bool> > key_PC1(2);
    for (int i = 0; i < 28; i++) {
        key_PC1[0].push_back(init_key[key_PC_1[i] - 1]);
        key_PC1[1].push_back(init_key[key_PC_2[i] - 1]);
    }

    //step 2: cycle left shift certain bit.
    //key_PC1[0] and key_PC1[1] left shift 16 times respectively, each time different num of bits.
    vector< vector<bool> > key_left_shift; // 1维是16轮，2维是每一轮的28*2=56位子秘钥
    for (int i = 0; i < 16; i++) {
        key_PC1[0] = left_shift(left_shift_table[i], key_PC1[0]);
        key_PC1[1] = left_shift(left_shift_table[i], key_PC1[1]);
        vector<bool> temp_key_ls;
        temp_key_ls.insert(temp_key_ls.end(), key_PC1[0].begin(), key_PC1[0].end());
        temp_key_ls.insert(temp_key_ls.end(), key_PC1[1].begin(), key_PC1[1].end());
        key_left_shift.push_back(temp_key_ls);
    }

    //step 3: replacement select.
    //others replace as key_choose, output 48bit, as next subkey
    vector< vector<bool> > key_PC2;
    for (int i = 0; i < 16; i++) {//16 times
        vector<bool> temp_key_PC2;
        for (int j = 0; j < 48; j++) {//each time generate 48 bit subkey.
            temp_key_PC2.push_back(key_left_shift[i][key_choose[j] - 1]);
        }
        key_PC2.push_back(temp_key_PC2);
    }
    return key_PC2;
}