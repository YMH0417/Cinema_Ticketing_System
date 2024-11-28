#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

// ������ģ����
long long modExp(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) // ���exp������
            result = (result * base) % mod;

        exp = exp >> 1; // exp����2
        base = (base * base) % mod;
    }
    return result;
}

// ���ļ��ж�ȡ��Կ���� p ��ģ�� n
bool readPubKey(const string& pubkeyFile, long long& p, long long& n) {
    ifstream in(pubkeyFile);
    if (!in.is_open()) {
        cerr << "�޷��򿪹�Կ�ļ���" << pubkeyFile << endl;
        return false;
    }

    in >> p >> n;
    in.close();

    if (p <= 0 || n <= 0) {
        cerr << "��Կ��ʽ����" << endl;
        return false;
    }

    return true;
}

// ���ļ��ж�ȡ˽Կ���� q ��ģ�� n
bool readPrivKey(const string& prikeyFile, long long& q, long long& n) {
    ifstream in(prikeyFile);
    if (!in.is_open()) {
        cerr << "�޷���˽Կ�ļ���" << prikeyFile << endl;
        return false;
    }

    in >> q >> n;
    in.close();

    if (q <= 0 || n <= 0) {
        cerr << "˽Կ��ʽ����" << endl;
        return false;
    }

    return true;
}

// ����ŷ������ phi(n) = (p - 1)(q - 1)
long long eulerTotient(long long p, long long q) {
    return (p - 1) * (q - 1);
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

// �����ļ�
void decryptFile(const string& inputFile, const string& outputFile, long long q, long long n, long long d) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open() || !out.is_open()) {
        cerr << "�޷����ļ���" << inputFile << endl;
        return;
    }

    long long encryptedByte;
    while (in.read(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte))) {
        char decryptedByte = static_cast<char>(modExp(encryptedByte, d, n)); // ʹ��˽Կ����
        out.put(decryptedByte);
    }

    in.close();
    out.close();

    cout << "�ļ��������: " << outputFile << endl;
}

// ������
int main() {
    long long p, q, n, d;
    string pubkeyFile = "pubkey.txt"; // ��Կ�ļ�
    string prikeyFile = "prikey.txt"; // ˽Կ�ļ�

    // �� pubkey.txt �ж�ȡ��Կ
    if (!readPubKey(pubkeyFile, p, n)) {
        return 1; // �����ȡ��Կʧ�ܣ��˳�����
    }

    // �� prikey.txt �ж�ȡ˽Կ
    if (!readPrivKey(prikeyFile, q, n)) {
        return 1; // �����ȡ˽Կʧ�ܣ��˳�����
    }

    // ����ŷ������ ��(n)
    long long phi = eulerTotient(p, q);

    // ����˽Կ d
    d = modInverse(p, phi);

    if (d == -1) {
        cerr << "�޷�����˽Կ d" << endl;
        return 1;
    }

    cout << "˽Կ d: " << d << endl;

    // �����ܵ��ļ��б�
    string inputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };
    string outputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };

    // ��ÿ���ļ����н���
    for (int i = 0; i < 3; ++i) {
        decryptFile(inputFiles[i], outputFiles[i], q, n, d);
    }

    return 0;
}
