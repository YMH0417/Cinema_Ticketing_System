#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;

bool isPrime(long long num) {
    if (num < 2) return false;
    for (long long i = 2; i <= sqrt(num); ++i) {
        if (num % i == 0) return false;
    }
    return true;
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

void generateKeys(long long& p, long long& q, long long& n, long long& e, long long& d) {
    srand(static_cast<unsigned>(time(nullptr)));
    do {
        p = rand() % 100000000 + 1000;
    } while (!isPrime(p));
    do {
        q = rand() % 100000000 + 1000;
    } while (!isPrime(q) || p == q);
    n = p * q;
    long long phi = (p - 1) * (q - 1);
    do {
        e = rand() % (phi - 2) + 2;
    } while (gcd(e, phi) != 1);
    d = modInverse(e, phi);
    cout << "公钥: (" << e << ", " << n << ")" << endl;
    cout << "私钥: (" << d << ", " << n << ")" << endl;
}

void saveKeysToFile(long long p, long long q, long long n) {
    ofstream pubkeyFile("pubkey.txt");
    ofstream prikeyFile("prikey.txt");
    if (!pubkeyFile.is_open() || !prikeyFile.is_open()) {
        cerr << "无法打开文件。" << endl;
        return;
    }
    pubkeyFile << p << " " << n << endl;
    prikeyFile << q << " " << n << endl;
    pubkeyFile.close();
    prikeyFile.close();
    cout << "公钥质数和私钥质数及模数已保存到 pubkey.txt 和 prikey.txt" << endl;
}

int main() {
    long long p, q, n, e, d;
    generateKeys(p, q, n, e, d);
    saveKeysToFile(p, q, n);
    return 0;
}
