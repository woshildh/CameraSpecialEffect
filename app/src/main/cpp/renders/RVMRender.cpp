//
// Created by ldh on 2022/5/15.
//

#include "RVMRender.h"
#include "../faces/ImageUtils.h"
#include "../utils/PngTool.h"

#define RVM_MODEL_MNN_BIN_PATH "resource/modnet_webcam_portrait_matting-256x256.opt.bin"
#define RVM_MODEL_MNN_PARAM_PATH "resource/modnet_webcam_portrait_matting-256x256.opt.param"
#define RVM_RENDER_VERT_FILE_PATH "shaders/rvm_vert.glsl"
#define RVM_RENDER_FRAG_FILE_PATH "shaders/rvm_frag.glsl"

RVMRender::RVMRender() {
    // 初始化数据
    init_data();
}

RVMRender::~RVMRender() {
    glDeleteVertexArrays(1, &rvm_vao_id);
    glDeleteBuffers(1, &rvm_vbo_id);
    glDeleteTextures(1, &fore_tex_id);
    glDeleteTextures(1, &alpha_tex_id);
}

void RVMRender::render() {
    // 要开启混色模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 清理帧
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 绘制帧
    render_frame();
    // 绘制背景
    render_fore_frame();
    // 保存图片
    save_picture();
}

void RVMRender::init_data() {
    // 初始化人像分割模型
    model = std::shared_ptr<RVMModel>(new RVMModel(4));
    model->init_model(asset_mgr, RVM_MODEL_MNN_PARAM_PATH, RVM_MODEL_MNN_BIN_PATH);
    // 加载程序
    rvm_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("RVMRender", RVM_RENDER_VERT_FILE_PATH, RVM_RENDER_FRAG_FILE_PATH, asset_mgr));
    // 分配vao和vbo
    // 顶点坐标数据
    float vert_buf[] = {
            -1.0, 1.0, 0.0, 0.0, // 左上
            -1.0, -1.0, 0.0, 1.0, // 左下
            1.0, -1.0, 1.0, 1.0, // 右下
            -1.0, 1.0, 0.0, 0.0,  // 左上
            1.0, 1.0, 1.0, 0.0, // 右上
            1.0, -1.0, 1.0, 1.0  // 右下
    };
    glUseProgram(rvm_program->program_id);
    glGenVertexArrays(1, &rvm_vao_id);
    glGenBuffers(1, &rvm_vbo_id);
    glBindVertexArray(rvm_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, rvm_vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buf), vert_buf, GL_STATIC_DRAW);
    // 初始化纹理数据
    PNGData png_data;
    PngTool::loadPngFromAssets(asset_mgr, "resource/ocean.png", &png_data);
    {
        glGenTextures(1, &alpha_tex_id);
        glBindTexture(GL_TEXTURE_2D, alpha_tex_id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    {
        glGenTextures(1, &fore_tex_id);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, fore_tex_id);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, png_data.width, png_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png_data.pixels);
    }
    // 打印日志
    LOGI("[RVMRender] program id: %d, alpha_tex_id: %d, fore_tex_id: %d, rvm_vao_id: %d, "
         "rvm_vbo_id: %d, state: %d", rvm_program->program_id, alpha_tex_id, fore_tex_id,
         rvm_vao_id, rvm_vbo_id, glGetError());
}

void RVMRender::render_fore_frame() {
    if(yuv == NULL) {
        return;
    }

    // 将 yuv 转成 mat
    pthread_mutex_lock(&frame_lock);
    cv::Mat src_mat, pha_mat;
    ImageUtils::I420_to_Mat(yuv, img_w, img_h, img_rotation, src_mat);
    pthread_mutex_unlock(&frame_lock);
    // 获取前景
    model->segmentation(src_mat, pha_mat);
    if(pha_mat.empty()) {
        return;
    }
    // 进行渲染
    glUseProgram(rvm_program->program_id);
    // 发送纹理数据
    {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, alpha_tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, pha_mat.cols, pha_mat.rows, 0, GL_RED, GL_FLOAT, pha_mat.data);
    }
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, fore_tex_id);
    // 设置顶点
    glBindVertexArray(rvm_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, rvm_vbo_id);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    // 绘制帧
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
