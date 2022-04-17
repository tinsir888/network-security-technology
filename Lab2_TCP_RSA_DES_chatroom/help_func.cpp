#include "./include/define_code.h"
#include "./include/help_func.h"
using namespace std;

// 循环左移
vector<bool> left_shift(int shiftBit, vector<bool> LSVector) {
    shiftBit = shiftBit % LSVector.size();
    vector<bool> result;
    for (int i = 0; i < LSVector.size() - shiftBit; i++) {
        result.push_back(LSVector[i + shiftBit]);
    }
    for (int i = 0; i < shiftBit; i++) {
        result.push_back(LSVector[i]);
    }
    return result;
}

// 二进制转十进制
int bin2dec(vector<bool> binary) {
    int result = 0, help = 1;
    for (int i = binary.size() - 1; i >= 0; i--) {
        int temp = binary[i] == 1 ? help : 0;
        result += temp;
        help = help << 1;
    }
    return result;
}

// 十进制转二进制
vector<bool> dec2bin(int decimal, int bit) { // 参数2：将十进制decimal转成bit位二进制
    int help = 1;
    vector<bool> result;
    for (int i = 0; i < bit; i++) {
        if (decimal & help) {
            result.insert(result.begin(), true);
        }
        else {
            result.insert(result.begin(), false);
        }
        help = help << 1;
    }
    return result;
}

// 异或运算^
vector<bool> XOR(vector<bool> input1, vector<bool> input2) {
    vector<bool> output;
    for (int i = 0; i < input1.size(); i++) {
        output.push_back(input1[i] ^ input2[i]);
    }
    return output;
}

// 将string转为int64，ASCII中0 = 48, 9 = 57
ull str2uint(string str) {
    ull result = 0;
    ull help = 1;
    for (int i = str.size() - 1; i >= 0; --i) {
        result += (str[i] - 48) * help;
        help *= 10;
    }
    return result;
}

// 将16bit的short转成2个字母的string
string short2str(unsigned short num) {
    string result = "";
    unsigned short num1 = num >> 8;
    result += num1;
    unsigned short num2 = num & 0b0000000011111111;
    result += num2;
    return result;
}