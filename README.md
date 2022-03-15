# Matrix Client

Mobile client for [Matrix](https://matrix.org)

## Build on Guix

in order to build on Guix, following dependencies need to be installed. 

- `json-modern-cxx`         `3.9.1`
- `curl`                    `7.79.1`
- `coeurl`                  `0.1.1`
- `lmdb`                    `0.9.29`
- `lmdbxx`                  `1.0.0`
- `libolm`                  `3.2.3`
- `matrix-client-library`   `0.0.23`
- `mtxclient`               `0.6.1`
- `qtbase`                  `5.15.2`
- `qtquickcontrols2`        `5.15.2`
- `qtdeclarative`           `5.15.2`
- `spdlog-shared-lib`       `1.9.2`

we also need to set following environment variables based on what set for `nheko` package:

```bash
cat ~/.guix-profile/bin/nheko
```

```bash
export QML2_IMPORT_PATH="..."
export QT_PLUGIN_PATH="..."
```

## Build 3rd party libraries

### Android

in order to build third-party libraries for Android, following dependencies need to be prepared:

1. Android SDK and NDK
    > after installing the Android SDK and NDK in order to have the command line access following variables need to be added to environment: 
    > ```bash
    > export ANDROID_HOME=$HOME/Android/Sdk
    > export ANDROID_SDK_ROOT=$HOME/Android/Sdk
    > export ANDROID_AVD_HOME=$HOME/.android/avd
    > export ANDROID_NDK=$ANDROID_HOME/ndk/23.1.7779620/
    > export PATH=$PATH:$ANDROID_HOME/emulator
    > export PATH=$PATH:$ANDROID_HOME/tools
    > export PATH=$PATH:$ANDROID_HOME/tools/bin
    > export PATH=$PATH:$ANDROID_HOME/platform-tools
    > export PATH=$PATH:$ANDROID_NDK
    > ```
2. Qt for Android
    > best way is to setup Qt from it's official net installer.

2. OpenSSL Library
    > prebuilt OpenSSL binaries for android can be fetched from [KDAB repository](https://github.com/KDAB/android_openssl). an easy way to set these libraries is to use Qt Creator:
    > 
    > `Tools` > `Options` > `Devices` > `Android` > `Android OpenSSL settings` > `Download OpenSSL`

Preparing the dependencies you need to run following command to build vendor libraries:
```shell
./vendor/build-android.sh all
```


### iOS

1. init submodules
    ```bash
    $ git submodule update --init
    ```

2. build openssl 
    ```bash
    $ bash ./vendor/setup_ios.sh openssl
    ```

3. build dependencies
    ```bash
    $ bash ./vendor/setup_ios.sh all
    ```

## Build for Android
since third-party libraries are not compatible with default NDK installed bt QtCreator, we need to setup custom Kit for our application. following steps describe how we can setup required configurations.

### Setup NDK 
install NDK `v23.23.1.7779620` using the SDK manager:

`Tools` > `Options` > `Devices` > `Android` > `SDK Manager` > `Tools`

**Note:** be aware the use same NDK version both for the application and third-party libraries.

### Prepare Compiler
installing the NDK, we need to add it manually to the list of compilers:

1. go to `Tools` > `Options` > `Kits` > `Compilers`
2. Clone default Clang C compiler: `Android Clang (C, arm, NDK 21.x.xxxxx)` and change it to the matching one for our installed NDK
3. Clone default Clang C++ compiler: `Android Clang (C++, arm, NDK 21.x.xxxxx)` and change it to the matching one for our installed NDK

### Setup Kit
in order to setup Kit:

1. go to `Tools` > `Options` > `Kits` > `Kits`
2. Clone default Android `5.15.x` Kit: `Android Qt %{Qt:Version} Clang Multi-Abi`
3. Set C and C++ compilers to the ones you created before


## Build for Linux

* Building the Application:

```bash
mkdir build
cd build
qmake ../MatrixClientApp.pro
```

* Building the Library:

```bash
mkdir build
cd build
qmake ../MatrixClientLib.pro
```