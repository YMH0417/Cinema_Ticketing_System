#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// 判断一个数是否为素数
bool isPrime(long long num) {
    if (num < 2) return false;
    for (long long i = 2; i <= sqrt(num); ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

// 求最大公约数
long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

// 计算模逆
long long modInverse(long long a, long long m) {
    a = a % m;
    for (long long x = 1; x < m; ++x) {
        if ((a * x) % m == 1) return x;
    }
    return -1; // 不存在模逆
}

// 生成伪RSA密钥对
void generateKeys(long long& p, long long& q, long long& n, long long& e, long long& d) {
    // 随机生成两个素数
    srand(static_cast<unsigned>(time(nullptr)));
    do {
        p = rand() % 9000 + 1000; // 生成范围较大的素数
    } while (!isPrime(p));

    do {
        q = rand() % 9000 + 1000;
    } while (!isPrime(q) || p == q);

    n = p * q; // 模数
    long long phi = (p - 1) * (q - 1); // 欧拉函数

    // 选择公钥e (1 < e < phi 且 gcd(e, phi) = 1)
    do {
        e = rand() % (phi - 2) + 2; // e ∈ [2, phi-1]
    } while (gcd(e, phi) != 1);

    // 计算私钥d (e * d ≡ 1 mod phi)
    d = modInverse(e, phi);

    // 输出公钥和私钥
    cout << "公钥: (" << e << ", " << n << ")" << endl;
    cout << "私钥: (" << d << ", " << n << ")" << endl;
}

// 保存公私钥的质数及模数到文件
void saveKeysToFile(long long p, long long q, long long n) {
    ofstream pubkeyFile("pubkey.txt");
    ofstream prikeyFile("prikey.txt");

    if (!pubkeyFile.is_open() || !prikeyFile.is_open()) {
        cerr << "无法打开文件。" << endl;
        return;
    }

    // 将公钥质数 p 和模数 n 写入 pubkey.txt
    pubkeyFile << p << " " << n << endl;

    // 将私钥质数 q 和模数 n 写入 prikey.txt
    prikeyFile << q << " " << n << endl;

    pubkeyFile.close();
    prikeyFile.close();

    cout << "公钥质数和私钥质数及模数已保存到 pubkey.txt 和 prikey.txt" << endl;
}

// 主函数
int main() {
    long long p, q, n, e, d;

    // 生成伪RSA密钥对
    generateKeys(p, q, n, e, d);

    // 保存公钥和私钥的质数及模数到文件
    saveKeysToFile(p, q, n);

    return 0;
}
