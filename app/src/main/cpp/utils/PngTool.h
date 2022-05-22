//
// Created by ldh on 2022/4/20.
//

#ifndef CAMERASPECIALEFFECT_PNGTOOL_H
#define CAMERASPECIALEFFECT_PNGTOOL_H

#include <string>
#include <lodepng.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

enum PNG_STATUS {
    STATUS_OK = 0,                // 正常
    STATUS_OTHER_ERROR = 1,        // 其他错误
    STATUS_LOAD_FILE_ERROR = 2,  // 加载图片失败
    STATUS_ENCODE_ERROR = 3,     // 编码失败
    STATUS_DECODE_ERROR = 4,     // 解码失败
};

typedef struct PNGData {
public:
    size_t width;  //宽
    size_t height;        // 高
    size_t depth;          // 通道数
    uint8_t *pixels = NULL;        // 像素
    PNGData() {
        width = height = depth = 0;
        pixels = NULL;
    }
    void alloc_mem(size_t w, size_t h, size_t d) {
        if(pixels)
            free(pixels);
        width = w, height = h, depth = d;
        pixels = (uint8_t*) malloc(w * h * d * sizeof(uint8_t));
    }
    ~PNGData() {
        if(pixels) {
            free(pixels);
            pixels = NULL;
        }
    }
};

class PngTool {
public:
    // 从 assets 中加载 png 图片
    static PNG_STATUS loadPngFromAssets(AAssetManager *mgr, const char*file_name, PNGData *png);
    // 保存 png 图片到本地
    static PNG_STATUS savePngToLocal(const char* path, PNGData *png);
    // 对pngdata的像素上下颠倒
    static void flipVertical(PNGData *png);
};


#endif //CAMERASPECIALEFFECT_PNGTOOL_H
