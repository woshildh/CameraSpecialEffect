//
// Created by ldh on 2022/5/13.
//

#include "StyleTransferRender.h"
#include "../faces/ImageUtils.h"
#include "../utils/PngTool.h"
#include <pthread.h>
#include <thread>
#define STYLE_TRANSFER_MODEL_PATH "resource/rain.mnn"

StyleTransferRender::StyleTransferRender() {
    init_data();
}

StyleTransferRender::~StyleTransferRender() {

}

void StyleTransferRender::render() {
    if(yuv == NULL || !model) {
        return;
    }
    // 清理
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 渲染帧
    render_frame();
    // 对当前帧进行风格迁移并保存
    save_st_frame();
}

void StyleTransferRender::init_data() {
    // 加载模型
    model = std::shared_ptr<StyleTransferModel>(new StyleTransferModel(4));
    model->init_model(asset_mgr, STYLE_TRANSFER_MODEL_PATH);
}

void StyleTransferRender::save_st_frame() {
    if(need_save_next == false) {
        return;
    }
    need_save_next = false;
    // 单独开一个线程进行保存
    thread t = std::thread(&StyleTransferRender::save_st_frame_thread, this);
    t.detach();
}

void StyleTransferRender::save_st_frame_thread() {
    // 转换yuv到bgr
    cv::Mat src_mat, dst_mat;
    pthread_mutex_lock(&frame_lock);
    if(yuv == NULL) {
        pthread_mutex_unlock(&frame_lock);
        return ;
    }
    ImageUtils::I420_to_Mat(yuv, img_w, img_h, img_rotation, src_mat);
    pthread_mutex_unlock(&frame_lock);
    if(src_mat.empty()) {
        return ;
    }
    // 进行转换
    model->transfer(src_mat, dst_mat);
    if(dst_mat.empty()) {
        return;
    }
    // 保存图片
    PNGData data;
    data.width = dst_mat.cols, data.height = dst_mat.rows, data.depth = dst_mat.channels();
    data.pixels = dst_mat.data;
    PngTool::savePngToLocal(save_dir.c_str(), &data);
    data.pixels = NULL;
}

