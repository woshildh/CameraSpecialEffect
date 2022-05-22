package com.example.cameraspecialeffect;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.content.DialogInterface;
import android.hardware.camera2.CameraAccessException;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.example.cameraspecialeffect.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    // 日志标签
    private String logTag = "MainActivity";
    // 所有的按钮
    private Button open_close_btn, choose_btn, change_btn, save_btn;
    // 展示视图
    GLSurfaceView show_view;
    // 相机管理类
    CameraHelper cameraHelper = null;
    // 弹出框
    private AlertDialog.Builder m_alertDialog = null;
    // 特效的id
    private int effect_id = 0;
    // 渲染器
    GLVideoRender m_render = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 获取button和视图的
        open_close_btn = findViewById(R.id.open_close_btn);
        change_btn = findViewById(R.id.change_btn);
        choose_btn = findViewById(R.id.choose_btn);
        save_btn = findViewById(R.id.save_btn);

        // 获取纹理视图
        show_view = findViewById(R.id.show_surface);
        // render
        m_render = new GLVideoRender(this, show_view);
        // 打开摄像头
        cameraHelper = new CameraHelper(this);
        cameraHelper.setVideoRender(m_render);
        // 设置弹出框
        m_alertDialog = new AlertDialog.Builder(this);
        final String items[] = {"原始", "人脸检测/关键点", "美白", "大头", "烟花", "爱心贴图", "3D对象", "烟花2", "风格迁移", "虚拟背景"};
        m_alertDialog.setTitle("选择特效").setSingleChoiceItems(items, 0, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                effect_id = i;
            }
        });
        m_alertDialog.setPositiveButton("确认", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                GLRenderJNI.setRenderType(effect_id);
                Log.i(logTag, "当前是特效是: " + items[effect_id]);
            }
        });
        // 设置按钮的回调
        open_close_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(cameraHelper.isOpend()) {
                    try {
                        cameraHelper.closeCamera();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else {
                    try {
                        cameraHelper.openCamera();
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
        change_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    cameraHelper.changeCameraOrientation();
                } catch (InterruptedException | CameraAccessException e) {
                    e.printStackTrace();
                }
            }
        });
        choose_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                m_alertDialog.setSingleChoiceItems(items, effect_id, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        effect_id = i;
                    }
                });
                m_alertDialog.show();
            }
        });
        save_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                GLRenderJNI.savePng();
            }
        });
    }
}