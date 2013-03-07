#include "drawable_marble_widget.h"

#include "ros/package.h"
#include "iostream"
#include "math.h"

namespace marble_plugin {


DrawableMarbleWidget::DrawableMarbleWidget(QWidget *parent)
  : MarbleWidget(parent)
{
    std::string path = ros::package::getPath("marble_plugin")+"/etc/arrow.png";
    loadImage(m_arrow, path);

    path = ros::package::getPath("marble_plugin")+"/etc/point.png";
    loadImage(m_point, path);

    m_car = &m_arrow;
}

void DrawableMarbleWidget::loadImage(QImage& car, std::string& path )
{
    QString qpath(path.c_str());
    QImage image(qpath);
    car = image;
}


void DrawableMarbleWidget::customPaint(Marble::GeoPainter *painter)
{
   MarbleWidget::customPaint( painter );

   painter->drawImage(m_actual_position, roateCar(m_car));
}

void DrawableMarbleWidget::receiveLastPosition(GeoDataCoordinates &postion)
{
    m_last_position = m_actual_position;
    m_actual_position = postion;

}

QImage DrawableMarbleWidget::roateCar(QImage *car_image)
{

    //we can do this because of really smal changes between last and actual position
    double x1 = m_last_position.latitude(GeoDataCoordinates::Radian);
    double x2 = m_actual_position.latitude(GeoDataCoordinates::Radian);

    double y1 = m_last_position.longitude(GeoDataCoordinates::Radian);
    double y2 = m_actual_position.longitude(GeoDataCoordinates::Radian);

//    std::cout << "x1 " << x1 << " y1 " << y1 << std::endl;
//    std::cout << "x2 " << x2 << " y2 " << y2 << std::endl;
//    std::cout << "------------------------------" << std::endl;

    double alpha;

    if(posChanged(x1,y1,x2,y2, 1.0e-9))
    {
        alpha = atan2(y2-y1,x2-x1)*180*M_1_PI;
        m_last_angle_grad = alpha;
    }
    else
    {
        alpha = m_last_angle_grad;
    }



    if(showAsArrow(x1,y1,x2,y2))
    {
        car_image = &m_arrow;
    }
    else if(showAsPoint(x1,y1,x2,y2))
    {
        car_image = &m_point;
    }

//    std::cout << " alpha " << alpha << " last angle " << m_last_angle_grad << std::endl;
//    std::cout << "------------------------------" << std::endl;

    QTransform rotaion;
    rotaion.rotate(alpha);
    return car_image->transformed(rotaion);
}

bool DrawableMarbleWidget::posChanged(double x1, double y1, double x2, double y2, double threshold)
{
    double diff = std::abs(std::max(x1-x2, y1-y2));
    std::cout << "diff " << diff << std::endl;
    return  diff > threshold;
}

bool DrawableMarbleWidget::showAsArrow(double x1, double y1, double x2, double y2)
{
    return posChanged(x1,y1,x2,y2, 1.0e-8);
}

bool DrawableMarbleWidget::showAsPoint(double x1, double y1, double x2, double y2)
{
    return posChanged(x1,y1,x2,y2, 1.0e-11);
}


double DrawableMarbleWidget::dist(double x1, double x2, double y1, double y2)
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

}
