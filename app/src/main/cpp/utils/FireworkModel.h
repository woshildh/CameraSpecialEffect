//
// Created by ldh on 2022/5/4.
//

#ifndef CAMERASPECIALEFFECT_FIREWORKMODEL_H
#define CAMERASPECIALEFFECT_FIREWORKMODEL_H

#define LDH_PI 3.1415

#include <vector>
#include <glm/glm.hpp>
#include <stdlib.h>

struct ExplodeAttribute {
    ExplodeAttribute() {
    }
    ~ExplodeAttribute() {

    }
    void init_data(float pos_x_, float pos_y_) {
        srand(rand());
        radius = (rand() % 10) + 5;
        orientation = LDH_PI * (rand() % 360) / 180.0;
        pos_x = pos_x_;
        pos_y = pos_y_;
        speed = (rand() % 4 + 1) * 0.001;
        color.r = rand() % 255;
        color.g = rand() % 255;
        color.b = rand() % 255;
    }
    // 更新当前的数据
    void update_data() {
        pos_x = pos_x + cos(orientation) * speed;
        pos_y = pos_y + sin(orientation) * speed;
    }
    // 爆炸点的半径
    float radius;
    // 点的位置
    float pos_x;
    float pos_y;
    // 爆炸点的方向
    float orientation;
    // 运动点的速度
    float speed;
    // 颜色
    glm::vec3 color;
};

class FireworkModel
{

public:
    FireworkModel(int pts_num_ = 100);
    ~FireworkModel();
    // 更新数据
    void update_data();
    // 获取当前阶段
    int get_stage() {
        return stage;
    }
    // 获取上升期的顶点数据
    std::vector<float> get_rise_vert_data();
    // 获取上升期的颜色
    glm::vec3  get_rise_color();
    // 获取上升期的宽度
    float get_rise_width();
    // 获取爆炸期所有顶点的颜色
    std::vector<float> get_explode_color();
    // 获取爆炸期所有点的位置
    std::vector<float> get_explode_pos();
    // 获取爆炸期所有点的半径
    std::vector<float> get_explode_radius();
private:
    // 阶段 0表示上升阶段 1表示爆炸阶段
    int stage;
    // 初始化数据
    void init_data();
    // 上升阶段的各种属性
    float pos_x;   // x方向的坐标
    float pos_y;   // y方向的坐标
    float rise_speed;   // 上升期的速度
    float rise_length;     // 烟花的长度
    float rise_width;      // 烟花的宽度
    float end_pos_y;  // 结束的高度
    glm::vec3 rise_color;  // 烟花的颜色
    int rise_step_num;  // 上升期烟花的段数
    // 爆炸期的属性
    int pts_num;   // 爆炸点的个数
    int explode_time;  // 爆炸的持续时间长度
    std::vector<ExplodeAttribute> explode_data;
};

#endif //CAMERASPECIALEFFECT_FIREWORKMODEL_H
