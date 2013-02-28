#include "drawable_marble_widget.h"

#include "ros/package.h"
#include "iostream"

namespace marble_plugin {


DrawableMarbleWidget::DrawableMarbleWidget(QWidget *parent)
  : MarbleWidget(parent)
{
    std::string path = ros::package::getPath("marble_plugin")+"/etc/arrow.png";
    QString qpath(path.c_str());
    QImage image(qpath);
    m_car = image;
}


void DrawableMarbleWidget::customPaint(Marble::GeoPainter *painter)
{
   MarbleWidget::customPaint( painter );

   painter->drawImage(m_actual_position, roateCar(&m_car));
}

void DrawableMarbleWidget::receiveLastPosition(GeoDataCoordinates &postion)
{
    m_last_position = m_actual_position;
    m_actual_position = postion;
}

QImage DrawableMarbleWidget::roateCar(QImage *image)
{
    //we can do this because of really smal changes between last and actual position
    double x1 = m_last_position.latitude(GeoDataCoordinates::Radian);
    double x2 = m_actual_position.latitude(GeoDataCoordinates::Radian);

    double y1 = m_last_position.longitude(GeoDataCoordinates::Radian);
    double y2 = m_actual_position.longitude(GeoDataCoordinates::Radian);

    double alpha = atan2(y2-y1,x2-x1)*180*M_1_PI;

    QTransform rotaion;
    rotaion.rotate(alpha);
    return image->transformed(rotaion);
}

double DrawableMarbleWidget::dist(double x1, double x2, double y1, double y2)
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

}
