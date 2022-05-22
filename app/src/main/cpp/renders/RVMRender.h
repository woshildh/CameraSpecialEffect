//
// Created by ldh on 2022/5/15.
//

#ifndef CAMERASPECIALEFFECT_RVMRENDER_H
#define CAMERASPECIALEFFECT_RVMRENDER_H

#include "BaseVideoRender.h"
#include "../utils/RVMModel.h"
#include <memory>

class RVMRender : public BaseVideoRender {

public:
    RVMRender();
    virtual ~RVMRender();
    virtual void render();

private:
    void init_data();
    // 程序id
    std::shared_ptr<ProgramLoader> rvm_program;
    // 纹理以及数据
    GLuint fore_tex_id;   // 前景的纹理
    GLuint alpha_tex_id;  // 分割部分的纹理
    GLuint rvm_vao_id;  // vao
    GLuint rvm_vbo_id;  // vbo
    // 人像分割的模型
    std::shared_ptr<RVMModel> model;
    // 绘制前景
    void render_fore_frame();
};


#endif //CAMERASPECIALEFFECT_RVMRENDER_H
