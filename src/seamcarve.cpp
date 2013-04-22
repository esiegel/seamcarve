#include <iostream>
#include <boost/format.hpp>

void do_seam_carving(std::string image_path) {
   std::cout << boost::format("SEAMCARVING %1%") % image_path << std::endl;
}
