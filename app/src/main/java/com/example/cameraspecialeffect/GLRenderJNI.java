package com.example.cameraspecialeffect;

import android.content.Context;
import android.content.res.AssetManager;
import android.media.MediaScannerConnection;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Surface;

import java.io.File;

public class GLRenderJNI {
    static {
        // 加载动态库
        System.loadLibrary("cameraspecialeffect");
    }
    private static String save_dir_name = "CameraEffectSpecial";
    // 声明 native 方法
    private static native void initRenderJNI(AssetManager mgr, Surface surface);
    private static native void setRenderTypeJNI(int type);
    private static native void sendFrameDataJNI(byte[] image, int width, int height, int orientation);
    private static native void releaseRenderJNI();
    private static native void setSizeJNI(int width, int height); // 设置窗口宽和高
    private static native void renderJNI();
    private static native void savePngJNI(String dir);
    // java方法
    private static AssetManager m_assetManager;
    private static String logTag = "GLRenderJNI";
    private static Context m_ctx;
    private static int render_type = 0;
    private static boolean need_change_type = false;
    // 初始化函数
    public static void initRender(Context ctx, Surface surface) {
        m_ctx = ctx;
        m_assetManager = ctx.getAssets();
        initRenderJNI(m_assetManager, surface);
    }
    // 设置特效类型
    public static void setRenderType(int type) {
        render_type = type;
        need_change_type = true;
    }
    // 根据帧进行渲染
    public static void sendFrameData(byte [] image, int width, int height, int orientation) {
        sendFrameDataJNI(image, width, height, orientation);
    }
    public static void render() {
        if(need_change_type) {
            setRenderTypeJNI(render_type);
            need_change_type = false;
        }
        renderJNI();
    }
    public static void setSize(int width, int height) {
        setSizeJNI(width, height);
    }
    // 释放资源
    public static void releaseRender() {
        releaseRenderJNI();
    }
    // 保存当前帧
    public static void savePng() {
        File appDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).getAbsoluteFile();
        if(appDir.exists() == false) {
            appDir.exists();
        }
//        File save_dir = new File(appDir, save_dir_name);
//        if(save_dir.exists() == false) {
//            save_dir.mkdir();
//        }
        String file_name = System.currentTimeMillis() + ".png";
        File png_path = new File(appDir, file_name);
        // 通知 C++ 保存路径
        Log.i("[GLRenderJNI]", "save png path: " + png_path.getAbsolutePath());
        savePngJNI(png_path.getAbsolutePath());
    }
}
