#ifndef RESIZEABLELABEL 
#define RESIZEABLELABEL

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

namespace seamcarve {
namespace ui {

   class ResizeableLabel: public QLabel {
      Q_OBJECT

   public:
      using QLabel::QLabel;

      void resizeEvent(QResizeEvent* event) override;
      bool eventFilter(QObject *object, QEvent *event) override;

   public slots:
      void windowResized();
   };

}
}

#endif
