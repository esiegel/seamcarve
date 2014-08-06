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
           << " New Size:" << size.width()    << "x" << size.height() << endl;

      //DELETE PIXELS
      if (size.height() < oldSize.height()) {
         QImage image = pixmap()->toImage();

         for (int row = oldSize.height() - 1; row >= size.height(); row--) {
            QRgb *line = (QRgb *)image.scanLine(row);
            for (int col = 0; col <= oldSize.width(); col++) {
               image.setPixel(col, row, QColor::QColor(0,0,0).rgba());
               //QRgb* pixel = line + col; 
               //cout << "about to set pixel" << endl;
               //*pixel = QColor::QColor(0,0,0).rgba();
            }
         }
         setPixmap(QPixmap::fromImage(image));
      }


      //

      //QImage resized_image = do_seam_carving();
      //QPixmap scaled = pixmap()->scaled(size.width(),
                                        //size.height(),
                                        //Qt::IgnoreAspectRatio,
                                        //Qt::SmoothTransformation);
      //setPixmap(scaled);

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
