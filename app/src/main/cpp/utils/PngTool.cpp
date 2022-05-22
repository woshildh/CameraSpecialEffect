//
// Created by ldh on 2022/4/20.
//

#include "PngTool.h"
#include <stdio.h>
#include <memory>
#include "../utils/Log.h"

PNG_STATUS PngTool::loadPngFromAssets(AAssetManager *mgr, const char*file_name, PNGData *png) {
    if(png == NULL) {
        return STATUS_OTHER_ERROR;
    }
    // 1. 读取 png 内容
    AAsset* asset = AAssetManager_open(mgr, file_name, AASSET_MODE_UNKNOWN);
    if(asset == NULL) {
        return STATUS_LOAD_FILE_ERROR;
    }
    off_t sz = AAsset_getLength(asset);
    std::shared_ptr<char> buf(new char[sz + 5]{0}, std::default_delete<char[]>());
    AAsset_read(asset, buf.get(), sz);
    AAsset_close(asset);
    // 2. 解码 png 内容
    unsigned char *buf2 = NULL;
    unsigned int w  = 0, h = 0;
    LodePNGState state = {0};
    lodepng_decode(&buf2, &w, &h, &state, reinterpret_cast<const unsigned char *>(buf.get()), sz);
    // 3. 进行赋值
    if(png->pixels) {
        free(png->pixels);
        png->pixels = NULL;
        png->height = png->width = png->depth = 0;
    }
    LOGI("[loadPngFromAssets] image: %s, state: %d", file_name, state.error);
    LOGI("[loadPngFromAssets] info: colortype: %d", state.info_raw.colortype);
    if(state.error != 0) {
        return STATUS_OTHER_ERROR;
    }
    png->pixels = buf2;
    png->width = w, png->height = h, png->depth = (state.info_raw.colortype == LCT_RGBA) ? 4 : 3;
    LOGI("PNG info: width is %d, height is %d, depth is %d", png->width, png->height, png->depth);
    LOGI("(%d, %d, %d, %d)", png->pixels[100 * w * 4 + 30 * 4],
         png->pixels[100 * w * 4 + 30 * 4 + 1], png->pixels[100 * w * 4 + 30 * 4 + 2], png->pixels[100 * w * 4 + 30 * 4 + 3]);
    return STATUS_OK;
}

PNG_STATUS PngTool::savePngToLocal(const char *path, PNGData *png) {
    if(path == NULL || png == NULL)
        return STATUS_OTHER_ERROR;
    // 首先打开文件检测是否正确
    FILE *fp = fopen(path, "wb");
    if(fp == NULL) {
        return STATUS_LOAD_FILE_ERROR;
    }
    fclose(fp);
    // 保存图片
    if(png->depth == 4) {
        lodepng_encode32_file(path, png->pixels, png->width, png->height);
    } else {
        lodepng_encode24_file(path, png->pixels, png->width, png->height);
    }
    return STATUS_OK;
}

void PngTool::flipVertical(PNGData *png) {
    if(png == NULL) {
        return;
    }
    size_t line_sz = png->depth * png->width;
    uint8_t *line_tmp = (uint8_t*)malloc(sizeof(uint8_t) * line_sz);
    for(int i = 0; i < png->height / 2; ++i) {
        uint8_t *addr_up = png->pixels + i * line_sz,
            *addr_low = png->pixels + (png->height - i - 1) * line_sz;
        // 进行数据的搬运
        memcpy(line_tmp, addr_up, line_sz);
        memcpy(addr_up, addr_low, line_sz);
        memcpy(addr_low, line_tmp, line_sz);
    }
    free(line_tmp);
}
