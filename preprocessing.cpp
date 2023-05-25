#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

Mat gen_obstacle(Mat tmpmap, Mat map, int obstacle_wide, int car_wide, int safe_distance, int obstacle_num_max) {
    int obstacle_num = 0;
    vector<int> row;
    vector<int> col;
    findNonZero(tmpmap == 0, row, col);
    while (obstacle_num < obstacle_num_max) {
        int idx = rand() % row.size();
        if (col[idx] < 100)//不能生成在初始位置
            continue;
        int safe_wide = obstacle_wide + car_wide + safe_distance;
        bool have_at_least_one_neigb_occ = false;//有一个被占据说明不安全，无法生成
        for (int l = row[idx] - safe_wide; l <= row[idx] + safe_wide; ++l) {
            if (l >= 600 || l < 0)
                continue;
            for (int m = col[idx] - safe_wide; m <= col[idx] + safe_wide; ++m) {
                if (m >= 1200 || m < 0)
                    continue;
                int this_neighbor_occ = tmpmap.at<uchar>(l, m);
                if (this_neighbor_occ == 1) {
                    have_at_least_one_neigb_occ = true;
                    break;
                }
            }
            if (have_at_least_one_neigb_occ)
                break;
        }
        if (!have_at_least_one_neigb_occ) {
            for (int l = row[idx] - obstacle_wide; l <= row[idx] + obstacle_wide; ++l) {
                if (l >= 600 || l < 0)
                    continue;
                for (int m = col[idx] - obstacle_wide; m <= col[idx] + obstacle_wide; ++m) {
                    if (m >= 1200 || m < 0)
                        continue;
                    map.at<uchar>(l, m) = 1;
                }
            }
            obstacle_num += 1;
            findNonZero(map == 0, row, col);
        }
    }
    return map;
}

Mat expand_race_track(Mat map, int index) {
    Mat new_map = map.clone();
    for (int i = 0; i < map.rows; ++i) {
        for (int j = 0; j < map.cols; ++j) {
            bool have_at_least_one_neigb_occ = false;
            int this_grid_occ = map.at<uchar>(i, j);
            if (this_grid_occ == 1)
                continue;
            for (int l = i - 1; l <= i + 1; ++l) {
                if (l >= 600 || l < 0)
                    continue;
                for (int m = j - 1; m <= j + 1; ++m) {
                    if (m >= 1200 || m < 0)
                        continue;
                    int this_neighbor_occ = map.at<uchar>(l, m);
                    if (this_neighbor_occ == 1) {
                        have_at_least_one_neigb_occ = true;
                        break;
                    }
                }
                if (have_at_least_one_neigb_occ)
                    break;
            }
            //根据have_at_least_one_neigb_occ的值决定膨胀与否
            if (have_at_least_one_neigb_occ) {
                for (int l = i - index; l <= i + index; ++l) {
                    if (l >= 600 || l < 0)
                        continue;
                    for (int m = j - index; m <= j + index; ++m) {
                        if (m >= 1200 || m < 0)
                            continue;
                        new_map.at<uchar>(l, m) = 0;
                    }
                }
            }
        }
    }
    return new_map;
}

Mat shrink_race_track(Mat map, int index) {
    Mat new_map = map.clone();
    for (int i = 0; i < map.rows; ++i) {
        for (int j = 0; j < map.cols; ++j) {
            bool have_at_least_one_neigb_occ = false;
            int this_grid_occ = map.at<uchar>(i, j);
            if (this_grid_occ == 1)
                continue;
            for (int l = i - 1; l <= i + 1; ++l) {
                if (l >= 600 || l < 0)
                    continue;
                for (int m = j - 1; m <= j + 1; ++m) {
                    if (m >= 1200 || m < 0)
                        continue;
                    int this_neighbor_occ = map.at<uchar>(l, m);
                    if (this_neighbor_occ == 1) {
                        have_at_least_one_neigb_occ = true;
                        break;
                    }
                }
                if (have_at_least_one_neigb_occ)
                    break;
            }
            //根据have_at_least_one_neigb_occ的值决定膨胀与否
            if (have_at_least_one_neigb_occ) {
                for (int l = i - index; l <= i + index; ++l) {
                    if (l >= 600 || l < 0)
                        continue;
                    for (int m = j - index; m <= j + index; ++m) {
                        if (m >= 1200 || m < 0)
                            continue;
                        new_map.at<uchar>(l, m) = 1;
                    }
                }
            }
        }
    }
    return new_map;
}

int main() {
    Mat img = imread("sysu6001200.png");
    Mat gray_img, sysu, map, out;
    cvtColor(img, gray_img, COLOR_RGB2GRAY);
    threshold(gray_img, sysu, 0, 255, THRESH_BINARY);
    map = sysu.clone();
    int car_long = 0.12;
    int car_wide = 0.06;
    int resolution = 0.01;
    Mat tmpmap = shrink_race_track(map, round(car_wide / resolution / 2));
    map = gen_obstacle(tmpmap, map, 12, round(car_wide / resolution), 20, 5);
    out = Mat::zeros(600, 1200, CV_8UC3);
    for (int i = 0; i < map.rows; ++i) {
        for (int j = 0; j < map.cols; ++j) {
            if (map.at<uchar>(i, j) == 0)
                out.at<Vec3b>(i, j)[0] = out.at<Vec3b>(i, j)[1] = out.at<Vec3b>(i, j)[2] = 255;
        }
    }
    imwrite("out.png", out);
    FileStorage f("sysu_standard.xml", FileStorage::WRITE);
    f << "map" << map;
    f << "out" << out;
    f.release();
    return 0;
}
