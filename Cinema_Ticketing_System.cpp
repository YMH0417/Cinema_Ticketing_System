#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
using namespace std;

// 快速幂模运算
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

// 计算欧拉函数
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
    return -1;
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
        char decryptedByte = static_cast<char>(modExp(encryptedByte, d, n));
        out.put(decryptedByte);
    }
    in.close();
    out.close();
}

// 解密多个文件
void decryptFiles(const string& pubkeyFile, const string& prikeyFile) {
    long long p, q, n, d;
    if (!readPubKey(pubkeyFile, p, n)) {
        return;
    }
    if (!readPrivKey(prikeyFile, q, n)) {
        return;
    }
    long long phi = eulerTotient(p, q);
    // 计算私钥 d
    d = modInverse(p, phi);
    if (d == -1) {
        cerr << "无法计算私钥。" << endl;
        return;
    }
    // 待解密的文件列表
    string inputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };
    string outputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };
    for (int i = 0; i < 3; ++i) {
        decryptFile(inputFiles[i], outputFiles[i], q, n, d);
    }
    cout << "数据文件已成功从缓存中加载。" << endl;
}

// 加密文件
void encryptFile(const string& inputFile, const string& outputFile, long long e, long long n) {
    ifstream in(inputFile, ios::binary);
    ofstream out(outputFile, ios::binary);
    if (!in.is_open() || !out.is_open()) {
        cerr << "无法打开文件：" << inputFile << endl;
        return;
    }
    char byte;
    while (in.get(byte)) {
        long long encryptedByte = modExp(byte, e, n);
        out.write(reinterpret_cast<char*>(&encryptedByte), sizeof(encryptedByte));
    }
    in.close();
    out.close();
}

// 加密多个文件
void encryptFiles(const string& pubkeyFile) {
    long long p, n;
    if (!readPubKey(pubkeyFile, p, n)) {
        return;
    }
    // 待加密的文件列表
    string inputFiles[] = { "seat.txt", "movie.txt", "ticket.txt" };
    string outputFiles[] = { "seat.bin", "movie.bin", "ticket.bin" };
    for (int i = 0; i < 3; ++i) {
        encryptFile(inputFiles[i], outputFiles[i], p, n);
    }
}

// 删除文件
void deleteFile(const string& filename) {
    if (remove(filename.c_str()) != 0) {
        cerr << "删除文件失败: " << filename << endl;
    }
}

// 座位状态常量
enum SeatStatus {
    AVAILABLE = 0,  // 可售
    SOLD = 1,       // 已售出
    DAMAGED = 2     // 不可用
};

// 电影信息类
class Movie {
public:
    int id;              // 序号
    string name;         // 电影名
    string timeSlot;     // 放映时段
    double price;        // 票价
    int hallId;          // 放映厅编号
    Movie(int i, string n, string t, double p, int h)
        : id(i), name(n), timeSlot(t), price(p), hallId(h) {
    }
};

// 售票系统
class CinemaSystem {
private:
    vector<Movie> movies;                  // 所有电影信息
    vector<vector<vector<int>>> ticketData; // 所有场次的售票信息
    void validateFileStream(ifstream& file, const string& filename) {
        if (!file.is_open()) {
            cerr << "无法打开文件: " << filename << endl;
            exit(EXIT_FAILURE);
        }
    }

public:
    // 初始化系统
    void initialize(const string& movieFile, const string& ticketFile) {
        loadMovieInfo(movieFile);
        loadTicketStatus(ticketFile);
    }

    // 加载影片信息
    void loadMovieInfo(const string& filename) {
        ifstream file(filename);
        validateFileStream(file, filename);
        int id, hallId;
        string name, timeSlot;
        double price;
        while (file >> id >> hallId >> name >> timeSlot >> price) {
            movies.emplace_back(id, name, timeSlot, price, hallId);
        }
        file.close();
    }

    // 加载售票信息
    void loadTicketStatus(const string& filename) {
        ifstream file(filename);
        validateFileStream(file, filename);
        int id, rows, cols;
        while (file >> id >> rows >> cols) {
            vector<vector<int>> matrix(rows, vector<int>(cols));
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    file >> matrix[r][c];
                }
            }
            ticketData.push_back(matrix);
        }
        file.close();
    }

    // 统计某部电影的票款
    double calculateRevenue(int movieId, bool printDetails = true) {
        if (movieId < 1 || movieId > static_cast<int>(movies.size())) {
            cerr << "无效的场次编号。" << endl;
            return 0;
        }
        double revenue = 0;
        const auto& movie = movies[movieId - 1];
        for (const auto& row : ticketData[movieId - 1]) {
            for (int seat : row) {
                if (seat == SOLD) {
                    revenue += movie.price;
                }
            }
        }
        if (printDetails) {
            cout << "单价:" << movie.price << " 售出:" << (int)(revenue / movie.price);
        }
        return revenue;
    }

    // 对当天票房排序
    void sortMoviesByRevenue() {
        vector<pair<double, Movie>> revenues;
        for (const auto& movie : movies) {
            double revenue = calculateRevenue(movie.id, false);
            revenues.emplace_back(revenue, movie);
        }
        sort(revenues.begin(), revenues.end(), [](const pair<double, Movie>& a, const pair<double, Movie>& b) {
            return a.first > b.first;
            });
        for (const auto& r : revenues) {
            cout << "影片:" << r.second.name
                << " 收入:" << fixed << setprecision(2) << r.first << " 元" << endl;
        }
    }

    // 显示座位情况
    void displaySeats(const vector<vector<int>>& seats) {
        int maxColWidth = to_string(seats[0].size()).length();
        cout << string(maxColWidth, ' ') << " ";
        for (size_t col = 0; col < seats[0].size(); ++col) {
            cout << col + 1 << string(3 - to_string(col + 1).length(), ' ');
        }
        cout << endl;
        for (size_t i = 0; i < seats.size(); ++i) {
            cout << i + 1 << string(maxColWidth - to_string(i + 1).length() + 1, ' ');
            for (int val : seats[i]) {
                if (val == AVAILABLE)
                    cout << "□  ";
                else if (val == SOLD)
                    cout << "●  ";
                else
                    cout << "X  ";
            }
            cout << endl;
        }
        cout << "●:已售出 □:可售 X:不可售" << endl;
    }

    // 查询某个放映厅的所有排片信息
    void queryHallSchedule(int hallId) {
        if (hallId < 1 || hallId > 5) {
            cerr << "无效的放映厅编号！" << endl;
            return;
        }
        cout << "-----第" << hallId << "放映厅排片信息-----" << endl;
        bool found = false;
        for (const auto& movie : movies) {
            if (movie.hallId == hallId) {
                cout << "序号:" << movie.id
                    << " 影片:" << movie.name
                    << " 时段:" << movie.timeSlot
                    << " 票价:" << fixed << setprecision(2) << movie.price << " 元" << endl;
                found = true;
            }
        }
        if (!found) {
            cout << "该放映厅暂无排片信息。" << endl;
        }
    }

    // 查询某场次的售票情况
    void queryTickets(int movieId) {
        if (movieId < 1 || movieId > static_cast<int>(movies.size())) {
            cerr << "无效的场次编号." << endl;
            return;
        }
        const auto& movie = movies[movieId - 1];
        cout << "-----第" << movie.hallId << "放映厅-----" << endl;
        cout << "放映时段:" << movie.timeSlot << endl;
        cout << "影片名:" << movie.name << endl;
        displaySeats(ticketData[movieId - 1]);
    }

    // 保存座位状态到文件
    void saveTicketStatus(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "无法打开文件: " << filename << endl;
            return;
        }
        int movieId = 0;
        for (const auto& matrix : ticketData) {
            int rows = matrix.size();
            int cols = matrix[0].size();
            movieId++;
            file << movieId << " " << rows << " " << cols << endl;
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    file << matrix[r][c] << " ";
                }
                file << endl;
            }
            file << endl;
        }
        file.close();
        cout << "座位状态已保存到文件。" << endl;
    }

    // 购票操作
    void buyTicket(int movieId, int row, int col) {
        if (movieId < 1 || movieId > static_cast<int>(ticketData.size())) {
            cerr << "无效的场次编号。" << endl;
            return;
        }
        auto& seat = ticketData[movieId - 1][row - 1][col - 1];
        if (seat == AVAILABLE) {
            seat = SOLD;
            cout << "购票成功。" << endl;
            saveTicketStatus("ticket.txt");
        }
        else {
            cout << "该座位无法购买。" << endl;
        }
    }

    // 退票操作
    void refundTicket(int movieId, int row, int col) {
        if (movieId < 1 || movieId > static_cast<int>(ticketData.size())) {
            cerr << "无效的场次编号。" << endl;
            return;
        }
        auto& seat = ticketData[movieId - 1][row - 1][col - 1];
        if (seat == SOLD) {
            seat = AVAILABLE;
            cout << "退票成功。" << endl;
            saveTicketStatus("ticket.txt");
        }
        else {
            cout << "该座位无法退票。" << endl;
        }
    }

    // 加载座位分布信息
    void loadSeatInfo(const string& filename) {
        ifstream file(filename);
        validateFileStream(file, filename);
        int rows, cols;
        int hallNumber = 1;
        while (file >> rows >> cols) {
            cout << "-----第" << hallNumber++ << "放映厅座位图-----" << endl;
            cout << "(" << rows << "x" << cols << "): " << endl;
            int maxColWidth = to_string(cols).length();
            cout << string(maxColWidth, ' ') << " ";
            for (int col = 0; col < cols; ++col) {
                cout << col + 1 << string(3 - to_string(col + 1).length(), ' ');
            }
            cout << endl;
            vector<vector<int>> seats(rows, vector<int>(cols));
            for (int r = 0; r < rows; r++) {
                cout << r + 1 << string(maxColWidth - to_string(r + 1).length() + 1, ' ');
                for (int c = 0; c < cols; c++) {
                    file >> seats[r][c];
                    if (seats[r][c] == AVAILABLE) {
                        cout << "□  ";
                    }
                    else {
                        cout << "X  ";
                    }
                }
                cout << endl;
            }
            cout << "□:正常 X:异常" << endl;
        }
        file.close();
    }
};

int main() {
    // 调用解密函数
    string pubkeyFile = "pubkey.txt";
    string prikeyFile = "prikey.txt";
    decryptFiles(pubkeyFile, prikeyFile);

    // 实现系统功能
    CinemaSystem system;
    system.initialize("movie.txt", "ticket.txt");
    int choice;
    do {
        cout << "=====影院售票系统菜单=====" << endl;
        cout << "[1] 获取每个放映厅的座位分布图" << endl;
        cout << "[2] 获取每个放映厅的排片信息" << endl;
        cout << "[3] 统计单部电影当天票款" << endl;
        cout << "[4] 当天票房排序" << endl;
        cout << "[5] 获取单部电影售票情况" << endl;
        cout << "[6] 购票" << endl;
        cout << "[7] 退票" << endl;
        cout << "[0] 退出系统" << endl;
        cout << "请选择功能:";
        cin >> choice;

        switch (choice) {
        case 1: {
            system.loadSeatInfo("seat.txt");
            break;
        }
        case 2: {
            for (int hallId = 1; hallId <= 5; hallId++)
                system.queryHallSchedule(hallId);
            break;
        }
        case 3: {
            int movieId;
            cout << "请输入电影编号:";
            cin >> movieId;
            cout << " 票款总计:" << fixed << setprecision(2)
                << system.calculateRevenue(movieId) << "元" << endl;
            break;
        }
        case 4: {
            system.sortMoviesByRevenue();
            break;
        }
        case 5: {
            int movieId;
            cout << "请输入电影编号:";
            cin >> movieId;
            system.queryTickets(movieId);
            break;
        }
        case 6: {
            for (int hallId = 1; hallId <= 5; hallId++)
                system.queryHallSchedule(hallId);
            int movieId, row, col;
            cout << "请输入电影编号:";
            cin >> movieId;
            system.queryTickets(movieId);
            cout << "请输入座位行号:";
            cin >> row;
            cout << "请输入座位列号:";
            cin >> col;
            system.buyTicket(movieId, row, col);
            encryptFiles(pubkeyFile);
            break;
        }
        case 7: {
            for (int hallId = 1; hallId <= 5; hallId++)
                system.queryHallSchedule(hallId);
            int movieId, row, col;
            cout << "请输入电影编号:";
            cin >> movieId;
            system.queryTickets(movieId);
            cout << "请输入座位行号:";
            cin >> row;
            cout << "请输入座位列号:";
            cin >> col;
            system.refundTicket(movieId, row, col);
            encryptFiles(pubkeyFile);
            break;
        }
        case 0:
            cout << "系统退出。" << endl;
            encryptFiles(pubkeyFile);
            deleteFile("seat.txt");
            deleteFile("movie.txt");
            deleteFile("ticket.txt");
            break;
        default:
            cout << "无效选择，请重新输入。" << endl;
            break;
        }
        char cls_help = getchar();
        cls_help = getchar();
        ::system("cls");
    } while (choice != 0);

    return 0;
}