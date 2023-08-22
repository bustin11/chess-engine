

#include "web.hpp"

using std::string, httplib::Request, httplib::Response;

namespace chess {

Web::Web(int port) : port_(port) { game_ = new Game(); }

Web::~Web() { delete game_; }

void Web::Run() {
  httplib::Server server;

  // {fen : fen_string}
  server.Post("/api/set-board-fen", [this](const Request &req, Response &res) {
    LOG_TRACE("Endpoint 'set-board-fen' was hit");
    CHESS_ASSERT(req.has_param("fen"), "Request at endpoint 'set-board-fen' has no parameter key 'fen'");

    string fen = req.get_param_value("fen");
    game_->load_custom_position_ = true;
    game_->custom_position_ = fen;
    res.set_content("ok", "text/plain");
  });

  // {move : "e2-e4", turn : "black/white"}
  server.Get("/api/move", [](const Request &req, Response &res) {
    LOG_TRACE("Endpoint 'moved' was hit");
    string move = req.get_param_value("move");
    // TODO(justin): Util: turn string to move
    // game->player_to_move->choseMove(move);
  });

  LOG_INFO("Server Listening http://127.0.0.1:%d", port_);
  server.listen("0.0.0.0", port_);
}

};  // namespace chess
