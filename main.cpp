#include <iostream>

#include "options.hpp"
#include "web.hpp"
#include "window.hpp"

void Usage(const char *target_name) { std::cout << "Usage: \n"; }

struct WebOpts {
  bool local_ = false;
  int port_ = 8080;
};

auto main(int argc, const char *argv[]) -> int {
  if (argc <= 2) {
    if (argc == 1) {
      Usage(argv[0]);
      return 0;
    }
    if (strcmp(argv[1], "--help") == 0) {
      Usage(argv[0]);
      return 0;
    }
  }

  auto parser = CmdOpts<WebOpts>::Create(
      {{"--port", &WebOpts::port_}, {"--local", &WebOpts::local_}});

  auto myopts = parser->parse(argc, argv);

  if (myopts.local_) {
    gui::WindowRun(argc, const_cast<char **>(argv));
    return 0;
  }


  // // TODO(justin): change this later
  // chess::Web web(myopts.port_);
  // web.Run();
  return 0;
}