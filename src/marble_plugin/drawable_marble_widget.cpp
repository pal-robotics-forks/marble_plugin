#include "drawable_marble_widget.h"

namespace marble_plugin {


DrawableMarbleWidget::DrawableMarbleWidget(QWidget *parent)
  : MarbleWidget(parent)
{
}


void DrawableMarbleWidget::customPaint(Marble::GeoPainter *painter)
{
  // MarbleWidget::customPaint( painter );

//    GeoDataCoordinates home(8.4, 49.0, 0.0, GeoDataCoordinates::Degree);
//    painter->setPen(Qt::green);
//    painter->drawEllipse(home, 7, 7);
//    painter->setPen(Qt::black);
//    painter->drawText(home, "Hello Marble!");

}

}
