# Matrix Client

Mobile client for [Matrix](https://matrix.org)


## Build 3rd party libraries

### Android

TODO

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
