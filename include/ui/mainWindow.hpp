#ifndef MAINWINDOW
#define MAINWINDOW

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>


namespace seamcarve {
namespace ui {

   class MainWindow : public QMainWindow {
      Q_OBJECT

      public slots:
         void displayAboutMessageBox();
   };

}
}

#endif
