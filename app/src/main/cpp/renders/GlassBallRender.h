//
// Created by ldh on 2022/5/3.
//

#ifndef CAMERASPECIALEFFECT_GLASSBALLRENDER_H
#define CAMERASPECIALEFFECT_GLASSBALLRENDER_H

#include "../utils/PngTool.h"
#include "BaseVideoRender.h"
#include "../utils/imported_model.h"
#include "../utils/ProgramLoader.h"
#include <memory>

class GlassBallRender : public BaseVideoRender {
private:
    // 3D 模型
    std::shared_ptr<ImportedModel> model;
    // 3D 模型的程序
    std::shared_ptr<ProgramLoader> glassball_program;
    // png 数据
    PNGData png_data;
    // 顶点
    size_t vert_num;
    GLuint vao_id;
    GLuint vert_vbo_id;
    GLuint tex_vbo_id;
    // 纹理
    GLuint tex_id;
    // 旋转相关
    float rotate_angle;
    GLint trans_mat_loc;
    // 初始化玻璃球相关的数据
    void init_glassball_data();
    // 渲染3D对象
    void render_glassball();
public:
    // 构造函数
    GlassBallRender();
    // 析构函数
    virtual ~GlassBallRender();
    // 渲染函数
    virtual void render();
};


#endif //CAMERASPECIALEFFECT_GLASSBALLRENDER_H
