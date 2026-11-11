/*
 * ============================================================================
 * Archivo: Globales.h
 * Descripcion: Define las estructuras matematicas basicas, variables de estado
 * global, parametros de la camara con zoom real hacia el centro del plano (0,0,0)
 * y el estado de navegacion con una sola tecla para el menu unificado.
 * Compatible con GLUT de 32 bits.
 * ============================================================================
 */

#ifndef GLOBALES_H
#define GLOBALES_H

#include <windows.h>
#define GLUT_DISABLE_ATEXIT_HACKS
#include "glut.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

using namespace std;

// Estructuras matematicas auxiliares
struct ColorRGB {
    float r, g, b;
    ColorRGB(float _r = 0.8f, float _g = 0.8f, float _b = 0.8f) : r(_r), g(_g), b(_b) {}
};

struct Vec3 {
    float x, y, z;
    Vec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
};

struct Vec2 {
    float u, v;
    Vec2(float _u = 0.0f, float _v = 0.0f) : u(_u), v(_v) {}
};

// Identificadores de tipos de figuras soportadas
enum TipoFigura {
    FIG_TETRAEDRO = 4,
    FIG_CUBO      = 6,
    FIG_OCTAEDRO  = 8,
    FIG_ICOSAEDRO = 20,
    FIG_CILINDRO  = 100,
    FIG_ESFERA    = 200
};

// Vistas internas de la unica ventana de menu (se ciclan con la tecla 'p' o ESPACIO)
enum MenuTab {
    TAB_CONTROLES   = 0,
    TAB_TEXTURAS    = 1,
    TAB_ESCENAS     = 2,
    TAB_ILUMINACION = 3
};

class Objeto3D;

// =========================================================================
// VARIABLES GLOBALES DEL ENTORNO
// =========================================================================
vector<Objeto3D*> figuras;
vector<string> archivosGuardados;
int figActiva = 0;

// Dos unicas ventanas: Escena 3D y Panel de Menu Unificado
int mainWindow = 0;
int menuWindow = 0;
int menuTabActual = TAB_CONTROLES;
int opcionLuzSeleccionada = 0;   // Parametro activo en el menu de Phong (0 a 12)
bool mostrarLineasUnion = false; // Permite hacer visible/invisible la linea de union con 'h'

// Parametros de Camara 3D con verdadero Zoom hacia el centro del plano (0,0,0)
float camaraDistancia = 45.0f; // Distancia radial real hacia (0,0,0)
float camaraRotX = 25.0f;      // Elevacion
float camaraRotY = -35.0f;     // Giro horizontal

// Posicion calculada del ojo de la camara en el mundo 3D
Vec3 posicionCamaraMundo(0.0f, 15.0f, 45.0f);

// Control de raton
int lastMouseX = -1;
int lastMouseY = -1;
bool isDraggingLeft = false;
bool isDraggingRight = false;

// Arreglos globales de texturas BMP
GLuint texturasGlobales[10] = {0};
vector<string> nombresTexturas;

#endif // GLOBALES_H
