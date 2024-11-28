#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// �ж�һ�����Ƿ�Ϊ����
bool isPrime(long long num) {
    if (num < 2) return false;
    for (long long i = 2; i <= sqrt(num); ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

// �����Լ��
long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

// ����ģ��
long long modInverse(long long a, long long m) {
    a = a % m;
    for (long long x = 1; x < m; ++x) {
        if ((a * x) % m == 1) return x;
    }
    return -1; // ������ģ��
}

// ����αRSA��Կ��
void generateKeys(long long& p, long long& q, long long& n, long long& e, long long& d) {
    // ���������������
    srand(static_cast<unsigned>(time(nullptr)));
    do {
        p = rand() % 9000 + 1000; // ���ɷ�Χ�ϴ������
    } while (!isPrime(p));

    do {
        q = rand() % 9000 + 1000;
    } while (!isPrime(q) || p == q);

    n = p * q; // ģ��
    long long phi = (p - 1) * (q - 1); // ŷ������

    // ѡ��Կe (1 < e < phi �� gcd(e, phi) = 1)
    do {
        e = rand() % (phi - 2) + 2; // e �� [2, phi-1]
    } while (gcd(e, phi) != 1);

    // ����˽Կd (e * d �� 1 mod phi)
    d = modInverse(e, phi);

    // �����Կ��˽Կ
    cout << "��Կ: (" << e << ", " << n << ")" << endl;
    cout << "˽Կ: (" << d << ", " << n << ")" << endl;
}

// ���湫˽Կ��������ģ�����ļ�
void saveKeysToFile(long long p, long long q, long long n) {
    ofstream pubkeyFile("pubkey.txt");
    ofstream prikeyFile("prikey.txt");

    if (!pubkeyFile.is_open() || !prikeyFile.is_open()) {
        cerr << "�޷����ļ���" << endl;
        return;
    }

    // ����Կ���� p ��ģ�� n д�� pubkey.txt
    pubkeyFile << p << " " << n << endl;

    // ��˽Կ���� q ��ģ�� n д�� prikey.txt
    prikeyFile << q << " " << n << endl;

    pubkeyFile.close();
    prikeyFile.close();

    cout << "��Կ������˽Կ������ģ���ѱ��浽 pubkey.txt �� prikey.txt" << endl;
}

// ������
int main() {
    long long p, q, n, e, d;

    // ����αRSA��Կ��
    generateKeys(p, q, n, e, d);

    // ���湫Կ��˽Կ��������ģ�����ļ�
    saveKeysToFile(p, q, n);

    return 0;
}
