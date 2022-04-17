#include <iostream>
#include <string.h>
#include <vector>
#include "../include/des.h"
#include "../include/define_code.h"
#include "../include/help_func.h"
using namespace std;

// 生成16个密钥中的每一个子密钥
vector< vector<bool> > CDesOperate::make_key(vector<bool> init_key) {
    // (0) 密钥的奇偶校验位
    for (int i = 0; i < 8; i++) {
        int cnt = 0;
        for (int j = 0; j < 7; j++) {
            if (init_key[i * 8 + j] == true) {
                cnt++;
            }
        }
        if (cnt % 2 == 0) //当前为偶数个1，本组最后一个数字改为1
            init_key[i * 8 + 7] = true;
        else// 否则改为0
            init_key[i * 8 + 7] = false;
    }

    // (1) 置换选择PC-1：从64bit初始密钥中选出56bit有效位
    // 64 = 8位奇偶校验 + 左28 + 右28, int = 4字节 = 4*8位
    // 初始密钥的第8、16、24、32、40、48、56、64位是奇偶校验位，其他为有效位
    vector< vector<bool> > key_pc_1(2);
    for (int i = 0; i < 28; i++) {
        key_pc_1[0].push_back(init_key[key_left[i] - 1]);
        key_pc_1[1].push_back(init_key[key_right[i] - 1]);
    }

    //（2）循环左移LS
    // key_pc_1[0]和key_pc_1[1]各自左移特定位，共16轮，每轮左移位数不同
    vector< vector<bool> > key_left_shift; // 1维是16轮，2维是每一轮的28*2=56位子秘钥
    for (int i = 0; i < 16; i++) {
        key_pc_1[0] = left_shift(lshift_table[i], key_pc_1[0]);
        key_pc_1[1] = left_shift(lshift_table[i], key_pc_1[1]);
        vector<bool> tmp_left_shift;
        tmp_left_shift.insert(tmp_left_shift.end(), key_pc_1[0].begin(), key_pc_1[0].end());
        tmp_left_shift.insert(tmp_left_shift.end(), key_pc_1[1].begin(), key_pc_1[1].end());
        key_left_shift.push_back(tmp_left_shift);
    }

    // （3）置换选择PC2
    // 将其余位置按照keyChoose置换位置，输出48bit，作为第N轮的子密钥
    vector< vector<bool> > key_pc_2; // 1维是16轮，2维是每一轮的48位子秘钥
    for (int i = 0; i < 16; i++) {
        vector<bool> tmp_pc_2;
        for (int j = 0; j < 48; j++) {
            tmp_pc_2.push_back(key_left_shift[i][key_choose[j] - 1]);
        }
        key_pc_2.push_back(tmp_pc_2);
    }
    return key_pc_2;
}