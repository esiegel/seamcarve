#include "seamcarve.hpp"
#include "ui/resizeableLabel.moc"

#include <QtCore/QSize>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QFileDialog>

#include <iostream>
using std::cout;
using std::endl;


namespace seamcarve {
namespace ui {

   void ResizeableLabel::resizeEvent(QResizeEvent* event) {
      if (this->pixmap() == NULL) {
         return QLabel::resizeEvent(event);
      }

      QSize oldSize = event->oldSize();
      QSize size    = event->size();
      cout << "Old Size:"  << oldSize.width() << "x" << oldSize.height()
           << " New Size:" << size.width()    << "x" << size.height() << endl;

      //QImage resized_image = do_seam_carving();
      QPixmap scaled = pixmap()->scaled(size.width(),
                                        size.height(),
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation);
      setPixmap(scaled);

      // send event to standard event handler.
      QLabel::resizeEvent(event);
   }

   void ResizeableLabel::openImage() {
      QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      QDir::homePath(),
                                                      tr("Images (*.png *.jpg)"));
      // set pixmap
      cout << "OPENING IMAGE " << fileName.toStdString() << endl;
      const QPixmap pixmap(fileName); 
      setPixmap(pixmap);

      // signal new image 
      emit signalImageOpened(pixmap.size());
   }

}
}
