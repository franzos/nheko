## Download and install by qt online installer

* Choose `~/Qt` to install. (The env paths for building the dependencies are based on this path)
* Enable Qt-5.15.2 and other requirements.

## Install SDK Manager

* Using a stable vpn.
* Download sdk from google repo and unzip in `~/Android/Sdk/`.
* If the extracted directory name was `cmdline-tools`, rename it to `tools`. So you should have this path after extraction: `~/Android/Sdk/tools`.

## SDK and other Android tools installation

* Open `qtcreator`.
* Go to `Edit` > `Prefrences` > `Devices` > `Android` > Click on `SDK Manager`.
* Enable These:
```
- Tools
   NDK 23.1.77...
   NDK 21.3.65...
   Android Emulator
   Android SDK Platform-Tools
   Android SDK Build-Tools 31
   Android SDK Build-Tools 28.0.3
   CMake

- Android 12.0(S)
   SDK Platform
   ARM 64 v8a System Image
```
* Apply.
* It takes time to download and install.

## OpenSSL Library
    > prebuilt OpenSSL binaries for android can be fetched from [KDAB repository](https://github.com/KDAB/android_openssl). an easy way to set these libraries is to use Qt Creator:

    > `Tools` > `Options` > `Devices` > `Android` > `Android OpenSSL settings` > `Download OpenSSL`


## Connect your android device

* Plug-in the android device via USB.
* Enable developer mode in the android device.
* Enable USB Debugging mode in the android device.
* If everything goes well you should see your connected device in the output of `adb devices` command in the shell.

