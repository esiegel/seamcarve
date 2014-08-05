#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtWidgets/QMainWindow>


namespace seamcarve {
namespace ui {

   class MainWindow : public QMainWindow {
      Q_OBJECT

      public slots:
         void displayAboutMessageBox();
         void imageOpened(QSize size);
   };

}
}

#endif
