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

// �����ļ�
void encryptFile(const string& inputFile, const string& outputFile, long long p, long long n) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);

    if (!in.is_open() || !out.is_open()) {
        cerr << "�޷����ļ���" << inputFile << endl;
        return;
    }

    char byte;
    while (in.get(byte)) {
        long long encryptedByte = modExp(static_cast<unsigned char>(byte), p, n); // ʹ�ù�Կ����
        out.write(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte));
    }

    in.close();
    out.close();

    cout << "�ļ��������: " << outputFile << endl;
}

// ������
int main() {
    long long p, n;
    string pubkeyFile = "pubkey.txt"; // ��Կ�ļ�

    // �� pubkey.txt �ж�ȡ��Կ���� p ��ģ�� n
    if (!readPubKey(pubkeyFile, p, n)) {
        return 1; // �����ȡ��Կʧ�ܣ��˳�����
    }

    // �����ܵ��ļ��б�
    string inputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };
    string outputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };

    // ��ÿ���ļ����м���
    for (int i = 0; i < 3; ++i) {
        encryptFile(inputFiles[i], outputFiles[i], p, n);
    }

    return 0;
}
