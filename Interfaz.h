
/*
 * ============================================================================
 * Archivo: Interfaz.h
 * Descripción: Gestiona el renderizado 3D con cámara en perspectiva y verdadero
 * zoom hacia el centro del plano (0,0,0), la ventana única de menú con navegación
 * por pestañas y todos los eventos de teclado y ratón.
 * ============================================================================
 */

#ifndef INTERFAZ_H
#define INTERFAZ_H

#include "Globales.h"
#include "IluminacionPhong.h"
#include "Texturas.h"
#include "Objetos3D.h"
#include "Archivos.h"

// Refresca ambas ventanas (Escena 3D y Menú Unificado)
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

// Configura la proyección en perspectiva para permitir un verdadero acercamiento 3D
inline void actualizarProyeccion() {
    glutSetWindow(mainWindow);
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (h == 0) h = 1;
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

// Ejes del plano cartesiano 3D centrados en (0, 0, 0)
inline void planoCart3D() {
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        // Eje Z (Azul)
        glColor3f(0.2f, 0.4f, 1.0f); glVertex3i(0, 0, -500); glVertex3i(0, 0, 500);
        // Eje X (Rojo)
        glColor3f(1.0f, 0.2f, 0.2f); glVertex3i(-500, 0, 0); glVertex3i(500, 0, 0);
        // Eje Y (Verde)
        glColor3f(0.2f, 1.0f, 0.2f); glVertex3i(0, -500, 0); glVertex3i(0, 500, 0);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

// Inicialización general del entorno OpenGL 3D
inline void inicializacion3D() {
    glClearColor(0.16f, 0.16f, 0.18f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    actualizarLuzPhongEscena();
    actualizarProyeccion();
    escanearArchivosLocales();
    cargarTodasLasTexturas();

    // Figuras iniciales de demostración
    figuras.push_back(new Figura3D(FIG_CUBO, 4.0f, -12.0f, 0.0f, 0.0f));
    figuras.push_back(new Figura3D(FIG_ESFERA, 4.5f, 0.0f, 0.0f, 0.0f));
    figuras.push_back(new Figura3D(FIG_CILINDRO, 4.0f, 12.0f, 0.0f, 0.0f));

    if (!figuras.empty()) figuras[0]->setSeleccionada(true);
}

// =========================================================================
// RENDERIZADO DE LA VENTANA PRINCIPAL 3D (VERDADERO ZOOM AL CENTRO)
// =========================================================================
inline void display_main_cb() {
    glutSetWindow(mainWindow);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Cálculo esférico de la posición de la cámara apuntando al centro del plano
    float radX = camaraRotX * M_PI / 180.0f;
    float radY = camaraRotY * M_PI / 180.0f;

    float eyeX = camaraPanX + camaraDistancia * cos(radX) * sin(-radY);
    float eyeY = camaraPanY + camaraDistancia * sin(radX);
    float eyeZ = camaraDistancia * cos(radX) * cos(-radY);

    posicionCamaraMundo = Vec3(eyeX, eyeY, eyeZ);

    // La cámara se acerca o aleja físicamente hacia el centro del plano (0,0,0) (+ paneo)
    gluLookAt(
        eyeX, eyeY, eyeZ,
        camaraPanX, camaraPanY, 0.0f,
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

// Función auxiliar para dibujar texto en el menú 2D
inline void dibujarTextoMenu(int x, int y, const string& texto, void* font = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2i(x, y);
    for (char c : texto) {
        glutBitmapCharacter(font, c);
    }
}

// Formatea flotantes a 2 decimales para el menú de Phong
inline string fmtFloat(float val) {
    stringstream ss;
    ss << fixed << setprecision(2) << val;
    return ss.str();
}

// =========================================================================
// RENDERIZADO DE LA VENTANA ÚNICA DE MENÚ (NAVEGACIÓN POR PESTAÑAS)
// =========================================================================
inline void display_menu_cb() {
    glutSetWindow(menuWindow);
    glClear(GL_COLOR_BUFFER_BIT);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Barra superior de pestañas navegables
    glColor3f(0.22f, 0.25f, 0.32f);
    glBegin(GL_QUADS);
        glVertex2i(0, h - 65); glVertex2i(w, h - 65);
        glVertex2i(w, h);      glVertex2i(0, h);
    glEnd();

    glColor3f(0.9f, 0.9f, 0.2f);
    dibujarTextoMenu(15, h - 22, "NAVEGACION DE MENU: [F1 - F4] o teclas [ < ] y [ > ]", GLUT_BITMAP_HELVETICA_12);

    string tabs[4] = { "[F1] Controles", "[F2] Texturas", "[F3] Escenas", "[F4] Luz Phong" };
    int tabX = 15;
    for (int i = 0; i < 4; i++) {
        if (menuTabActual == i) glColor3f(0.2f, 1.0f, 0.4f);
        else                    glColor3f(0.75f, 0.75f, 0.75f);
        dibujarTextoMenu(tabX, h - 48, tabs[i], GLUT_BITMAP_HELVETICA_12);
        tabX += 102;
    }

    int yPos = h - 90;
    glColor3f(1.0f, 1.0f, 1.0f);

    // ---------------------------------------------------------------------
    // PESTAÑA 1: CONTROLES GENERALES Y NUEVAS FIGURAS
    // ---------------------------------------------------------------------
    if (menuTabActual == TAB_CONTROLES) {
        vector<string> lineas = {
            "===== INSERTAR FIGURAS 3D =====",
            "[ 4 ] : Tetraedro       [ 6 ] : Cubo",
            "[ 8 ] : Octaedro        [ 2 ] : Icosaedro",
            "[ 5 / u ] : CILINDRO    [ 0 / o ] : ESFERA",
            "",
            "===== GESTION Y AGRUPACION =====",
            "[ TAB ] : Cambiar figura activa entera",
            "[ Shift + TAB ] : Seleccionar CARA (o toda si es -1)",
            "[ BACKSPACE ] : Eliminar figura activa",
            "[ m ] : Duplicar figura activa",
            "[ g ] : AGRUPAR CON LINEA INVISIBLE Y GUARDAR",
            "[ h ] : Mostrar/Ocultar linea conectora (" + string(mostrarLineasUnion ? "VISIBLE" : "OCULTA") + ")",
            "[ p ] : Ir al menu de Iluminacion Phong",
            "",
            "===== CAMARA (ZOOM REAL AL CENTRO) =====",
            "[ Clic Izq + Arrastrar ] : Orbitar camara",
            "[ Clic Der + Arrastrar ] : Zoom real hacia (0,0,0)",
            "[ I, K, J, L ] : Rotar camara   [ r ] : Centrar vista",
            "[ Flechas ] : Desplazar centro de vista",
            "",
            "===== TRANSFORMAR FIGURA ACTIVA =====",
            "[ W, A, S, D, Q, E ] : Trasladar en X, Y, Z",
            "[ + / - ] : Escala Global",
            "[ c/C, v/V, b/B ] : Escala en eje X, Y, Z",
            "[ x, y, z ] : Rotar figura sobre sus ejes"
        };
        for (const string& l : lineas) {
            dibujarTextoMenu(18, yPos, l);
            yPos -= 19;
        }
    }
    // ---------------------------------------------------------------------
    // PESTAÑA 2: APLICADOR Y VALIDADOR DE TEXTURAS BMP
    // ---------------------------------------------------------------------
    else if (menuTabActual == TAB_TEXTURAS) {
        dibujarTextoMenu(18, yPos, "===== TEXTURAS DISPONIBLES (Carpeta texturas/) =====");
        yPos -= 22;
        dibujarTextoMenu(18, yPos, "Pulsa 0 al 9 para aplicar a la cara (o figura entera):");
        yPos -= 28;

        if (nombresTexturas.empty()) {
            glColor3f(1.0f, 0.5f, 0.5f);
            dibujarTextoMenu(18, yPos, "No se encontraron archivos .bmp en 'texturas/'.");
        } else {
            for (size_t i = 0; i < nombresTexturas.size(); i++) {
                string estado = (texturasGlobales[i] != 0) ? "[OK]" : "[Error]";
                if (texturasGlobales[i] != 0) glColor3f(0.6f, 1.0f, 0.6f);
                else                          glColor3f(1.0f, 0.4f, 0.4f);

                string texto = "[ " + to_string(i) + " ] " + nombresTexturas[i] + " " + estado;
                dibujarTextoMenu(18, yPos, texto);
                yPos -= 22;
            }
        }
        yPos -= 15;
        glColor3f(0.85f, 0.85f, 0.85f);
        dibujarTextoMenu(18, yPos, "[ n ] : Quitar textura de la cara/figura activa");
    }
    // ---------------------------------------------------------------------
    // PESTAÑA 3: IMPORTADOR DE ESCENAS Y GRUPOS LOCALES
    // ---------------------------------------------------------------------
    else if (menuTabActual == TAB_ESCENAS) {
        dibujarTextoMenu(18, yPos, "===== IMPORTADOR DE ESCENAS GUARDADAS =====");
        yPos -= 22;
        dibujarTextoMenu(18, yPos, "Pulsa 1 al 9 para cargar grupo al escenario:");
        yPos -= 28;

        if (archivosGuardados.empty()) {
            glColor3f(1.0f, 0.7f, 0.4f);
            dibujarTextoMenu(18, yPos, "Sin archivos (Presiona 'g' para agrupar y exportar).");
        } else {
            glColor3f(0.6f, 0.9f, 1.0f);
            for (size_t i = 0; i < archivosGuardados.size() && i < 9; i++) {
                dibujarTextoMenu(18, yPos, "[ " + to_string(i + 1) + " ] " + archivosGuardados[i]);
                yPos -= 22;
            }
        }
    }
    // ---------------------------------------------------------------------
    // PESTAÑA 4: ILUMINACIÓN PARAMÉTRICA DE PHONG (CONSTRUIDA DESDE CERO)
    // ---------------------------------------------------------------------
    else if (menuTabActual == TAB_ILUMINACION) {
        dibujarTextoMenu(18, yPos, "===== ILUMINACION PARAMETRICA DE PHONG =====");
        yPos -= 20;
        glColor3f(0.8f, 0.9f, 1.0f);
        dibujarTextoMenu(18, yPos, "Usa [Flecha Arriba/Abajo] para elegir parametro");
        yPos -= 18;
        dibujarTextoMenu(18, yPos, "Usa [Flecha Izq/Der] o [ - / + ] para ajustar valor");
        yPos -= 26;

        MaterialPhong matActual;
        if (!figuras.empty() && figActiva >= 0 && figActiva < (int)figuras.size()) {
            matActual = figuras[figActiva]->getMaterial();
        }

        vector<string> parametros = {
            "1. Coef. Ambiental Objeto (ka) : " + fmtFloat(matActual.ka),
            "2. Coef. Difuso Objeto (kd)    : " + fmtFloat(matActual.kd),
            "3. Coef. Especular Objeto (ks) : " + fmtFloat(matActual.ks),
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
                dibujarTextoMenu(18, yPos, ">> " + parametros[i] + " <<");
            } else {
                glColor3f(0.9f, 0.9f, 0.9f);
                dibujarTextoMenu(18, yPos, "   " + parametros[i]);
            }
            yPos -= 20;
        }
    }

    glutSwapBuffers();
}

// =========================================================================
// AJUSTE INTERACTIVO DE PARÁMETROS DE PHONG
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
// CONTROLES DE RATÓN Y TECLADO UNIFICADOS
// =========================================================================
inline void mouse_cb(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) { isDraggingLeft = true; lastMouseX = x; lastMouseY = y; }
        else                    { isDraggingLeft = false; }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) { isDraggingRight = true; lastMouseY = y; }
        else                    { isDraggingRight = false; }
    }
    // Soporte adicional para rueda del ratón en GLUT (botones 3 y 4)
    else if (button == 3 && state == GLUT_DOWN) {
        camaraDistancia = max(3.0f, camaraDistancia - 2.5f);
        actualizarEscena();
    } else if (button == 4 && state == GLUT_DOWN) {
        camaraDistancia = min(250.0f, camaraDistancia + 2.5f);
        actualizarEscena();
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
        // Verdadero acercamiento/alejamiento de la cámara hacia el centro (0,0,0)
        int deltaY = y - lastMouseY;
        camaraDistancia += deltaY * 0.35f;
        if (camaraDistancia < 3.0f)   camaraDistancia = 3.0f;
        if (camaraDistancia > 250.0f) camaraDistancia = 250.0f;
        lastMouseY = y;
        actualizarEscena();
    }
}

inline void keyboard_cb(unsigned char key, int x, int y) {
    // Navegación rápida entre pestañas del menú con '<' / '>' o ',' / '.'
    if (key == ',' || key == '<') {
        menuTabActual = (menuTabActual + 3) % 4;
        actualizarEscena();
        return;
    }
    if (key == '.' || key == '>') {
        menuTabActual = (menuTabActual + 1) % 4;
        actualizarEscena();
        return;
    }

    // Acceso directo a la pestaña de Iluminación de Phong con 'p'
    if (key == 'p' || key == 'P') {
        menuTabActual = TAB_ILUMINACION;
        actualizarEscena();
        return;
    }

    // Mostrar/Ocultar líneas invisibles de unión con 'h'
    if (key == 'h' || key == 'H') {
        mostrarLineasUnion = !mostrarLineasUnion;
        actualizarEscena();
        return;
    }

    // Resetear vista al centro exacto del plano (0,0,0)
    if (key == 'r' || key == 'R') {
        camaraPanX = 0.0f;
        camaraPanY = 0.0f;
        camaraDistancia = 45.0f;
        camaraRotX = 25.0f;
        camaraRotY = -35.0f;
        actualizarEscena();
        return;
    }

    // Eliminar figura activa con Backspace o Suprimir
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

    // Agrupar con línea invisible y exportar escena completa (texturas + iluminación)
    if (key == 'g' || key == 'G') {
        agruparYGuardarEscenaActual();
        actualizarEscena();
        return;
    }

    // Duplicar figura o grupo activo
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

    // Quitar textura en la cara/figura activa
    if (key == 'n' || key == 'N') {
        if (!figuras.empty()) {
            figuras[figActiva]->setTexturaCaraActiva(-1);
            actualizarEscena();
        }
        return;
    }

    // Comportamiento contextual de las teclas numéricas según la pestaña activa del menú
    if (menuTabActual == TAB_TEXTURAS && key >= '0' && key <= '9') {
        int texID = key - '0';
        if (!figuras.empty()) {
            figuras[figActiva]->setTexturaCaraActiva(texID);
            actualizarEscena();
        }
        return;
    }
    else if (menuTabActual == TAB_ESCENAS && key >= '1' && key <= '9') {
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

    // Inserción de Figuras (Tetraedro, Cubo, Octaedro, Icosaedro + Cilindro y Esfera)
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

    // Selección de figura entera (TAB) o cara individual (Shift + TAB)
    if (key == 9) {
        int mod = glutGetModifiers();
        if (mod == GLUT_ACTIVE_SHIFT) {
            if (!figuras.empty()) figuras[figActiva]->ciclarCaraActiva();
        } else {
            if (!figuras.empty()) {
                figuras[figActiva]->setCaraActiva(-1);
                figuras[figActiva]->setSeleccionada(false);
                figActiva = (figActiva + 1) % figuras.size();
                figuras[figActiva]->setSeleccionada(true);
            }
        }
        actualizarEscena();
        return;
    }

    // Si estamos en la pestaña de Iluminación Phong, '+' y '-' también ajustan el parámetro si se desea
    if (menuTabActual == TAB_ILUMINACION && (key == '[' || key == ']')) {
        ajustarParametroPhongSeleccionado(key == ']' ? 1.0f : -1.0f);
        return;
    }

    // Controles de rotación de cámara con I, K, J, L
    switch (key) {
        case 'j': case 'J': camaraRotY -= 5.0f; break;
        case 'l': case 'L': camaraRotY += 5.0f; break;
        case 'i': case 'I': camaraRotX = min(89.0f, camaraRotX + 5.0f); break;
        case 'k': case 'K': camaraRotX = max(-89.0f, camaraRotX - 5.0f); break;
    }

    // Transformaciones de la figura o grupo activo
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

// Teclas especiales (F1-F4 para navegar por la ventana única de menú y flechas)
inline void special_cb(int key, int x, int y) {
    // Cambiar entre pestañas en la única ventana de menú
    if (key == GLUT_KEY_F1) { menuTabActual = TAB_CONTROLES;   actualizarEscena(); return; }
    if (key == GLUT_KEY_F2) { menuTabActual = TAB_TEXTURAS;    actualizarEscena(); return; }
    if (key == GLUT_KEY_F3) { menuTabActual = TAB_ESCENAS;     actualizarEscena(); return; }
    if (key == GLUT_KEY_F4) { menuTabActual = TAB_ILUMINACION; actualizarEscena(); return; }

    // Si la pestaña activa es Iluminación de Phong, las flechas navegan y ajustan los valores
    if (menuTabActual == TAB_ILUMINACION) {
        if (key == GLUT_KEY_UP) {
            opcionLuzSeleccionada = (opcionLuzSeleccionada + 12) % 13;
            actualizarEscena();
            return;
        }
        if (key == GLUT_KEY_DOWN) {
            opcionLuzSeleccionada = (opcionLuzSeleccionada + 1) % 13;
            actualizarEscena();
            return;
        }
        if (key == GLUT_KEY_LEFT) {
            ajustarParametroPhongSeleccionado(-1.0f);
            return;
        }
        if (key == GLUT_KEY_RIGHT) {
            ajustarParametroPhongSeleccionado(1.0f);
            return;
        }
    }

    // En las demás pestañas, las flechas desplazan la cámara
    switch (key) {
        case GLUT_KEY_LEFT:  camaraPanX -= 2.0f; break;
        case GLUT_KEY_RIGHT: camaraPanX += 2.0f; break;
        case GLUT_KEY_UP:    camaraPanY += 2.0f; break;
        case GLUT_KEY_DOWN:  camaraPanY -= 2.0f; break;
    }
    actualizarEscena();
}

#endif // INTERFAZ_H
