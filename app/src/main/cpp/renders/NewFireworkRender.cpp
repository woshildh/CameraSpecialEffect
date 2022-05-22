//
// Created by ldh on 2022/5/8.
//

#include "NewFireworkRender.h"
#include "../utils/PngTool.h"
#include <glm/gtc/type_ptr.hpp>

#define NEW_FIREWORK_VERT_SHADER_FILE "shaders/new_firework_vert.glsl"
#define NEW_FIREWORK_FRAG_SHADER_FILE "shaders/new_firework_frag.glsl"

NewFireworkRender::NewFireworkRender(int fire_num_) : fire_num(fire_num_) {
    // 初始化数据
    init_data();
}

NewFireworkRender::~NewFireworkRender() {
    glDeleteTextures(1, &rise_tex_id);
    glDeleteTextures(1, &explode_tex_id);
    glDeleteVertexArrays(1, &fm_vao_id);
    glDeleteBuffers(1, &fm_vbo_id);
}

void NewFireworkRender::render() {
    if(yuv == NULL) {
        return;
    }
    // 清理颜色缓冲区和深度缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 渲染帧
    render_frame();
    // 渲染烟花
    render_firework();
    // 保存图像
    save_picture();
}

void NewFireworkRender::init_data() {
    // 加载程序
    fm_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("NewFireworkRender", NEW_FIREWORK_VERT_SHADER_FILE,
             NEW_FIREWORK_FRAG_SHADER_FILE, asset_mgr));
    glUseProgram(fm_program->program_id);
    // 生成vao和vbo并且发送数据
    {
        // 顶点坐标数据
        float vert_buf[] = {
                -1.0, 1.0, 0.0, 0.0, // 左上
                -1.0, -1.0, 0.0, 1.0, // 左下
                1.0, -1.0, 1.0, 1.0, // 右下
                -1.0, 1.0, 0.0, 0.0,  // 左上
                1.0, 1.0, 1.0, 0.0, // 右上
                1.0, -1.0, 1.0, 1.0  // 右下
        };
        glGenVertexArrays(1, &fm_vao_id);
        glGenBuffers(1, &fm_vbo_id);
        glBindVertexArray(fm_vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, fm_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buf), vert_buf, GL_STATIC_DRAW);
    }
    // 获取统一变量位置
    {
        mvp_mat_loc = glGetUniformLocation(fm_program->program_id, "mvp_mat");
        color_loc = glGetUniformLocation(fm_program->program_id, "rise_color");
        stage_loc = glGetUniformLocation(fm_program->program_id, "stage");
    }
    // 生成纹理并且读取图片发送数据
    {
        glGenTextures(1, &rise_tex_id);
        glGenTextures(1, &explode_tex_id);
        // 读取png数据
        PNGData explode_png, rise_png;
        PngTool::loadPngFromAssets(asset_mgr, "resource/rise.png", &rise_png);
        PngTool::loadPngFromAssets(asset_mgr, "resource/explode.png", &explode_png);
        // 发送数据到缓冲 上升期
        glBindTexture(GL_TEXTURE_2D, rise_tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rise_png.width, rise_png.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rise_png.pixels);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // 发送数据到缓冲区 爆炸期
        glBindTexture(GL_TEXTURE_2D, explode_tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, explode_png.width, explode_png.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, explode_png.pixels);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    // 初始化烟花模型
    fm_data = std::vector<NewFireworkModel>(fire_num);
    // 打印调试信息
    LOGI("[NewFireworkRender] program_id: %d, fm_vao_id: %d, fm_vbo_id: %d,"
         " mvp_mat_loc: %d, color_loc: %d, stage_loc: %d, rise_tex_id: %d, explode_tex_id: %d, state: %d",
         fm_program->program_id, fm_vao_id, fm_vbo_id, mvp_mat_loc, color_loc, stage_loc, rise_tex_id, explode_tex_id, glGetError());
}

void NewFireworkRender::render_firework() {
    // 要开启混色模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(fm_program->program_id);
    for(int i = 0; i < fm_data.size(); ++i) {
        // 更新数据
        fm_data[i].update();
        // 获取数据并绘制
        glm::vec3 color = fm_data[i].get_rise_color();
        int stage = fm_data[i].get_stage();
        glm::mat4 mvp_mat = fm_data[i].get_mvp_mat();
        // 发送统一变量数据
        glUniform1i(stage_loc, stage);
        glUniform3fv(color_loc, 1, glm::value_ptr(color));
        glUniformMatrix4fv(mvp_mat_loc, 1, GL_FALSE, glm::value_ptr(mvp_mat));
        // 设置 vao 和 vbo 属性
        glBindVertexArray(fm_vao_id);
        glBindBuffer(GL_ARRAY_BUFFER, fm_vbo_id);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        // 绑定纹理
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, rise_tex_id);
        glActiveTexture(GL_TEXTURE0 + 4);
        glBindTexture(GL_TEXTURE_2D, explode_tex_id);
        // 进行绘制
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}
