
#ifndef GLOBALES_H
#define GLOBALES_H

#include <windows.h>
#define GLUT_DISABLE_ATEXIT_HACKS
#include "glut.h"
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

using namespace std;

struct ColorRGB { float r, g, b; };

class Objeto3D;

extern vector<Objeto3D*> figuras;
extern vector<string> archivosGuardados;
extern vector<string> nombresTexturas;
extern GLuint texturasGlobales[10];
extern int figActiva;

extern int mainWindow, menuWindow, selectorWindow, texturaWindow;

extern float camaraZoom;
extern float camaraRotX;
extern float camaraRotY;
extern float camaraPosX;
extern float camaraPosY;

#endif
