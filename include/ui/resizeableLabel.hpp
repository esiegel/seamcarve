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

   public slots:
      void windowResized();
   };

}
}

#endif
