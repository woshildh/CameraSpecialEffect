//
// Created by ldh on 2022/5/14.
//

#include "RVMModel.h"
#include "../utils/Log.h"
#include <chrono>
using namespace std::chrono;

RVMModel::RVMModel(int num_thread_) {
    num_thread = num_thread_;
}

RVMModel::~RVMModel() {
}

void RVMModel::init_model(AAssetManager *mgr, const char *param_path, const char *bin_path) {
    net = std::shared_ptr<ncnn::Net>(new ncnn::Net());
    net->opt.use_vulkan_compute = true; // default
    net->opt.use_fp16_arithmetic = true;
    net->load_param(mgr, param_path);
    net->load_model(mgr, bin_path);
}

void RVMModel::segmentation(cv::Mat &image, cv::Mat &background) {
    if(image.empty()) {
        return;
    }
    ncnn::Mat input;
    this->transform(image, input);
    // 2. inference & extract
    auto extractor = net->create_extractor();
    extractor.set_light_mode(true);  // default
    extractor.set_num_threads(num_thread);
    extractor.input("input", input);
    // 生成背景
    generate_background(extractor, image,background);
}

void RVMModel::transform(cv::Mat &mat, ncnn::Mat &in) {
    if(mat.empty()) {
        return;
    }
    cv::Mat mat_rs;
    cv::resize(mat, mat_rs, cv::Size(0, 0), input_width / (float)mat.cols, input_height / (float)mat.rows);
    // will do deepcopy inside ncnn
    in = ncnn::Mat::from_pixels(mat_rs.data, ncnn::Mat::PIXEL_BGR2RGB, input_width, input_height);
    in.substract_mean_normalize(mean_vals, norm_vals);
}

void RVMModel::generate_background(ncnn::Extractor &extractor, cv::Mat &image,
                                   cv::Mat &background) {
//    system_clock::time_point t1 = system_clock::now();

    ncnn::Mat output;
    extractor.extract("output", output);
//    system_clock::time_point t2 = system_clock::now();

//    LOGI("render_fore_frame spend %d ms", std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1));

    const unsigned int out_h = input_height;
    const unsigned int out_w = input_width;

    float *output_ptr = (float *) output.data;

    background = cv::Mat(out_h, out_w, CV_32FC1);
    memcpy(background.data, output_ptr, sizeof(float) * out_h * out_w);
}

