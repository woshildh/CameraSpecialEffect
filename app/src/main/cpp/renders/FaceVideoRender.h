//
// Created by ldh on 2022/4/22.
//

#ifndef CAMERASPECIALEFFECT_FACEVIDEORENDER_H
#define CAMERASPECIALEFFECT_FACEVIDEORENDER_H

#include "BaseVideoRender.h"
#include "../faces/ImageUtils.h"
#include <memory>

class FaceVideoRender : public BaseVideoRender {
private:
    // 人脸检测模型
    std::shared_ptr<UltraFace> detect_model;
    // 人脸关键点模型
    std::shared_ptr<Pfld> pts_model;
    // 绘制人脸部分信息
    void render_face_frame();
    // 人脸程序管理
    std::shared_ptr<ProgramLoader> face_program;
    // 矩形框标志的位置
    GLint is_rect_loc;
    // vao 和 vbo
    GLuint face_vao;
    GLuint face_vbo;
    // 初始化人脸程序需要的数据
    void init_face_data();
public:
    FaceVideoRender(bool use_gaussian_filter_ = false);
    virtual ~FaceVideoRender();
    virtual void render();
};


#endif //CAMERASPECIALEFFECT_FACEVIDEORENDER_H
