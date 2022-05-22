//
// Created by ldh on 2022/5/8.
//

#include "NewFireworkModel.h"
#include <glm/gtc/matrix_transform.hpp>

NewFireworkModel::NewFireworkModel() {
    // 初始化数据
    init_data();
}

NewFireworkModel::~NewFireworkModel() {
}

void NewFireworkModel::update() {
    if(stage == 0) {
        rise_pos_y += rise_step_y;
        if(rise_pos_y >= rise_end_pos_y) {
            stage = 1;
        }
    } else {
        explode_scale += explode_scale_step;
        explode_delay_time--;
        // 如果爆炸的时间过了就重新初始化数据
        if(explode_delay_time <= 0) {
            init_data();
        }
    }
}

int NewFireworkModel::get_stage() {
    return stage;
}

glm::vec3 NewFireworkModel::get_rise_color() {
    return rise_color;
}

void NewFireworkModel::init_data() {
    // 阶段设置为上升期
    stage = 0;
    // 初始化上升期数据
    {
        srand(rand());
        rise_scale = 0.02 + 0.002 * (rand() % 5);
        rise_pos_x = -0.8 + (rand() % 50) * (1.6 / 50);
        rise_pos_y = -1.5 + rand() % 10 * 0.05;
        rise_step_y = 0.005 + rand() % 10 * 0.001;
        rise_end_pos_y = 0.5 + (rand() % 10) * 0.03;
        rise_color.r = rand() % 200 + 50;
        rise_color.g = rand() % 200 + 50;
        rise_color.b = rand() % 200 + 50;
    }
    // 初始化爆炸期数据
    {
        explode_delay_time = 80 + (rand() % 70);
        explode_scale = 0.00;
        explode_scale_step = 0.002 + rand() % 3 * 0.001;
    }
}

glm::mat4 NewFireworkModel::get_mvp_mat() {
    glm::mat4 mvp_mat;
    if(stage == 0) {
        // 先进行缩放
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(rise_scale, rise_scale, 1.0f));
        // 进行平移
        glm::mat4 trans_mat = glm::translate(glm::mat4(1.0), glm::vec3(rise_pos_x, rise_pos_y, 0.0f));
        // 组合
        mvp_mat = trans_mat * scale_mat;
    } else {
        // 先进行缩放
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0), glm::vec3(explode_scale, explode_scale, 1.0f));
        // 进行平移
        glm::mat4 trans_mat = glm::translate(glm::mat4(1.0), glm::vec3(rise_pos_x, rise_pos_y, 0.0f));
        // 组合
        mvp_mat = trans_mat * scale_mat;
    }
    return mvp_mat;
}
