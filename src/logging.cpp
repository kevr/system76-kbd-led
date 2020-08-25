#include "logging.hpp"

bool logging::state::debug = true;

void logging::set_debug(bool enabled)
{
    logging::state::debug = enabled;
    if (logging::state::debug)
        logging::debug("Debug logging enabled.");
}
