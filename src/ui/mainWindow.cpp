#include "ui/mainWindow.moc"

#include <QtWidgets/QMessageBox>
#include <iostream>
using std::cout;
using std::endl;
using std::string;


namespace seamcarve {
namespace ui {

   void MainWindow::displayAboutMessageBox() {
      QMessageBox::about(this, "ABOUT",
                         tr("<h1>Image resizing using content-aware seamcarving</h1>"
                            "<p>This is a work in progress and only supports removing seams.</p>"
                            "<p>More info: http://www.faculty.idc.ac.il/arik/SCWeb/imret/index.html</p>"));
   }

   void MainWindow::imageOpened(QSize size) {
      cout << "Fit window to image size" << endl;

      int margin = 25;
      QSize new_size(size.width() + margin, size.height() + margin);
      resize(new_size);
   }

}
}
