//
// Created by ldh on 2022/4/18.
//

#include "ProgramLoader.h"
#include "Log.h"


ProgramLoader::ProgramLoader(std::string logTag, std::string vert_file_name,
                             std::string frag_file_name, AAssetManager *mgr) {
    status = -1;
    if(mgr == NULL) {
        LOGI("[%s]: AssetManager is NULL.", logTag.c_str());
        return;
    }
    log_tag = logTag;
    // 开始加载程序
    vert_shader = loadShaderFile(mgr, vert_file_name, GL_VERTEX_SHADER);
    frag_shader = loadShaderFile(mgr, frag_file_name, GL_FRAGMENT_SHADER);
    program_id = glCreateProgram();
    glAttachShader(program_id, vert_shader);
    glAttachShader(program_id, frag_shader);
    glLinkProgram(program_id);
    // 检查程序状态
    LOGI("link program: %d, state: %d", program_id, glGetError());
    // 检查状态
    GLint linked = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked);
    if(!linked) {
        GLint length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
        if(length > 0) {
            char *info = (char*) malloc(sizeof(char) * length);
            glGetProgramInfoLog(program_id, length, NULL, info);
            LOGE("[%s] Link program failed. %s", log_tag.c_str(), info);
            free(info);
        }
        glDeleteProgram(program_id);
    } else {
        status = 0;
    }
}

ProgramLoader::~ProgramLoader() {
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    glDeleteProgram(program_id);
}

GLuint ProgramLoader::loadShaderFile(AAssetManager *mgr, std::string file_name, int shader_type) {
    // 首先读取文件内容
    AAsset* asset = AAssetManager_open(mgr, file_name.c_str(), AASSET_MODE_UNKNOWN);
    if(asset == NULL) {
        LOGI("[%s] AAssetManager_open failed. Please check %s", log_tag.c_str(), file_name.c_str());
        return -1;
    }
    off_t sz = AAsset_getLength(asset);
    std::shared_ptr<unsigned char> content(new unsigned char[sz + 5]{0}, std::default_delete<unsigned char[]>());
    AAsset_read(asset, content.get(), sz);
    // 关闭
    AAsset_close(asset);

    // 创建shader
    GLuint shader = glCreateShader(shader_type);
    const char* src = reinterpret_cast<const char *>(content.get());
    glShaderSource(shader, 1, reinterpret_cast<const GLchar *const *>(&src),
                   NULL);
    glCompileShader(shader);
    LOGI("Compile shader: %d, state: %d", shader, glGetError());
    // 检查 shader 状态
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        LOGI("[%s] Compile shader failed. state: %d", log_tag.c_str(), glGetError());
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if(length) {
            char *info = (char*) malloc(sizeof(char) * length);
            glGetShaderInfoLog(shader, length, &length, info);
            LOGE("[%s] Compile shader failed. %s", log_tag.c_str(), info);
            free(info);
        }
        glDeleteShader(shader);
    }
    return shader;
}
