//
// Created by ldh on 2022/5/13.
//

#include "StyleTransferModel.h"
#include <vector>
#include "../utils/Log.h"
#include <chrono>
using namespace std;
using namespace chrono;


#define INPUT_IMG_W 400
#define INPUT_IMG_H 400

StyleTransferModel::StyleTransferModel(int num_thread_) {
    config.numThread = num_thread_;
    config.type      = static_cast<MNNForwardType>(MNN_FORWARD_OPENCL);
    backendConfig.precision = (MNN::BackendConfig::PrecisionMode) 2;
    config.backendConfig = &backendConfig;
}

StyleTransferModel::~StyleTransferModel() {

}

void StyleTransferModel::init_model(AAssetManager *mgr, const char *model_path) {
    // 检查是否非空
    if(mgr == NULL || model_path == NULL) {
        return;
    }
    // 1. 读取文件内容
    AAsset* asset = AAssetManager_open(mgr, model_path, AASSET_MODE_UNKNOWN);
    if(asset == NULL) {
        LOGI("[%s] AAssetManager_open failed. Please check %s", "UltraFace", model_path);
        return;
    }
    off_t sz = AAsset_getLength(asset);
    std::shared_ptr<unsigned char> content(new unsigned char[sz + 5]{0}, std::default_delete<unsigned char[]>());
    AAsset_read(asset, content.get(), sz);
    // 关闭
    AAsset_close(asset);
    // 2. 加载模型
    interpreter = std::shared_ptr<MNN::Interpreter>(MNN::Interpreter::createFromBuffer(content.get(), sz));
    // 3. 创建会话
    session = interpreter->createSession(config);
    input_tensor = interpreter->getSessionInput(session, "img_placeholder");
}

void StyleTransferModel::transfer(cv::Mat &src_mat, cv::Mat &dst_mat) {
    // resize 图片
    cv::Mat resize_src_mat;
    size_t rows = src_mat.rows, cols = src_mat.cols;
    cv::resize(src_mat, resize_src_mat, cv::Size(0, 0), INPUT_IMG_W / (float )cols, INPUT_IMG_H / (float)rows);
    // 做后处理和拷贝
    interpreter->resizeTensor(input_tensor, {1, 3, INPUT_IMG_H, INPUT_IMG_W});
    interpreter->resizeSession(session);
    std::shared_ptr<MNN::CV::ImageProcess> pretreat(
            MNN::CV::ImageProcess::create(MNN::CV::BGR, MNN::CV::RGB, means, 3,
                                          norms, 3));
    pretreat->convert(resize_src_mat.data, INPUT_IMG_W, INPUT_IMG_H, resize_src_mat.step[0], input_tensor);
    // 运行
    system_clock::time_point t1 = system_clock::now();
    interpreter->runSession(session);
    system_clock::time_point t2 = system_clock::now();
    // 获取输出
    output_Tensor = interpreter->getSessionOutput(session, "add_37");
    if(output_Tensor == NULL) {
        LOGI("[StyleTransferModel] output tensor is NULL");
        return;
    }
    // 进行转换
    const float* buffer = output_Tensor->host<float>();
    cv::Mat tmp(output_Tensor->height(), output_Tensor->width(), CV_32FC3, (void*)buffer);
    tmp.convertTo(dst_mat, CV_8UC3);
    auto minus = t2 - t1;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(minus);
    LOGI("[StyleTransferModel] shape: (%d, %d, %d, %d), time spend: %lld ms", output_Tensor->batch(),
         output_Tensor->channel(), output_Tensor->height(), output_Tensor->width(), ms);
}
