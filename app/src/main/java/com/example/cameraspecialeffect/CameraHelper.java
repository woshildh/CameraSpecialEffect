package com.example.cameraspecialeffect;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;

public class CameraHelper {
    private final String logTag = "[CameraHelper]";
    private Context m_ctx;
    // 相机资源管理对象
    private CameraDevice m_cameraDevice;
    private CameraManager m_cameraManager;
    private CameraCaptureSession m_captureSession;
    Boolean isOpenedFlag = false;
    String m_cameraId;  //摄像头id
    List<Size> m_preview_sz;   //预览尺寸
    Boolean use_front = false;  //摄像头是否朝前
    ImageListner mImageListner;
    // 线程
    private HandlerThread m_backgroundThread;
    private Handler m_backgroundHandler;
    private ImageReader m_imageReader;
    // 相机锁
    Semaphore camera_lock = new Semaphore(1);
    // 相机状态回调对象
    CameraDevice.StateCallback m_state_callback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            m_cameraDevice = cameraDevice;
            createCaptureSession();
            Log.i(logTag, "Open camera succeed");
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            // 关闭设备
            m_cameraDevice.close();
            m_cameraDevice = null;
            Log.i(logTag, "close camera succeed");
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int i) {
            m_cameraDevice.close();
            m_cameraDevice = null;
            Log.i(logTag, "Camera error: " + i);
        }
    };

    CameraHelper(Context ctx) {
        m_ctx = ctx;
        // 先获取相机的属性
        try {
            getCameraCharacteristic(use_front);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
        // 初始化 imagereader
        if(m_imageReader == null) {
            if(m_preview_sz.size() == 0) {
                Log.e(logTag, "createCaptureRequest: m_preview_sz is empty");
            } else {
                mImageListner = new ImageListner();
            }
        }
    }
    // 获取属性
    private void getCameraCharacteristic(Boolean use_front) throws CameraAccessException {
        CameraManager manager = (CameraManager) m_ctx.getSystemService(Context.CAMERA_SERVICE);
        String [] camList = manager.getCameraIdList();
        for(String cam: camList) {
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(cam);
            Integer is_back_face = characteristics.get(CameraCharacteristics.LENS_FACING);
            // 使用前置摄像头时
            if(use_front == true) {
                if(is_back_face == null || is_back_face != CameraCharacteristics.LENS_FACING_FRONT) {
                    continue;
                }
            } else { // 使用后置摄像头时
                if(is_back_face == null || is_back_face != CameraCharacteristics.LENS_FACING_BACK) {
                    continue;
                }
            }
            // 使用当前的camera
            m_cameraId = cam;
            // 获取预览尺寸
            StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            // 获取相机旋转角度
            int rotation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
            // 设置预览尺寸
            m_preview_sz = Arrays.asList(map.getOutputSizes(ImageFormat.YUV_420_888));
            // 通知 listener 旋转角度
            if(mImageListner != null)
                mImageListner.setRotation(rotation);
            Log.i(logTag, "preview size: " + m_preview_sz + ", rotation is: " + rotation);
            break;
        }
    }
    // 打开摄像头
    public Boolean openCamera() throws CameraAccessException {
        // 检查权限
        if(ContextCompat.checkSelfPermission(m_ctx, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            Log.i(logTag, "没有相机权限");
            return false;
        }
        // 更新是否使用前置摄像头
        getCameraCharacteristic(use_front);
        // imagereader设置监听对象
        m_imageReader = ImageReader.newInstance(m_preview_sz.get(0).getWidth(), m_preview_sz.get(0).getHeight(), ImageFormat.YUV_420_888, 2);
        m_imageReader.setOnImageAvailableListener(mImageListner, m_backgroundHandler);
        // 获取管理对象
        if(m_cameraManager == null) {
            m_cameraManager = (CameraManager) m_ctx.getSystemService(Context.CAMERA_SERVICE);
        }
        m_cameraManager.openCamera(m_cameraId, m_state_callback, m_backgroundHandler);
        startBackgroundThread();
        isOpenedFlag = true;
        return true;
    }
    private void createCaptureSession() {
        if(m_cameraDevice == null || m_imageReader ==  null) {
            return;
        }
        try {
            m_cameraDevice.createCaptureSession(Collections.singletonList(m_imageReader.getSurface()),
                    sessionStateCallback, m_backgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(logTag, "createCaptureSession: error");
        }
    }
    private final CameraCaptureSession.StateCallback sessionStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(@NonNull CameraCaptureSession cameraCaptureSession) {
            m_captureSession = cameraCaptureSession;
            try {
                CaptureRequest captureRequest = createCaptureRequest();
                if(captureRequest != null) {
                    m_captureSession.setRepeatingRequest(captureRequest, null, m_backgroundHandler);
                } else {
                    Log.e(logTag, "captureRequest is null");
                }
            } catch(CameraAccessException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {
            Log.e(logTag, "onConfigureFailed: error");
        }
    };
    private CaptureRequest createCaptureRequest() {
        if (null == m_cameraDevice) return null;
        try {
            CaptureRequest.Builder builder = m_cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            builder.addTarget(m_imageReader.getSurface());
            return builder.build();
        } catch (CameraAccessException e) {
            Log.e(logTag, e.getMessage());
            return null;
        }
    }
    public void startBackgroundThread() {
        m_backgroundThread = new HandlerThread("CameraBackground");
        m_backgroundThread.start();
        m_backgroundHandler = new Handler(m_backgroundThread.getLooper());
    }
    // 关闭摄像头
    public void closeCamera() throws InterruptedException {
        try {
            if (null != m_captureSession) {
                m_captureSession.close();
                m_captureSession = null;
            }
            if (null != m_cameraDevice) {
                m_cameraDevice.close();
                m_cameraDevice = null;
            }
            if (null != m_imageReader) {
                m_imageReader.close();
                m_imageReader = null;
            }
        } finally {

        }
        if(m_imageReader != null)
            m_imageReader.setOnImageAvailableListener(null, null);
        isOpenedFlag = false;
    }
    // 切换摄像头方向
    public void changeCameraOrientation() throws InterruptedException, CameraAccessException {
        // 如果摄像头没有打开，则直接返回
        if(isOpend() == false) {
            return;
        }
        // 关闭摄像头
        closeCamera();
        // 改变朝向
        use_front = !use_front;
        // 打开摄像头
        openCamera();
    }
    // 获取摄像头朝向
    public Boolean cameraIsFront() {
        return use_front;
    }
    // 获取摄像头是否打开的状态
    public Boolean isOpend() {
        return isOpenedFlag;
    }
    // 设置render
    public void setVideoRender(GLVideoRender render) {
        if(mImageListner != null) {
            mImageListner.setRender(render);
        }
    }
}
