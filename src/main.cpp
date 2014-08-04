#include "seamcarve.hpp"
#include "seamcarveui.hpp" // Generated UI.
#include "ui/mainWindow.hpp_moc"

#include <iostream>
#include <string>


QMainWindow* create_window() {
   auto window = new seamcarve::ui::MainWindow;
   Ui::MainWindow* ui = new Ui::MainWindow;
   ui->setupUi(window);

   return window;
}

int main(int argc, char const* argv[]) {
   using namespace seamcarve;

   QApplication app(argc, (char**) argv);
   QMainWindow* window = create_window();
   window->show();

   return app.exec();
}
