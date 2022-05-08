#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <streambuf>

using namespace std;

void print_help_info(int argc, char *argv[]); // print help infos.
void print_test_info(int argc, char *argv[]); // print testing infos.
void print_calc_md5(int argc, char *argv[]); // calc and print md5 digest.
void print_input_md5_recalc(int argc, char *argv[]); // input md5 digest, recalc md5 digest for given file, then compare by bit.
void print_read_md5_recalc(int argc, char *argv[]); // read md5 digest, recalc md5 digest for given file, then compare by bit.

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))              //F function
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))              //G function
#define H(x, y, z) ((x) ^ (y) ^ (z))                         //H function
#define I(x, y, z) ((y) ^ ((x) | (~z)))                      //I function
#define LS(x, n) (((x) << (n)) | ((x) >> (32-(n)))) //32 bit num x cycle left shift

#define FF(a, b, c, d, x, s, ac) { (a) += F ((b), (c), (d)) + (x) + ac; (a) = LS ((a), (s)); (a) += (b); }
#define GG(a, b, c, d, x, s, ac) { (a) += G ((b), (c), (d)) + (x) + ac; (a) = LS ((a), (s)); (a) += (b); }
#define HH(a, b, c, d, x, s, ac) { (a) += H ((b), (c), (d)) + (x) + ac; (a) = LS ((a), (s)); (a) += (b); }
#define II(a, b, c, d, x, s, ac) { (a) += I ((b), (c), (d)) + (x) + ac; (a) = LS ((a), (s)); (a) += (b); }

#define T(i) 4294967296 * abs(sin(i))

/* main function:
1. generate md5 digest for string of any length.
2. generate md5 digest for files of any size.
3. verify the complete of files by using md5 digest.
*/
class MD5 {
public:
    void calc_md5_digest(const string &str); // calc md5 for given str.
    void calc_md5_digest(ifstream &in); // calc md5 for give file.
    string to_str(); // convert md5 digest into string

private:
    void reset_state(); // reset_state initial var.
    void calc_md5_digest(vector<uint8_t> input); // calc md5 for given byte-stream
    void md5_on_subgup(const vector<uint8_t> block); // md5 operation on a 512-bit message subgroup
    vector<uint32_t> divide_64byte_to_32bit(const vector<uint8_t>input); // Divide the 64byte data trunk into 16 subgroups of 32bit size
    string dec2hex(uint32_t decimal); //32 bit decimal num to hexdecimal num, stored in a string.
    vector<uint8_t> int642int8vec(uint64_t num); // convert 64bit int into vector<uint8_t>

private:
    uint32_t state[4]; // represent the 4 initial vector
};