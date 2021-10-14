#ifndef CGL_APPLICATION_H
#define CGL_APPLICATION_H

// STL
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// libCGL
#include "CGL/CGL.h"
#include "CGL/osdtext.h"
#include "CGL/renderer.h"

#include "Vector.h"

using namespace std;

namespace CGL {

struct AppConfig {
  AppConfig() {
    // Environment variables
    steps_per_frame = 256;
  }
  float steps_per_frame = 256;
};

class Application : public Renderer {
public:
  Application(AppConfig config);
  ~Application();

  void init();
  void render();
  void resize(size_t w, size_t h);
  Vector2f bilerp2(Vector2f vf[], int u, int v);
  Vector3f bilerp3(Vector3f vf[], int u, int v);
  float sample1(float vf[], int u, int v);
  Vector2f sample2(Vector2f vf[], int u, int v);
  Vector3f sample3(Vector3f vf[], int u, int v);
  Vector2f backtrace(Vector2f vf[], Vector2f p);
  void advect();
  void vorticity(Vector2f vf[]);
  void divergence(Vector2f vf[]);
  void pressure_jacobi(float pf[], float new_pf[]);
  void subtract_gradient(Vector2f vf[], float pf[]);
  void enhance_vorticity(Vector2f vf[], float cf[]);


  std::string name();
  std::string info();

  // void keyboard_event(int key, int event, unsigned char mods);
  // void cursor_event(float x, float y);
  // void scroll_event(float offset_x, float offset_y);
  // void mouse_event(int key, int event, unsigned char mods);

private:
  AppConfig config;

  size_t screen_width;
  size_t screen_height;

  int f_strength, curl_strength, jacobi_iters;
  float dt, dye_decay;
  Vector2f gravity;
  float velocity_curls[1<<18], velocity_divs[1<<18], pressures[1<<18];
  Vector2f velocities[1<<18];
  Vector3f dyes[1<<18];

}; // class Application

} // namespace CGL

#endif // CGL_APPLICATION_H
