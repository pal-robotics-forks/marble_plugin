#include "drawable_marble_widget.h"

#include "ros/package.h"
#include "iostream"
#include "math.h"

namespace marble_plugin {


DrawableMarbleWidget::DrawableMarbleWidget(QWidget *parent)
  : MarbleWidget(parent),
    m_current_pos(M_PI_2,M_PI_2),
    m_matched_pos(M_PI_2,M_PI_2)
{
  std::string path;
  path = ros::package::getPath("marble_plugin")+"/etc/arrow.png";
  loadImage(m_arrow, path);

  path = ros::package::getPath("marble_plugin")+"/etc/sattelite.png";
  loadImage(m_current_pos_icon, path);

  path = ros::package::getPath("marble_plugin")+"/etc/matched.png";
  loadImage(m_matched, path);
}

void DrawableMarbleWidget::loadImage( QImage& icon , std::string& path )
{
  QString qpath(path.c_str());
  QImage image(qpath);
  icon = image;
}

void DrawableMarbleWidget::customPaint(Marble::GeoPainter *painter)
{
  MarbleWidget::customPaint( painter );

  // @TODO: Check if both points are available

  if( ! m_current_pos.isPole() )
  {
    painter->drawImage( m_current_pos , m_current_pos_icon );
  }

  if( ! m_matched_pos.isPole() )
  {
    painter->drawImage( m_matched_pos , roateCar( &m_arrow ) );
  }

  if( ! ( m_current_pos.isPole() || m_matched_pos.isPole() ) )
  {
    painter->save();
    painter->setPen( QPen(Qt::red  , 2 ) );
    painter->drawLine( m_matched_pos , m_current_pos );
    painter->restore();
  }

  painter->setPen(QPen(Qt::blue, 5));
  foreach (QPolygonF polygon, lines) {
    painter->drawPolyline(polygon);
  }
}

void DrawableMarbleWidget::setMatchedPosition( GeoDataCoordinates &postion )
{
  m_last_matched_position = m_matched_pos;
  m_matched_pos = postion;
}

void DrawableMarbleWidget::setCurrentPosition( GeoDataCoordinates &postion )
{
  m_current_pos = postion;
}


QImage DrawableMarbleWidget::roateCar(QImage *car_image)
{
  double x1 = m_last_matched_position.latitude(GeoDataCoordinates::Radian);
  double x2 = m_matched_pos.latitude(GeoDataCoordinates::Radian);

  double y1 = m_last_matched_position.longitude(GeoDataCoordinates::Radian);
  double y2 = m_matched_pos.longitude(GeoDataCoordinates::Radian);

  static double alpha = atan2( y2 - y1 , x2 - x1 ) * 180 * M_1_PI;
  if(posChanged( x1 , y1 , x2 , y2 ,  1.0e-9 ) )
  {
    alpha = atan2( y2 - y1 , x2 - x1 ) * 180 * M_1_PI;
  }

  QTransform rotation;
  rotation.rotate( alpha );
  return car_image->transformed( rotation );
}

bool DrawableMarbleWidget::posChanged(double x1, double y1, double x2, double y2, double threshold)
{
  double diff = std::abs(std::max(x1-x2, y1-y2));
  return  diff > threshold;
}

void DrawableMarbleWidget::visualizationCallback(const visualization_msgs::Marker &marker) {
  //save the marker in the right data structure, so customPaint() can use it to paint
  switch (marker.type) {
  case visualization_msgs::Marker::LINE_STRIP:
  {
    //read out points and create a line
    QPolygonF polygon;
    for (size_t i=0; i<marker.points.size(); i++) {
      polygon << QPointF(marker.points.at(i).x, marker.points.at(i).y);
    }
    //save the Line
    if (!lines.contains(polygon)) {
      lines.append(polygon);
    }
    break;
  }
  case visualization_msgs::Marker::CUBE:
    break;
  }
}

}
