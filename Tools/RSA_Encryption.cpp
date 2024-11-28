#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

long long modExp(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;

        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

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

void encryptFile(const string& inputFile, const string& outputFile, long long p, long long n) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open() || !out.is_open()) {
        cerr << "无法打开文件：" << inputFile << endl;
        return;
    }

    char byte;
    while (in.get(byte)) {
        long long encryptedByte = modExp(static_cast<unsigned char>(byte), p, n);
        out.write(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte));
    }

    in.close();
    out.close();

    cout << "文件加密完成: " << outputFile << endl;
}

int main() {
    long long p, n;
    string pubkeyFile = "pubkey.txt";

    if (!readPubKey(pubkeyFile, p, n)) {
        return 1;
    }

    string inputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };
    string outputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };

    for (int i = 0; i < 3; ++i) {
        encryptFile(inputFiles[i], outputFiles[i], p, n);
    }

    return 0;
}
