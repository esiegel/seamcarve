#include "ui/mainWindow.moc"

#include <iostream>
using std::cout;
using std::endl;
using std::string;


namespace seamcarve {
namespace ui {

   void MainWindow::displayAboutMessageBox() {
      cout << "ABOUT BUTTON CLICKED" << endl;
   }

   void MainWindow::imageOpened(QSize size) {
      cout << "Fit window to image size" << endl;

      int margin = 25;
      QSize new_size(size.width() + margin, size.height() + margin);
      resize(new_size);
   }

}
}
