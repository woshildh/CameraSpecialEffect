//
// Created by ldh on 2022/5/4.
//

#include "FireworkModel.h"
#include <stdlib.h>

FireworkModel::FireworkModel(int pts_num_) {
    pts_num = pts_num_;
    // 初始化数据
    init_data();
}

FireworkModel::~FireworkModel() {

}

void FireworkModel::update_data() {
    if(stage == 0) {
        pos_y += rise_speed;
        if(pos_y >= end_pos_y) {
            stage = 1;
        }
    } else if(stage == 1) {
        // 更新每个烟花的数据
        for(int i = 0; i < pts_num; ++i) {
            explode_data[i].update_data();
        }
        // 爆炸时间进行递减
        explode_time--;
        if(explode_time <= 0) {
            init_data();
        }
    }
}

std::vector<float> FireworkModel::get_rise_vert_data() {
    std::vector<float> vert_data;
    float step_y = rise_length / (rise_step_num * 2 - 1);
    for(int i = 0; i < rise_step_num * 2; ++i) {
        vert_data.push_back(pos_x);
        vert_data.push_back(pos_y - step_y * i);
    }
    return vert_data;
}

glm::vec3 FireworkModel::get_rise_color() {
    return rise_color;
}

float FireworkModel::get_rise_width() {
    return rise_width;
}

void FireworkModel::init_data() {
    stage = 0;
    // 随机初始化上升期的数据
    {
        srand(rand());
        pos_x = -1 + (rand() % 200) / 100.0;  // 初始x的位置在 (-1 , 1) 之间
        pos_y = -1.5 + (rand() % 50) / 100.0; // 初始y的位置 在 (-1.5, -1.0) 之间
        rise_speed = (rand() % 3 + 1) * 0.005;    // 上升期的速度在 (0.0025 ~ 0.01) 之间
        rise_length = (rand() % 4 + 2) * 0.02;        // 上升期烟花的长度在 (0.1, 0.3) 之间
        rise_width = 10 + rand() % 10;
        end_pos_y = 0.3 + (rand() % 50) / 200.0; // 结束位置在 [0.5, 0.75]之间
        rise_color.r = rand() % 255;  // 生成颜色 r
        rise_color.g = rand() % 255;  // 生成颜色 g
        rise_color.b = rand() % 255;  // 生成颜色 b
        rise_step_num = rand() % 2 + 4;
    }

    // 随机初始化爆炸期的数据
    {
        explode_time = rand() % 200 + 100;
        explode_data = std::vector<ExplodeAttribute>(pts_num);
        for(int i = 0; i < pts_num; ++i)
            explode_data[i].init_data(pos_x, end_pos_y);
    }
}

std::vector<float> FireworkModel::get_explode_color() {
    std::vector<float> verts_colors(pts_num * 3);
    for(int i = 0; i < pts_num; ++i) {
        verts_colors[i * 3] = explode_data[i].color.r;
        verts_colors[i * 3 + 1] = explode_data[i].color.g;
        verts_colors[i * 3 + 2] = explode_data[i].color.b;
    }
    return verts_colors;
}

std::vector<float> FireworkModel::get_explode_pos() {
    std::vector<float> verts_pos(pts_num * 2);
    for(int i = 0; i < pts_num; ++i) {
        verts_pos[i * 2] = explode_data[i].pos_x;
        verts_pos[i * 2 + 1] = explode_data[i].pos_y;
    }
    return verts_pos;
}

std::vector<float> FireworkModel::get_explode_radius() {
    std::vector<float> vert_radius(pts_num);
    for(int i = 0; i < pts_num; ++i) {
        vert_radius[i] = explode_data[i].radius;
    }
    return vert_radius;
}
