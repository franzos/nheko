current_dir=$(basename "$PWD")
expected_dir="matrix-client"

if [ "$current_dir" != "$expected_dir" ]; then
    echo "Please run this script in the parent folder '$expected_dir'"
    exit 1
fi

rm -rf build
mkdir build
cd build || exit
qmake ../MatrixClientLib.pro
make
qmake ../MatrixClientApp.pro
make