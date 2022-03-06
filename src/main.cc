// SPDX-FileCopyrightText: 2021 Sveriges Television AB
//
// SPDX-License-Identifier: GPL-2.0-or-later


#include <fstream>
#include <iostream>
#include <vector>

#include "spdlog/spdlog.h"

#include "Version.hh"
#include "VivictPP.hh"
#include "Controller.hh"
#include "SourceConfig.hh"
#include "vmaf/VmafLog.hh"
#include "gtk/Application.hh"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#ifndef VPP_VERSION
#define VPP_VERSION "unknown version"
#endif

const std::string FOOTER =
  R"(
KEYBOARD SHORTCUTS

SPACE  Play/Pause video
,      Step forward 1 frame
.      Step backward 1 frame
/ or - Seek forward 5 seconds
m      Seek backward 5 seconds
?      Seek Forward 60s
M      Seek backward 60s
Alt-?  Seek Forward 10min
Alt-M  Seek Backward 10min
<      Decrease left frame offset
>      Increase left frame offset

f      Toggle full screen
u      Zoom in
i      Zoom out
0      Reset pan and zoom to default
t      Toggle visibility of time
d      Toggle visibility of Stream and Frame metadata
p      Toggle visibility of vmaf plot (if vmaf data present)

q      Quit application

See also  https://github.com/svt/vivictpp#readme

Vivict++ )" + std::string(VPP_VERSION) + " " + std::string(VPP_GIT_HASH);

int main(int argc, char **argv) {
  try {
    vivictpp::logging::initializeLogging();
    /*
    auto gtkApp = Gtk::Application::create(argc, argv, "se.svt.vivictpp");

    CLI::App app{"Vivict++ - Vivict Video Comparison Tool ++"};
    app.footer(FOOTER);

    std::string leftVideo;
    app.add_option("leftVideo", leftVideo, "Path or url to first (left) video")->required();

    std::string rightVideo;
    app.add_option("rightVideo", rightVideo, "Path or url to second (right) video");

    std::string leftFilter("");
    std::string rightFilter("");
    app.add_option("--left-filter", leftFilter, "Video filters for left video");
    app.add_option("--right-filter", rightFilter, "Video filters for left video");

    bool disableAudio;
    app.add_flag("--disable-audio",  disableAudio, "Disable audio");

    std::string leftVmaf("");
    std::string rightVmaf("");
    app.add_option("--left-vmaf", leftVmaf, "Path to csv-file containing vmaf data for left video");
    app.add_option("--right-vmaf", rightVmaf, "Path to csv-file containing vmaf data for right video");

    CLI11_PARSE(app, argc, argv);

    std::vector<std::string> sources = {leftVideo};
    if (!rightVideo.empty()) {
      sources.push_back(rightVideo);
    }
    std::vector<std::string> filters = {leftFilter, rightFilter};
    std::vector<std::string> vmafLogfiles = {leftVmaf, rightVmaf};



    std::vector<SourceConfig> sourceConfigs;
    for (size_t i = 0; i<sources.size(); i++) {
        std::string filter = i < filters.size() ? filters[i] : "";
        std::string vmafLogFile = i < vmafLogfiles.size() ? vmafLogfiles[i] : "";
        sourceConfigs.push_back(SourceConfig(sources[i], filter, vmafLogFile));
    }

    for (auto sourceConfig : sourceConfigs) {
        spdlog::debug("Source: path={} filters={}", sourceConfig.path, sourceConfig.filter);
    }

    VivictPPConfig vivictPPConfig(sourceConfigs, disableAudio);

//    auto eventLoop = std::make_shared<vivictpp::sdl::SDLEventLoop>(vivictPPConfig.sourceConfigs);


    std::shared_ptr<vivictpp::gtk::GtkApplication> g(std::make_shared<vivictpp::gtk::GtkApplication>());

    //   vivictpp::Controller controller(eventLoop, eventLoop, vivictPPConfig);
    vivictpp::Controller controller(g, g, vivictPPConfig);


    return gtkApp->run(*g);
    */

    
    auto application = vivictpp::gtk::Application::create();
    return application->run(argc, argv);

//    return controller.run();
  } catch (const std::exception &e) {
    std::cerr << "Vivict had an unexpected error: " << e.what() << std::endl;
    return 1;
  }
}
