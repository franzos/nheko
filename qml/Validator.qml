import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import MatrixClient 1.0
import QmlInterface 1.0

Item {  

    RegExpValidator { 
        id: _urlRegex
        regExp: /^https?:\/\/(?:.*\/)?[^\/.]+$/ 
    }

    RegExpValidator {
        id: _userIdRegex
        regExp: /^@.*$/ 
    }

    function matrixServerRegex(){
        return _urlRegex
    }

    function userIdRegex(){
        return _userIdRegex
    }

    function checkMatrixServerUrl(url){
        if (url[url.length -1] == "/")
            url = url.slice(0, -1)
        return url
    }
}