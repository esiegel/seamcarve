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


      // resize using seamcarving
      QImage next_image = seamcarve::resize(pixmap()->toImage(), size);
      imagePixmap = QPixmap::fromImage(next_image);
      setPixmap(imagePixmap);

      // send event to standard event handler.
      QLabel::resizeEvent(event);
   }


   /**********************SLOTS***********************/

   void ResizeableLabel::energyCheckboxClicked(bool checked) {
      cout << "ENERGY " << (checked ? "ON" : "OFF") << endl;
      if (!imagePixmap.isNull()) {
         setPixmap(checked ? energyPixmap : imagePixmap);
      }
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
      imagePixmap = QPixmap(filename);

      QImage energy_image = calculate_energy_image(imagePixmap.toImage());
      energyPixmap = QPixmap::fromImage(energy_image);

      // signal new image 
      setPixmap(imagePixmap);

      // signal new image 
      emit signalImageOpened(imagePixmap.size());
   }

   /**********************PRIVATE***********************/

}
}
