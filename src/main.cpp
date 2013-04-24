#include <iostream>
#include <string>
#include <FreeImagePlus.h>

#include <configure.hpp>
#include <seamcarve.hpp>


int main(int argc, char const* argv[]) {

   // map to hold cmdline parsing
   boost::optional<Config> config = create_config(argc, argv);
   if (!config) {
      return 1;
   }

   do_seam_carving(config.get());

   return 0;
}
