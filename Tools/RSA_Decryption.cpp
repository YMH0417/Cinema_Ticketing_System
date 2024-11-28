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

long long eulerTotient(long long p, long long q) {
    return (p - 1) * (q - 1);
}

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

long long modInverse(long long a, long long m) {
    a = a % m;
    for (long long x = 1; x < m; ++x) {
        if ((a * x) % m == 1) return x;
    }
    return -1;
}

void decryptFile(const string& inputFile, const string& outputFile, long long q, long long n, long long d) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open() || !out.is_open()) {
        cerr << "无法打开文件：" << inputFile << endl;
        return;
    }

    long long encryptedByte;
    while (in.read(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte))) {
        char decryptedByte = static_cast<char>(modExp(encryptedByte, d, n));
        out.put(decryptedByte);
    }

    in.close();
    out.close();

    cout << "文件解密完成: " << outputFile << endl;
}

int main() {
    long long p, q, n, d;
    string pubkeyFile = "pubkey.txt";
    string prikeyFile = "prikey.txt";

    if (!readPubKey(pubkeyFile, p, n)) {
        return 1;
    }

    if (!readPrivKey(prikeyFile, q, n)) {
        return 1;
    }

    long long phi = eulerTotient(p, q);

    d = modInverse(p, phi);

    if (d == -1) {
        cerr << "无法计算私钥 d" << endl;
        return 1;
    }

    cout << "私钥 d: " << d << endl;

    string inputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };
    string outputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };

    for (int i = 0; i < 3; ++i) {
        decryptFile(inputFiles[i], outputFiles[i], q, n, d);
    }

    return 0;
}
