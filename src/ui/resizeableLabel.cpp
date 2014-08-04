#include "ui/resizeableLabel.moc"

#include <QtGui/QResizeEvent>
#include <QtWidgets/QFileDialog>

#include <iostream>
using std::cout;
using std::endl;


namespace seamcarve {
namespace ui {

   void ResizeableLabel::resizeEvent(QResizeEvent* event) {
      QSize oldSize = event->oldSize();
      QSize size    = event->size();
      cout << "Old Size:"  << oldSize.width() << "x" << oldSize.height()
           << " New Size:" << size.width()    << "x" << size.height() << endl;

      // send event to standard event handler.
      QLabel::resizeEvent(event);
   }

   bool ResizeableLabel::eventFilter(QObject *object, QEvent *event) {
      cout << "GETTING EVENT" << endl;
      return true;
   }

   void ResizeableLabel::openImage() {
      QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      QDir::homePath(),
                                                      tr("Images (*.png *.xpm *.jpg)"));
      cout << "OPENING IMAGE " << fileName.toStdString() << endl;
      const QPixmap pixmap(fileName); 
      this->setPixmap(pixmap);
   }

}
}
