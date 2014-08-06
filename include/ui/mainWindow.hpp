#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtWidgets/QMainWindow>


namespace seamcarve {
namespace ui {

   class MainWindow : public QMainWindow {
      Q_OBJECT

      signals:
         void signalImageFromCmdline(QString filename);

      public slots:
         void displayAboutMessageBox();
         void imageOpened(QSize size);
   };

}
}

#endif
