#include "configure.hpp"
#include "seamcarve.hpp"
#include "seamcarveui.hpp" // Generated UI.
#include "ui/mainWindow.hpp"

#include <iostream>
#include <string>


seamcarve::ui::MainWindow* create_window() {
   auto window = new seamcarve::ui::MainWindow;
   auto ui = Ui::MainWindow();
   ui.setupUi(window);

   return window;
}

int main(int argc, char const* argv[]) {
   using namespace seamcarve;

   // UI setup
   QApplication app(argc, (char**) argv);
   auto window = create_window();
   window->show();

   // Get cmdline config, to potentially set image.
   boost::optional<Config> optConfig = create_config(argc, argv);
   if (!optConfig) {
      return 1;
   }
   Config config = optConfig.get();
   QString filename = QString::fromStdString(config.image_path);

   emit window->signalImageFromCmdline(filename);

   return app.exec();
}
