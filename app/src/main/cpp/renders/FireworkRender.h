//
// Created by ldh on 2022/5/4.
//

#ifndef CAMERASPECIALEFFECT_FIREWORKRENDER_H
#define CAMERASPECIALEFFECT_FIREWORKRENDER_H

#include "BaseVideoRender.h"
#include "../utils/ProgramLoader.h"
#include "../utils/FireworkModel.h"
#include <vector>
#include <memory>

class FireworkRender : public BaseVideoRender {

public:
    FireworkRender(int fire_num = 10);
    virtual ~FireworkRender();
    virtual void render();

private:
    std::shared_ptr<ProgramLoader> fw_rise_program;
    std::shared_ptr<ProgramLoader> fw_explode_program;
    // 烟花数组
    std::vector<FireworkModel> fm;
    // 初始化烟花特效所需的数据
    void init_firework_data();
    // 渲染烟花上升期特效
    void render_rise_stage(int idx);
    // 渲染烟花爆炸期特效
    void render_explode_stage(int idx);
    // 渲染烟花特效
    void render_fireworks();
    // 上升期 vao 和 vbo
    GLuint rise_vao_id;
    GLuint rise_vbo_id;
    // 上升期统一变量位置
    GLint rise_color_loc;
    GLint rise_width_loc;
    // 爆炸期的vao和vbo
    GLuint explode_vao_id;
    GLuint explode_color_vbo_id;
    GLuint explode_pos_vbo_id;
    GLuint explode_radius_vbo_id;
};


#endif //CAMERASPECIALEFFECT_FIREWORKRENDER_H
