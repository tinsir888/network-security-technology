#include "md5.h"
//MD5 calc_md5_digest
// calc md5 digest for given string
void MD5::calc_md5_digest(const string &str) {
    reset_state();
    // translate into standard byte stream
    // the call calc_md5_digest()
    vector<uint8_t> input;
    for (int i = 0; i < str.size(); ++i)
        input.push_back(str[i]);
    calc_md5_digest(input);
}

// calc md5 digest for given file
void MD5::calc_md5_digest(ifstream &in) {
    reset_state();
    string str((istreambuf_iterator<char>(in)), istreambuf_iterator<char>()); 
    vector<uint8_t> input;
    for (int i = 0; i < str.size(); ++i)
        input.push_back(str[i]);
    calc_md5_digest(input);
}
// pre-process for byte stream, then call md5_on_subgup() to calc each 64byte data trunk
void MD5::calc_md5_digest(vector<uint8_t> input) {
    // padding: N * 512 + 448
    vector<uint8_t> true_length = int642int8vec(input.size() * 8);// true_length.size()=8
    vector<uint8_t> tmp_padding(64, (uint8_t)0); // max padding: 512bit=64*8
    tmp_padding[0] = (uint8_t)128;
    if (input.size() * 8 % 512 == 448) 
        input.insert(input.end(), tmp_padding.begin(), tmp_padding.end());
    else {
        int index = 0;
        while(input.size() * 8 % 512 != 448) 
            input.push_back(tmp_padding[index++]);
    }
    input.insert(input.end(), true_length.begin(), true_length.end());

    // start md5 calc, each time calc 512bit=8*64
    int md5_subgroup = input.size() / 64;
    for (int i = 0; i < md5_subgroup; ++i) {
        vector<uint8_t> md5_input;
        md5_input.insert(md5_input.end(), input.begin() + i * 64, input.begin() + (i + 1) * 64); 
        md5_on_subgup(md5_input);
    }
}

// Divide the 64byte data block into 16 subgroups of 32bit size
vector<uint32_t> MD5::divide_64byte_to_32bit(const vector<uint8_t>input) {
    // 4 8bit nums of input merge into a 32bit num output, but should reverse, ABCD->DCBA 
    vector<uint32_t> output;
    for (int i = 0; i < input.size() / 4; ++i) {
        uint32_t temp = 0;
        for (int j = 3; j >= 0; --j) {
            temp += input[i * 4 + j];
            if (j != 0) 
                temp <<= 8;
        }
        output.push_back(temp);
    }
    return output;
}


//CODE PROCESS
//32 bit decimal num to hexdecimal num, stored in a string.
string MD5::dec2hex(uint32_t decimal) {
    string hex;
    uint32_t help = 4026531840;
    string dig_dec2hex[16]={"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};
    for (int i = 0; i < 8; ++i) {
        int tmp_res = (decimal & help) >> 28;
        hex += dig_dec2hex[tmp_res];
        decimal <<= 4;
    }
    return hex;
}

// convert a int46 into vector<uint8_t>
vector<uint8_t> MD5::int642int8vec(uint64_t num) {
    vector<uint8_t> res(8);
    uint8_t help = 255;
    for (int i = 0; i < 8; ++i) {
        res[i] = num & help;
        num >>= 8;
    }
    return res;
}


//MD5 FUNCTION
void MD5::reset_state() {
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
}

string MD5::to_str() {
    string res;
    for(int i = 0; i < 4; ++i) {
        res += dec2hex(htobe32(state[i]));
    }
    return res;
}

void MD5::md5_on_subgup(const vector<uint8_t> block) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    vector<uint32_t> x = divide_64byte_to_32bit(block);
    /* 1st round */
    FF(a, b, c, d, x[0],  7,  T(1)); 
    FF(d, a, b, c, x[1],  12, T(2)); 
    FF(c, d, a, b, x[2],  17, T(3)); 
    FF(b, c, d, a, x[3],  22, T(4)); 
    FF(a, b, c, d, x[4],  7,  T(5)); 
    FF(d, a, b, c, x[5],  12, T(6)); 
    FF(c, d, a, b, x[6],  17, T(7)); 
    FF(b, c, d, a, x[7],  22, T(8)); 
    FF(a, b, c, d, x[8],  7,  T(9)); 
    FF(d, a, b, c, x[9],  12, T(10)); 
    FF(c, d, a, b, x[10], 17, T(11)); 
    FF(b, c, d, a, x[11], 22, T(12));
    FF(a, b, c, d, x[12], 7,  T(13)); 
    FF(d, a, b, c, x[13], 12, T(14)); 
    FF(c, d, a, b, x[14], 17, T(15)); 
    FF(b, c, d, a, x[15], 22, T(16)); 
    
    /* 2nd round */
    GG (a, b, c, d, x[1],  5,  T(17));
    GG (d, a, b, c, x[6],  9,  T(18)); 
    GG (c, d, a, b, x[11], 14, T(19)); 
    GG (b, c, d, a, x[0],  20, T(20)); 
    GG (a, b, c, d, x[5],  5,  T(21)); 
    GG (d, a, b, c, x[10], 9,  T(22)); 
    GG (c, d, a, b, x[15], 14, T(23)); 
    GG (b, c, d, a, x[4],  20, T(24)); 
    GG (a, b, c, d, x[9],  5,  T(25)); 
    GG (d, a, b, c, x[14], 9,  T(26)); 
    GG (c, d, a, b, x[3],  14, T(27)); 
    GG (b, c, d, a, x[8],  20, T(28)); 
    GG (a, b, c, d, x[13], 5,  T(29)); 
    GG (d, a, b, c, x[2],  9,  T(30)); 
    GG (c, d, a, b, x[7],  14, T(31)); 
    GG (b, c, d, a, x[12], 20, T(32)); 

    /* 3rd round */
    HH (a, b, c, d, x[5],  4,  T(33)); 
    HH (d, a, b, c, x[8],  11, T(34)); 
    HH (c, d, a, b, x[11], 16, T(35)); 
    HH (b, c, d, a, x[14], 23, T(36)); 
    HH (a, b, c, d, x[1],  4,  T(37)); 
    HH (d, a, b, c, x[4],  11, T(38)); 
    HH (c, d, a, b, x[7],  16, T(39)); 
    HH (b, c, d, a, x[10], 23, T(40)); 
    HH (a, b, c, d, x[13], 4,  T(41)); 
    HH (d, a, b, c, x[0],  11, T(42)); 
    HH (c, d, a, b, x[3],  16, T(43)); 
    HH (b, c, d, a, x[6],  23, T(44)); 
    HH (a, b, c, d, x[9],  4,  T(45)); 
    HH (d, a, b, c, x[12], 11, T(46)); 
    HH (c, d, a, b, x[15], 16, T(47)); 
    HH (b, c, d, a, x[2],  23, T(48)); 
    
    /* 4th round */
    II (a, b, c, d, x[0],  6,  T(49));
    II (d, a, b, c, x[7],  10, T(50));
    II (c, d, a, b, x[14], 15, T(51));
    II (b, c, d, a, x[5],  21, T(52));
    II (a, b, c, d, x[12], 6,  T(53));
    II (d, a, b, c, x[3],  10, T(54));
    II (c, d, a, b, x[10], 15, T(55));
    II (b, c, d, a, x[1],  21, T(56));
    II (a, b, c, d, x[8],  6,  T(57));
    II (d, a, b, c, x[15], 10, T(58));
    II (c, d, a, b, x[6],  15, T(59));
    II (b, c, d, a, x[13], 21, T(60));
    II (a, b, c, d, x[4],  6,  T(61));
    II (d, a, b, c, x[11], 10, T(62));
    II (c, d, a, b, x[2],  15, T(63));
    II (b, c, d, a, x[9],  21, T(64));
    
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}