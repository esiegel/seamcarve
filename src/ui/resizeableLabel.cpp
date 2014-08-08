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
      QImage next_image = seamcarve::resize(imagePixmap.toImage(), size);
      imagePixmap = QPixmap::fromImage(next_image);

      if (show_energy) {
         QImage energy_image = calculate_energy_image(next_image);
         energyPixmap = QPixmap::fromImage(energy_image);
         energy_pixmap_stale = false;
      } else {
         energy_pixmap_stale = true;
      }

      setPixmap(show_energy ? energyPixmap : imagePixmap);

      // send event to standard event handler.
      QLabel::resizeEvent(event);
   }


   /**********************SLOTS***********************/

   void ResizeableLabel::energy_checkbox_clicked(bool checked) {
      cout << "ENERGY " << (checked ? "ON" : "OFF") << endl;

      show_energy = checked;

      if (!imagePixmap.isNull()) {

         // update energy pixmap only when necessary
         if (checked && energy_pixmap_stale) {
            QImage energy_image = calculate_energy_image(imagePixmap.toImage());
            energyPixmap        = QPixmap::fromImage(energy_image);
            energy_pixmap_stale = false;
         }

         setPixmap(checked ? energyPixmap : imagePixmap);
      }
   }

   void ResizeableLabel::open_image() {
      QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      QDir::homePath(),
                                                      tr("Images (*.png *.jpg)"));
      // set pixmap
      cout << "OPENING IMAGE " << filename.toStdString() << endl;
      open_image_from_filename(filename);
   }

   void ResizeableLabel::open_image_from_filename(QString filename) {
      imagePixmap = QPixmap(filename);

      // signal new image 
      setPixmap(imagePixmap);

      // signal new image 
      emit signal_image_opened(imagePixmap.size());
   }

   /**********************PRIVATE***********************/

}
}
