//
// Created by ldh on 2022/5/3.
//

#include "GlassBallRender.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

#define GLASSBALL_VERT_SHADER_FILE "shaders/glassball_vert.glsl"
#define GLASSBALL_FRAG_SHADER_FILE "shaders/glassball_frag.glsl"

GlassBallRender::GlassBallRender() {
    init_glassball_data();
}

GlassBallRender::~GlassBallRender() {
    // 释放相关的数据
    glDeleteBuffers(1, &vert_vbo_id);
    glDeleteBuffers(1, &tex_vbo_id);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteTextures(1, &tex_id);
}

void GlassBallRender::init_glassball_data() {
    // 加载3D对象
    model = std::shared_ptr<ImportedModel>(new ImportedModel(asset_mgr, "resource/rose.obj"));
    LOGI("[ModelShow] Model vertices num: %d", model->get_vetices_num());
    // 加载png图像
    PngTool::loadPngFromAssets(asset_mgr, "resource/rose.png", &png_data);
    LOGI("[GlassBallRender] Png data: %d, %d, %d", png_data.width, png_data.height, png_data.depth);
    // 加载程序
    glassball_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("GlassBallRender", GLASSBALL_VERT_SHADER_FILE, GLASSBALL_FRAG_SHADER_FILE, asset_mgr));
    // 初始化纹理相关数据
    glUseProgram(glassball_program->program_id);
    // 获取统一变量的位置
    trans_mat_loc = glGetUniformLocation(glassball_program->program_id, "transform_mat");
    // 加载数据
    auto verts_model = model->get_vertices();
    auto tex_model = model->get_texture_coords();
    vert_num = model->get_vetices_num();
    vector<float> vertices;
    vector<float> tex_coord;
    vertices.reserve(vert_num * 3);
    tex_coord.reserve(vert_num * 2);
    for(int i = 0; i < vert_num; ++i) {
        vertices.push_back(verts_model[i].x);
        vertices.push_back(verts_model[i].y);
        vertices.push_back(verts_model[i].z);

        tex_coord.push_back(tex_model[i].s);
        tex_coord.push_back(tex_model[i].t);
    }
    // 生成vao
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    // 生成vbo并发送数据
    glGenBuffers(1, &vert_vbo_id);
    glGenBuffers(1, &tex_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tex_coord.size(), tex_coord.data(), GL_STATIC_DRAW);
    // 生成纹理并发送数据
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, png_data.width, png_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png_data.pixels);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LOGI("[GlassBallRender] vao_id: %d, vbo_id: %d %d, tex_id: %d, trans_mat_loc: %d",
          vao_id, vert_vbo_id, tex_vbo_id, tex_id, trans_mat_loc, glGetError());
}

void GlassBallRender::render() {
    if(yuv == NULL) {
        return;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 先渲染视频帧
    BaseVideoRender::render_frame();
    // 渲染3D对象
    render_glassball();
    // 保存当前图像
    save_picture();
}

void GlassBallRender::render_glassball() {
    // 允许深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // 允许混色
    // 要开启混色模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 使用3D对象的程序
    glUseProgram(glassball_program->program_id);
    // 进行旋转, 绕着y轴转
    rotate_angle += 0.02;
    glm::mat4 pers = glm::perspective(1.0472f, (float)win_w / (win_h + 0.001f), 0.1f, 1000.0f);  // 透视投影
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -5.0f));    // 平移
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotate_angle, glm::vec3(0.0f, 1.0f, 0.0f));  //旋转
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(50.0, 100.0, 50.0));
    trans = pers * trans * scale * rot;
    glUniformMatrix4fv(trans_mat_loc, 1, GL_FALSE, glm::value_ptr(trans));
    // 设置顶点属性
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_id);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo_id);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    // 设置纹理属性
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, vert_num);
}
