# Change Log

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)

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
