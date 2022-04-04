#include "MatrixQuickView.h"

namespace PX::GUI::MATRIX{

MatrixQuickView::MatrixQuickView(QWindow *parent): 
    QmlInterface(parent) ,QQuickView(mainLibQMLurl(), parent){
}
}