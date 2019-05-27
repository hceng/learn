package cn.hceng.lights;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.support.v4.app.NotificationCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    private Button button = null;
    boolean flashing = false;
    final private int LED_NOTIFICATION_ID = 12;

    private Handler mHander = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initView();
    }

    private void initView() {
        button = (Button) findViewById(R.id.BUTTON);
        button.setOnClickListener(new MyButtonListener());
    }

    class MyButtonListener implements View.OnClickListener {
        @Override
        public void onClick(View v) {
            flashing = !flashing;
            if (flashing) {
                button.setText("Stop Flashing the Light");
            } else {
                button.setText("Flashing Light after 20S");
            }

            mHander.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (flashing)
                        FlashingLight();
                    else
                        StopFlashingLight();

                }
            }, 20000);
        }
    }

    private void FlashingLight() {
        Intent mIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.hceng.cn"));
        PendingIntent mPendingIntent = PendingIntent.getActivity(this, 0, mIntent, 0);

        NotificationManager mNotificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

        if (Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) { //Android O之上，需要渠道
            NotificationChannel notificationChannel =
                    new NotificationChannel("channelid1", "channelname", NotificationManager.IMPORTANCE_HIGH);//如果用IMPORTANCE_NOENE需要在系统的设置里面开启渠道，通知才能正常弹出
            mNotificationManager.createNotificationChannel(notificationChannel);
        }
        NotificationCompat.Builder mBuilder = new NotificationCompat.Builder(MainActivity.this, "channelid1");
        mBuilder.setContentTitle("Notification") //设置通知栏标题
                .setContentText("Lights demo") //设置通知栏显示内容
                .setSmallIcon(R.mipmap.ic_launcher) //设置通知ICON
                .setContentIntent(mPendingIntent) //设置点击通知事件
                .setAutoCancel(true) //设置点击通知后自动取消
                .setDefaults(Notification.FLAG_SHOW_LIGHTS) //Notification.FLAG_SHOW_LIGHTS  Notification.DEFAULT_SOUND(需要权限)
                .setLights(Color.BLUE, 1000, 1000); //ledARGB ledOnMS ledOffMS

        Notification mNotification = mBuilder.build();

        //设置lights参数的另一种方式
        /*
        mNotification.flags |= Notification.FLAG_SHOW_LIGHTS;
        mNotification.ledARGB = 0xFF0000FF; //Color.BLUE
        mNotification.ledOnMS = 1000;
        mNotification.ledOffMS = 100;
        */
        mNotificationManager.notify(LED_NOTIFICATION_ID, mNotification); //发出通知
    }

    private void StopFlashingLight() {
        NotificationManager mNotificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        mNotificationManager.cancel(LED_NOTIFICATION_ID); //取消通知
    }
}

