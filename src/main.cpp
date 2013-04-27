#include <iostream>
#include <string>
#include <FreeImagePlus.h>

#include <configure.hpp>
#include <seamcarve.hpp>


int main(int argc, char const* argv[]) {

   // map to hold cmdline parsing
   boost::optional<seamcarve::Config> optConfig = 
      seamcarve::create_config(argc, argv);

   if (!optConfig) {
      return 1;
   }

   seamcarve::Config config = optConfig.get();

   seamcarve::do_seam_carving(config);

   return 0;
}
