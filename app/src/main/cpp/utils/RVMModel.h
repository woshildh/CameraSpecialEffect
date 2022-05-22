//
// Created by ldh on 2022/5/14.
//

#ifndef CAMERASPECIALEFFECT_RVMMODEL_H
#define CAMERASPECIALEFFECT_RVMMODEL_H

#include <ncnn/net.h>
#include "opencv2/opencv.hpp"
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

class RVMModel {
public:
    RVMModel(int num_thread_ = 4);
    ~RVMModel();
    void init_model(AAssetManager *mgr, const char* param_path, const char* bin_path);
    // image 是输入的图像 [h, w, 3], background
    void segmentation(cv::Mat &image, cv::Mat &background);
private:
    ncnn::UnlockedPoolAllocator blob_pool_allocator;
    ncnn::PoolAllocator workspace_pool_allocator;
    int input_width = 256;
    int input_height = 256;
    int num_thread = 4;
    std::shared_ptr<ncnn::Net> net;
    const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
    const float norm_vals[3] = {1.f / 127.5f, 1.f / 127.5f, 1.f / 127.5f};
    int num_outputs = 1;
    // 预处理图像
    void transform(cv::Mat &mat_rs, ncnn::Mat &in);
    // 后处理生成背景分割图
    void generate_background(ncnn::Extractor &extractor, cv::Mat &image, cv::Mat &background);
};


#endif //CAMERASPECIALEFFECT_RVMMODEL_H
