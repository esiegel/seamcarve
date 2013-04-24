#include <configure.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options.hpp>

namespace boptions = boost::program_options;

/*
 * Cmdline parse format.
 */
boptions::options_description command_line_format() {
   std::string msg = "Interactive Seam Carving for resizing images";
   boptions::options_description desc(msg);

   desc.add_options()
       ("help,h", "This Help message")
       ("image_path,i", boptions::value<std::string>(), "Image Path");

   return desc;
}

/*
 *Parses cmdline arguments variable map.
 */
boptions::variables_map parse_command_line(int argc,
                                           char const* argv[]) {

   // map to hold cmdline parsing
   boptions::variables_map vmap;

   boptions::options_description desc = command_line_format();
   boptions::store(boptions::parse_command_line(argc, argv, desc), vmap);
   boptions::notify(vmap);
   return vmap;
}

boost::optional<Config> create_config(int argc, char const* argv[]) {

   boptions::variables_map vmap = parse_command_line(argc, argv);

   if (vmap.count("help")) {
      std::cout << command_line_format() << std::endl;
      return boost::optional<Config>();
   }

   if (!vmap.count("image_path")) {
      std::cout << command_line_format() << std::endl;
      std::cout << "MISSING image_path" << std::endl;
      return boost::optional<Config>();
   }

   Config config;
   config.image_path = vmap["image_path"].as<std::string>();

   return boost::optional<Config>(config);
}
