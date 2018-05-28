// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "server.h"

#include <dlib/logger.h>

FX::Server::Server() : dlib::server_iostream() {}

void FX::Server::SetBind(const std::string &bind) { set_listening_ip(bind); }

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
  std::cout << "server: new connection [" << connection_id << "]" << std::endl;
  while (_resultStore != nullptr && out.good()) {
    _resultStore->Get().Serialize(out);
    _resultStore->Wait();
  }
  std::cout << "server: connection [" << connection_id << "] closed"
            << std::endl;
}