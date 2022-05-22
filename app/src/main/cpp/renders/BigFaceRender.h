//
// Created by ldh on 2022/4/27.
//

#ifndef CAMERASPECIALEFFECT_BIGFACERENDER_H
#define CAMERASPECIALEFFECT_BIGFACERENDER_H

#include "BaseVideoRender.h"
#include "BaseVideoRender.h"
#include "../faces/ImageUtils.h"
#include "../faces/UltraFace.hpp"
#include <memory>

class BigFaceRender : public BaseVideoRender {

private:
    // 人脸检测模型
    std::shared_ptr<UltraFace> detect_model;
    // 初始化大脸特效所需要的数据模型
    void init_bigface_data();
    // opengl相关资源
    GLint  img_size_loc;   // 图像尺寸的位置
    GLint  face_rect_loc;  //人脸矩形框的位置
    GLint  has_face_loc;   // 是否有人脸矩形框的标志
    // 发送人脸数据到程序
    void send_face_data();
public:
    BigFaceRender();
    virtual ~BigFaceRender();
    virtual void render();
};


#endif //CAMERASPECIALEFFECT_BIGFACERENDER_H
