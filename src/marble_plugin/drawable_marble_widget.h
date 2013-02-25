#ifndef DRAWABLE_MARBLEWIDGET_H
#define DRAWABLE_MARBLEWIDGET_H

#include "marble/MarbleWidget.h"
#include "marble/GeoPainter.h"


using namespace Marble;

namespace marble_plugin {

class DrawableMarbleWidget : public MarbleWidget
{
  Q_OBJECT
  
  public:

    DrawableMarbleWidget(QWidget *parent=0);
    void receiveLastPosition(GeoDataCoordinates& postion);


  public slots:

    
  protected:
    virtual void customPaint(GeoPainter *painter);

  private slots:

    
  private:

    QImage roateCar(QImage* image);
    double dist(double x1, double x2, double y1, double y2);

    GeoDataCoordinates m_actual_position;
    GeoDataCoordinates m_last_position;
    QImage m_car;

    
  private:
//    Q_DISABLE_COPY(DrawableMarbleWidget);
};

}

#endif //DRAWABLE_MARBLEWIDGET_H
