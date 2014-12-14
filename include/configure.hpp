#ifndef CONFIGURE_HPP
#define CONFIGURE_HPP

#include <boost/optional.hpp>
#include <iostream>

namespace seamcarve {

   /**
    * Configuration Container
    */
   typedef struct {
      std::string image_path;
   } Config;
      
   /**
    * Parse args and create Config object.
    */
   boost::optional<Config> create_config(int argc, char const* argv[]);

}

#endif
