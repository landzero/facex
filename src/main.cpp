// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "server.h"

int main(int argc, char **argv) {
  FX::Result res;
  res.size.width = 100;
  FX::ResultStore rstore;
  rstore.Set(res);
  FX::Server server;
  server.SetResultStore(&rstore);
  server.SetPort(6699);
  server.StartAsync();
  int i = 0;
  for (;;) {
    res.size.width = 100 + i;
    rstore.Set(res);
    i++;
    sleep(1);
  }
  return 0;
}
