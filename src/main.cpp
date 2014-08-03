#include "configure.hpp"
#include "seamcarve.hpp"
#include "ui/mainWindow.hpp_moc"
#include "seamcarveui.hpp" // Generated UI.

#include <iostream>
#include <string>
#include <FreeImagePlus.h>


QMainWindow* create_window() {
   auto window = new seamcarve::ui::MainWindow;
   Ui::MainWindow* ui = new Ui::MainWindow;
   ui->setupUi(window);

   return window;
}

int main(int argc, char const* argv[]) {
   using namespace seamcarve;

   // map to hold cmdline parsing
   boost::optional<Config> optConfig = create_config(argc, argv);

   if (!optConfig) {
      return 1;
   }

   Config config = optConfig.get();

   QApplication app(argc, (char**) argv);
   QMainWindow* window = create_window();
   window->show();

   do_seam_carving(config);

   return app.exec();
}
