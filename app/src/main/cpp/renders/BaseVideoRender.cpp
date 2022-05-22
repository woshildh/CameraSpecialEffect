//
// Created by ldh on 2022/4/18.
//

#include "BaseVideoRender.h"
#include "GLES3/gl3.h"
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../utils/PngTool.h"
#include <time.h>
#include <string>

#define FRAME_VERT_SHADER_FILE "shaders/frame_vert.glsl"
#define FRAME_FRAG_SHADER_FILE "shaders/frame_frag.glsl"
#define FRAME_VERT_SHADER_FILTER_FILE "shaders/bilateblur_vert.glsl"
#define FRAME_FRAG_SHADER_FILTER_FILE "shaders/bilateblur_frag.glsl"

AAssetManager* BaseVideoRender::asset_mgr = NULL;

void BaseVideoRender::setAssetManager(AAssetManager *mgr) {
    asset_mgr = mgr;
}

BaseVideoRender::BaseVideoRender(bool use_gaussian_filter_, std::string vert_shader_file_, std::string frag_shader_file_) {
    use_gaussian_filter = use_gaussian_filter_;
    win_h = win_w = img_rotation = img_h = img_w = 0;
    frame_vao = frame_vbo = 0;
    frame_texture_id[0] = frame_texture_id[1] = frame_texture_id[2] = 0;
    yuv = NULL;
    // 初始化锁
    pthread_mutex_init(&frame_lock, NULL);
    // 加载帧程序
    if(vert_shader_file_.empty() || frag_shader_file_.empty()) {
        load_frame_program();
    } else {
        load_frame_program_with_file(vert_shader_file_, frag_shader_file_);
    }
    // 初始化其他信息
    init_frame_data();
}

BaseVideoRender::~BaseVideoRender() {
    // 释放 yuv 数据
    if(yuv != NULL) {
        delete[] yuv;
        yuv = NULL;
    }
    // 释放锁
    pthread_mutex_destroy(&frame_lock);
}

void BaseVideoRender::setWindowSize(int w, int h) {
    win_h = h;
    win_w = w;
}

void BaseVideoRender::setYUVData(unsigned char* data, int width, int height, int rotation) {
    pthread_mutex_lock(&frame_lock);
    if(yuv != NULL) {
        delete[] yuv;
        yuv = NULL;
    }
    img_w = width;img_h = height; img_rotation = rotation;
    yuv = data;
    pthread_mutex_unlock(&frame_lock);
}

void BaseVideoRender::load_frame_program() {
    if(use_gaussian_filter == false) {
        load_frame_program_with_file(FRAME_VERT_SHADER_FILE, FRAME_FRAG_SHADER_FILE);
    } else {
        load_frame_program_with_file(FRAME_VERT_SHADER_FILTER_FILE, FRAME_FRAG_SHADER_FILTER_FILE);
    }
}

void BaseVideoRender::render_frame() {
    if(frame_program == NULL || yuv == NULL || frame_program->status == -1) {
        return;
    }
    // 禁用深度测试
    glDisable(GL_DEPTH_TEST);
    // 使用当前程序
    glUseProgram(frame_program->program_id);
    // 设置视口
    glViewport(0.0, 0.0, this->win_w, this->win_h);
    // 绑定vao和vbo
    glBindVertexArray(frame_vao);
    glBindBuffer(GL_ARRAY_BUFFER, frame_vbo);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    // 申请锁
    pthread_mutex_lock(&frame_lock);
    // 发送统一变量
    glUniform2f(tex_size_loc, img_w, img_h);
    // 计算旋转矩阵并发送
    glm::mat4 rotation(1.0f);
    rotation = glm::rotate(rotation, glm::radians((float )-1 * img_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(frame_rotation_loc, 1, GL_FALSE, glm::value_ptr(rotation));
    // 发送纹理数据
    unsigned char *y_plane = yuv,
            *u_plane = yuv + img_w * img_h,
            *v_plane = yuv + img_w * img_h + (img_w * img_h) / 4;
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frame_texture_id[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, img_w, img_h, 0, GL_RED, GL_UNSIGNED_BYTE, y_plane);
    }
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, frame_texture_id[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, img_w / 2, img_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, u_plane);
    }
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, frame_texture_id[2]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, img_w / 2, img_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, v_plane);
    }
    // 释放锁
    pthread_mutex_unlock(&frame_lock);
    // 进行绘制
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void BaseVideoRender::save_picture() {
    if(need_save_next == true) {
        PNGData data;
        data.alloc_mem(win_w, win_h, 3);
        glReadPixels(0, 0, win_w, win_h, GL_RGB, GL_UNSIGNED_BYTE, data.pixels);
        PngTool::flipVertical(&data);
        PngTool::savePngToLocal(save_dir.c_str(), &data);
        LOGI("ReadPixels state: %d", glGetError());
    }
    need_save_next = false;
}
void BaseVideoRender::render() {
    // 清理颜色缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // 渲染帧
    render_frame();
    // 保存当前的图片
    save_picture();
}

void BaseVideoRender::init_frame_data() {
    glUseProgram(frame_program->program_id);
    // 顶点坐标数据
    float vert_buf[] = {
            -1.0, 1.0, 0.0, 0.0, // 左上
            -1.0, -1.0, 0.0, 1.0, // 左下
            1.0, -1.0, 1.0, 1.0, // 右下
            -1.0, 1.0, 0.0, 0.0,  // 左上
            1.0, 1.0, 1.0, 0.0, // 右上
            1.0, -1.0, 1.0, 1.0  // 右下
    };
    // 生成vao和vbo数据并发送
    glGenVertexArrays(1, &frame_vao);
    glBindVertexArray(frame_vao);
    glGenBuffers(1, &frame_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, frame_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buf), vert_buf, GL_STATIC_DRAW);
    // 初始化纹理
    glGenTextures(3, frame_texture_id);
    for(int i = 0; i < 3; ++i) {
        glBindTexture(GL_TEXTURE_2D, frame_texture_id[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    //  获取 rotation 矩阵
    frame_rotation_loc = glGetUniformLocation(frame_program->program_id, "frame_rotation");
    tex_size_loc = glGetUniformLocation(frame_program->program_id, "tex_size");
    LOGI("[BaseVideoRender] frame_rotation_loc: %d, tex_size_loc: %d", frame_rotation_loc, tex_size_loc);
}

void BaseVideoRender::save_next_frame(const char *path) {
    save_dir = std::string(path);
    need_save_next = true;
}

void BaseVideoRender::load_frame_program_with_file(std::string vert_shader_file,
                                                   std::string frag_shader_file) {
    frame_program = std::unique_ptr<ProgramLoader>(new ProgramLoader("BaseVideoRender", vert_shader_file, frag_shader_file, asset_mgr));
    if(frame_program->status != 0) {
        LOGE("BaseVideoRender load_frame_program error");
    }
}

