#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
using namespace std;

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
            exit(EXIT_FAILURE);  // 确保程序在读取文件失败时退出
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

    // 加载座位分布信息
    void loadSeatInfo(const string& filename) {
        ifstream file(filename);
        validateFileStream(file, filename);

        int rows, cols;
        int hallNumber = 1;

        while (file >> rows >> cols) {
            cout << "-----第" << hallNumber++ << "放映厅座位图-----" << endl;
            cout << "(" << rows << "x" << cols << "): " << endl;
            vector<vector<int>> seats(rows, vector<int>(cols));
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    file >> seats[r][c];
                    if (seats[r][c] == AVAILABLE) {
                        cout << "□";
                    }
                    else {
                        cout << "X";
                    }
                }
                cout << endl;
            }
            cout << "□:正常 X:异常" << endl;
        }
        file.close();
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
            cout << "该放映厅暂无排片信息！" << endl;
        }
    }

    // 查询某场次的售票情况
    void queryTickets(int movieId) {
        if (movieId < 1 || movieId > static_cast<int>(movies.size())) {
            cerr << "无效的场次编号！" << endl;
            return;
        }

        const auto& movie = movies[movieId - 1];
        cout << "-----第" << movie.hallId << "放映厅-----" << endl;
        cout << "放映时段:" << movie.timeSlot << endl;
        cout << "影片名:" << movie.name << endl;
        displaySeats(ticketData[movieId - 1]);
    }

    // 显示座位情况
    void displaySeats(const vector<vector<int>>& seats) {
        for (const auto& row : seats) {
            for (int val : row) {
                if (val == AVAILABLE)
                    cout << "□";
                else if (val == SOLD)
                    cout << "●";
                else
                    cout << "X";
            }
            cout << endl;
        }
        cout << "●:已售出 □:可售 X:不可售" << endl;
    }

    // 购票
    void buyTicket(int movieId, int row, int col) {
        if (movieId < 1 || movieId > static_cast<int>(ticketData.size())) {
            cerr << "无效的场次编号！" << endl;
            return;
        }

        auto& seat = ticketData[movieId - 1][row - 1][col - 1];
        if (seat == AVAILABLE) {
            seat = SOLD;
            cout << "购票成功！" << endl;
        }
        else {
            cout << "该座位无法购买！" << endl;
        }
    }

    // 退票
    void refundTicket(int movieId, int row, int col) {
        if (movieId < 1 || movieId > static_cast<int>(ticketData.size())) {
            cerr << "无效的场次编号！" << endl;
            return;
        }

        auto& seat = ticketData[movieId - 1][row - 1][col - 1];
        if (seat == SOLD) {
            seat = AVAILABLE;
            cout << "退票成功！" << endl;
        }
        else {
            cout << "该座位无法退票！" << endl;
        }
    }

    // 统计某部电影的票款
    double calculateRevenue(int movieId) {
        if (movieId < 1 || movieId > static_cast<int>(movies.size())) {
            cerr << "无效的场次编号！" << endl;
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
        cout << "单价:" << movie.price << " 售出:" << revenue / movie.price;
        return revenue;
    }

    // 对当天票房排序
    void sortMoviesByRevenue() {
        vector<pair<double, Movie>> revenues;
        for (const auto& movie : movies) {
            double revenue = calculateRevenue(movie.id);
            revenues.emplace_back(revenue, movie);
        }

        sort(revenues.begin(), revenues.end(), [](const pair<double, Movie>& a, const pair<double, Movie>& b) {
            return a.first > b.first;
            });

        cout << "当天票房排序" << endl;
        for (const auto& r : revenues) {
            cout << "影片:" << r.second.name
                << " 收入:" << fixed << setprecision(2) << r.first << " 元" << endl;
        }
    }
};

int main() {
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
            for (int hallId =1; hallId <=5; hallId++)
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
            int movieId, row, col;
            cout << "请输入电影编号:";
            cin >> movieId;
            cout << "请输入座位行号:";
            cin >> row;
            cout << "请输入座位列号:";
            cin >> col;
            system.buyTicket(movieId, row, col);
            break;
        }
        case 7: {
            int movieId, row, col;
            cout << "请输入电影编号:";
            cin >> movieId;
            cout << "请输入座位行号:";
            cin >> row;
            cout << "请输入座位列号:";
            cin >> col;
            system.refundTicket(movieId, row, col);
            break;
        }
        case 0:
            cout << "系统退出！" << endl;
            break;
        default:
            cout << "无效选择，请重新输入！" << endl;
            break;
        }
        char cls_help=getchar();
        cls_help = getchar();
        ::system("cls");
    } while (choice != 0);

    return 0;
}
