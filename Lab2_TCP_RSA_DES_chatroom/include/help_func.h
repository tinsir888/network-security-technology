#include <iostream>
#include <vector>
#include "define_code.h"
using namespace std;

// 循环左移
vector<bool> left_shift(int shiftBit, vector<bool> LSVector);

// 二进制转十进制
int bin2dec(vector<bool> binary);

// 十进制转二进制
vector<bool> dec2bin(int decimal, int bit);

// 异或运算^
vector<bool> XOR(vector<bool> input1, vector<bool> input2);

// 将string转为int64
ull str2uint(string str);

// 将16bit的short转成2个字母的string
string short2str(unsigned short num);