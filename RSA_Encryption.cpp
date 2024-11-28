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

// 加密文件
void encryptFile(const string& inputFile, const string& outputFile, long long p, long long n) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open() || !out.is_open()) {
        cerr << "无法打开文件：" << inputFile << endl;
        return;
    }

    char byte;
    while (in.get(byte)) {
        long long encryptedByte = modExp(static_cast<unsigned char>(byte), p, n); // 使用公钥加密
        out.write(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte));
    }

    in.close();
    out.close();

    cout << "文件加密完成: " << outputFile << endl;
}

// 主函数
int main() {
    long long p, n;
    string pubkeyFile = "pubkey.txt"; // 公钥文件

    // 从 pubkey.txt 中读取公钥质数 p 和模数 n
    if (!readPubKey(pubkeyFile, p, n)) {
        return 1; // 如果读取公钥失败，退出程序
    }

    // 待加密的文件列表
    string inputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };
    string outputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };

    // 对每个文件进行加密
    for (int i = 0; i < 3; ++i) {
        encryptFile(inputFiles[i], outputFiles[i], p, n);
    }

    return 0;
}
