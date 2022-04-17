#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include "../include/rsa.h"
#include "../include/define_code.h"


// 模乘运算
ull mul_mod (ull a, ull b, ull n) {
    return (a % n) * (b % n) % n;
}

// 计算(a^q)%n，蒙哥马利算法求快速幂
ull CRsaOperate::montgomary(ull base, ull pow, ull n) {
    ull a = base, b = pow, c=1;
    while(b) {
        while(!(b & 1)) {
            b>>=1; 
            a = mul_mod(a, a, n);
        }   
        b--; 
        c = mul_mod(a, c, n);
    } 
    return c;
}

// 米勒罗宾素数测试
bool CRsaOperate::miller_rabin_prime_test(ull n) {
    ull q = n - 1, k = 0; 
    while (!(q & 1)) {
        ++k;
        q >>= 1;
    }
    ull a = 2 + rand() % (n - 1); // 随机数a满足 2 ≤ a < n − 1
    if (montgomary(a, q, n) == 1) {
        return true;
    }
    for (ull j = 0; j < k; j++) {
        ull z = 1;
        for(ull w = 0; w < j; w++) {
            z *= 2;
        }
        if(montgomary(a, z * q, n) == n-1) {
            return true;
        }
    }
    return false;
}

// 重复调用miller_rabin_prime_test()判别某数n是否是质数
bool CRsaOperate::miller_rabin(ull n, int time) {
    for(int i = 0; i < time; ++i) {
        if(!miller_rabin_prime_test(n)) {
            return false;
        }
    }
    return true;
}

// 质数生成函数
ull CRsaOperate::rand_prime(char bits) {
    ull base;
    
    static int rand_noise = 0;
    rand_noise++;
    srand((unsigned)time(NULL) + rand_noise);

    do {
        base = (unsigned long)1 << (bits - 1); //保证最高位是 1
        base += rand() % base; //加上一个随机数
        base |= 1; //保证最低位是1，即保证是奇数
    } while(!miller_rabin(base, 30)); //测试 30 次
    return base; //全部通过认为是质数
}

// 判断两个参数是否互质
bool CRsaOperate::is_relatively_prime(ull p, ull q) {
    ull a = max(p, q);
    ull b = min(p, q);
    int t;
    if(p == q)
        return false; //两数相等,最大公约数就是本身
    else {
        while(b) { // 辗转相除法
            a = a % b;
            t = a;
            a = b;
            b = t;
        }
        if (a == 1) return true;
        else return false;
    }
}

// 返回小于n且与n互质的正整数个数
ull CRsaOperate::phi(ull n) {
    ull res = n, a = n;
    for (ull i = 2; i * i <= a; ++i) {
        if (a % i == 0) {
            res = res / i * (i - 1);//先进行除法是防止中间数据的溢出
            while (a % i == 0) {
                a /= i;
            }
        }
    }
    if (a > 1) {
        res = res / a * (a - 1);
    }
    return res;
}
// 服务器端生成RSA公私钥对
int CRsaOperate::RSA_key_pair_gen(RSA_key_pair &key_pair) {
    // 生成公钥{e，n}
    ull prime_p = rand_prime(16);
    ull prime_q = rand_prime(16);
    ull n = prime_p * prime_q;
    
    int euler = phi(n);
    ull e;
    while(1) {
        e = rand() % 65536 + 1;
        // e = rand() % (euler - 1) + 1;
        if (is_relatively_prime(e, euler)) {
            break;
        }
    }
    
    // 生成私钥{d，n}
    ull max = 0xffffffffffffffff - euler;
    ull i = 1, d = 0;

    while(1) {
        if ( ((i * euler) + 1) % e == 0) {
            d = ((i * euler) + 1) / e;
            break;
        }
        i++;
        ull temp = (i + 1) * euler;
        if (temp > max)
            break;
    }
    
    // 如果循环结束后d值仍然为0表示秘钥生成失败
    if (d == 0) {
        return -1;
    }

    key_pair.public_key_e = e;
    key_pair.private_key_d = d;
    key_pair.n = n;
    key_pair.en = to_string(e) + "," + to_string(n);
    return 0;
}
