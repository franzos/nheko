package org.pantherx.matrixclient;

import org.qtproject.qt5.android.bindings.QtApplication;
import org.qtproject.qt5.android.bindings.QtActivity;

import android.util.Log;

import android.view.WindowManager;

import android.os.Bundle;
import android.content.Intent;
import com.google.firebase.messaging.MessageForwardingService;

public class MainActivity extends QtActivity {


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

    }

    private static final String EXTRA_MESSAGE_ID_KEY_SERVER = "message_id";

    private static final String EXTRA_MESSAGE_ID_KEY = "google.message_id";

    private static final String EXTRA_FROM = "google.message_id";

    @Override
    protected void onNewIntent(Intent intent) {
        Bundle extras = intent.getExtras();
        if (extras != null) {
            String from = extras.getString(EXTRA_FROM);
            String messageId = extras.getString(EXTRA_MESSAGE_ID_KEY);

            if (messageId == null) {
                messageId = extras.getString(EXTRA_MESSAGE_ID_KEY_SERVER);
            }

            if (from != null && messageId != null) {
                Intent message = new Intent(this, MessageForwardingService.class);
                message.setAction(MessageForwardingService.ACTION_REMOTE_INTENT);
                message.putExtras(intent);
                startService(message);
            }
            setIntent(intent);    
        } else {
            Log.d("MainActivity", "No extras");
            super.onNewIntent(intent);
        }
    }
}
