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
      void signalImageOpened(QSize size);

   public slots:
      void energyCheckboxClicked(bool checked);
      void openImage();
      void openImageFromFilename(QString filename);

   private:
      QPixmap imagePixmap;
      QPixmap energyPixmap;
   };

}
}

#endif
