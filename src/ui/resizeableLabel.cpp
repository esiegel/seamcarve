#include "ui/resizeableLabel.hpp"

#include <QtGui/QResizeEvent>
#include <QtWidgets/QFileDialog>
#include <iostream>


typedef seamcarve::ui::ResizeableLabel RL;

void RL::resizeEvent(QResizeEvent* event) {
   QSize oldSize = event->oldSize();
   QSize size    = event->size();
   std::cout << "Old Size:"  << oldSize.width() << "x" << oldSize.height()
             << " New Size:" << size.width()    << "x" << size.height() << std::endl;

   // send event to standard event handler.
   QLabel::resizeEvent(event);
}

bool RL::eventFilter(QObject *object, QEvent *event) {
   std::cout << "GETTING EVENT" << std::endl;
   return true;
}

void RL::openImage() {
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                   QDir::homePath(),
                                                   tr("Images (*.png *.xpm *.jpg)"));
   std::cout << "OPENING IMAGE " << fileName.toStdString() << std::endl;
   const QPixmap pixmap(fileName); 
   this->setPixmap(pixmap);
}
