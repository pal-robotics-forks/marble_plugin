#ifndef COLORED_POLYGON_H
#define COLORED_POLYGON_H


#include <marble/GeoDataLineString.h>
#include <std_msgs/ColorRGBA.h>

struct ColoredPolygon
{
public:
  Marble::GeoDataLineString polygon;
  std_msgs::ColorRGBA color;
};


#endif // COLORED_POLYGON_H
