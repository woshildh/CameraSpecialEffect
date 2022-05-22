package com.example.cameraspecialeffect;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLVideoRender implements GLSurfaceView.Renderer {
    GLSurfaceView m_glSurfaceView;
    Context m_ctx;
    GLVideoRender(Context ctx, GLSurfaceView surfaceView) {
        m_glSurfaceView = surfaceView;
        // 将这个视图放到最顶层
        m_glSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        m_glSurfaceView.setZOrderOnTop(true);
        // 设置 EGL 环境
        m_glSurfaceView.setEGLContextClientVersion(3);
        m_glSurfaceView.setRenderer(this);
        m_glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        m_ctx = ctx;
    }
    public void destroy() {
        GLRenderJNI.releaseRender();
    }
    public void drawVideoFrame(byte[] data, int width, int height, int rotation) {
        GLRenderJNI.sendFrameData(data, width, height, rotation);
    }
    public void requestRender() {
        if (m_glSurfaceView != null) {
            m_glSurfaceView.requestRender();
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        // 初始化
        GLRenderJNI.initRender(m_ctx, m_glSurfaceView.getHolder().getSurface());
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        // 设置窗口的宽和高
        GLRenderJNI.setSize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        // 在UI线程中进行绘制
        GLRenderJNI.render();
    }
}
