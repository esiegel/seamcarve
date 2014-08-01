#ifndef CENTRALWIDGET
#define CENTRALWIDGET

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

namespace seamcarve {
namespace ui {

   class CentralWidget: public QWidget {
      Q_OBJECT


      public slots:
         void displayAboutMessageBox();
   };

}
}

#endif
