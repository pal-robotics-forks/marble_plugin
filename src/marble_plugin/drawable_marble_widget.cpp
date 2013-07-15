#include "drawable_marble_widget.h"

#include "ros/package.h"
#include "iostream"
#include "math.h"

using namespace marble_plugin;


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

  painter->setPen(QPen(Qt::blue, 2));
  // Polygon of QPointF doesn't work here, because this is interpreted as screen coordinates
  foreach (GeoDataLineString line, m_marker_line) {
    painter->drawPolyline(line);
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

std::pair<double, double> DrawableMarbleWidget::toGpsCoordinates(double x, double y)
{
  //Schlossplatz. TODO: get it from bag file
  double ref_lat = 49.021267;
  double ref_lon = 8.3983866;

  return GetAbsoluteCoordinates(x, y, ref_lat, ref_lon);
}

void DrawableMarbleWidget::visualizationCallback(const visualization_msgs::MarkerConstPtr &marker) {
  //save the marker in the right data structure, so customPaint() can use it to paint
  switch (marker->type) {
  case visualization_msgs::Marker::LINE_STRIP:
  {
    if(marker->ns == "object_track_line_filtered" || marker->ns == "track_driven_line")
    {
      //read out points and create a line
      GeoDataLineString geo_polygon;
      for (size_t i=0; i<marker->points.size(); i++) {
        std::pair<double, double> coords = toGpsCoordinates(marker->points.at(i).x, marker->points.at(i).y);

        GeoDataCoordinates geo_coords;
        geo_coords.set(coords.second, coords.first, GeoDataCoordinates::Degree, GeoDataCoordinates::Degree);
        geo_polygon.append(geo_coords);
     }

      m_marker_line.enqueue(geo_polygon);

      if(m_marker_line.size() > 100 )
      {
        m_marker_line.dequeue();
      }
    }

    break;
  }
  case visualization_msgs::Marker::CUBE:
    break;
  }
}

// --- From Gps Tools

std::pair<double, double> DrawableMarbleWidget::GetAbsoluteCoordinates( double x , double y , double ref_lat , double ref_lon, double ref_bearing)
{
  double d = sqrt(x*x+y*y);
  double bearing = atan2(x,y) + ref_bearing;

  return GetNewPointBearingDistance(ref_lat, ref_lon, bearing, d);
}

std::pair<double, double> DrawableMarbleWidget::GetNewPointBearingDistance(double a_lat, double a_lon, double bearing, double distance)
{
  const double R = 6371000; //Earth radius in m

  a_lat = a_lat /180 * M_PI;
  a_lon = a_lon /180 * M_PI;

  double b_lat = asin( sin(a_lat)*cos(distance/R) + cos(a_lat)*sin(distance/R)*cos(bearing) );
  double b_lon = a_lon + atan2(sin(bearing)*sin(distance/R)*cos(a_lat), cos(distance/R)-sin(a_lat)*sin(b_lat));

  b_lat = b_lat * 180 / M_PI;
  b_lon = b_lon * 180 / M_PI;

  return std::make_pair(b_lat, b_lon);

}
