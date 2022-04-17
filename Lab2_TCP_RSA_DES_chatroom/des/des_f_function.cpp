#include <iostream>
#include <vector>
#include "../include/des.h"
#include "../include/help_func.h"
using namespace std;

// f函数 = 选择扩展运算E + 密钥加运算 + 选择压缩运算S + 置换P
vector<bool> CDesOperate::f_function(vector<bool> input, vector<bool> key) {
    vector<bool> f_step_1 = E_box(input);
    vector<bool> f_step_2 = key_add(f_step_1, key);
    vector<bool> f_step_3 = select_comp_op(f_step_2);
    return replace_op(f_step_3);
    //vector<bool> f_step_4 = replace_op(f_step_3);
    //return f_step_4;
}

//（1）选择扩展运算(E盒)：将输入的右边32bit扩展成为48bit输出
vector<bool> CDesOperate::E_box(vector<bool> input) {
    vector<bool> output;
    for (int i = 0; i < 48; i++)
        output.push_back(input[des_E[i] - 1]);
    return output;
}

//（2）秘钥加运算：将选择扩展运算输出的48bit作为输入，与48bit的子密钥进行异或运算^
vector<bool> CDesOperate::key_add(vector<bool> input, vector<bool> key) {
    return XOR(input, key);
}

//（3）选择压缩运算
vector<bool> CDesOperate::select_comp_op(vector<bool> input) {
    // 1、将密钥加运算的输出作为48bit输入，将其分为8组，每组6bit，
    vector<vector<bool> > input_gp;
    for (int i = 0; i < 8; i++) {
        vector<bool> tmp_gp;
        for (int j = 0; j < 6; j++) {
            tmp_gp.push_back(input[i * 6 + j]);
        }
        input_gp.push_back(tmp_gp);
    }

    vector<bool> result;
    // 2、分别进入8个S盒进行运算，得出8*4=32bit的输出结果，拼接后输出
    for (int i = 0; i < 8; i++) {
        int tmpi = des_S[i][bin2dec(input_gp[i])];
        vector<bool> tmp_bl = dec2bin(tmpi, 4);
        result.insert(result.end(), tmp_bl.begin(), tmp_bl.end());
    }
    return result;
}

//（4）置换运算P
vector<bool> CDesOperate::replace_op(vector<bool> input) {
    vector<bool> result;
    for (int i = 0; i < 32; i++) {
        result.push_back(input[des_P[i] - 1]);
    }
    return result;
}