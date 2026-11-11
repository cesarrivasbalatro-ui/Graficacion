/*
 * ============================================================================
 * Archivo: main.cpp
 * Descripción: Punto de entrada principal del modelador 3D OpenGL/GLUT.
 * Inicializa la ventana de renderizado 3D y la ventana única de menú navegable.
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

    // 1. Ventana Principal (Escena 3D con Iluminación de Phong, Texturas y Zoom)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 650);
    glutInitWindowPosition(40, 60);
    mainWindow = glutCreateWindow("Escena 3D");
    inicializacion3D();
    glutDisplayFunc(display_main_cb);
    glutReshapeFunc(reshape_main_cb);
    glutKeyboardFunc(keyboard_cb);
    glutSpecialFunc(special_cb);
    glutMouseFunc(mouse_cb);
    glutMotionFunc(motion_cb);

    // 2. Única Ventana de Menú Unificado (Navegable por pestañas F1-F4)
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(430, 560);
    glutInitWindowPosition(960, 60);
    menuWindow = glutCreateWindow("Panel de Control Unificado");
    glClearColor(0.14f, 0.14f, 0.16f, 1.0f);
    glutDisplayFunc(display_menu_cb);
    glutKeyboardFunc(keyboard_cb);
    glutSpecialFunc(special_cb);

    glutMainLoop();
    return 0;
}
