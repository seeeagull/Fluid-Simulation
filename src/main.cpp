#include "CGL/CGL.h"
#include "CGL/viewer.h"

#include "application.h"
typedef uint32_t gid_t;

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace CGL;

int main(int argc, char **argv) {
  AppConfig config;
  int opt;

  // create application
  Application *app = new Application(config);

  // create viewer
  Viewer viewer = Viewer();

  // set renderer
  viewer.set_renderer(app);

  // init viewer
  viewer.init();

  // start viewer
  viewer.start();

  return 0;
}
