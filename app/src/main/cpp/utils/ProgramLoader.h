//
// Created by ldh on 2022/4/18.
//

#ifndef CAMERASPECIALEFFECT_PROGRAMLOADER_H
#define CAMERASPECIALEFFECT_PROGRAMLOADER_H

#include "Log.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <GLES3/gl3.h>
#include <string>

// 用于程序的加载
class ProgramLoader {
private:
    GLuint vert_shader;
    GLuint frag_shader;
    GLuint createShader();
    // 加载着色器
    GLuint loadShaderFile(AAssetManager *mgr, std::string file_name, int shader_type);
    // 创建程序
    GLuint loadProgram();
    std::string log_tag;
public:
    ProgramLoader(std::string logTag, std::string vert_file_name, std::string frag_file_name, AAssetManager *mgr);
    ~ProgramLoader();
    GLuint program_id;
    int status;
};


#endif //CAMERASPECIALEFFECT_PROGRAMLOADER_H
