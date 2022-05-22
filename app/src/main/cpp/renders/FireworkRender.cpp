//
// Created by ldh on 2022/5/4.
//


#include "FireworkRender.h"
#include <glm/gtc/type_ptr.hpp>

#define FIREWORK_RISE_VERT_SHADER_FILE "shaders/firework_rise_vert.glsl"
#define FIREWORK_RISE_FRAG_SHADER_FILE "shaders/firework_rise_frag.glsl"
#define FIREWORK_EXPLODE_VERT_SHADER_FILE "shaders/firework_expode_vert.glsl"
#define FIREWORK_EXPLODE_FRAG_SHADER_FILE "shaders/firework_expode_frag.glsl"

FireworkRender::FireworkRender(int fire_num) : BaseVideoRender(false), fm(fire_num) {
    // 初始化相关数据
    init_firework_data();
}

FireworkRender::~FireworkRender() {
    // 释放 OpenGL 资源
    glDeleteVertexArrays(1, &rise_vao_id);
    glDeleteVertexArrays(1, &explode_vao_id);
    glDeleteBuffers(1, &rise_vbo_id);
    glDeleteBuffers(1, &explode_color_vbo_id);
    glDeleteBuffers(1, &explode_pos_vbo_id);
    glDeleteBuffers(1, &explode_radius_vbo_id);
}

void FireworkRender::render() {
    // 清除颜色
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 渲染帧
    render_frame();
    // 渲染烟花
    render_fireworks();
    // 保存图像
    save_picture();
}

void FireworkRender::init_firework_data() {
    // 加载程序
    fw_rise_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("FireworkRender",
              FIREWORK_RISE_VERT_SHADER_FILE, FIREWORK_RISE_FRAG_SHADER_FILE, asset_mgr));
    fw_explode_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("FireworkRender",
              FIREWORK_EXPLODE_VERT_SHADER_FILE, FIREWORK_EXPLODE_FRAG_SHADER_FILE, asset_mgr));
    // 获取上升程序的所有统一变量属性位置，申请vao和vbo
    {
        glUseProgram(fw_rise_program->program_id);
        // 申请vao和vbo
        glGenVertexArrays(1, &rise_vao_id);
        glGenBuffers(1, &rise_vbo_id);
        // 获取统一变量位置
        rise_color_loc = glGetUniformLocation(fw_rise_program->program_id, "color");
        rise_width_loc = glGetUniformLocation(fw_rise_program->program_id, "width");
        // 打印调试
        LOGI("[FireworkRender] rise state vao id: %d, vbo_id: %d,  rise_color_loc: %d, rise_width_loc: %d, state: %d\n",
             rise_vao_id, rise_vbo_id, rise_color_loc, rise_width_loc, glGetError());
    }
    // 申请爆炸期的 vao 和 vbo
    {
        glUseProgram(fw_explode_program->program_id);
        // 申请vao和vbo
        glGenVertexArrays(1, &explode_vao_id);
        glGenBuffers(1, &explode_color_vbo_id);
        glGenBuffers(1, &explode_pos_vbo_id);
        glGenBuffers(1, &explode_radius_vbo_id);
        LOGI("[FireworkRender] explode state vao id: %d, vbo_id: {%d, %d, %d}, state: %d\n",
             explode_vao_id, explode_pos_vbo_id, explode_color_vbo_id, explode_radius_vbo_id, glGetError());
    }
}

void FireworkRender::render_rise_stage(int idx) {
    if(fm.size() <= idx) {
        return;
    }
    glUseProgram(fw_rise_program->program_id);
    // 发送统一变量
    glUniform3fv(rise_color_loc, 1, glm::value_ptr(fm[idx].get_rise_color()));
    glUniform1f(rise_width_loc, fm[idx].get_rise_width());
    // 发送顶点数据
    glBindVertexArray(rise_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, rise_vbo_id);
    std::vector<float> vert_data = fm[idx].get_rise_vert_data();
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vert_data.size(), vert_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    // 进行绘制
    glLineWidth(fm[idx].get_rise_width());
    glDrawArrays(GL_LINES, 0, vert_data.size());
}

void FireworkRender::render_explode_stage(int idx) {
    if(fm.size() <= idx) {
        return;
    }
    std::vector<float> verts_pos = fm[idx].get_explode_pos();
    std::vector<float> verts_color = fm[idx].get_explode_color();
    std::vector<float> verts_radius = fm[idx].get_explode_radius();
    glUseProgram(fw_explode_program->program_id);
    // 发送数据
    {
        glBindVertexArray(explode_vao_id);
        // 发送位置数据
        glBindBuffer(GL_ARRAY_BUFFER, explode_pos_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts_pos.size(), verts_pos.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        // 发送颜色数据
        glBindBuffer(GL_ARRAY_BUFFER, explode_color_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts_color.size(), verts_color.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(3);
        // 发送半径数据
        glBindBuffer(GL_ARRAY_BUFFER, explode_radius_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verts_radius.size(), verts_radius.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(4);
    }
    glDrawArrays(GL_POINTS, 0, verts_radius.size());
}

void FireworkRender::render_fireworks() {
    // 要开启混色模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(int idx = 0; idx < fm.size(); ++idx) {
        fm[idx].update_data();
        if(fm[idx].get_stage() == 0) {
            render_rise_stage(idx);
        } else if(fm[idx].get_stage() == 1) {
            render_explode_stage(idx);
        }
    }
}
