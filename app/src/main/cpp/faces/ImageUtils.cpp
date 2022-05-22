//
// Created by ldh on 2022/4/22.
//

#include "ImageUtils.h"
#include "../utils/Log.h"

enum RotateFlags {
    ROTATE_90_CLOCKWISE = 0, //!<Rotate 90 degrees clockwise顺时针旋转90
    ROTATE_180 = 1, //!<Rotate 180 degrees clockwise 旋转180
    ROTATE_90_COUNTERCLOCKWISE = 2, //!<Rotate 270 degrees clockwise旋转270度
};

// 将一张图像中的x坐标转换为opengl坐标系的坐标
float x_to_gl(float x, int width) {
    return x / (width / 2.0) - 1.0;
}

// 将一张图像中的y坐标转换为opengl坐标系的坐标
float y_to_gl(float y, int height) {
    return 1.0 - y / (height / 2.0);
}

bool
ImageUtils::I420_to_Mat(uint8_t *pixels, size_t width, size_t height, int rotation, cv::Mat &mat) {
    // 构建YUV数据
    size_t numOfPixel = width * height;
    cv::Mat tmpMat(height * 3 / 2, width, CV_8UC1);
    memcpy(tmpMat.data, pixels, numOfPixel * 3 / 2);

    // 转换颜色格式
    cv::cvtColor(tmpMat, mat, cv::ColorConversionCodes::COLOR_YUV2BGR_I420);
    // 进行旋转
    if(rotation == 90) {
        cv::rotate(mat, mat, ROTATE_90_CLOCKWISE);
    } else if(rotation == 180) {
        cv::rotate(mat, mat, ROTATE_180);
    } else if(rotation == 270) {
        cv::rotate(mat, mat, ROTATE_90_COUNTERCLOCKWISE);
    }
//    cv::imwrite("/storage/emulated/0/DCIM/CameraSepecialEffect/test.jpg", mat);
    return true;
}

std::vector<float>
ImageUtils::FaceInfo_to_GLPos(vector<FaceInfo> &faces_info, int width, int height) {
    std::vector<float> pts(faces_info.size() * 4 * 2);
    for(int i = 0; i < faces_info.size(); ++i) {
        // 左上
        pts[i * 8] = x_to_gl(faces_info[i].x1, width);
        pts[i * 8 + 1] = y_to_gl(faces_info[i].y1, height);
        // 右上
        pts[i * 8 + 2] = x_to_gl(faces_info[i].x2, width);
        pts[i * 8 + 3] = y_to_gl(faces_info[i].y1, height);
        // 右下
        pts[i * 8 + 4] = x_to_gl(faces_info[i].x2, width);
        pts[i * 8 + 5] = y_to_gl(faces_info[i].y2, height);
        // 左下
        pts[i * 8 + 6] = x_to_gl(faces_info[i].x1, width);
        pts[i * 8 + 7] = y_to_gl(faces_info[i].y2, height);
    }
    return pts;
}

std::vector<float>
ImageUtils::LandmakrInfo_to_GLPos(vector<LandmarkInfo> &pts_info, int width, int height) {
    std::vector<float> pts(pts_info.size() * 2);
    for(int i = 0; i < pts_info.size(); ++i) {
        pts[i * 2] = x_to_gl(pts_info[i].pt.x, width);
        pts[i * 2 + 1] = y_to_gl(pts_info[i].pt.y, height);
    }
    return pts;
}
