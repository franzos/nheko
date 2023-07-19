package org.pantherx.matrixclient;

import com.google.firebase.messaging.cpp.ListenerService;
import com.google.firebase.messaging.RemoteMessage;

import android.app.ActivityManager;
import android.content.Context;
import android.util.Log;

import java.util.Map;
import org.json.JSONObject;
import org.json.JSONException;

public class NotificationListenerService extends ListenerService {
    private static final String TAG = "FIREBASE.MESSAGING.NATIVE";

    @Override
    public void onMessageReceived(RemoteMessage message) {
        Log.d(TAG, "A message has been received.");

        Context ctx = this.getApplicationContext();
        Map<String, String> data = message.getData();
        if (ctx != null && !this.isInForeground()) {
            processIncomingMessage(ctx, data);
        }
        super.onMessageReceived(message);
    }

    protected Boolean isInForeground() {
        ActivityManager.RunningAppProcessInfo processInfo = new ActivityManager.RunningAppProcessInfo();
        ActivityManager.getMyMemoryState(processInfo);
        if (processInfo.importance == ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
            Log.d(TAG, "Application is in foreground");
            return true;
        }
        Log.d(TAG, "Application is in background");
        return false;
    }

    protected void processIncomingMessage(Context ctx, Map<String, String> data) {
        Log.d(TAG, "NEW MESSAGE: (data)");
        for (Map.Entry<String, String> entry : data.entrySet()) {
            Log.d(TAG, "   - " + entry.getKey() + " -> " + entry.getValue());
        }

        String title = data.get("sender_display_name");
        String body = "New notification";
        try {
            String msgType = data.get("type");
            JSONObject content = new JSONObject(data.get("content"));
            if (msgType != null && content != null) {
                switch (msgType) {
                    case "m.room.message":
                        body = content.getString("body");
                        break;
                    case "m.call.invite":
                        JSONObject offer = content.getJSONObject("offer");
                        String sdp = offer.getString("sdp");
                        body = "Incoming " + (sdp.contains("video") ? "Video " : "Audio ") + "Call";
                    default:
                        // additional message types can be handled here
                        break;
                }
            } else {
                Log.d(TAG, "Message type or content is null");
            }
        } catch (JSONException e) {
            Log.d(TAG, "Message content is not JSON");
        }
        NotificationClient.Notify(ctx, title, body);
    }
}
