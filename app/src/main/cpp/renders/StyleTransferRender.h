//
// Created by ldh on 2022/5/13.
//

#ifndef CAMERASPECIALEFFECT_STYLETRANSFERRENDER_H
#define CAMERASPECIALEFFECT_STYLETRANSFERRENDER_H

#include "BaseVideoRender.h"
#include "../utils/StyleTransferModel.h"
#include <memory>

class StyleTransferRender : public BaseVideoRender {
public:
    StyleTransferRender();
    virtual ~StyleTransferRender();
    virtual void render();
private:
    // 模型
    std::shared_ptr<StyleTransferModel> model;
    // 初始化所有数据
    void init_data();
    // 渲染风格迁移的帧
    void render_style_transfer_frame();
    // 保存风格迁移后的帧
    void save_st_frame();
    void save_st_frame_thread();
};


#endif //CAMERASPECIALEFFECT_STYLETRANSFERRENDER_H
