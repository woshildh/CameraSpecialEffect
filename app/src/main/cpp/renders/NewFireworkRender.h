//
// Created by ldh on 2022/5/8.
//

#ifndef CAMERASPECIALEFFECT_NEWFIREWORKRENDER_H
#define CAMERASPECIALEFFECT_NEWFIREWORKRENDER_H

#include "BaseVideoRender.h"
#include <vector>
#include "../utils/NewFireworkModel.h"

class NewFireworkRender : public BaseVideoRender {
public:
    NewFireworkRender(int fire_num_ = 5);
    virtual ~NewFireworkRender();
    virtual void render();

private:
    // 烟花的数量
    int fire_num;
    // opengles 程序
    std::shared_ptr<ProgramLoader> fm_program;
    // 上升期的纹理
    GLuint rise_tex_id;
    // 爆炸期的纹理
    GLuint explode_tex_id;
    // 纹理的变换矩阵统一变量位置
    GLint mvp_mat_loc;
    // 颜色的统一变量位置
    GLint color_loc;
    // 当前阶段的统一变量位置
    GLint stage_loc;
    // vao 和 vbo
    GLuint fm_vao_id;
    GLuint fm_vbo_id;
    // 初始化数据
    void init_data();
    // 绘制烟花
    void render_firework();
    // 烟花模型
    std::vector<NewFireworkModel> fm_data;
};


#endif //CAMERASPECIALEFFECT_NEWFIREWORKRENDER_H
