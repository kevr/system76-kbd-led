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
#include <sys/stat.h>
using color::center;
using color::left;
using color::right;

// Alias boost::program_options to boost::po.
namespace boost
{
namespace po = program_options;
};

// Local aliases, structs, and function declarations.
#define USAGE_LINE                                                            \
    " [-h,--help] [-v,--verbose] [-t,--toggle] [-x,--restore] [-l,--left "    \
    "<arg>] [-c,--center <arg>] [-r,--right <arg>] [-e,--extra <arg>] "       \
    "[-b,--brightness <arg>] [-i,--increment <arg>]"

struct app_cache {
    fs::brightness_cache<uint32_t> brightness;
    fs::hw_brightness_cache<uint32_t> hw_brightness;
    fs::color_cache<std::string> color;
};

int print_help(const std::string &usage,
               const boost::po::options_description &desc, int rc = 0);
int print_error(const std::string &error, int rc = 1);

// Main entry point.
int main(int argc, char *argv[])
{
    // Produce program options description.
    boost::po::options_description desc("Program options");
    auto add_option = desc.add_options();

    using boost::po::value;
    add_option("help,h", "Display the help message.");
    add_option("verbose,v", "Enable debug logging.");
    add_option("toggle,t", "Toggle keyboard.");
    add_option("restore,x", "Restore colors and brightness.");
    add_option("left,l", value<std::string>(), "Left color (rgb).");
    add_option("center,c", value<std::string>(), "Center color (rgb).");
    add_option("right,r", value<std::string>(), "Right color (rgb).");
    add_option("extra,e", value<std::string>(), "Extra color (rgb).");
    add_option("brightness,b", value<int>(), "Brightness overriding value.");
    add_option("increment,i", value<int>(), "Brightness increment (-/+).");

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
        print_error(e.what());
        return print_help(usage, desc, 1);
    }
    boost::po::notify(vm);

    if (vm.count("help"))
        return print_help(usage, desc);

    logging::set_debug(vm.count("verbose"));

    if (!fs::exists("/var/cache/system76-kbd-led")) {
        int rc = mkdir("/var/cache/system76-kbd-led", 0777);
        if (rc == -1 && errno != EEXIST) {
            return print_error(
                "mkdir() failed on: /var/cache/system76-kbd-led");
        }
    }

    app_cache cache;

    color::keyboard kb;
    led::brightness<uint32_t> brightness;

    if (vm.count("restore")) {
        if (!cache.color.exists())
            return print_error("cannot restore without a color cache.", 1);

        auto colors = cache.color.data().value();
        kb.left_region().set_color(colors[0]);
        kb.center_region().set_color(colors[1]);
        kb.right_region().set_color(colors[2]);
        kb.extra_region().set_color(colors[3]);

        if (!cache.brightness.exists())
            return print_error("cannot restore without a brightness cache.",
                               2);

        brightness.set_value(cache.brightness.data().value());
        logging::debug("Restored brightness:", brightness.level(), '.');
    }

    if (vm.count("left")) {
        kb.left_region().set_color(
            color::rgb(vm.at("left").as<std::string>()));
    }

    if (vm.count("center")) {
        kb.center_region().set_color(
            color::rgb(vm.at("center").as<std::string>()));
    }

    if (vm.count("right")) {
        kb.right_region().set_color(
            color::rgb(vm.at("right").as<std::string>()));
    }

    if (vm.count("extra")) {
        kb.extra_region().set_color(
            color::rgb(vm.at("extra").as<std::string>()));
    }

    auto colors = kb.regions();
    for (auto &color : colors)
        std::cout << std::to_string(color) << std::endl;

    if (!cache.hw_brightness.exists()) {
        cache.hw_brightness.set_data(brightness.hw_level());
        cache.brightness.set_data(brightness.level());
    } else {
        if (brightness.hw_level() &&
            brightness.hw_level() != cache.hw_brightness.data().value()) {
            cache.hw_brightness.set_data(brightness.hw_level());
        }
    }

    if (vm.count("brightness")) {
        brightness.set_value(vm.at("brightness").as<int>());
        if (brightness.level())
            cache.brightness.set_data(brightness.level());
    }

    // If -b was given, apply the increment to brightness.
    if (vm.count("increment")) {
        brightness.increment(vm.at("increment").as<int>());
        if (brightness.level())
            cache.brightness.set_data(brightness.level());
    }

    // If brightness level is > 0 and it mismatches the cache, update it.
    if (brightness.level() > 0 &&
        brightness.level() != cache.brightness.data().value())
        cache.brightness.set_data(brightness.level());

    if (vm.count("toggle"))
        brightness.set_value(
            brightness.level() ? 0 : cache.brightness.data().value());

    // Store color cache if it doesn't yet exist, otherwise update
    // it if it's different than what we have.
    try {
        if (!cache.color.exists() ||
            cache.color.data().value() != kb.regions())
            cache.color.set_data(kb.regions());
    } catch (std::out_of_range &e) {
        // If we could not properly parse out our current color cache,
        // override it with a valid value of our current keyboard.
        cache.color.set_data(kb.regions());
    }
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

int print_error(const std::string &error, int rc)
{
    std::cerr << "error: " << error << std::endl;
    return rc;
}
