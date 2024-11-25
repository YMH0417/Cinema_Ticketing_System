#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

using namespace std;

// 座位状态常量
enum SeatStatus {
    AVAILABLE = 0,  // 可售
    SOLD = 1,       // 已售出
    DAMAGED = 2     // 损坏
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

    // 保存售票信息
    void saveTicketStatus(const string& filename) {
        ofstream file(filename);
        if (!file) {
            cerr << "无法保存售票情况到文件: " << filename << endl;
            return;
        }

        for (size_t i = 0; i < ticketData.size(); i++) {
            file << i + 1 << " " << ticketData[i].size() << " " << ticketData[i][0].size() << endl;
            for (const auto& row : ticketData[i]) {
                for (int val : row) {
                    file << val << " ";
                }
                file << endl;
            }
            file << endl;
        }
        file.close();
    }

    // 查询某个放映厅的所有排片信息
    void queryHallSchedule(int hallId) {
        if (hallId < 1) {
            cerr << "无效的放映厅编号！" << endl;
            return;
        }

        cout << "----- 第" << hallId << "放映厅排片信息 -----" << endl;
        bool found = false;
        for (const auto& movie : movies) {
            if (movie.hallId == hallId) {
                cout << "序号: " << movie.id
                    << ", 影片: " << movie.name
                    << ", 时段: " << movie.timeSlot
                    << ", 票价: " << fixed << setprecision(2) << movie.price << " 元" << endl;
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
        cout << "----- 第" << movie.hallId << "放映厅 -----" << endl;
        cout << "放映时段: " << movie.timeSlot << endl;
        cout << "影片名: " << movie.name << endl;
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
        cout << "●: 已售出, □: 可售, X: 损坏" << endl;
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
            saveTicketStatus("ticket.txt");
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
            saveTicketStatus("ticket.txt");
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
        return revenue;
    }

    // 对当天票房排序
    void sortMoviesByRevenue() {
        vector<pair<double, Movie>> revenues;
        for (const auto& movie : movies) {
            double revenue = calculateRevenue(movie.id);
            revenues.push_back(make_pair(revenue, movie));
        }

        // 排序
        sort(revenues.begin(), revenues.end(), [](const pair<double, Movie>& a, const pair<double, Movie>& b) {
            return a.first > b.first; // 按票房从高到低排序
            });

        // 输出结果
        cout << "当天票房排序：" << endl;
        for (size_t i = 0; i < revenues.size(); ++i) {
            cout << "影片: " << revenues[i].second.name
                << ", 收入: " << revenues[i].first << " 元" << endl;
        }
    }
};

int main() {
    CinemaSystem system;
    system.initialize("movie.txt", "ticket.txt");

    // 示例功能调用
    system.queryHallSchedule(1);  // 查询第1放映厅的所有排片信息
    system.queryTickets(1);       // 查询第1场次的售票情况
    system.buyTicket(1, 2, 3);    // 第1场次购票
    system.queryTickets(1);       // 再次查看售票情况
    system.refundTicket(1, 2, 2); // 退票
    system.queryTickets(1);       // 再次查看售票情况
    cout << "《MovieA》的票款: " << system.calculateRevenue(1) << " 元" << endl;
    system.sortMoviesByRevenue(); // 排序票房

    return 0;
}
