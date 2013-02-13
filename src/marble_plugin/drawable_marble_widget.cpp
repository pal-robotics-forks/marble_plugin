#include "drawable_marble_widget.h"

namespace marble_plugin {


DrawableMarbleWidget::DrawableMarbleWidget(QWidget *parent)
  : MarbleWidget(parent)
{
}


void DrawableMarbleWidget::customPaint(Marble::GeoPainter *painter)
{

  QPen pen(Qt::red);
  pen.setWidth(20);
  painter->setPen(pen);

  painter->drawEllipse(50,50, 60, 60);

}

}
