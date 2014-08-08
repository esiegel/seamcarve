#ifndef RESIZEABLE_LABEL_HPP 
#define RESIZEABLE_LABEL_HPP

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtWidgets/QLabel>


namespace seamcarve {
namespace ui {

   class ResizeableLabel: public QLabel {
      Q_OBJECT

   public:
      using QLabel::QLabel;

      void resizeEvent(QResizeEvent* event) override;

   signals:
      void signal_image_opened(QSize size);

   public slots:
      void energy_checkbox_clicked(bool checked);
      void open_image();
      void open_image_from_filename(QString filename);

   private:
      bool energy_pixmap_stale = true;
      bool show_energy = false;
      QPixmap imagePixmap;
      QPixmap energyPixmap;
   };

}
}

#endif
