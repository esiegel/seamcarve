#include <configure.hpp>
#include <boost/program_options.hpp>


namespace seamcarve  {

   namespace opts = boost::program_options;

   opts::options_description create_parse_format() {
      std::string msg = "Interactive Seam Carving for resizing images";
      opts::options_description desc(msg);

      desc.add_options()
          ("help,h", "This Help message")
          ("image_path,i", opts::value<std::string>(), "Image Path");

      return desc;
   }

   opts::variables_map parse_cmd_args(int argc, char const* argv[]) {

      // map to hold cmdline parsing
      opts::variables_map vmap;

      opts::options_description desc = create_parse_format();
      opts::store(opts::parse_command_line(argc, argv, desc), vmap);
      opts::notify(vmap);
      return vmap;
   }

   boost::optional<Config> create_config(int argc, char const* argv[]) {

      opts::variables_map vmap = parse_cmd_args(argc, argv);

      if (vmap.count("help")) {
         std::cout << create_parse_format() << std::endl;
         return boost::optional<Config>();
      }

      if (!vmap.count("image_path")) {
         std::cout << create_parse_format() << std::endl;
         std::cout << "MISSING image_path" << std::endl;
         return boost::optional<Config>();
      }

      Config config;
      config.image_path = vmap["image_path"].as<std::string>();

      return boost::optional<Config>(config);
   }
}
