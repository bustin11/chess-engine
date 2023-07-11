

#include "web.hpp"

using std::string, httplib::Request, httplib::Response;

namespace chess {

Web::Web(int port) : _port(port) { _game = StandardGame(); }

void Web::run() {
  httplib::Server server;

  // {fen : fen_string}
  server.Post("/api/set-board-fen", [this](const Request &req, Response &res) {
    LOG_TRACE("Endpoint 'set-board-fen' was hit");
    CHESS_ASSERT(
        req.has_param("fen"),
        "Request at endpoint 'set-board-fen' has no parameter key 'fen'");

    string fen = req.get_param_value("fen");
    _game.set_board_fen(fen);
    res.set_content("ok", "text/plain");
  });

  // {move : "e2-e4", turn : "black/white"}
  server.Get("/api/move", [this](const Request &req, Response &res) {
    LOG_TRACE("Endpoint 'moved' was hit");
    string move = req.get_param_value("move");
    _game.make_move(move);
  });

  LOG_INFO("Server Listening http://127.0.0.1:%d", _port);
  server.listen("0.0.0.0", _port);
}

}; // namespace chess
