#import "UIKit/UIKit.h"
#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>

#include "../../cpp/notifications/notificationhandler.h"
#include <QByteArray>
#include <QString>
#include <QDebug>


@interface QIOSApplicationDelegate : UIResponder<UIApplicationDelegate>
@end

@interface QIOSApplicationDelegate (NotificationDelegate)
@end


@implementation QIOSApplicationDelegate
- (void) applicationDidBecomeActive:(UIApplication *)application {
    application.applicationIconBadgeNumber = 0;
}
@end


@implementation QIOSApplicationDelegate(NotificationDelegate)


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [application registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];

    [application registerForRemoteNotifications];
    NSLog(@"registered for remote notifications");
    NotificationHandler::Instance()->submitLog("push notification registered");
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    NSLog(@"Did register for remote notifications with device token (%@)", deviceToken);

    // APNS push token needs to be converted to base64:
    //      https://github.com/matrix-org/matrix-ios-sdk/blob/822095fc29093ad0aa5e2dd48ff5b3c71183aef4/README.rst#L431
    NSString *tokenStr = [deviceToken base64EncodedStringWithOptions:0];
    NSLog(@"Device token = %@", tokenStr);
    NotificationHandler::Instance()->setToken(QByteArray::fromNSData(deviceToken));
    NotificationHandler::Instance()->submitLog("device token received");
}

- (void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error {
    NSLog(@"Did Fail to register for remote notifications");
    NSLog(@"%@, %@", error, error.localizedDescription);
    NSString *errStr = [error localizedDescription];
    NotificationHandler::Instance()->submitError(QString::fromNSString(errStr));
}

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))handler {
    NotificationHandler::Instance()->submitLog("New message received");

    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:userInfo
            options:NSJSONWritingPrettyPrinted
            error:&error];

    if (!jsonData) {
        NotificationHandler::Instance()->submitError("Failed to parse received message");
    } else {
        NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        NotificationHandler::Instance()->submitMessage(QString::fromNSString(jsonString));
    }

    handler(UIBackgroundFetchResultNewData);
}

@end

