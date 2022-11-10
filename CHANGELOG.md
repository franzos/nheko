# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)

## [0.1.39] - NOT RELEASED YET
### Fixed

* Fixing message preview issue for long messages in room list.
* Fixing layout issue for last message in chat.


## [0.1.38]
### Updated

* Enable CIBA integration as default.


## [0.1.37]
### Fixed

* Fixing issue in attach Enter/Return press to login.
* Removed filename in file attachments.
* Disable attach button tooltip on android.
* Fixing issue in refreshing the CM profile.
* Remove QMediaPlayer object and retrieve video metadata in QML.


## [0.1.36]
### Fixed

* Update to fixing issue to integrate gui library in other applications.


## [0.1.35]
### Changed

* LOGIN_TYPE typedef for keeping backward compatibility.


## [0.1.34]
### Changed

* Update to use CIBA from `px-auth-library-cpp`.
* Enable/Disable the CIBA codes by a build flag.


## [0.1.33]
### Fixed and Changed

* Update media control buttons to having save as icon.
* ANDROID: fixing thumbnail issue.
* Single attach at the moment in uploading.
* Fixing the warnings.


## [0.1.32]
### Fixed and Changed

* Update media control buttons. (Download/Save, Play/Pause, Volume)
* Fixing issue in sending the thumbnail of video messages.


## [0.1.31]
### Fixed

* Fixing issue in enable room encryption.
* Fixing issue in big button size in Add user dialg.
* Fixing issue in openning the audio/video messages in Android.


## [0.1.30]
### Updated

* Fixing issue in image decryption.


## [0.1.29]
### Updated

* Sorting the room list based on the last message time.
* Show the last message time for each room in room list page.
* Show ApplicationWindow as maximized in android.


## [0.1.28]
### Updated

* Mention others in the chat.
* Show the device verification status forever on the header.


## [0.1.27]
### Updated

* Enable Device verification.


## [0.1.26]
### Updated

* Settings Screen.


## [0.1.25]
### Updated

* Room Settings added.


## [0.1.24]
### Updated

* Improvement and fixing minor issue in showing title and avatar in the header.


## [0.1.23]
### Updated

* Send attachments (Video/Audio/Text File and ...).
* Download attachments.
* Fixing some minor warnings in qml codes.


## [0.1.22]
### Updated

* Cancel ciba login request in the login form.


## [0.1.21]
### Updated

* Add logout method to the gui library to logout and cleanup properly.


## [0.1.20]
### Updated

* Set parent for `RoomListModel` at creation time.
* Set Cpp as ownership for `TimelineModel` to delete properly when parent deleted.


## [0.1.19]
### Updated

* List of users in start chat and invite to the room.



## [0.1.18]
### Updated

* Speaker volume control added.



## [0.1.17]
### Updated

* Show and change the audio device input volume.
* Display the audio device input level meter.



## [0.1.15]
### Updated

* Send commands.
* Fixing crash issue 

## [0.1.14]
### Updated

* Update with upstream.


## [0.1.13]
### Fixed and Changed

* Click and see the user profile in member list dialog.
* Fixing an issue in shortkey assignment.
* Fixing an issue in showing the avatar in member list.
* Move image providers classes from backend library to here.
* Fixing jdenticon warning message.


## [0.1.12]
### Changed

* Video/Audio input settings.


## [0.1.11]
### Changed

* Disable auto start matrix client backend in gui library.


## [0.1.10]
### Fixed and Changed

* CIBA Login with access token.
* Temporarily disable verification setup button.
* Fixing issue in scrolling in Android OS.
* Fixing issue to show the menu on pressAndHold on messages in Android OS.


## [0.1.9]
### Fixed and Changed

* Add desktop file to show the application in the main menu.


## [0.1.8]
### Fixed and Changed

* Desktop notification.
* Update tap handler on messages to working better in Android.


## [0.1.6]
### Changed 

* Invisible Video/Audio calls if Camera/Mic disconnected.


## [0.1.5]
### Fixed 

* Update pro file to install header files correctly.


## [0.1.3]
### Fixed and Changed

* Adding Room members
* Fixing issue in login options

## [0.1.2]
### Fixed and Changed

* Go to end chat in the timeline view.
* Fixing issue to reload the user id field after logout based on the default value.
* Fixing minor issues.


## [0.1.1]
### Fixed

* Fixing issue in *.pro file in building and installing the target.


## [0.1.0]
### Fixed and Changed

* Visible/Invisible password in login form.
* Fixing issue in showing member count in room information page.
* Reply message support.
* Forwrad message support.
* Remove message support.
* Rename the executable file to `matrix-client`.
* Copy link to event.
* Copy message.
* Mark as read the message in option menu.
* Reaction feature.
* Emoji feature.
* Pin/Unpin message in room.
* Show avatar in the timeline header.
* Show raw message.
* Read receipts of message.
* Status indicator for messages.
* Markdown support.


## [0.0.27]
### Fixed and Changed

* Fixing issue in set default server from integrated applications.
* Set User id programatically.
* Support Login programatically in GUI library.


## [0.0.26]
### Fixed

* Fixing build error issue in applications which integrated with library.


## [0.0.25]
### Fixed and Changed

* Fixing minor issue in setting the parent of qml objects.
* Addding Server Discovery.
* Integrate Cibalogin and Password login.
* Adding login option combo box.
* Fixing issue in GUI library in stacking the Timeline after each Video Call request.
* Adding set default server address.
* Get CM user information and showing in the profile screen.

## [0.0.24]
### Changed

* Seg fault after running application if there was missed call in call auto accept.


## [0.0.23]
### Changed

* Avatar supported.


## [0.0.22]
### Changed

* Adding Material Accent and Foreground colors to Configuration.h.
* Remove extra space between slide menu and top menu.
* Force Active focus on TextField in sending message after each send.


## [0.0.21]
### Changed

* Add left margin for message box.
* Android Material theme Primary Color in Configuration.h.



## [0.0.19]
### Changed

* Handle backend `showNotification` signal and show message as pop-up.
* Fixing issue in openning PageInfo several times on title click in the room page.
* Check user existence before invitation and create direct chat.
* Add padding and gray line to side menu.


## [0.0.17]
### Fixed

* Fixing segmentation fault after multiple call request.
* Fixing sefmentation fault after room creating and send/recieve message.
* Fixing issue in decline the invitation.
* Clean cache dir if Application Version is updated.
* Update calls icons.


## [0.0.16]
### Fixed

* Fixed Android build issues related to `androidextras` and `svg` support


## [0.0.15]
### Fixed and Changed

* Disconnect signals on qml object destruction. (Segmentation fault issue)
* Adding Active Call bar status.
* Adjsut UI based on Backend Voip Support.
* Fixing issue in showing the Menu after login with CIBA.


## [0.0.14]
### Changed

* Adding User profile.


## [0.0.13]
### Changed

* Auto accept invitation in Auto Accept Call mode.
* Fixing issue in call requests (send call request to the previous contact)


## [0.0.12]
### Changed

* Fixing an issue in openning the Room Info Page.
* Optimization. (Move header from all pages to main page).


## [0.0.11]
### Fixed

* Fixing minor issue in pri file to make the project as library.


## [0.0.9]
### Changed

* Update to support single video/voice call application with auto answer feature.


## [0.0.8]
### Changed

* Update icons.
* Update QML style to match with OS Theme.
* Get Matrix Server address from user input in Login and CIBA Login pages.
* Set validator on Matrix Server and USER ID in the Forms.


## [0.0.7]
### Changed

* Update icons.
* Update GUI based on call state in Video/Voice Call.
* Add About Dialog


## [0.0.6]
### Changed

* Invite to room.
* Leave reoom.
* GUI General improvment.
* Voice/Video Call.


## [0.0.5]
### Changed

* Integrate with ciba login
* Add error Handeling
* Enable HighDpiScaling


## [android-0.0.1]
### Initial version

* Initial version released and tested in Android.
