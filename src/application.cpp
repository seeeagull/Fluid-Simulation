// #include <chrono>
#include <iostream>
#include <vector>
// #include <opencv2/opencv.hpp>

#include "application.h"
#include "Vector.h"

namespace CGL {

Application::Application(AppConfig config) { this->config = config; }

Application::~Application() {}

void Application::init()
{

  //gravity = Vector2f(0, -9.8);
  dt = 1.0f / 256;
  dye_decay = 1 - 1 / 120;
  f_strength = 2000.0;
  curl_strength = 5;
  jacobi_iters = 400;

  screen_width = 128;
  screen_height = 256;

  glEnable(GL_POINT_SMOOTH);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width ; ++ j)
    {
      pressures[i * screen_width + j] = 0,
      dyes[i * screen_width + j] = Vector3f(0),
      velocities[i * screen_width + j] = Vector2f(0);
    }
  }
  for(int i = screen_height / 2, p = 0; p < 32; ++ p, -- i)
  {
    for(int j = screen_width / 2 - 16, k = 0; k < 32; ++ k, ++ j)
    {
      dyes[i * screen_width + j] = Vector3f(1, 1, 1);
      velocities[i * screen_width + j] = Vector2f(f_strength) * p * 0.03125f;
    }
  }
}


void Application::resize(size_t w, size_t h) {
  // screen_width = w;
  // screen_height = h;
  screen_width = 128;
  screen_height = 256;
}

Vector2f Application::bilerp2(Vector2f vf[], int u, int v)
{
  float s = u - 0.5, t = v - 0.5;
  int iu = (int)s, iv = (int)t;
  float fu = s - iu, fv = t - iv;
  Vector2f a, b, c, d;
  a = sample2(vf, iu, iv);
  b = sample2(vf, iu + 1, iv);
  c = sample2(vf, iu, iv + 1);
  d = sample2(vf, iu + 1, iv + 1);
  return lerp2(lerp2(a, b, fu),lerp2(c, d, fu), fv);
}

Vector3f Application::bilerp3(Vector3f vf[], int u, int v)
{
  float s = u - 0.5, t = v - 0.5;
  int iu = (int)s, iv = (int)t;
  float fu = s - iu, fv = t - iv;
  Vector3f a, b, c, d;
  a = sample3(vf, iu, iv);
  b = sample3(vf, iu + 1, iv);
  c = sample3(vf, iu, iv + 1);
  d = sample3(vf, iu + 1, iv + 1);
  return lerp3(lerp3(a, b, fu),lerp3(c, d, fu), fv);
}

float Application::sample1(float vf[], int u, int v)
{
    if(u < 0) u = 0;
    else if(u >= screen_height) u = screen_height - 1;
    if(v < 0) v = 0;
    else if(v >= screen_width) v = screen_width - 1;
    
    return vf[u * screen_width + v];
}

Vector2f Application::sample2(Vector2f vf[], int u, int v)
{
    if(u < 0) u = 0;
    else if(u >= screen_height) u = screen_height - 1;
    if(v < 0) v = 0;
    else if(v >= screen_width) v = screen_width - 1;
    
    return vf[u * screen_width + v];
}

Vector3f Application::sample3(Vector3f vf[], int u, int v)
{
    if(u < 0) u = 0;
    else if(u >= screen_height) u = screen_height - 1;
    if(v < 0) v = 0;
    else if(v >= screen_width) v = screen_width - 1;
    
    return vf[u * screen_width + v];
}

//RK2
Vector2f Application::backtrace(Vector2f vf[], Vector2f p)
{
  Vector2f vp = bilerp2(vf, p.x, p.y);
  Vector2f p_mid = p - vp * 0.5f * dt;
  Vector2f vp_mid = bilerp2(vf, p_mid.x, p_mid.y);
  p -= vp_mid * dt;
  return p;
}

void Application::advect()
{
  Vector2f tmpv[screen_height][screen_width];
  Vector3f tmpd[screen_height][screen_width];

  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      Vector2f p = Vector2f(i + 0.5, j + 0.5);
      p = backtrace(velocities, p);
      tmpv[i][j] = bilerp2(velocities, p.x, p.y) * dye_decay;
    }
  }
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      Vector2f p = Vector2f(i + 0.5, j + 0.5);
      p = backtrace(velocities, p);
      tmpd[i][j] = bilerp3(dyes, p.x, p.y) * dye_decay;
    }
  }
  
  for(int i = 0; i < screen_height; ++ i)
    for(int j = 0; j < screen_width; ++ j)
      velocities[i * screen_width + j] = tmpv[i][j];
  for(int i = 0; i < screen_height; ++ i)
    for(int j = 0; j < screen_width; ++ j)
      dyes[i * screen_width + j] = tmpd[i][j];
}

void Application::vorticity(Vector2f vf[])
{
  Vector2f vl, vr, vb, vt;
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      vl = sample2(vf, i - 1, j);
      vr = sample2(vf, i + 1, j);
      vb = sample2(vf, i, j - 1);
      vt = sample2(vf, i, j + 1);
      velocity_divs[i * screen_width + j] = 0.5 * (vr.x - vl.x + vt.y - vb.y);
    }
  }
}

void Application::divergence(Vector2f vf[])
{
  Vector2f vl, vr, vb, vt;
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      vl = sample2(vf, i - 1, j);
      vr = sample2(vf, i + 1, j);
      vb = sample2(vf, i, j - 1);
      vt = sample2(vf, i, j + 1);
      velocity_curls[i * screen_width + j] = 0.5 * (vr.y - vl.y + vb.x - vt.x);
    }
  }
}

void Application::pressure_jacobi(float pf[], float new_pf[])
{
  float tmp[screen_height][screen_width];
  float pl, pr, pb, pt;
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      pl = sample1(pf, i - 1, j);
      pr = sample1(pf, i + 1, j);
      pb = sample1(pf, i, j - 1);
      pt = sample1(pf, i, j + 1);
      tmp[i][j] = (pl + pr + pb + pt - velocity_divs[i * screen_width + j]) * 0.25f;
    }
  }

  for(int i = 0; i < screen_height; ++ i)
    for(int j = 0; j < screen_width; ++ j)
      new_pf[i * screen_width + j] = tmp[i][j];
}

void Application::subtract_gradient(Vector2f vf[], float pf[])
{
  float pl, pr, pb, pt;
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      pl = sample1(pf, i - 1, j);
      pr = sample1(pf, i + 1, j);
      pb = sample1(pf, i, j - 1);
      pt = sample1(pf, i, j + 1);
      vf[i * screen_width + j] -= Vector2f(pr - pl, pt - pb) * 0.5f;
    }
  }
}

void Application::enhance_vorticity(Vector2f vf[], float cf[])
{
  //visual enhancement
  //anti-physics
  float cl, cr, cb, ct, cc;
  Vector2f force;
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
      cl = sample1(cf, i - 1, j);
      cr = sample1(cf, i + 1, j);
      cb = sample1(cf, i, j - 1);
      ct = sample1(cf, i, j + 1);
      cc = sample1(cf, i, j);
      force = Vector2f(fabs(ct) - fabs(cb) + 1e-3, fabs(cl) - fabs(cr) + 1e-3).normalized();
      force *= cc * curl_strength;
      vf[i * screen_width + j] = vf[i * screen_width + j] + force * dt;
      if(vf[i * screen_width + j].norm() < 1e-3)vf[i * screen_width + j] = Vector2f(1e-3, 1e-3);
    }
  }
}

void Application::render()
{
  advect();

  glPointSize(8);
  glBegin(GL_POINTS);
  for(int i = 0; i < screen_height; ++ i)
  {
    for(int j = 0; j < screen_width; ++ j)
    {
        // printf("%d %d %d\n",(int)dyes[i * screen_width + j].x, (int)dyes[i * screen_width + j].y, (int)dyes[i * screen_width + j].z);
        glColor3f(0.5 * dyes[i * screen_width + j].x, 0.7 * dyes[i * screen_width + j].y, 0.9 * dyes[i * screen_width + j].z);
        glVertex2f((float)i / screen_height * 2 - 1, (float)j / screen_width * 2 - 1);
        // window.at<cv::Vec3b>(i, j)[1] = 127 * dyes[i * screen_width + j].x,
        // window.at<cv::Vec3b>(i, j)[2] = 127 * dyes[i * screen_width + j].y,
        // window.at<cv::Vec3b>(i, j)[3] = 255 * dyes[i * screen_width + j].z;
    }
    // printf("\n");
  }
  glEnd();

  divergence(velocities);

  if(curl_strength > 0)
  {
    vorticity(velocities);
    enhance_vorticity(velocities, velocity_curls);
  }

  for(int i = 1; i <= jacobi_iters; ++ i)
      pressure_jacobi(pressures, pressures);
  
  subtract_gradient(velocities, pressures);
}

string Application::name() { return "Rope Simulator"; }

string Application::info() {
  ostringstream steps;
  steps << "Steps per frame: " << config.steps_per_frame;

  return steps.str();
}

}
