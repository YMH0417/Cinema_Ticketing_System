#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// 快速幂模运算
long long modExp(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) // 如果exp是奇数
            result = (result * base) % mod;

        exp = exp >> 1; // exp除以2
        base = (base * base) % mod;
    }
    return result;
}

// 从文件中读取公钥质数 p 和模数 n
bool readPubKey(const string& pubkeyFile, long long& p, long long& n) {
    ifstream in(pubkeyFile);
    if (!in.is_open()) {
        cerr << "无法打开公钥文件：" << pubkeyFile << endl;
        return false;
    }

    in >> p >> n;
    in.close();

    if (p <= 0 || n <= 0) {
        cerr << "公钥格式错误。" << endl;
        return false;
    }

    return true;
}

// 从文件中读取私钥质数 q 和模数 n
bool readPrivKey(const string& prikeyFile, long long& q, long long& n) {
    ifstream in(prikeyFile);
    if (!in.is_open()) {
        cerr << "无法打开私钥文件：" << prikeyFile << endl;
        return false;
    }

    in >> q >> n;
    in.close();

    if (q <= 0 || n <= 0) {
        cerr << "私钥格式错误。" << endl;
        return false;
    }

    return true;
}

// 计算欧拉函数 phi(n) = (p - 1)(q - 1)
long long eulerTotient(long long p, long long q) {
    return (p - 1) * (q - 1);
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

// 解密文件
void decryptFile(const string& inputFile, const string& outputFile, long long q, long long n, long long d) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open() || !out.is_open()) {
        cerr << "无法打开文件：" << inputFile << endl;
        return;
    }

    long long encryptedByte;
    while (in.read(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte))) {
        char decryptedByte = static_cast<char>(modExp(encryptedByte, d, n)); // 使用私钥解密
        out.put(decryptedByte);
    }

    in.close();
    out.close();

    cout << "文件解密完成: " << outputFile << endl;
}

// 主函数
int main() {
    long long p, q, n, d;
    string pubkeyFile = "pubkey.txt"; // 公钥文件
    string prikeyFile = "prikey.txt"; // 私钥文件

    // 从 pubkey.txt 中读取公钥
    if (!readPubKey(pubkeyFile, p, n)) {
        return 1; // 如果读取公钥失败，退出程序
    }

    // 从 prikey.txt 中读取私钥
    if (!readPrivKey(prikeyFile, q, n)) {
        return 1; // 如果读取私钥失败，退出程序
    }

    // 计算欧拉函数 φ(n)
    long long phi = eulerTotient(p, q);

    // 计算私钥 d
    d = modInverse(p, phi);

    if (d == -1) {
        cerr << "无法计算私钥 d" << endl;
        return 1;
    }

    cout << "私钥 d: " << d << endl;

    // 待解密的文件列表
    string inputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };
    string outputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };

    // 对每个文件进行解密
    for (int i = 0; i < 3; ++i) {
        decryptFile(inputFiles[i], outputFiles[i], q, n, d);
    }

    return 0;
}
