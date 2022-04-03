#include "MatrixQuickView.h"
#include "ColorImageProvider.h"

namespace PX::GUI::MATRIX{

MatrixQuickView::MatrixQuickView(QWindow *parent): 
    QmlInterface(parent) ,QQuickView(mainLibQMLurl(), parent){
        engine()->addImageProvider(QStringLiteral("colorimage"), new ColorImageProvider());
}
}