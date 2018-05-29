// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "capture.h"
#include "server.h"

#include <dlib/cmd_line_parser.h>

int main(int argc, char **argv) {
  dlib::command_line_parser cli;
  cli.add_option("help", "display this help message");
  cli.add_option("h", "short for 'help'");
  cli.set_group_name("Camera Options");
  cli.add_option("camera", "camera to open, default 0", 1);
  cli.add_option("fps", "set camera fps, may not work, default 30", 1);
  cli.add_option("width", "set camera width, may not work, default 800", 1);
  cli.add_option("height", "set camera height, may not work, default 600", 1);
  cli.set_group_name("Server Options");
  cli.add_option("bind", "host to bind, default '127.0.0.1'", 1);
  cli.add_option("port", "port to listen, default 6699", 1);

  cli.parse(argc, argv);

  const char *one_time_options[] = {"help", "h"};
  cli.check_one_time_options(one_time_options);

  if (cli.option("help") || cli.option("h")) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  facex --camera 1 --width 800 --height 600 --bind 127.0.0.1 "
                 "--port 6699"
              << std::endl
              << std::endl;
    cli.print_options();
    return EXIT_SUCCESS;
  }

  const int camera = dlib::get_option(cli, "camera", 0);
  const double fps = dlib::get_option(cli, "fps", 30);
  const double width = dlib::get_option(cli, "width", 800);
  const double height = dlib::get_option(cli, "height", 600);
  const std::string bind = dlib::get_option(cli, "bind", "127.0.0.1");
  const int port = dlib::get_option(cli, "port", 6699);

  FX::ResultStore resultStore;
  FX::Capture cap;
  FX::Server server;

  cap.SetResultStore(&resultStore);
  server.SetResultStore(&resultStore);

  server.SetBind(bind);
  server.SetPort(port);
  server.StartAsync();

  std::cout << "server: started " << bind << ":" << port << std::endl;

  cap.Open(camera);
  if (!cap.IsOpened()) {
    std::cout << "camera: failed to open [" << camera << "]" << std::endl;
    return EXIT_FAILURE;
  }

  cap.SetSize(width, height);
  cap.SetFPS(fps);

  std::cout << "camera: opened [" << camera << "]" << std::endl;

  cap.Start();
  return EXIT_SUCCESS;
}
