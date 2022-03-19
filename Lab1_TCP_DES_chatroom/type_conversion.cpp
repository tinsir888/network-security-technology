#include "generalheader.h"
using namespace std;

// left cycle shift
vector<bool> left_shift(int shift_bit, vector<bool> ls_vector) {
    shift_bit = shift_bit % ls_vector.size();
    vector<bool> res;
    for (int i = 0; i < ls_vector.size() - shift_bit; i++) {
        res.push_back(ls_vector[i + shift_bit]);
    }
    for (int i = 0; i < shift_bit; i++) {
        res.push_back(ls_vector[i]);
    }
    return res;
}
int binary2dec(vector<bool> binary) {
    int res = 0, digit = 1;
    for (int i = binary.size() - 1; i >= 0; i--) {
        int temp = binary[i] ? digit : 0;
        res += temp;
        digit <<= 1;
    }
    return res;
}
vector<bool> dec2binary(int decimal, int bit) { //convert decimal into 'bit' bits binary.
    int digit = 1;
    vector<bool> res;
    for (int i = 0; i < bit; i++) {
        if (decimal & digit) {
            res.insert(res.begin(), 1);
        }
        else {
            res.insert(res.begin(), 0);
        }
        digit <<= 1;
    }
    return res;
}
vector<bool> XOR(vector<bool> input1, vector<bool> input2) {
    vector<bool> output;
    for (int i = 0; i < input1.size(); i++) {
        output.push_back(input1[i] ^ input2[i]);
    }
    return output;
}