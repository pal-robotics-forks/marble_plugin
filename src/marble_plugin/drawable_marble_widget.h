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


  public slots:

    
  protected:
    virtual void customPaint(GeoPainter *painter);

  private slots:

    
  private:

    
  private:
//    Q_DISABLE_COPY(DrawableMarbleWidget);
};

}

#endif //DRAWABLE_MARBLEWIDGET_H
