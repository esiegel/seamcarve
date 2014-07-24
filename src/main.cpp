#include <iostream>
#include <string>
#include <FreeImagePlus.h>

#include <configure.hpp>
#include <seamcarve.hpp>

// Generated UI.  See resources dir.
#include <seamcarveui.hpp>


int main(int argc, char const* argv[]) {
   using namespace seamcarve;

   // map to hold cmdline parsing
   boost::optional<Config> optConfig = create_config(argc, argv);

   if (!optConfig) {
      return 1;
   }

   Config config = optConfig.get();

   do_seam_carving(config);

   QApplication app(argc, (char**) argv);
   QDialog* dialog= new QDialog;
   Ui::SeamcarveUI ui;
   ui.setupUi(dialog);

   dialog->show();
   return app.exec();
}
