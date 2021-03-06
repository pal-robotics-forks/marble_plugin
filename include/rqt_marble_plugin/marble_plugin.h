// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

/* -- BEGIN LICENSE BLOCK ----------------------------------------------

Copyright (c) 2013, TB
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
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

#ifndef _MARBLE_PLUGIN_H
#define _MARBLE_PLUGIN_H

// ROS Plugin Includes
#include <rqt_gui_cpp/plugin.h>
#include <ros/ros.h>

// Message Includes
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/Twist.h>

// Qt Includes
#include <QtCore/QObject>
#include <QMutex>

#include <marble/MapThemeManager.h>

// Own Includes
#include <ui_marble_plugin.h>
#include "drawable_marble_widget.h"
#include "manage_kml_dialog.h"

namespace rqt_marble_plugin {

class MarblePlugin
  : public rqt_gui_cpp::Plugin
{


  Q_OBJECT


public:


  MarblePlugin();
  virtual void initPlugin(qt_gui_cpp::PluginContext& context);
  virtual void shutdownPlugin();
  virtual void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const;
  virtual void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings);

  // Comment in to signal that the plugin has a way to configure it
  //bool hasConfiguration() const;
  //void triggerConfiguration();

  void GpsCallbackCurrent( const sensor_msgs::NavSatFixConstPtr& gpspt );
  void GpsCallbackMatched( const sensor_msgs::NavSatFixConstPtr& gpspt );


private slots:
  void gpsCoordinateSelected(qreal lon, qreal lat, GeoDataCoordinates::Unit unit);
  void processMarkerCheckBoxCLicked();

private:

Q_SIGNALS:

  void NewGPSPosition(qreal,qreal);

  void ZoomIn(FlyToMode);
  void ZoomOut(FlyToMode);
  void flyTo(GeoDataLookAt, FlyToMode);

  private Q_SLOTS:

  void ChangeGPSTopicCurrentGPS(const QString &topic_name);
  void ChangeGPSTopicMatchedGPS(const QString &topic_name);

  void ChangeMarbleModelTheme(int idx );
  void FindNavSatFixTopics();

  void ManageKML();

  private:

  void subscribeVisualization();
  void clearKMLData();
  void addKMLData(std::map<QString, bool>& kml_files, bool overwrite);
  void mapcontrolCallback(const geometry_msgs::TwistConstPtr &msg);

  Ui_Form ui_;

  QWidget* widget_;

  Marble::MapThemeManager m_map_theme_manager;

  ros::Subscriber m_current_pos_subscriber;
  ros::Subscriber m_matched_pos_subscriber;
  ros::Subscriber m_visualization_subscriber;
  ros::Subscriber m_visualization_marker_array_subscriber;
  ros::Subscriber m_reference_gps_subscriber;
  ros::Publisher m_selected_gps_pos_publisher;

  ros::Subscriber m_mapcontrol_subscriber;

  std::map< QString, bool> m_last_kml_data;

};
} // namespace
#endif // _MARBLE_PLUGIN_H

