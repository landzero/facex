// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "server.h"

#include <dlib/logger.h>

FX::Server::Server() : dlib::server_iostream() {
  _resultStore = NULL;
  set_listening_ip("127.0.0.1");
}

void FX::Server::SetPort(int port) { set_listening_port(port); }

void FX::Server::Start() { start(); }
void FX::Server::StartAsync() { start_async(); }

void FX::Server::SetResultStore(FX::ResultStore *resultStore) {
  _resultStore = resultStore;
}

void FX::Server::on_connect(std::istream &in, std::ostream &out,
                            const std::string &foreign_ip,
                            const std::string &local_ip,
                            unsigned short foreign_port,
                            unsigned short local_port,
                            dlib::uint64 connection_id) {
  while (_resultStore != NULL && out.good()) {
    _resultStore->Get().Serialize(out);
    _resultStore->Wait();
  }
}