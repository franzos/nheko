# This is a workaround for Qt5: https://issues.guix.gnu.org/47655

# Directory to search
SEARCH_DIR_QT5_QML=$(printenv | grep QML2_IMPORT_PATH | cut -f2 -d"=" | cut -f1 -d":")
SEARCH_DIR_QT5_PLUGINS=$(printenv | grep QT_PLUGIN_PATH | cut -f2 -d"=" | cut -f1 -d":")


qt5_qml_path_list=""
qt5_plugins_path_list=""

check_and_print_qt5_qml() {
    local path=$1
    if [[ $path == *"lib/qt5/qml"* ]]; then
        echo "$path" | cut -d'/' -f1-7
    fi
}

check_and_print_qt5_plugins() {
    local path=$1
    if [[ $path == *"lib/qt5/plugins"* ]]; then
        echo "$path" | cut -d'/' -f1-7
    fi
}

while IFS= read -r path; do
    # If it's a symbolic link
    if [ -L "$path" ]; then
        target=$(readlink -f "$path")
        result=$(check_and_print_qt5_qml "$target")
        if [ -n "$result" ]; then  # Check if result is non-empty
            qt5_qml_path_list+="$result\n"
        fi
    fi
done < <(find "$SEARCH_DIR_QT5_QML")

while IFS= read -r path; do
    if [ -L "$path" ]; then
        target=$(readlink -f "$path")
        result=$(check_and_print_qt5_plugins "$target")
        if [ -n "$result" ]; then 
            qt5_plugins_path_list+="$result\n"
        fi
    fi
done < <(find "$SEARCH_DIR_QT5_PLUGINS")

qml_env_path=$(printf "%b" "$qt5_qml_path_list" | sort | uniq | tr '\n' ':')
qt_plugin_env_path=$(printf "%b" "$qt5_plugins_path_list" | sort | uniq | tr '\n' ':')

echo QML2_IMPORT_PATH=$qml_env_path$QML2_IMPORT_PATH
echo QT_PLUGIN_PATH=$qt_plugin_env_path$QT_PLUGIN_PATH