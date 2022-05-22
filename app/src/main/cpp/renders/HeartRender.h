//
// Created by ldh on 2022/5/1.
//

#ifndef CAMERASPECIALEFFECT_HEARTRENDER_H
#define CAMERASPECIALEFFECT_HEARTRENDER_H

#include "BaseVideoRender.h"
#include <memory>
#include <GLES3/gl3.h>
#include "../utils/ProgramLoader.h"
#include "../utils/PngTool.h"
#include "../faces/UltraFace.hpp"
#include "../faces/Pfld.hpp"

class HeartRender : public BaseVideoRender {

public:
    HeartRender();
    ~HeartRender();
    // 渲染函数
    virtual void render();
    // 初始化爱心数据
    void init_heart_data();
private:
    // 绘制爱心纹理
    void render_heart();

    PNGData png_data; // 心形图片的 rgba 数据
    std::shared_ptr<ProgramLoader> heart_program;
    GLuint eye_vbo;                    // 眼睛的位置
    GLuint eye_vao;                    // 眼睛的vao
    GLuint heart_tex_id;                  // 爱心纹理坐标
    GLint transform_mat_loc;           // 变换矩阵的位置
    // 人脸检测模型
    std::shared_ptr<UltraFace> detect_model;
    // 人脸关键点模型
    std::shared_ptr<Pfld> pts_model;
};


#endif //CAMERASPECIALEFFECT_HEARTRENDER_H
