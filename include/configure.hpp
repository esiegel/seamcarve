#ifndef CONFIGURE_HPP
#define CONFIGURE_HPP

#include <iostream>
#include <boost/optional.hpp>

namespace seamcarve {

   /**
    * Configuration Container
    */
   typedef struct _Config {
      std::string image_path;
   } Config;
      
   /**
    * Parse args and create Config object.
    */
   boost::optional<Config> create_config(int argc, char const* argv[]);

}

#endif
