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
         void signal_image_from_cmdline(QString filename);

      public slots:
         void display_about_message_box();
         void image_opened(QSize size);
   };

}
}

#endif
