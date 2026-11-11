/*
 * ============================================================================
 * Archivo: Interfaz.h
 * Descripcion: Renderizado de la Escena 3D (con verdadero zoom hacia el centro
 * del plano) y de la Ventana Unica de Menu navegable mediante una sola tecla
 * ('p' o ESPACIO), 100% compatible con GLUT de 32 bits sin glutSpecialFunc.
 * ============================================================================
 */

#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "Globales.h"
#include "IluminacionPhong.h"
#include "Texturas.h"
#include "Objetos3D.h"
#include "Archivos.h"

inline void actualizarEscena() {
    if (mainWindow != 0) {
        glutSetWindow(mainWindow);
        glutPostRedisplay();
    }
    if (menuWindow != 0) {
        glutSetWindow(menuWindow);
        glutPostRedisplay();
    }
}

inline void actualizarProyeccion() {
    if (mainWindow == 0) return;
    glutSetWindow(mainWindow);
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h <= 0) h = 1;
    float aspecto = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspecto, 0.5, 2000.0);
    glMatrixMode(GL_MODELVIEW);
}

inline void reshape_main_cb(int w, int h) {
    actualizarProyeccion();
    actualizarEscena();
}

inline void reshape_menu_cb(int w, int h) {
    glutSetWindow(menuWindow);
    glViewport(0, 0, w, h);
    glutPostRedisplay();
}

inline void planoCart3D() {
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glColor3f(0.2f, 0.4f, 1.0f); glVertex3i(0, 0, -500); glVertex3i(0, 0, 500);
        glColor3f(1.0f, 0.2f, 0.2f); glVertex3i(-500, 0, 0); glVertex3i(500, 0, 0);
        glColor3f(0.2f, 1.0f, 0.2f); glVertex3i(0, -500, 0); glVertex3i(0, 500, 0);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

inline void inicializacion3D() {
    glClearColor(0.16f, 0.16f, 0.18f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    actualizarLuzPhongEscena();
    actualizarProyeccion();
    escanearArchivosLocales();
    cargarTodasLasTexturas();

    figuras.push_back(new Figura3D(FIG_CUBO, 4.0f, -12.0f, 0.0f, 0.0f));
    figuras.push_back(new Figura3D(FIG_ESFERA, 4.5f, 0.0f, 0.0f, 0.0f));
    figuras.push_back(new Figura3D(FIG_CILINDRO, 4.0f, 12.0f, 0.0f, 0.0f));

    if (!figuras.empty()) figuras[0]->setSeleccionada(true);
}

// =========================================================================
// RENDERIZADO DE LA VENTANA PRINCIPAL 3D (VERDADERO ZOOM AL CENTRO 0,0,0)
// =========================================================================
inline void display_main_cb() {
    glutSetWindow(mainWindow);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float radX = camaraRotX * M_PI / 180.0f;
    float radY = camaraRotY * M_PI / 180.0f;

    float eyeX = camaraDistancia * cos(radX) * sin(-radY);
    float eyeY = camaraDistancia * sin(radX);
    float eyeZ = camaraDistancia * cos(radX) * cos(-radY);

    posicionCamaraMundo = Vec3(eyeX, eyeY, eyeZ);

    // La camara se acerca o aleja fisicamente apuntando siempre al centro del plano (0,0,0)
    gluLookAt(
        eyeX, eyeY, eyeZ,
        0.0f, 0.0f, 0.0f,
        0.0f, (cos(radX) >= 0.0f ? 1.0f : -1.0f), 0.0f
    );

    actualizarLuzPhongEscena();
    planoCart3D();
    dibujarIndicadorLuzPhong();

    for (size_t i = 0; i < figuras.size(); i++) {
        figuras[i]->dibujar();
    }

    glutSwapBuffers();
}

inline void dibujarTextoMenu(int x, int y, const string& texto) {
    glRasterPos2i(x, y);
    for (size_t i = 0; i < texto.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, texto[i]);
    }
}

inline string fmtFloat(float val) {
    stringstream ss;
    ss << fixed << setprecision(2) << val;
    return ss.str();
}

// =========================================================================
// RENDERIZADO DE LA VENTANA UNICA DE MENU (NAVEGACION CON TECLA 'p')
// =========================================================================
inline void display_menu_cb() {
    glutSetWindow(menuWindow);
    glClear(GL_COLOR_BUFFER_BIT);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (w <= 0) w = 430;
    if (h <= 0) h = 560;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Encabezado fijo de navegacion con una sola tecla
    glColor3f(0.22f, 0.25f, 0.34f);
    glBegin(GL_QUADS);
        glVertex2i(0, h - 68); glVertex2i(w, h - 68);
        glVertex2i(w, h);      glVertex2i(0, h);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.2f);
    dibujarTextoMenu(14, h - 22, "PULSA [ p ] (o ESPACIO) PARA CAMBIAR DE MENU");

    string tabs[4] = { "1.Controles", "2.Texturas", "3.Escenas", "4.Luz Phong" };
    int tabX = 14;
    for (int i = 0; i < 4; i++) {
        if (menuTabActual == i) {
            glColor3f(0.2f, 1.0f, 0.4f);
            dibujarTextoMenu(tabX, h - 48, ">>" + tabs[i]);
        } else {
            glColor3f(0.7f, 0.7f, 0.7f);
            dibujarTextoMenu(tabX, h - 48, "  " + tabs[i]);
        }
        tabX += 100;
    }

    int yPos = h - 92;
    glColor3f(1.0f, 1.0f, 1.0f);

    // ---------------------------------------------------------------------
    // MENU 1: CONTROLES GENERALES Y FIGURAS (INCLUYE CILINDRO Y ESFERA)
    // ---------------------------------------------------------------------
    if (menuTabActual == TAB_CONTROLES) {
        vector<string> lineas = {
            "===== INSERTAR FIGURAS 3D =====",
            "[ 4 ] : Tetraedro       [ 6 ] : Cubo",
            "[ 8 ] : Octaedro        [ 2 ] : Icosaedro",
            "[ 5 / u ] : CILINDRO    [ 0 / o ] : ESFERA",
            "",
            "===== SELECCION Y AGRUPACION =====",
            "[ p / ESPACIO ] : Cambiar a siguiente Menu",
            "[ TAB ] : Cambiar figura activa entera",
            "[ f ] : Seleccionar CARA de la figura activa",
            "[ BACKSPACE ] : Eliminar figura activa",
            "[ m ] : Duplicar figura activa",
            "[ g ] : AGRUPAR (LINEA INVISIBLE) Y GUARDAR",
            "[ h ] : Ver linea union (" + string(mostrarLineasUnion ? "VISIBLE" : "INVISIBLE") + ")",
            "",
            "===== CAMARA (ZOOM REAL AL CENTRO) =====",
            "[ Clic Izq + Arrastrar ] : Orbitar camara",
            "[ Clic Der + Arrastrar ] : Zoom hacia (0,0,0)",
            "[ I, K, J, L ] : Rotar camara   [ r ] : Resetear",
            "",
            "===== TRANSFORMAR FIGURA ACTIVA =====",
            "[ W, A, S, D, Q, E ] : Trasladar en X, Y, Z",
            "[ + / - ] : Escala Global",
            "[ c/C, v/V, b/B ] : Escala en eje X, Y, Z",
            "[ x, y, z ] : Rotar figura en su eje"
        };
        for (size_t i = 0; i < lineas.size(); i++) {
            dibujarTextoMenu(16, yPos, lineas[i]);
            yPos -= 19;
        }
    }
    // ---------------------------------------------------------------------
    // MENU 2: TEXTURAS BMP
    // ---------------------------------------------------------------------
    else if (menuTabActual == TAB_TEXTURAS) {
        dibujarTextoMenu(16, yPos, "===== MENU DE TEXTURAS (Carpeta texturas/) =====");
        yPos -= 22;
        glColor3f(0.8f, 0.9f, 1.0f);
        dibujarTextoMenu(16, yPos, "Pulsa [ f ] para elegir cara, o 0-9 para aplicar:");
        yPos -= 26;

        if (nombresTexturas.empty()) {
            glColor3f(1.0f, 0.5f, 0.5f);
            dibujarTextoMenu(16, yPos, "No se encontraron archivos .bmp en 'texturas/'.");
        } else {
            for (size_t i = 0; i < nombresTexturas.size(); i++) {
                string estado = (texturasGlobales[i] != 0) ? "[OK]" : "[Error]";
                if (texturasGlobales[i] != 0) glColor3f(0.5f, 1.0f, 0.5f);
                else                          glColor3f(1.0f, 0.4f, 0.4f);

                dibujarTextoMenu(16, yPos, "[ " + to_string(i) + " ] " + nombresTexturas[i] + " " + estado);
                yPos -= 22;
            }
        }
        yPos -= 15;
        glColor3f(0.9f, 0.9f, 0.9f);
        dibujarTextoMenu(16, yPos, "[ n ] : Quitar textura de la cara/figura activa");
        yPos -= 22;
        dibujarTextoMenu(16, yPos, "[ p ] : Ir al siguiente menu (Importar Escenas)");
    }
    // ---------------------------------------------------------------------
    // MENU 3: IMPORTADOR DE ESCENAS GUARDADAS
    // ---------------------------------------------------------------------
    else if (menuTabActual == TAB_ESCENAS) {
        dibujarTextoMenu(16, yPos, "===== IMPORTADOR DE ESCENAS Y GRUPOS =====");
        yPos -= 22;
        glColor3f(0.8f, 0.9f, 1.0f);
        dibujarTextoMenu(16, yPos, "Pulsa del 1 al 9 para cargar grupo al escenario:");
        yPos -= 26;

        if (archivosGuardados.empty()) {
            glColor3f(1.0f, 0.7f, 0.4f);
            dibujarTextoMenu(16, yPos, "Sin archivos (Pulsa 'g' para agrupar y guardar).");
        } else {
            glColor3f(0.6f, 0.95f, 1.0f);
            for (size_t i = 0; i < archivosGuardados.size() && i < 9; i++) {
                dibujarTextoMenu(16, yPos, "[ " + to_string(i + 1) + " ] " + archivosGuardados[i]);
                yPos -= 22;
            }
        }
        yPos -= 15;
        glColor3f(0.9f, 0.9f, 0.9f);
        dibujarTextoMenu(16, yPos, "[ p ] : Ir al siguiente menu (Iluminacion Phong)");
    }
    // ---------------------------------------------------------------------
    // MENU 4: ILUMINACION PARAMETRICA DE PHONG (CON UNA SOLA TECLA)
    // ---------------------------------------------------------------------
    else if (menuTabActual == TAB_ILUMINACION) {
        dibujarTextoMenu(16, yPos, "===== ILUMINACION PARAMETRICA DE PHONG =====");
        yPos -= 20;
        glColor3f(0.4f, 1.0f, 0.6f);
        dibujarTextoMenu(16, yPos, "[ n ] : Cambiar al siguiente parametro de la lista");
        yPos -= 18;
        dibujarTextoMenu(16, yPos, "[ + ] / [ - ] : Subir o bajar valor seleccionado");
        yPos -= 24;

        MaterialPhong matActual;
        if (!figuras.empty() && figActiva >= 0 && figActiva < (int)figuras.size()) {
            matActual = figuras[figActiva]->getMaterial();
        }

        vector<string> parametros = {
            "1. Coef. Ambiental Figura (ka) : " + fmtFloat(matActual.ka),
            "2. Coef. Difuso Figura (kd)    : " + fmtFloat(matActual.kd),
            "3. Coef. Especular Figura (ks) : " + fmtFloat(matActual.ks),
            "4. Brillo Phong (Shininess n)  : " + fmtFloat(matActual.shininess),
            "5. Color Figura Rojo (R)       : " + fmtFloat(matActual.colorBase.r),
            "6. Color Figura Verde (G)      : " + fmtFloat(matActual.colorBase.g),
            "7. Color Figura Azul (B)       : " + fmtFloat(matActual.colorBase.b),
            "8. Luz Intensidad Ambiental Ia : " + fmtFloat(luzEscena.intensidadAmb),
            "9. Luz Intensidad Difusa Id    : " + fmtFloat(luzEscena.intensidadDif),
            "10. Luz Intensidad Especular Is: " + fmtFloat(luzEscena.intensidadEsp),
            "11. Posicion Luz X             : " + fmtFloat(luzEscena.posicion.x),
            "12. Posicion Luz Y             : " + fmtFloat(luzEscena.posicion.y),
            "13. Posicion Luz Z             : " + fmtFloat(luzEscena.posicion.z)
        };

        for (size_t i = 0; i < parametros.size(); i++) {
            if ((int)i == opcionLuzSeleccionada) {
                glColor3f(1.0f, 0.95f, 0.2f);
                dibujarTextoMenu(16, yPos, ">> " + parametros[i] + " <<");
            } else {
                glColor3f(0.88f, 0.88f, 0.88f);
                dibujarTextoMenu(16, yPos, "   " + parametros[i]);
            }
            yPos -= 20;
        }
    }

    glutSwapBuffers();
}

// =========================================================================
// AJUSTE INTERACTIVO DE PARAMETROS DE PHONG
// =========================================================================
inline void ajustarParametroPhongSeleccionado(float direccion) {
    MaterialPhong mat;
    bool hayFigura = (!figuras.empty() && figActiva >= 0 && figActiva < (int)figuras.size());
    if (hayFigura) mat = figuras[figActiva]->getMaterial();

    switch (opcionLuzSeleccionada) {
        case 0: mat.ka = min(1.0f, max(0.0f, mat.ka + direccion * 0.05f)); break;
        case 1: mat.kd = min(1.0f, max(0.0f, mat.kd + direccion * 0.05f)); break;
        case 2: mat.ks = min(1.0f, max(0.0f, mat.ks + direccion * 0.05f)); break;
        case 3: mat.shininess = min(128.0f, max(1.0f, mat.shininess + direccion * 4.0f)); break;
        case 4:
            mat.colorBase.r = min(1.0f, max(0.0f, mat.colorBase.r + direccion * 0.05f));
            if (hayFigura) figuras[figActiva]->setColorBaseGlobal(mat.colorBase.r, mat.colorBase.g, mat.colorBase.b);
            break;
        case 5:
            mat.colorBase.g = min(1.0f, max(0.0f, mat.colorBase.g + direccion * 0.05f));
            if (hayFigura) figuras[figActiva]->setColorBaseGlobal(mat.colorBase.r, mat.colorBase.g, mat.colorBase.b);
            break;
        case 6:
            mat.colorBase.b = min(1.0f, max(0.0f, mat.colorBase.b + direccion * 0.05f));
            if (hayFigura) figuras[figActiva]->setColorBaseGlobal(mat.colorBase.r, mat.colorBase.g, mat.colorBase.b);
            break;
        case 7: luzEscena.intensidadAmb = min(1.0f, max(0.0f, luzEscena.intensidadAmb + direccion * 0.05f)); break;
        case 8: luzEscena.intensidadDif = min(1.0f, max(0.0f, luzEscena.intensidadDif + direccion * 0.05f)); break;
        case 9: luzEscena.intensidadEsp = min(1.0f, max(0.0f, luzEscena.intensidadEsp + direccion * 0.05f)); break;
        case 10: luzEscena.posicion.x += direccion * 2.0f; break;
        case 11: luzEscena.posicion.y += direccion * 2.0f; break;
        case 12: luzEscena.posicion.z += direccion * 2.0f; break;
    }

    if (hayFigura && opcionLuzSeleccionada <= 3) {
        figuras[figActiva]->setMaterial(mat);
    }
    actualizarEscena();
}

// =========================================================================
// CONTROLES DE RATON Y TECLADO (100% COMPATIBLE CON GLUT 32 BITS)
// =========================================================================
inline void mouse_cb(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) { isDraggingLeft = true; lastMouseX = x; lastMouseY = y; }
        else                    { isDraggingLeft = false; }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) { isDraggingRight = true; lastMouseY = y; }
        else                    { isDraggingRight = false; }
    }
}

inline void motion_cb(int x, int y) {
    if (isDraggingLeft) {
        camaraRotY += (x - lastMouseX) * 0.5f;
        camaraRotX += (y - lastMouseY) * 0.5f;
        if (camaraRotX > 89.0f)  camaraRotX = 89.0f;
        if (camaraRotX < -89.0f) camaraRotX = -89.0f;
        lastMouseX = x;
        lastMouseY = y;
        actualizarEscena();
    } else if (isDraggingRight) {
        // Zoom real acercando o alejando la camara hacia el centro del plano (0,0,0)
        int deltaY = y - lastMouseY;
        camaraDistancia += deltaY * 0.35f;
        if (camaraDistancia < 3.0f)   camaraDistancia = 3.0f;
        if (camaraDistancia > 250.0f) camaraDistancia = 250.0f;
        lastMouseY = y;
        actualizarEscena();
    }
}

inline void keyboard_cb(unsigned char key, int x, int y) {
    // 1. UNICA TECLA PARA NAVEGAR ENTRE LOS 4 MENUS: 'p', 'P' o Barra Espaciadora
    if (key == 'p' || key == 'P' || key == ' ') {
        menuTabActual = (menuTabActual + 1) % 4;
        actualizarEscena();
        return;
    }

    // 2. Seleccionar cara individual con una sola tecla ('f' o 'F') compatible con 32 bits
    if (key == 'f' || key == 'F') {
        if (!figuras.empty()) {
            figuras[figActiva]->ciclarCaraActiva();
            actualizarEscena();
        }
        return;
    }

    // 3. Mostrar/Ocultar lineas invisibles de union con 'h'
    if (key == 'h' || key == 'H') {
        mostrarLineasUnion = !mostrarLineasUnion;
        actualizarEscena();
        return;
    }

    // 4. Resetear camara apuntando al centro (0,0,0)
    if (key == 'r' || key == 'R') {
        camaraDistancia = 45.0f;
        camaraRotX = 25.0f;
        camaraRotY = -35.0f;
        actualizarEscena();
        return;
    }

    // 5. Controles exclusivos cuando esta abierto el Menu 4 (Iluminacion Phong)
    if (menuTabActual == TAB_ILUMINACION) {
        if (key == 'n' || key == 'N') {
            opcionLuzSeleccionada = (opcionLuzSeleccionada + 1) % 13;
            actualizarEscena();
            return;
        }
        if (key == '+' || key == '=') {
            ajustarParametroPhongSeleccionado(1.0f);
            return;
        }
        if (key == '-' || key == '_') {
            ajustarParametroPhongSeleccionado(-1.0f);
            return;
        }
    }

    // 6. Controles exclusivos cuando esta abierto el Menu 2 (Texturas BMP)
    if (menuTabActual == TAB_TEXTURAS) {
        if (key >= '0' && key <= '9') {
            int texID = key - '0';
            if (!figuras.empty() && texID < (int)nombresTexturas.size()) {
                figuras[figActiva]->setTexturaCaraActiva(texID);
                actualizarEscena();
            }
            return;
        }
        if (key == 'n' || key == 'N') {
            if (!figuras.empty()) {
                figuras[figActiva]->setTexturaCaraActiva(-1);
                actualizarEscena();
            }
            return;
        }
    }

    // 7. Controles exclusivos cuando esta abierto el Menu 3 (Importar Escenas)
    if (menuTabActual == TAB_ESCENAS && key >= '1' && key <= '9') {
        int idx = key - '1';
        if (idx >= 0 && idx < (int)archivosGuardados.size()) {
            ifstream archivo(archivosGuardados[idx].c_str());
            if (archivo.is_open()) {
                Objeto3D* cargado = cargarObjetoDesdeStream(archivo);
                if (cargado) {
                    if (!figuras.empty()) figuras[figActiva]->setSeleccionada(false);
                    figuras.push_back(cargado);
                    figActiva = (int)figuras.size() - 1;
                    figuras[figActiva]->setSeleccionada(true);
                    actualizarEscena();
                }
                archivo.close();
            }
        }
        return;
    }

    // 8. Eliminar figura activa con Backspace (8) o Suprimir (127)
    if (key == 8 || key == 127) {
        if (!figuras.empty()) {
            delete figuras[figActiva];
            figuras.erase(figuras.begin() + figActiva);
            if (!figuras.empty()) {
                if (figActiva >= (int)figuras.size()) figActiva = 0;
                figuras[figActiva]->setSeleccionada(true);
            }
        }
        actualizarEscena();
        return;
    }

    // 9. Agrupar figuras con linea invisible y guardar escena en archivo .txt
    if (key == 'g' || key == 'G') {
        agruparYGuardarEscenaActual();
        actualizarEscena();
        return;
    }

    // 10. Duplicar figura activa
    if (key == 'm' || key == 'M') {
        if (!figuras.empty() && figActiva >= 0 && figActiva < (int)figuras.size()) {
            Objeto3D* clon = figuras[figActiva]->clonar();
            if (clon) {
                figuras[figActiva]->setSeleccionada(false);
                clon->trasladar(3.0f, 0.0f, 0.0f);
                figuras.push_back(clon);
                figActiva = (int)figuras.size() - 1;
                figuras[figActiva]->setSeleccionada(true);
            }
        }
        actualizarEscena();
        return;
    }

    // 11. Insertar figuras (Poliedros + Cilindro y Esfera)
    if (key == '4' || key == '6' || key == '8' || key == '2' ||
        key == '5' || key == 'u' || key == 'U' ||
        key == '0' || key == 'o' || key == 'O') {

        int tipo = FIG_CUBO;
        if (key == '4') tipo = FIG_TETRAEDRO;
        else if (key == '6') tipo = FIG_CUBO;
        else if (key == '8') tipo = FIG_OCTAEDRO;
        else if (key == '2') tipo = FIG_ICOSAEDRO;
        else if (key == '5' || key == 'u' || key == 'U') tipo = FIG_CILINDRO;
        else if (key == '0' || key == 'o' || key == 'O') tipo = FIG_ESFERA;

        if (!figuras.empty()) figuras[figActiva]->setSeleccionada(false);
        figuras.push_back(new Figura3D(tipo, 4.5f, 0.0f, 0.0f, 0.0f));
        figActiva = (int)figuras.size() - 1;
        figuras[figActiva]->setSeleccionada(true);
        actualizarEscena();
        return;
    }

    // 12. Cambiar figura activa entera con TAB (ASCII 9)
    if (key == 9) {
        if (!figuras.empty()) {
            figuras[figActiva]->setCaraActiva(-1);
            figuras[figActiva]->setSeleccionada(false);
            figActiva = (figActiva + 1) % figuras.size();
            figuras[figActiva]->setSeleccionada(true);
            actualizarEscena();
        }
        return;
    }

    // 13. Rotacion de camara por teclado (I, K, J, L)
    switch (key) {
        case 'j': case 'J': camaraRotY -= 5.0f; break;
        case 'l': case 'L': camaraRotY += 5.0f; break;
        case 'i': case 'I': if (camaraRotX < 85.0f) camaraRotX += 5.0f; break;
        case 'k': case 'K': if (camaraRotX > -85.0f) camaraRotX -= 5.0f; break;
    }

    // 14. Transformaciones sobre la figura o grupo activo
    if (!figuras.empty() && figActiva >= 0 && figActiva < (int)figuras.size()) {
        Objeto3D* obj = figuras[figActiva];
        switch (key) {
            case 'd': case 'D': obj->trasladar(1.0f, 0.0f, 0.0f); break;
            case 'a': case 'A': obj->trasladar(-1.0f, 0.0f, 0.0f); break;
            case 'w': case 'W': obj->trasladar(0.0f, 1.0f, 0.0f); break;
            case 's': case 'S': obj->trasladar(0.0f, -1.0f, 0.0f); break;
            case 'q': case 'Q': obj->trasladar(0.0f, 0.0f, 1.0f); break;
            case 'e': case 'E': obj->trasladar(0.0f, 0.0f, -1.0f); break;

            case '+': obj->escalar(1.1f, 1.1f, 1.1f); break;
            case '-': obj->escalar(0.9f, 0.9f, 0.9f); break;
            case 'c': obj->escalarX(1.1f); break;
            case 'C': obj->escalarX(0.9f); break;
            case 'v': obj->escalarY(1.1f); break;
            case 'V': obj->escalarY(0.9f); break;
            case 'b': obj->escalarZ(1.1f); break;
            case 'B': obj->escalarZ(0.9f); break;

            case 'x': case 'X': obj->rotar(5.0f, 0.0f, 0.0f); break;
            case 'y': case 'Y': obj->rotar(0.0f, 5.0f, 0.0f); break;
            case 'z': case 'Z': obj->rotar(0.0f, 0.0f, 5.0f); break;
        }
    }

    actualizarEscena();
}

#endif // INTERFAZ_H
