#include "seamcarve.hpp"
#include "ui/resizeableLabel.moc"

#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QFileDialog>

#include <iostream>
using std::cout;
using std::endl;
using std::string;


namespace seamcarve {
namespace ui {

   void ResizeableLabel::resizeEvent(QResizeEvent* event) {
      if (this->pixmap() == NULL) {
         return QLabel::resizeEvent(event);
      }

      QSize oldSize = event->oldSize();
      QSize size    = event->size();
      cout << "Old Size:"  << oldSize.width() << "x" << oldSize.height()
           << " New Size:" << size.width()    << "x" << size.height()
           << " PIXMAP:" << pixmap()->size().width() << "x" << pixmap()->size().height() << endl;


      //DELETE PIXELS
      if (size.height() < oldSize.height()) {
         QImage image = pixmap()->toImage();

         int num_pixels = size.height() * image.width();
         QRgb* data = new QRgb[num_pixels];

         // copy old data
         memcpy(data, image.bits(), num_pixels * sizeof(QRgb)); 

         // new image
         QImage nextImage = QImage((uchar*)data, image.width(), size.height(), image.format());

         setPixmap(QPixmap::fromImage(nextImage));
      }

      // send event to standard event handler.
      QLabel::resizeEvent(event);
   }

   void ResizeableLabel::openImage() {
      QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      QDir::homePath(),
                                                      tr("Images (*.png *.jpg)"));
      // set pixmap
      cout << "OPENING IMAGE " << filename.toStdString() << endl;
      openImageFromFilename(filename);
   }

   void ResizeableLabel::openImageFromFilename(QString filename) {
      const QPixmap pixmap(filename);
      setPixmap(pixmap);

      // signal new image 
      emit signalImageOpened(pixmap.size());
   }

}
}
