//
// Created by ldh on 2022/5/8.
//

#ifndef CAMERASPECIALEFFECT_NEWFIREWORKMODEL_H
#define CAMERASPECIALEFFECT_NEWFIREWORKMODEL_H

#include <vector>
#include <glm/glm.hpp>

class NewFireworkModel {
public:
    NewFireworkModel();
    ~NewFireworkModel();
    void update();
    // 获取当前的阶段
    int get_stage();
    // 获取当前的mvp矩阵
    glm::mat4 get_mvp_mat();
    // 获取上升期的颜色
    glm::vec3 get_rise_color();
private:
    // 初始化数据
    void init_data();
    // 当前阶段 0表示上升 1表示爆炸
    int stage;
    // 上升期的缩放系数
    float rise_scale;
    // 位置
    float rise_pos_x;      // 上升期的位置 x
    float rise_pos_y;      // 上升期的位置 y
    float rise_end_pos_y;  // 结束的位置 y
    float rise_step_y;     // 上升期的步长 y
    glm::vec3 rise_color;  //上升期的颜色
    // 爆炸期的缩放系数
    float explode_scale;  //爆炸期的缩放系数
    float explode_scale_step;  // 爆炸期的缩放系数的步长
    int explode_delay_time;   // 爆炸期的持续时间
};


#endif //CAMERASPECIALEFFECT_NEWFIREWORKMODEL_H
