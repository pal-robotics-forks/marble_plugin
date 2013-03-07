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
* \author  Tobias Bär <baer@fzi.de> Jan Aidel <aiden@fzi.de>
* \date    2013-01-11
*
*/
//----------------------------------------------------------------------


#ifndef DRAWABLE_MARBLEWIDGET_H
#define DRAWABLE_MARBLEWIDGET_H

#include "marble/MarbleWidget.h"
#include "marble/GeoPainter.h"


using namespace Marble;

namespace marble_plugin {

class DrawableMarbleWidget : public MarbleWidget
{
  Q_OBJECT
  
  public:

    DrawableMarbleWidget(QWidget *parent=0);
    void receiveLastPosition(GeoDataCoordinates& postion);

    

protected:
    virtual void customPaint(GeoPainter *painter);

  private:

    QImage roateCar(QImage* car_image);

    void loadImage(QImage& car, std::string& path );
    double dist(double x1, double x2, double y1, double y2);
    bool posChanged(double x1, double y1, double x2, double y2, double threshold);

    bool showAsPoint(double x1, double y1, double x2, double y2);
    bool showAsArrow(double x1, double y1, double x2, double y2);

    GeoDataCoordinates m_actual_position;
    GeoDataCoordinates m_last_position;
    double m_last_angle_grad;
    double m_actual_angle_grad;
    QImage* m_car;
    QImage m_arrow;
    QImage m_point;

    
  private:
    Q_DISABLE_COPY(DrawableMarbleWidget);
};

}

#endif //DRAWABLE_MARBLEWIDGET_H
