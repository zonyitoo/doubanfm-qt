#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include "plugins/doubanfmplugin.h"

#ifdef WITH_MPRIS_PLUGIN
#pragma message("Enabled MPRIS plugin")
#include "plugins/mpris/doubanmprisplugin.h"
#endif

inline void load_plugins() {
#ifdef WITH_MPRIS_PLUGIN
    REGISTER_PLUGIN("MPRIS_PLUGIN", DoubanMprisPlugin);
#endif
}

#endif // PLUGIN_HPP
