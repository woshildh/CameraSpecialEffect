//
// Created by ldh on 2022/5/1.
//

#include "HeartRender.h"
#include "../faces/ImageUtils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#define HEART_VERY_SHADER_FILE "shaders/heart_vert.glsl"
#define HEART_FRAG_SHADER_FILE "shaders/heart_frag.glsl"
#define HEART_PNG_NAME "resource/heart.png"

HeartRender::HeartRender() : BaseVideoRender(false, "", "") {
    // 初始化爱心相关的数据
    init_heart_data();
}

HeartRender::~HeartRender() {
    glDeleteBuffers(1, &eye_vbo);
    glDeleteVertexArrays(1, &eye_vao);
    glDeleteTextures(1, &heart_tex_id);
}

void HeartRender::init_heart_data() {
    // 2. 加载相关程序
    heart_program = std::shared_ptr<ProgramLoader>(new ProgramLoader("HeartRender", HEART_VERY_SHADER_FILE, HEART_FRAG_SHADER_FILE, asset_mgr));
    glUseProgram(heart_program->program_id);
    // 1. 分配opengl资源
    float vert_buf[] = {
            -1.0, 1.0, 0.0, 0.0, // 左上
            -1.0, -1.0, 0.0, 1.0, // 左下
            1.0, -1.0, 1.0, 1.0, // 右下
            -1.0, 1.0, 0.0, 0.0,  // 左上
            1.0, 1.0, 1.0, 0.0, // 右上
            1.0, -1.0, 1.0, 1.0  // 右下
    };
    glGenVertexArrays(1, &eye_vao);
    glBindVertexArray(eye_vao);
    glGenBuffers(1, &eye_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, eye_vbo);    // 顶点坐标数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert_buf), vert_buf, GL_STATIC_DRAW);

    // 3. 加载 png 数据
    PngTool::loadPngFromAssets(asset_mgr, HEART_PNG_NAME, &png_data);
    // 4. 加载人脸检测和人脸关键点模型
    detect_model = std::make_shared<UltraFace>(asset_mgr, "resource/RFB-320.mnn", 240, 360, 4, 0.95, 0.4);
    pts_model = std::make_shared<Pfld>(asset_mgr, "resource/pfld-lite.mnn");
    // 初始化爱心纹理数据
    glGenTextures(1, &heart_tex_id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, heart_tex_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, png_data.width, png_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, png_data.pixels);

    // 获取变换矩阵的统一变量位置
    transform_mat_loc = glGetUniformLocation(heart_program->program_id, "transform_mat");

    // 打印信息
    LOGI("[HeartRender] init state: %d, program id: %d, eye vao: %d, eye vbo: %d, eye tex id: %d, transform_mat_loc: %d",
         glGetError(), heart_program->program_id, eye_vao, eye_vbo, heart_tex_id, transform_mat_loc);
}


void HeartRender::render() {
    // 清理颜色缓冲区
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 绘制视频帧
    BaseVideoRender::render_frame();
    // 绘制爱心贴图
    render_heart();
    // 保存图片
    save_picture();
}

void HeartRender::render_heart() {
    // 要开启混色模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 使用绘制爱心的程序
    glUseProgram(heart_program->program_id);
    pthread_mutex_lock(&frame_lock);
    if(yuv == NULL) {
        pthread_mutex_unlock(&frame_lock);
        return;
    }
    // 首先转换数据yuv->bgr
    cv::Mat frame_mat;
    ImageUtils::I420_to_Mat(yuv, img_w, img_h, img_rotation, frame_mat);
    pthread_mutex_unlock(&frame_lock);
    if(frame_mat.empty()) {
        return;
    }
    // 进行人脸检测和人脸关键点检测
    vector<FaceInfo> face_info;
    detect_model->detect(frame_mat, face_info);
    if(face_info.empty()) {
        return;
    }
    LOGI("There are %d face", face_info.size());
    // 查找面积最大的人脸图像
    int max_area_id = 0;
    float max_area = 0;
    for(int i = 0; i < face_info.size(); ++i) {
        float area = (face_info[i].x1 - face_info[i].x2) * (face_info[i].y1 - face_info[i].y2);
        if(area > max_area) {
            max_area_id = i;
            max_area = area;
        }
    }
    if(max_area < 1000) {
        return;
    }
    // 获取 resize 之后的帧
    cv::Mat resize_mat;
    cv::Size croped_wh;
    cv::Point s_point;
    std::vector<LandmarkInfo> landmarks;
    cv::Point pt1(face_info[max_area_id].x1, face_info[max_area_id].y1);
    cv::Point pt2(face_info[max_area_id].x2, face_info[max_area_id].y2);
    resize_mat = pts_model->Get_Resize_Croped_Img(frame_mat, pt1, pt2, s_point, croped_wh);
    // 进行关键点检测
    pts_model->Get_Landmark_Points(resize_mat, croped_wh, s_point, landmarks);
    // 计算左眼变换矩阵并绘制
    glBindVertexArray(eye_vao);
    glBindBuffer(GL_ARRAY_BUFFER, eye_vbo);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, heart_tex_id);
    {
        // 左眼宽度
        float left_eye_w = 2.2 * abs(landmarks[64].pt.x - landmarks[60].pt.x) / (float)frame_mat.cols;
        float left_eye_x = landmarks[96].pt.x / (frame_mat.cols / 2.0) - 1.0,
                left_eye_y = 1.0 - landmarks[96].pt.y / (frame_mat.rows / 2.0);
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(left_eye_w, left_eye_w, 1.0));
        glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(left_eye_x, left_eye_y, 0.0));
        trans_mat =  trans_mat * scale_mat;
        // 发送数据
        glUniformMatrix4fv(transform_mat_loc, 1, GL_FALSE, glm::value_ptr(trans_mat));
        // 进行绘制
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    {
        // 右眼宽度
        float left_eye_w = 2.2 * abs(landmarks[72].pt.x - landmarks[68].pt.x) / (float)frame_mat.cols;
        float left_eye_x = landmarks[97].pt.x / (frame_mat.cols / 2.0) - 1.0,
                left_eye_y = 1.0 - landmarks[97].pt.y / (frame_mat.rows / 2.0);
        glm::mat4 scale_mat = glm::scale(glm::mat4(1.0f), glm::vec3(left_eye_w, left_eye_w, 1.0));
        glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(left_eye_x, left_eye_y, 0.0));
        trans_mat =  trans_mat * scale_mat;
        // 发送数据
        glUniformMatrix4fv(transform_mat_loc, 1, GL_FALSE, glm::value_ptr(trans_mat));
        // 进行绘制
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

