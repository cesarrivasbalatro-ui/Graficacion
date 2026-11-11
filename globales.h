/*
 * ============================================================================
 * Archivo: Globales.h
 * Descripción: Define las estructuras matemáticas básicas, variables de estado
 * global, parámetros de la cámara orbital con zoom hacia el centro del plano y
 * estados de navegación para la ventana de menú unificada.
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

// Estructuras matemáticas auxiliares
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

// Pestañas de navegación para la ventana única de menú
enum MenuTab {
    TAB_CONTROLES   = 0,
    TAB_TEXTURAS    = 1,
    TAB_ESCENAS     = 2,
    TAB_ILUMINACION = 3
};

// Declaración adelantada de la clase base
class Objeto3D;

// =========================================================================
// VARIABLES GLOBALES DEL ENTORNO
// =========================================================================
inline vector<Objeto3D*> figuras;
inline vector<string> archivosGuardados;
inline int figActiva = 0;

// Solo 2 ventanas: Escena 3D principal y Menú Unificado
inline int mainWindow = 0;
inline int menuWindow = 0;
inline int menuTabActual = TAB_CONTROLES;
inline int opcionLuzSeleccionada = 0; // Índice del parámetro activo en el menú de Phong
inline bool mostrarLineasUnion = false; // Permite visualizar opcionalmente la línea invisible

// Parámetros de Cámara 3D (Verdadero Zoom hacia el centro del plano)
inline float camaraDistancia = 45.0f; // Distancia radial real hacia el centro (0,0,0)
inline float camaraRotX = 25.0f;      // Ángulo de elevación (Pitch)
inline float camaraRotY = -35.0f;     // Ángulo acimutal (Yaw)
inline float camaraPanX = 0.0f;       // Desplazamiento lateral opcional
inline float camaraPanY = 0.0f;       // Desplazamiento vertical opcional

// Posición calculada del ojo de la cámara en el mundo 3D (usada por Phong y gluLookAt)
inline Vec3 posicionCamaraMundo(0.0f, 15.0f, 45.0f);

// Control de ratón
inline int lastMouseX = -1;
inline int lastMouseY = -1;
inline bool isDraggingLeft = false;
inline bool isDraggingRight = false;

// Arreglos globales de texturas
inline GLuint texturasGlobales[10] = {0};
inline vector<string> nombresTexturas;

#endif // GLOBALES_H
