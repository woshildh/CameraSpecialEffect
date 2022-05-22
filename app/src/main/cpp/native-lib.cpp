#include <jni.h>
#include <string>
#include <GLES3/gl3.h>
#include "renders/BaseVideoRender.h"
#include "renders/FaceVideoRender.h"
#include "renders/BigFaceRender.h"
#include "renders/HeartRender.h"
#include "renders/GlassBallRender.h"
#include "renders/FireworkRender.h"
#include "renders/NewFireworkRender.h"
#include "renders/StyleTransferRender.h"
#include "renders/RVMRender.h"

#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include "pthread.h"

// 定义窗口的宽高
size_t g_width = 0, g_height = 0;

// 全局的AssetManager
AAssetManager *g_asset_mgr = NULL;

// 渲染器类型
int render_type = 0;

// 全局的渲染器
BaseVideoRender *g_render = NULL;
pthread_mutex_t render_mutex;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_initRenderJNI(JNIEnv *env, jclass clazz,
                                                               jobject mgr, jobject surface) {
    if(env && mgr) {
        g_asset_mgr = AAssetManager_fromJava(env, mgr);
        if(g_asset_mgr == NULL) {
            LOGE("[JNI-Interface] AAssetManager_fromJava() is NULL!");
        }
        // 设置 asset
        BaseVideoRender::setAssetManager(g_asset_mgr);
    } else {
        LOGE("[JNI-Interface] env or mgr is NULL!");
    }
    g_render = new BaseVideoRender();
    g_render->setWindowSize(g_width, g_height);
    pthread_mutex_init(&render_mutex, NULL);
    LOGI("Render load succeed");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_setRenderTypeJNI(JNIEnv *env, jclass clazz, jint type) {
    if(type == render_type) {
        return;
    }
    render_type = type;
    delete g_render;
    g_render = NULL;
    if(type == 0) {
        g_render = new BaseVideoRender(false);
    } else if(type == 2) {
        g_render = new BaseVideoRender(true);
    } else if(type == 1) {
        g_render = new FaceVideoRender(false);
    } else if(type == 3) {
        g_render = new BigFaceRender();
    } else if(type == 4) {
        g_render = new FireworkRender();
    } else if(type == 5) {
        g_render = new HeartRender();
    } else if(type == 6) {
        g_render = new GlassBallRender();
    } else if(type == 7) {
        g_render = new NewFireworkRender();
    } else if(type == 8) {
        g_render = new StyleTransferRender();
    } else if(type == 9) {
        g_render = new RVMRender();
    }
    if(g_render) {
        g_render->setWindowSize(g_width, g_height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_releaseRenderJNI(JNIEnv *env, jclass clazz) {
    if(g_render) {
        delete g_render;
        g_render = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_setSizeJNI(JNIEnv *env, jclass clazz, jint width,
                                                            jint height) {
    g_height = height;
    g_width = width;
    if(g_render) {
        g_render->setWindowSize(width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_sendFrameDataJNI(JNIEnv *env, jclass clazz,
                                                                  jbyteArray image, jint width, jint height,
                                                               jint orientation) {
    if(g_render == NULL) {
        return;
    }
    // 读取数据
    size_t sz = env->GetArrayLength(image);
    unsigned char* buffer = new unsigned char[sz];
    env->GetByteArrayRegion(image, 0, sz, reinterpret_cast<jbyte *>(buffer));
    // 设置数据
    if(g_render)
        g_render->setYUVData(buffer, width, height, orientation);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_renderJNI(JNIEnv *env, jclass clazz) {
    if(g_render != NULL) {
        g_render->render();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_cameraspecialeffect_GLRenderJNI_savePngJNI(JNIEnv *env, jclass clazz,
                                                            jstring dir) {
    const char* tmp = env->GetStringUTFChars(dir, NULL);
    if(g_render) {
        g_render->save_next_frame(tmp);
    }
    env->ReleaseStringUTFChars(dir, tmp);
}