#include <iostream>
#include <vector>
#include <random>

using namespace std;

// 定义二维坐标点结构体
struct Point {
    int x;
    int y;

    Point(int x_, int y_) : x(x_), y(y_) {}
};

int main() {
    // 生成初始地图
    const int map_width = 1200;
    const int map_height = 600;
    vector<vector<int>> map(map_height, vector<int>(map_width, 0));

    // 生成障碍物
    const int obstacle_wide = 12; // 障碍物宽度
    const int safe_distance = 20; // 安全距离
    const int car_wide = 6; // 车辆宽度

    vector<Point> obstacle_list; // 障碍物列表
    int obstacle_num_max = 5; // 障碍物最大数目

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> disx(obstacle_wide / 2, map_width - obstacle_wide / 2);
    uniform_int_distribution<> disy(obstacle_wide / 2, map_height - obstacle_wide / 2);

    while (obstacle_list.size() < obstacle_num_max) {
        int x = disx(gen); // 随机生成障碍物中心横坐标
        int y = disy(gen); // 随机生成障碍物中心纵坐标

        bool safe = true; // 记录该位置是否安全

        for (auto& obstacle : obstacle_list) {
            double dx = x - obstacle.x;
            double dy = y - obstacle.y;
            // 判断障碍物间距是否大于安全距离
            if (dx * dx + dy * dy < (obstacle_wide + car_wide + safe_distance) * (obstacle_wide + car_wide + safe_distance)) {
                safe = false;
                break;
            }
        }

        if (safe) {
            obstacle_list.emplace_back(x, y);
            for (int i = y - obstacle_wide / 2; i <= y + obstacle_wide / 2; ++i) {
                for (int j = x - obstacle_wide / 2; j <= x + obstacle_wide / 2; ++j) {
                    map[i][j] = 1; // 将障碍物区域标记为1
                }
            }
        }
    }

    // 输出地图
    for (int i = 0; i < map_height; ++i) {
        for (int j = 0; j < map_width; ++j) {
            cout << map[i][j];
        }
        cout << endl;
    }

    return 0;
}
