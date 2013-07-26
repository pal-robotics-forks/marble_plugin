#include "drawable_marble_widget.h"

#include "ros/package.h"
#include "iostream"
#include "math.h"

#include <boost/foreach.hpp>

using namespace marble_plugin;


DrawableMarbleWidget::DrawableMarbleWidget(QWidget *parent)
  : MarbleWidget(parent),
    m_current_pos(M_PI_2,M_PI_2),
    m_matched_pos(M_PI_2,M_PI_2),
    m_ref_lat(49.021267),
    m_ref_lon(8.3983866)
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
  // Polygon of QPointF doesn't work here, because it is interpreted as screen coordinates
  foreach (ColoredPolygon line, m_marker_line)
  {
    //set color
    QColor lineColor(Qt::blue);
    std_msgs::ColorRGBA color_msg = line.color;
    getColor(lineColor, color_msg);

    //draw line
    painter->setPen(QPen(lineColor, 2));
    painter->drawPolyline(line.polygon);
  }

  int i = 0;
  foreach (Circle circle, m_marker_circle)
  {
    //set color
    QColor color(Qt::blue);
    std_msgs::ColorRGBA color_msg = circle.color;
    getColor(color, color_msg);

    //draw line
    painter->setPen(QPen(color, 2));
    painter->setBrush(QBrush(color, Qt::SolidPattern));
    painter->drawEllipse(circle.mid, circle.r, circle.r, true);
    i++;
  }
  m_marker_circle.clear();

}

void DrawableMarbleWidget::getColor(QColor& outputColor, std_msgs::ColorRGBA color_msg)
{
  outputColor.setRed(255*color_msg.r);
  outputColor.setGreen(255*color_msg.g);
  outputColor.setBlue(255*color_msg.b);
  outputColor.setAlpha(255*color_msg.a);

  //convert white to red, in order to see something
//  if(outputColor.red() == 255 && outputColor.green() == 255 && outputColor.blue() == 255)
//  {
//    outputColor.setBlue(0);
//    outputColor.setGreen(0);
//  }
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
  return GetAbsoluteCoordinates(x, y, m_ref_lat, m_ref_lon, -M_PI_2);
}


void DrawableMarbleWidget::visualizationMarkerArrayCallback(const visualization_msgs::MarkerArrayConstPtr &markers)
{
  BOOST_FOREACH(visualization_msgs::Marker marker, markers->markers)
  {
    addMarker(marker);
  }
}

void DrawableMarbleWidget::visualizationCallback(const visualization_msgs::MarkerConstPtr &marker) {
  addMarker(*marker);
}

void DrawableMarbleWidget::addMarker(const visualization_msgs::Marker &marker)
{
  //save the marker in the right data structure, so customPaint() can use it to paint
  switch (marker.type) {
  case visualization_msgs::Marker::LINE_STRIP:
  {
    //read out points and create a line
    GeoDataLineString geo_polygon;
    for (size_t i=0; i<marker.points.size(); i++) {
      std::pair<double, double> coords = toGpsCoordinates(marker.points.at(i).x, marker.points.at(i).y);

      GeoDataCoordinates geo_coords;
      geo_coords.set(coords.second, coords.first, GeoDataCoordinates::Degree, GeoDataCoordinates::Degree);
      geo_polygon.append(geo_coords);
    }

    ColoredPolygon polygon;
    polygon.polygon = geo_polygon;
    polygon.color = marker.color;

    m_marker_line.enqueue(polygon);

    if(m_marker_line.size() > 100 )
      m_marker_line.dequeue();

    break;
  }

  case visualization_msgs::Marker::LINE_LIST:
  {
    //read out points and create a line
    for (size_t i=0; i<marker.points.size()-1; i+=2) {
      std::pair<double, double> coords1 = toGpsCoordinates(marker.points.at(i).x, marker.points.at(i).y);
      std::pair<double, double> coords2 = toGpsCoordinates(marker.points.at(i+1).x, marker.points.at(i+1).y);

      GeoDataCoordinates geo_coords1;
      geo_coords1.set(coords1.second, coords1.first, GeoDataCoordinates::Degree, GeoDataCoordinates::Degree);
      GeoDataCoordinates geo_coords2;
      geo_coords2.set(coords2.second, coords2.first, GeoDataCoordinates::Degree, GeoDataCoordinates::Degree);
      GeoDataLineString geo_polygon;
      geo_polygon.append(geo_coords1);
      geo_polygon.append(geo_coords2);

      ColoredPolygon polygon;
      polygon.polygon = geo_polygon;
      polygon.color = marker.color;

      m_marker_line.enqueue(polygon);

      if(m_marker_line.size() > 100 )
        m_marker_line.dequeue();
    }

    break;
  }
  case visualization_msgs::Marker::CUBE:
    //! \todo project CUBES to ground plane and draw them as filled polygons
    break;
  case visualization_msgs::Marker::SPHERE_LIST:
    for (size_t i=0; i<marker.points.size(); i++) {
      std::pair<double, double> coords = toGpsCoordinates(marker.points.at(i).x, marker.points.at(i).y);
      GeoDataCoordinates geo_coords;
      geo_coords.set(coords.second, coords.first, GeoDataCoordinates::Degree, GeoDataCoordinates::Degree);

      Circle circle;
      circle.mid = geo_coords;
      circle.r = 0.00002*marker.scale.x;
      circle.color = marker.color;

      m_marker_circle.enqueue(circle);
//      if(m_marker_circle.size() > 4 )
//        m_marker_line.dequeue();
    }

    break;
  }
}

void DrawableMarbleWidget::referenceGpsCallback(const sensor_msgs::NavSatFixConstPtr &reference)
{
  m_ref_lat = reference->latitude;
  m_ref_lon = reference->longitude;
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
