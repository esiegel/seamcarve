#include <seamcarve.hpp>
#include <iostream>
#include <boost/format.hpp>

void seamcarve::do_seam_carving(Config config) {
   std::cout << boost::format("SEAMCARVING %1%") % config.image_path << std::endl;
}
