#ifndef RESIZEABLE_LABEL_HPP 
#define RESIZEABLE_LABEL_HPP

#include <QtCore/QObject>
#include <QtCore/QSize>
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
      void openImage();
   };

}
}

#endif
