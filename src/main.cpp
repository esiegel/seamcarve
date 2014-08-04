#include "seamcarve.hpp"
#include "seamcarveui.hpp" // Generated UI.
#include "ui/mainWindow.hpp"

#include <iostream>
#include <string>


QMainWindow* create_window() {
   auto window = new seamcarve::ui::MainWindow;
   auto ui = Ui::MainWindow();
   ui.setupUi(window);

   return window;
}

int main(int argc, char const* argv[]) {
   QApplication app(argc, (char**) argv);
   auto window = create_window();
   window->show();

   return app.exec();
}
