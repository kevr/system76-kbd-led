/**
 * @brief Main entry point for `system76-kbd-led`, the C++ version.
 *
 * @author Kevin Morris
 * @license MIT
 **/
#include "brightness.hpp"
#include "color.hpp"
#include "fs.hpp"
#include "keyboard.hpp"
#include "logging.hpp"
#include <boost/program_options.hpp>
#include <iostream>
using color::center;
using color::left;
using color::right;

#define USAGE_LINE " --command arg [-hv] [--help] [--verbose]"

// Alias boost::program_options to boost::po.
namespace boost
{
namespace po = program_options;
};

int print_help(const std::string &usage,
               const boost::po::options_description &desc, int rc = 0);

int main(int argc, char *argv[])
{
    // Produce program options description.
    boost::po::options_description desc("Program options");
    desc.add_options()("help,h", "Display the help message.")(
        "verbose,v", "Enable debug logging.")("toggle,t", "Toggle keyboard.")(
        "restore,x", "Restore colors and brightness.")(
        "left,l", boost::po::value<std::string>(), "Left color (rgb).")(
        "center,c", boost::po::value<std::string>(), "Center color (rgb).")(
        "right,r", boost::po::value<std::string>(), "Right color (rgb).")(
        "extra,e", boost::po::value<std::string>(),
        "Extra color (rgb).")("brightness,b", boost::po::value<int>(),
                              "Brightness increment (-/+).");

    // Create a variables_map and parse the command line arguments into it.
    boost::po::variables_map vm;

    // Prepare usage line.
    std::string usage(argv[0]);
    usage.append(USAGE_LINE);

    boost::po::command_line_parser parser(argc, argv);
    parser.options(desc).allow_unregistered();

    try {
        boost::po::store(parser.run(), vm);
    } catch (boost::po::unknown_option &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return print_help(usage, desc, 1);
    }
    boost::po::notify(vm);

    if (vm.count("help"))
        return print_help(usage, desc);

    logging::set_debug(vm.count("verbose"));

    // Initialize cache objects.
    fs::brightness_cache<uint32_t> b_cache;
    fs::hw_brightness_cache<uint32_t> hw_cache;
    fs::color_cache<std::string> c_cache;

    color::keyboard kb;
    led::brightness<uint32_t> brightness;

    if (vm.count("restore")) {

        if (!c_cache.exists()) {
            std::cerr << "error: cannot restore without a color cache."
                      << std::endl;
            return 1;
        }

        auto colors = c_cache.data();

        kb.left_region().set_color(colors[0], true);
        kb.center_region().set_color(colors[1], true);
        kb.right_region().set_color(colors[2], true);

        if (!b_cache.exists()) {
            std::cerr << "error: cannot restore without a brightness cache."
                      << std::endl;
            return 2;
        }

        brightness.set_value(b_cache.data().value());
        logging::debug("Restored brightness:", brightness.level(), '.');
    }

    if (vm.count("left")) {
        kb.left_region().set_color(color::rgb(vm.at("left").as<std::string>()),
                                   true);
    }

    if (vm.count("center")) {
        kb.center_region().set_color(
            color::rgb(vm.at("center").as<std::string>()), true);
    }

    if (vm.count("right")) {
        kb.right_region().set_color(
            color::rgb(vm.at("right").as<std::string>()), true);
    }

    std::array<color::rgb, 4> colors = {
        kb.left_region().color(), kb.center_region().color(),
        kb.right_region().color(), kb.extra_region().color()};

    for (int i = 0; i < colors.size(); ++i) {
        std::cout << std::to_string(colors[i]) << std::endl;
    }

    if (!hw_cache.exists()) {
        hw_cache.set_data(brightness.hw_level());
        b_cache.set_data(brightness.level());
    } else {
        if (brightness.hw_level() &&
            brightness.hw_level() != hw_cache.data().value()) {
            hw_cache.set_data(brightness.hw_level());
        }
    }

    // If brightness level is currently > 0, update it.
    if (brightness.level() > 0)
        b_cache.set_data(brightness.level());

    // If -b was given, apply the increment to brightness.
    if (vm.count("brightness")) {
        brightness.increment(vm.at("brightness").as<int>());
        b_cache.set_data(brightness.level());
    }

    if (vm.count("toggle"))
        brightness.set_value(brightness.level() ? 0 : b_cache.data().value());

    // Store color cache if it doesn't yet exist, otherwise update
    // it if it's different than what we have.
    if (!c_cache.exists()) {
        c_cache.set_data(kb.left_region().color(), kb.center_region().color(),
                         kb.right_region().color());
    } else {
        auto cached_colors = c_cache.data();
        if (cached_colors[0] != kb.left_region().color() ||
            cached_colors[1] != kb.center_region().color() ||
            cached_colors[2] != kb.right_region().color()) {
            c_cache.set_data(kb.left_region().color(),
                             kb.center_region().color(),
                             kb.right_region().color());
        }
    }

    logging::debug("Color cache:", std::to_string(c_cache.data()[0]),
                   std::to_string(c_cache.data()[1]),
                   std::to_string(c_cache.data()[2]));

    logging::debug("Brightness: { level:", brightness.level(),
                   ", max_level:", brightness.max_level(),
                   ", hw_level:", brightness.hw_level(), " }");
    return 0;
}

int print_help(const std::string &usage,
               const boost::po::options_description &desc, int rc)
{
    auto space_iter = usage.find_first_of(" ");
    auto slash_iter = usage.find_last_of("/", space_iter);

    std::cout << "usage: " << usage << "\n\n" << desc << std::endl;
    return rc;
}

