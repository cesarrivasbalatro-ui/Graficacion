/*
 * ============================================================================
 * Archivo: IluminacionPhong.h
 * Descripcion: Implementa desde cero las funciones parametricas del modelo de
 * iluminacion de Phong (Ambiental + Difusa + Especular) para modelos 3D,
 * evaluando normales, direccion de luz y vector de vision hacia la camara.
 * ============================================================================
 */

#ifndef ILUMINACION_PHONG_H
#define ILUMINACION_PHONG_H

#include "Globales.h"

// Parametros de material de Phong propios de cada figura 3D
struct MaterialPhong {
    float ka;           // Coeficiente ambiental [0.0 - 1.0]
    float kd;           // Coeficiente difuso    [0.0 - 1.0]
    float ks;           // Coeficiente especular [0.0 - 1.0]
    float shininess;    // Exponente de brillo   [1.0 - 128.0]
    ColorRGB colorBase; // Color base del objeto

    MaterialPhong()
        : ka(0.30f), kd(0.75f), ks(0.60f), shininess(32.0f), colorBase(0.75f, 0.75f, 0.85f) {}
};

// Parametros globales de la fuente de luz de Phong en la escena
struct ParametrosLuzPhong {
    Vec3 posicion;       // Posicion 3D de la luz (Lx, Ly, Lz)
    float intensidadAmb; // Intensidad ambiental global Ia [0.0 - 1.0]
    float intensidadDif; // Intensidad difusa Id           [0.0 - 1.0]
    float intensidadEsp; // Intensidad especular Is        [0.0 - 1.0]
    ColorRGB colorLuz;   // Color de la fuente luminosa
    bool mostrarFoco;    // Dibuja una esfera guia en la posicion de la luz

    ParametrosLuzPhong()
        : posicion(20.0f, 25.0f, 25.0f),
          intensidadAmb(0.35f),
          intensidadDif(0.85f),
          intensidadEsp(0.90f),
          colorLuz(1.0f, 1.0f, 1.0f),
          mostrarFoco(true) {}
};

// Instancia global de la luz de la escena
ParametrosLuzPhong luzEscena;

// =========================================================================
// FUNCIONES VECTORIALES AUXILIARES PARA PHONG
// =========================================================================
inline Vec3 normalizarVec3(const Vec3& v) {
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len > 0.00001f) {
        return Vec3(v.x / len, v.y / len, v.z / len);
    }
    return Vec3(0.0f, 1.0f, 0.0f);
}

inline float productoPunto(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 productoCruz(const Vec3& a, const Vec3& b) {
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

inline Vec3 rotarVectorModelo(const Vec3& v, float rx, float ry, float rz) {
    float radX = rx * M_PI / 180.0f;
    float radY = ry * M_PI / 180.0f;
    float radZ = rz * M_PI / 180.0f;

    // Rotacion Z
    float x1 = v.x * cos(radZ) - v.y * sin(radZ);
    float y1 = v.x * sin(radZ) + v.y * cos(radZ);
    float z1 = v.z;

    // Rotacion Y
    float x2 =  x1 * cos(radY) + z1 * sin(radY);
    float y2 =  y1;
    float z2 = -x1 * sin(radY) + z1 * cos(radY);

    // Rotacion X
    float x3 = x2;
    float y3 = y2 * cos(radX) - z2 * sin(radX);
    float z3 = y2 * sin(radX) + z2 * cos(radX);

    return Vec3(x3, y3, z3);
}

// =========================================================================
// ECUACION PARAMETRICA DE ILUMINACION DE PHONG
// =========================================================================
inline ColorRGB calcularColorPhongParametrico(
    const Vec3& posMundo,
    const Vec3& normalMundo,
    const ColorRGB& colorSuperficie,
    const MaterialPhong& mat,
    const ParametrosLuzPhong& luz = luzEscena,
    const Vec3& posCamara = posicionCamaraMundo
) {
    Vec3 N = normalizarVec3(normalMundo);
    Vec3 L = normalizarVec3(Vec3(
        luz.posicion.x - posMundo.x,
        luz.posicion.y - posMundo.y,
        luz.posicion.z - posMundo.z
    ));
    Vec3 V = normalizarVec3(Vec3(
        posCamara.x - posMundo.x,
        posCamara.y - posMundo.y,
        posCamara.z - posMundo.z
    ));

    // 1. Componente Ambiental
    float ambR = luz.intensidadAmb * mat.ka * colorSuperficie.r * luz.colorLuz.r;
    float ambG = luz.intensidadAmb * mat.ka * colorSuperficie.g * luz.colorLuz.g;
    float ambB = luz.intensidadAmb * mat.ka * colorSuperficie.b * luz.colorLuz.b;

    // 2. Componente Difusa
    float NdotL = productoPunto(N, L);
    float diffFactor = (NdotL > 0.0f) ? NdotL : 0.0f;

    float difR = luz.intensidadDif * mat.kd * diffFactor * colorSuperficie.r * luz.colorLuz.r;
    float difG = luz.intensidadDif * mat.kd * diffFactor * colorSuperficie.g * luz.colorLuz.g;
    float difB = luz.intensidadDif * mat.kd * diffFactor * colorSuperficie.b * luz.colorLuz.b;

    // 3. Componente Especular
    float espR = 0.0f, espG = 0.0f, espB = 0.0f;
    if (NdotL > 0.0f) {
        Vec3 R = normalizarVec3(Vec3(
            2.0f * NdotL * N.x - L.x,
            2.0f * NdotL * N.y - L.y,
            2.0f * NdotL * N.z - L.z
        ));
        float RdotV = productoPunto(R, V);
        if (RdotV > 0.0f) {
            float specFactor = pow(RdotV, mat.shininess);
            espR = luz.intensidadEsp * mat.ks * specFactor * luz.colorLuz.r;
            espG = luz.intensidadEsp * mat.ks * specFactor * luz.colorLuz.g;
            espB = luz.intensidadEsp * mat.ks * specFactor * luz.colorLuz.b;
        }
    }

    ColorRGB resultado;
    resultado.r = (ambR + difR + espR > 1.0f) ? 1.0f : ((ambR + difR + espR < 0.0f) ? 0.0f : ambR + difR + espR);
    resultado.g = (ambG + difG + espG > 1.0f) ? 1.0f : ((ambG + difG + espG < 0.0f) ? 0.0f : ambG + difG + espG);
    resultado.b = (ambB + difB + espB > 1.0f) ? 1.0f : ((ambB + difB + espB < 0.0f) ? 0.0f : ambB + difB + espB);
    return resultado;
}

inline void actualizarLuzPhongEscena() {
    GLfloat amb[] = {
        luzEscena.intensidadAmb * luzEscena.colorLuz.r,
        luzEscena.intensidadAmb * luzEscena.colorLuz.g,
        luzEscena.intensidadAmb * luzEscena.colorLuz.b,
        1.0f
    };
    GLfloat dif[] = {
        luzEscena.intensidadDif * luzEscena.colorLuz.r,
        luzEscena.intensidadDif * luzEscena.colorLuz.g,
        luzEscena.intensidadDif * luzEscena.colorLuz.b,
        1.0f
    };
    GLfloat esp[] = {
        luzEscena.intensidadEsp * luzEscena.colorLuz.r,
        luzEscena.intensidadEsp * luzEscena.colorLuz.g,
        luzEscena.intensidadEsp * luzEscena.colorLuz.b,
        1.0f
    };
    GLfloat pos[] = {
        luzEscena.posicion.x,
        luzEscena.posicion.y,
        luzEscena.posicion.z,
        1.0f
    };

    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, esp);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
}

inline void dibujarIndicadorLuzPhong() {
    if (!luzEscena.mostrarFoco) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(luzEscena.posicion.x, luzEscena.posicion.y, luzEscena.posicion.z);
    glColor3f(1.0f, 0.95f, 0.2f);
    glutWireSphere(0.8, 8, 8);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

#endif // ILUMINACION_PHONG_H
