#include <iostream>

#include "options.hpp"
#include "web.hpp"
#include "window.hpp"

using namespace chess;

void usage(const char *target_name) { std::cout << "Usage: \n"; }

struct WebOpts {
  bool local = false;
  int port = 8080;
};

int main(int argc, const char *argv[]) {
  if (argc <= 2) {
    if (argc == 1) {
      usage(argv[0]);
      return 0;
    }
    if (strcmp(argv[1], "--help") == 0) {
      usage(argv[0]);
      return 0;
    }
  }

  auto parser = CmdOpts<WebOpts>::Create(
      {{"--port", &WebOpts::port}, {"--local", &WebOpts::local}});

  auto myopts = parser->parse(argc, argv);

  // precomputeMoveData();

  if (myopts.local) {
    gui::window_run(argc, const_cast<char **>(argv));
    return 0;
  }

  Web web(myopts.port);
  web.run();
  return 0;
}