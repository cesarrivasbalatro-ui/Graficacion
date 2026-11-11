/*
 * ============================================================================
 * Archivo: main.cpp
 * Descripcion: Punto de entrada principal compatible con GLUT de 32 bits.
 * Crea la ventana de la Escena 3D y una unica ventana de Menu navegable con 'p'.
 * ============================================================================
 */

#include "Globales.h"
#include "IluminacionPhong.h"
#include "Texturas.h"
#include "Objetos3D.h"
#include "Archivos.h"
#include "Interfaz.h"

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    glutInit(&argc, argv);

    // 1. Ventana Principal (Escena 3D)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(880, 620);
    glutInitWindowPosition(40, 60);
    mainWindow = glutCreateWindow("Escena 3D - Phong, Texturas y Zoom al Centro");
    inicializacion3D();
    glutDisplayFunc(display_main_cb);
    glutReshapeFunc(reshape_main_cb);
    glutKeyboardFunc(keyboard_cb);
    glutMouseFunc(mouse_cb);
    glutMotionFunc(motion_cb);

    // 2. Unica Ventana de Menu (Se navega entre todas las vistas con la tecla 'p')
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(440, 560);
    glutInitWindowPosition(940, 60);
    menuWindow = glutCreateWindow("Menu Unificado (Pulsa 'p' para cambiar vista)");
    glClearColor(0.14f, 0.14f, 0.16f, 1.0f);
    glutDisplayFunc(display_menu_cb);
    glutReshapeFunc(reshape_menu_cb);
    glutKeyboardFunc(keyboard_cb);

    glutMainLoop();
    return 0;
}
