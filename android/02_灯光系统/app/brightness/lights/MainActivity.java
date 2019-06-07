package cn.hceng.lights;

import android.provider.Settings;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.SeekBar;


public class MainActivity extends AppCompatActivity {

    private SeekBar mBacklightSeekBar = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();
    }

    private void initView() {

        mBacklightSeekBar = (SeekBar) findViewById(R.id.SEEKBAR);

        //关闭自动背光,以便后面手动设置亮度
        Settings.System.putInt(getContentResolver(),
                Settings.System.SCREEN_BRIGHTNESS_MODE,
                Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL);

        //获取当前背光,自动设置当前SeekBar位置
        try {
            int brightness = android.provider.Settings.System.getInt(getContentResolver(),
                    android.provider.Settings.System.SCREEN_BRIGHTNESS);
            mBacklightSeekBar.setProgress(brightness * 100 / 255);
        } catch (Settings.SettingNotFoundException e) {
            e.printStackTrace();
        }

        //监听SeekBar改变,对应设置背光
        mBacklightSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                android.provider.Settings.System.putInt(getContentResolver(),
                        android.provider.Settings.System.SCREEN_BRIGHTNESS,
                        mBacklightSeekBar.getProgress() * 255 / 100);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }
}

