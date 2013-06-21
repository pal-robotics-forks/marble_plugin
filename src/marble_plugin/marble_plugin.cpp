// this is for emacs file handling -*- mode: c++; indent-tabs-mode: nil -*-

/* -- BEGIN LICENSE BLOCK ----------------------------------------------

Copyright (c) 2013, TB
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by TB. The name of the
TB may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  -- END LICENSE BLOCK ----------------------------------------------*/

//----------------------------------------------------------------------
/*!\file
*
* \author  Tobias Baer <baer@fzi.de> 
*          Jan Aidel <aiden@fzi.de>
* \date    2013-01-11
*
*/
//----------------------------------------------------------------------


// Qt Includes
#include <QLineEdit>
#include <QFileInfo>
#include <QFileDialog>
#include <QStringList>
#include <QStandardItemModel>
#include <QModelIndex>

// Own Includes
#include "marble_plugin.h"

// ROS Plugin Includes
#include <pluginlib/class_list_macros.h>

// Marble Includes
#include <marble/MarbleWidget.h>
#include <marble/MarbleModel.h>
#include <marble/MapThemeManager.h>
#include <marble/GeoPainter.h>

#include <ros/package.h>
#include "drawable_marble_widget.h"
#include "manage_kml_dialog.h"

// @TODO: setDistance does not work on reloading
// @TODO: ComboBox for the MarbleWidget projection method
// @TOOD: Draw icon on the current gps pos (MarbleWidget needs to be subclassed (custom paint))

namespace marble_plugin {

MarblePlugin::MarblePlugin()
  : rqt_gui_cpp::Plugin()
  , widget_(0)
{
  // give QObjects reasonable names
  setObjectName("MarbleWidgetPlugin");
}



void MarblePlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
  // access standalone command line arguments
  QStringList argv = context.argv();

  // create QWidget
  widget_ = new QWidget();

  // add widget to the user interface
  ui_.setupUi( widget_ );

  ui_.MarbleWidget->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
  ui_.MarbleWidget->setProjection( Marble::Mercator );
  ui_.MarbleWidget->centerOn( 115.87164 , -31.93452 , false );  // My Happy Place: The Scotto
  ui_.MarbleWidget->setDistance(0.05);

  context.addWidget(widget_);
  ui_.comboBox_theme->setModel( ui_.MarbleWidget->model()->mapThemeManager()->mapThemeModel() );


  //set refresh icon
  QIcon refresh_icon;
  std::string path = ros::package::getPath("marble_plugin")+"/etc/refresh.png";
  QString icon_path(path.c_str());
  refresh_icon.addFile(icon_path);
  ui_.refreshButton->setIcon(refresh_icon);

  //setup the ros publisher for publishing the selected gps position
  m_selected_gps_pos_publisher = getNodeHandle().advertise< sensor_msgs::NavSatFix >("gps_position", 10);

  FindNavSatFixTopics();

  // Connections
  connect(ui_.comboBox_current_gps, SIGNAL(activated (const QString &)), this, SLOT (ChangeGPSTopicCurrentGPS(const QString &)));
  connect(ui_.comboBox_matched_gps, SIGNAL(activated (const QString &)), this, SLOT (ChangeGPSTopicMatchedGPS(const QString &)));


  connect(ui_.refreshButton, SIGNAL(clicked()), this, SLOT(FindNavSatFixTopics()));
  connect(ui_.manageKMLButton, SIGNAL(clicked()), this, SLOT(ManageKML()));

  connect( this , SIGNAL(NewGPSPosition(qreal,qreal)) , ui_.MarbleWidget , SLOT(centerOn(qreal,qreal)) );
  connect( ui_.comboBox_theme , SIGNAL(currentIndexChanged(int)) , this , SLOT(ChangeMarbleModelTheme(int)));

  connect( ui_.MarbleWidget, SIGNAL(mouseClickGeoPosition(qreal,qreal,GeoDataCoordinates::Unit)), this, SLOT(gpsCoordinateSelected(qreal,qreal,GeoDataCoordinates::Unit)));

  // AutoNavigation Connections ... soon
  /*
  m_autoNavigation = new Marble::AutoNavigation( ui_.MarbleWidget->model(), ui_.MarbleWidget->viewport(), this );

  connect( m_autoNavigation, SIGNAL( zoomIn( FlyToMode ) ),
                       ui_.MarbleWidget, SLOT( zoomIn() ) );
  connect( m_autoNavigation, SIGNAL( zoomOut( FlyToMode ) ),
                       ui_.MarbleWidget, SLOT( zoomOut() ) );
  connect( m_autoNavigation, SIGNAL( centerOn( const GeoDataCoordinates &, bool ) ),
                       ui_.MarbleWidget, SLOT( centerOn( const GeoDataCoordinates & ) ) );

  connect( ui_.MarbleWidget , SIGNAL( visibleLatLonAltBoxChanged() ),
                        m_autoNavigation, SLOT( inhibitAutoAdjustments() ) );
    */
}

void MarblePlugin::ManageKML()
{
    ManageKmlDialog kmlDialog(m_last_kml_data);
    if(kmlDialog.exec() == QDialog::Accepted)
    {
        std::map< QString, bool> kml_files = kmlDialog.getKmlFiles();
        addKMLData(kml_files, true);
    }
}

void MarblePlugin::addKMLData(std::map< QString, bool>& kml_files, bool overwrite)
{
    if(overwrite)
    {
        clearKMLData();
    }

    std::map<QString, bool>::iterator it;
    for(it=kml_files.begin(); it != kml_files.end(); it++)
    {
        QString filepath = it->first;
        bool show = it->second;

        if(show)
        {
            ui_.MarbleWidget->model()->addGeoDataFile( filepath );
        }
        m_last_kml_data[filepath] = show;
    }
}

void MarblePlugin::clearKMLData()
{
    for(std::map< QString, bool>::iterator it = m_last_kml_data.begin(); it != m_last_kml_data.end(); it++)
    {
        ui_.MarbleWidget->model()->removeGeoData(it->first);
    }
    m_last_kml_data.clear();
}



void MarblePlugin::FindNavSatFixTopics()
{
    using namespace ros::master;

    std::vector<TopicInfo> topic_infos;
    getTopics(topic_infos);

    //GPS Topics
    ui_.comboBox_current_gps->clear();
    ui_.comboBox_matched_gps->clear();
    for(std::vector<TopicInfo>::iterator it=topic_infos.begin(); it!=topic_infos.end();it++)
    {
        TopicInfo topic = (TopicInfo)(*it);
        if(topic.datatype.compare("sensor_msgs/NavSatFix")==0)
        {
            QString lineEdit_string(topic.name.c_str());
            ui_.comboBox_current_gps->addItem(lineEdit_string);
            ui_.comboBox_matched_gps->addItem(lineEdit_string);
        }
    }
}


void MarblePlugin::ChangeMarbleModelTheme(int idx )
{
    QStandardItemModel* model = ui_.MarbleWidget->model()->mapThemeManager()->mapThemeModel();
    QModelIndex index = model->index( idx , 0 );
    QString theme = model->data( index , Qt::UserRole+1  ).toString();

    ui_.MarbleWidget->setMapThemeId( theme );
}

void MarblePlugin::ChangeGPSTopicCurrentGPS(const QString &topic_name)
{
    m_current_pos_subscriber.shutdown();
    m_current_pos_subscriber = getNodeHandle().subscribe< sensor_msgs::NavSatFix >(
                topic_name.toStdString().c_str() , 1 , &MarblePlugin::GpsCallbackCurrent, this );

    int idx = ui_.comboBox_current_gps->findText( topic_name );
    if( idx != -1 )
    {
        ui_.comboBox_current_gps->setCurrentIndex( idx );
    }
}

void MarblePlugin::ChangeGPSTopicMatchedGPS(const QString &topic_name)
{
    m_matched_pos_subscriber.shutdown();
    m_matched_pos_subscriber = getNodeHandle().subscribe< sensor_msgs::NavSatFix >(
                topic_name.toStdString().c_str() , 1 , &MarblePlugin::GpsCallbackMatched, this );

    int idx = ui_.comboBox_matched_gps->findText( topic_name );
    if( idx != -1 )
    {
        ui_.comboBox_matched_gps->setCurrentIndex( idx );
    }

}

void MarblePlugin::GpsCallbackMatched( const sensor_msgs::NavSatFixConstPtr& gpspt )
{
    // std::cout << "GPS Callback Matched " << gpspt->longitude << " " << gpspt->latitude << std::endl;
    assert( widget_ );

    GeoDataCoordinates postition(gpspt->longitude, gpspt->latitude, gpspt->altitude, GeoDataCoordinates::Degree);
    ui_.MarbleWidget->setMatchedPosition(postition);

    // Emit NewGPSPosition only, if it changes significantly. Has to be somehow related to the zoom
    static qreal _x = -1;
    static qreal _y = -1;

    qreal x;
    qreal y;

    // Recenter if lat long is not on screen
    bool recenter = !ui_.MarbleWidget->screenCoordinates(gpspt->longitude,gpspt->latitude , x , y );
    recenter |= ui_.checkBox_center->isChecked();

    // Recenter if lat long within <threshold> pixels away from center
    qreal threshold = 20;
    recenter |=  ((x - _x) * (x - _x) + (y - _y) * (y - _y)) > threshold;

    if( recenter )
    {
        ui_.MarbleWidget->screenCoordinates(gpspt->longitude,gpspt->latitude , _x , _y );
        emit NewGPSPosition( gpspt->longitude , gpspt->latitude );
    }
}

void MarblePlugin::GpsCallbackCurrent( const sensor_msgs::NavSatFixConstPtr& gpspt )
{
    // std::cout << "GPS Callback Current " << gpspt->longitude << " " << gpspt->latitude << std::endl;
    assert( widget_ );

    GeoDataCoordinates postition(gpspt->longitude, gpspt->latitude, gpspt->altitude, GeoDataCoordinates::Degree);
    ui_.MarbleWidget->setCurrentPosition(postition);

    // @TODO: Marble Widget does not repaint
}

void MarblePlugin::gpsCoordinateSelected(qreal lon, qreal lat, GeoDataCoordinates::Unit unit) {
  GeoDataCoordinates coords(lon, lat, unit);

  sensor_msgs::NavSatFix msg;
  msg.longitude = coords.longitude(GeoDataCoordinates::Degree);
  msg.latitude = coords.latitude(GeoDataCoordinates::Degree);

  m_selected_gps_pos_publisher.publish(msg);
}

void MarblePlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const
{
  // save intrinsic configuration, usually using:
    QString topic(m_current_pos_subscriber.getTopic().c_str());
    instance_settings.setValue( "marble_plugin_topic_current", topic );

    topic = QString(m_matched_pos_subscriber.getTopic().c_str());
    instance_settings.setValue( "marble_plugin_topic_matched", topic );

    instance_settings.setValue( "marble_plugin_zoom" , ui_.MarbleWidget->distance() );
    instance_settings.setValue( "marble_theme_index" , ui_.comboBox_theme->currentIndex() );
    instance_settings.setValue( "marble_center" , ui_.checkBox_center->isChecked() );


    //save kml files
    int i=0;
    instance_settings.setValue("kml_number", m_last_kml_data.size());
    for(std::map< QString, bool>::const_iterator it = m_last_kml_data.begin(); it != m_last_kml_data.end(); it++)
    {
        QString key("kml_file_");
        key.append(i);
        instance_settings.setValue(key, it->first);

        //TODO: save visibility. Another way must be foud here
//        key.append("_show");
//        instance_settings.setValue(key, it->second);
        i++;
    }
}

void MarblePlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{
  // restore intrinsic configuration, usually using:
    const QString topic_current = instance_settings.value("marble_plugin_topic_current").toString();
    ChangeGPSTopicCurrentGPS(topic_current);

    const QString topic_matched = instance_settings.value("marble_plugin_topic_matched").toString();
    ChangeGPSTopicMatchedGPS(topic_matched);

    ui_.comboBox_theme->setCurrentIndex( instance_settings.value( "marble_theme_index" , 0 ).toInt() );
    ui_.checkBox_center->setChecked( instance_settings.value( "marble_center" , true ).toBool());


    //load kml files
    std::map< QString, bool> kml_files;
    int number = instance_settings.value("kml_number",0).toInt();
    for(int i=0; i<number; i++)
    {
        QString key("kml_file_");
        key.append(i);
        kml_files[instance_settings.value(key,0).toString()] = true;
    }
    addKMLData(kml_files, true);


  // std::cout << "Set distance " << instance_settings.value( "marble_plugin_zoom" ).toReal() << std::endl;

  // @TODO: Does not work since the KML loading changes the zoom
  ui_.MarbleWidget->setDistance( instance_settings.value( "marble_plugin_zoom" , 0.05 ).toReal() );
}

void MarblePlugin::shutdownPlugin()
{
  // unregister all publishers here
  m_current_pos_subscriber.shutdown();
  m_matched_pos_subscriber.shutdown();
}

/*bool hasConfiguration() const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used to open a dialog to offer the user a set of configuration
}*/

} // namespace
PLUGINLIB_DECLARE_CLASS(marble_plugin, MarblePlugin, marble_plugin::MarblePlugin, rqt_gui_cpp::Plugin)

