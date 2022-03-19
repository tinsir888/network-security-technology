#include "generalheader.h"
// F function is select expand E + key add + select compress + repalce P
vector<bool> CDesOperate::f_func(vector<bool> input, vector<bool> key) {
    vector<bool> f_func_step1, f_func_step2, f_func_step3;//, f_func_step4 
    f_func_step1 = E_Box(input);
    f_func_step2 = key_add(f_func_step1, key);
    f_func_step3 = select_comp_operation(f_func_step2);
    //f_func_step4 = replace_operation(f_func_step3);
    return replace_operation(f_func_step3);
}

//step 1: E BOX expand input right 32 bit to 48 bit
vector<bool> CDesOperate::E_Box(vector<bool> input) {
    vector<bool> e_box_output;
    for (int i = 0; i < 48; i++) {
        e_box_output.push_back(input[des_E_box[i] - 1]);
    }
    return e_box_output;
}

//step 2: key add. step 1 e box output 48bit as input, xor with 48 bit subkey.
vector<bool> CDesOperate::key_add(vector<bool> input, vector<bool> key) {
    return XOR(input, key);
}

//step 3: select compress
vector<bool> CDesOperate::select_comp_operation(vector<bool> input) {
    //1. step 2 output 48bit as input, cut into 8 groups by 6 bits.
    vector<vector<bool> > input_group;
    for (int i = 0; i < 8; i++) {
        vector<bool> temp_group;
        for (int j = 0; j < 6; j++) {
            temp_group.push_back(input[i * 6 + j]);
        }
        input_group.push_back(temp_group);
    }

    vector<bool> res;
    //2. each group send into 8 S-Box respectively, get 8*4=32bit output
    for (int i = 0; i < 8; i++) {
        int temp_int = des_S_box[i][binary2dec(input_group[i])];
        vector<bool> temp_bool = dec2binary(temp_int, 4);
        res.insert(res.end(), temp_bool.begin(), temp_bool.end());
    }
    return res;
}
//step 4: replace P
vector<bool> CDesOperate::replace_operation(vector<bool> input) {
    vector<bool> result;
    for (int i = 0; i < 32; i++) {
        result.push_back(input[rep_P[i] - 1]);
    }
    return result;
}