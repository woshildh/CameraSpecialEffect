//
// Created by ldh on 2022/5/13.
//

#ifndef CAMERASPECIALEFFECT_STYLETRANSFERMODEL_H
#define CAMERASPECIALEFFECT_STYLETRANSFERMODEL_H

#include "Interpreter.hpp"

#include "MNNDefine.h"
#include "Tensor.hpp"
#include "ImageProcess.hpp"
#include "../third_party/opencv/include/opencv2/opencv.hpp"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <memory>


class StyleTransferModel {

public:
    StyleTransferModel(int num_thread_ = 4);
    ~StyleTransferModel();
    // 初始化模型
    void init_model(AAssetManager *mgr, const char* model_path);
    // 进行转换
    void transfer(cv::Mat &src_mat, cv::Mat &dst_mat);

private:
    // 均值和方差
    float means[3] = {103.94f, 116.78f, 123.68f};
    float norms[3] = {0.017f, 0.017f, 0.017f};
    // mnn 模型
    std::shared_ptr<MNN::Interpreter> interpreter;
    // 推断时用到的变量
    MNN::Session*  session;
    MNN::Tensor* input_tensor;
    MNN::Tensor* output_Tensor;
    // 配置
    MNN::ScheduleConfig config;
    MNN::BackendConfig backendConfig;
};


#endif //CAMERASPECIALEFFECT_STYLETRANSFERMODEL_H
