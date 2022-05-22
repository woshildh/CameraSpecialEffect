//
// Created by ldh on 2022/4/27.
//

#include "BigFaceRender.h"

#define BIGFACE_VERT_SHADER_FILE "shaders/bigface_vert.glsl"
#define BIGFACE_FRAG_SHADER_FILE "shaders/bigface_frag.glsl"



BigFaceRender::BigFaceRender() : BaseVideoRender(false, BIGFACE_VERT_SHADER_FILE, BIGFACE_FRAG_SHADER_FILE) {
    // 初始化大脸特效数据
    init_bigface_data();
}

BigFaceRender::~BigFaceRender() {

}

void BigFaceRender::init_bigface_data() {
    // 使用当前程序
    glUseProgram(frame_program->program_id);
    // 获取程序中的位置
    img_size_loc = glGetUniformLocation(frame_program->program_id, "img_size");
    face_rect_loc = glGetUniformLocation(frame_program->program_id, "face_rect");
    has_face_loc = glGetUniformLocation(frame_program->program_id, "has_face");
    // 加载人脸检测模型
    detect_model = std::shared_ptr<UltraFace>(new UltraFace(asset_mgr, "resource/RFB-320.mnn", 300, 510, 4, 0.95, 0.25));
    // 打印输出日志
    LOGI("[BigFaceRender] img_size_loc: %d, face_rect_loc: %d, has_face_loc: %d", img_size_loc, face_rect_loc, has_face_loc);
}

void BigFaceRender::send_face_data() {
    if(yuv == NULL) {
        return;
    }
    // 使用当前程序
    glUseProgram(frame_program->program_id);
    // 发送图像尺寸数据
    glUniform2f(img_size_loc, img_w, img_h);
    // 将Yuv数据转成bgr数据
    cv::Mat frame;
    pthread_mutex_lock(&frame_lock);
    ImageUtils::I420_to_Mat(yuv, img_w, img_h, img_rotation, frame);
    pthread_mutex_unlock(&frame_lock);
    // 进行人脸检测
    vector<FaceInfo> faces;
    detect_model->detect(frame, faces);
    // 如果没有检测到人脸
    if(faces.size() == 0) {
        glUniform1i(has_face_loc, 0);
        return ;
    } else {
        glUniform1i(has_face_loc, 1);
    }
    LOGI("[BigFaceRender] There are %d faces", faces.size());
    // 发送最大人脸框的数据到 opengl 程序中
    float max_area = 0;
    float rect[4] = {0, 0, 0, 0};
    for(int i = 0; i < faces.size(); ++i) {
        float area = abs(faces[i].x1 - faces[i].x2) * abs(faces[i].y1 - faces[i].y2);
        if(area > max_area) {
            rect[0] = faces[i].x1 / frame.cols;
            rect[1] = faces[i].y1 / frame.rows;
            rect[2] = faces[i].x2 / frame.cols;
            rect[3] = faces[i].y2 / frame.rows;
            max_area = area;
        }
    }
    glUniform4fv(face_rect_loc, 1, rect);
}


void BigFaceRender::render() {
    // 清除颜色缓冲区
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 发送人数据
    send_face_data();
    // 渲染视频帧
    render_frame();
    // 保存
    save_picture();
}



