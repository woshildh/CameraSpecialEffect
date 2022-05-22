//
// Created by ldh on 2022/4/22.
//

#ifndef CAMERASPECIALEFFECT_IMAGEUTILS_H
#define CAMERASPECIALEFFECT_IMAGEUTILS_H

#include <opencv2/opencv.hpp>
#include "UltraFace.hpp"
#include "Pfld.hpp"
#include <vector>

class ImageUtils {
public:
    // 将 YUV I420 转成 opencv mat
    static bool
    I420_to_Mat(uint8_t *pixels, size_t width, size_t height, int rotation,
                cv::Mat &mat);
    // 将 FaceInfo 转成 OpenGl 需要的坐标
    static std::vector<float> FaceInfo_to_GLPos(std::vector<FaceInfo> &faces_info, int width, int height);

    // 将关键点信息转成OpenGl 需要的坐标
    static std::vector<float> LandmakrInfo_to_GLPos(std::vector<LandmarkInfo> &pts_info, int width, int height);
};


#endif //CAMERASPECIALEFFECT_IMAGEUTILS_H
