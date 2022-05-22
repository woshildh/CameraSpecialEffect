//
// Created by ldh on 2022/4/22.
//

#include "FaceVideoRender.h"
#include "../utils/Log.h"
#include "../faces/ImageUtils.h"
#include <GLES3/gl3.h>

#define FACE_VERT_SHADER_FILE "shaders/face_vert.glsl"
#define FACE_FRAG_SHADER_FILE "shaders/face_frag.glsl"

using namespace std;

FaceVideoRender::FaceVideoRender(bool use_gaussian_filter_) : BaseVideoRender(
        use_gaussian_filter_) {
    // 加载人脸相关的数据
    init_face_data();
}

FaceVideoRender::~FaceVideoRender() {
    // 释放vao和vbo资源
    glDeleteVertexArrays(1, &face_vao);
    glDeleteBuffers(1, &face_vbo);
}

void FaceVideoRender::render() {
    // 清理颜色缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    // 禁用深度测试
    glDisable(GL_DEPTH_TEST);
    // 渲染视频帧
    render_frame();
    // 渲染人脸框和关键点
    render_face_frame();
}

void FaceVideoRender::render_face_frame() {
    if(yuv == NULL) {
        return;
    }
    pthread_mutex_lock(&frame_lock);
    cv::Mat frame_mat;
    ImageUtils::I420_to_Mat(yuv, img_w, img_h, img_rotation, frame_mat);
    pthread_mutex_unlock(&frame_lock);
    if(frame_mat.empty()) {
        return;
    }


    vector<FaceInfo> face_info;
    detect_model->detect(frame_mat, face_info);
    if(face_info.empty()) {
        return;
    }
    LOGI("There are %d faces", face_info.size());
    // 绘制人脸框
    vector<float> lines = ImageUtils::FaceInfo_to_GLPos(face_info, frame_mat.cols, frame_mat.rows);
    glUseProgram(face_program->program_id);
    glUniform1i(is_rect_loc, 1);
    for(int i = 0; i < face_info.size(); ++i) {
        glBindVertexArray(face_vao);
        glBindBuffer(GL_ARRAY_BUFFER, face_vbo);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), lines.data() + 8 * i, GL_STATIC_DRAW);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(3);
        glLineWidth(8);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    // 绘制人脸关键点
    cv::Mat resize_mat;
    cv::Size croped_wh;
    cv::Point s_point;
    vector<LandmarkInfo> landmarks;
    glUseProgram(face_program->program_id);
    glUniform1i(is_rect_loc, 0);
    for(int i = 0; i < face_info.size(); ++i) {
        // 获取 resize 之后的帧
        cv::Point pt1(face_info[i].x1, face_info[i].y1);
        cv::Point pt2(face_info[i].x2, face_info[i].y2);
        resize_mat = pts_model->Get_Resize_Croped_Img(frame_mat, pt1, pt2, s_point, croped_wh);
        // 进行关键点检测
        pts_model->Get_Landmark_Points(resize_mat, croped_wh, s_point, landmarks);
        vector<float> pts_vec = ImageUtils::LandmakrInfo_to_GLPos(landmarks, frame_mat.cols, frame_mat.rows);
        {
            glBindVertexArray(face_vao);
            glBindBuffer(GL_ARRAY_BUFFER, face_vbo);
            glBufferData(GL_ARRAY_BUFFER, pts_vec.size() * sizeof(float), pts_vec.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(3);
            glDrawArrays(GL_POINTS, 0, pts_vec.size() / 2);
        }
    }
    // 保存图像
    save_picture();
}

void FaceVideoRender::init_face_data() {
    // 加载程序
    face_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("FaceVideoRender", FACE_VERT_SHADER_FILE, FACE_FRAG_SHADER_FILE, asset_mgr));
    // 使用当前程序
    glUseProgram(face_program->program_id);
    // 获取 is_rect 的位置
    is_rect_loc = glGetUniformLocation(face_program->program_id, "is_rect");
    LOGI("[FaceVideoRender] is_rect_loc: %d", is_rect_loc);
    // 分配vao和vbo
    glGenVertexArrays(1, &face_vao);
    glGenBuffers(1, &face_vbo);
    // 加载深度学习模型
    detect_model = std::shared_ptr<UltraFace>(new UltraFace(asset_mgr, "resource/RFB-320.mnn", 300, 510, 4, 0.95, 0.25));
    pts_model = std::shared_ptr<Pfld>(new Pfld(asset_mgr,"resource/pfld-lite.mnn", 4));
}
