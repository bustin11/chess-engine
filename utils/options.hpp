/**
 * @file options.hpp
 * Thanks to
 * https://www.codeproject.com/Tips/5261900/Cplusplus-Lightweight-Parsing-Command-Line-Argumen
 *
 * Light weight parser since argparse is pretty big, don't need that many
 * arguments
 *
 * More Info on Understanding the contents
 * https://stackoverflow.com/questions/6568486/when-do-we-need-a-private-constructor-in-c#:~:text=A%20private%20constructor%20in%20C%2B%2B,%2F%2F%20Restricting%20object%20creation%20%7D%3B
 * https://stackoverflow.com/questions/3106110/what-is-move-semantics
 * https://stackoverflow.com/questions/13230480/what-does-auto-tell-us
 *
 */

#pragma once

#include <functional> // function
#include <map>
#include <memory>  // unique_pointer
#include <sstream> // streamstream
#include <string>
#include <string_view>
#include <variant>
#include <vector>

template <class Opts> struct CmdOpts : Opts {

  // restricting the number of command argument types
  using MyProp = std::variant<std::string Opts::*, int Opts::*, double Opts::*,
                              bool Opts::*>;
  using MyArg = std::pair<std::string, MyProp>;

  ~CmdOpts() = default;

  Opts parse(int argc, const char *argv[]) {
    std::vector<std::string_view> vargv(argv, argv + argc);
    for (int idx = 0; idx < argc; ++idx) {
      for (auto &cbk : callbacks) {
        cbk.second(idx, vargv);
      }
    }
    // https://stackoverflow.com/questions/146452/what-are-pod-types-in-c
    // yes, we are slicing ... Opts should be POD
    // -> we could also enforce it via type traits
    return static_cast<Opts>(*this);
  }

  static std::unique_ptr<CmdOpts> Create(std::initializer_list<MyArg> args) {
    auto cmdOpts = std::unique_ptr<CmdOpts>(new CmdOpts());
    for (auto arg : args)
      cmdOpts->register_callback(arg);
    return cmdOpts;
  }

private:
  using callback_t =
      std::function<void(int, const std::vector<std::string_view> &)>;
  std::map<std::string, callback_t> callbacks;

  CmdOpts() = default;
  CmdOpts(const CmdOpts &) = delete;
  CmdOpts(CmdOpts &&) = delete;
  CmdOpts &operator=(const CmdOpts &) = delete;
  CmdOpts &operator=(CmdOpts &&) = delete;

  auto register_callback(std::string name, MyProp prop) {
    callbacks[name] =
        [this, name, prop](int idx, const std::vector<std::string_view> &argv) {
          if (argv[idx] == name) {
            // https://levelup.gitconnected.com/understanding-std-visit-in-c-a-type-safe-way-to-traverse-variant-objects-dbeff9b47003
            std::visit(
                [this, idx, &argv](auto &&arg) {
                  if (idx < argv.size() - 1) {
                    std::stringstream value;
                    value << argv[idx + 1];
                    value >> this->*arg;
                  }
                },
                prop);
          }
        };
  };

  auto register_callback(MyArg p) {
    return register_callback(p.first, p.second);
  }
};
