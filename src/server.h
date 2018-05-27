// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __FX_SERVER_H__
#define __FX_SERVER_H__

#include <dlib/server.h>

#include "result.h"

namespace FX {
class Server : public dlib::server_iostream {
public:
  Server();
  void Start();
  void StartAsync();
  void SetPort(int port);
  void SetResultStore(ResultStore *resultStore);
  void on_connect(std::istream &in, std::ostream &out,
                  const std::string &foreign_ip, const std::string &local_ip,
                  unsigned short foreign_port, unsigned short local_port,
                  dlib::uint64 connection_id) override;

private:
  ResultStore *_resultStore;
};
} // namespace FX

#endif