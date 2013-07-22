// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

/* -- BEGIN LICENSE BLOCK ----------------------------------------------

Copyright (c) 2013, TB
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by TB.  The name of the
TB may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  -- END LICENSE BLOCK ----------------------------------------------*/

//----------------------------------------------------------------------
/*!\file
*
* \author  Tobias Bär <baer@fzi.de>
*          Jan Aidel <aiden@fzi.de>
* \date    2013-01-11
*
*/
//----------------------------------------------------------------------


#ifndef DRAWABLE_MARBLEWIDGET_H
#define DRAWABLE_MARBLEWIDGET_H

#include "marble/MarbleWidget.h"
#include "marble/GeoPainter.h"
#include "marble/GeoDataLineString.h"

#include <QList>
#include <QPolygonF>
#include <QQueue>

#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <sensor_msgs/NavSatFix.h>
#include <std_msgs/ColorRGBA.h>

using namespace Marble;

namespace marble_plugin {

class DrawableMarbleWidget : public MarbleWidget
{
  Q_OBJECT

public:
  DrawableMarbleWidget(QWidget *parent=0);

  void setCurrentPosition( GeoDataCoordinates& postion );
  void setMatchedPosition( GeoDataCoordinates& postion );

  void visualizationMarkerArrayCallback(const visualization_msgs::MarkerArrayConstPtr &markers);
  void visualizationCallback(const visualization_msgs::MarkerConstPtr &marker);

  void referenceGpsCallback(const sensor_msgs::NavSatFixConstPtr &reference);

protected:
  virtual void customPaint(GeoPainter *painter);

private:
  void addMarker(const visualization_msgs::Marker& marker);
  QImage roateCar(QImage* car_image);

  void loadImage(QImage& car, std::string& path );
  bool posChanged(double x1, double y1, double x2, double y2, double threshold);
  std::pair<double, double> toGpsCoordinates(double x, double y);

  /*! Get absolute coordinates in DEGREE of a given position to a reference position */
  std::pair<double, double> GetAbsoluteCoordinates( double x , double y , double ref_lat , double ref_lon, double ref_bearing = 0. );
  std::pair<double, double> GetNewPointBearingDistance(double a_lat, double a_lon, double bearing, double distance);

  QImage m_arrow;
  QImage m_current_pos_icon;
  QImage m_matched;


  QQueue<GeoDataLineString> m_marker_line;
  QQueue<std_msgs::ColorRGBA> m_colors;

  GeoDataCoordinates m_current_pos;
  GeoDataCoordinates m_matched_pos;
  GeoDataCoordinates m_last_matched_position;


  double m_ref_lat;
  double m_ref_lon;


  Q_DISABLE_COPY(DrawableMarbleWidget);
};

}

#endif //DRAWABLE_MARBLEWIDGET_H
