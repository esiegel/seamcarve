#include "ui/mainWindow.moc"

#include <iostream>
using std::cout;
using std::endl;


namespace seamcarve {
namespace ui {

   void MainWindow::displayAboutMessageBox() {
      cout << "ABOUT BUTTON CLICKED" << endl;
   }

   void MainWindow::imageOpened(QSize size) {
      cout << "Fit window to image size" << endl;
      resize(size);
   }

}
}
