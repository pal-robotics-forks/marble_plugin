#ifndef CIRCLE_H
#define CIRCLE_H

#include <marble/GeoDataCoordinates.h>
#include <std_msgs/ColorRGBA.h>

struct Circle
{
public:

  Marble::GeoDataCoordinates mid;
  std_msgs::ColorRGBA color;
  double r;

};

struct CircleSet
{
public:
  std::list<Circle> circles;
  ros::Time creation_time;
  ros::Duration lifetime;
};

#endif // CIRCLE_H
