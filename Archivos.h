/*
 * ============================================================================
 * Archivo: Archivos.h
 * Descripcion: Almacenamiento y lectura en archivos locales (escena_*.txt)
 * guardando figuras agrupadas con lineas invisibles, texturas e iluminacion.
 * ============================================================================
 */

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "Globales.h"
#include "Objetos3D.h"

inline Objeto3D* cargarObjetoDesdeStream(ifstream& archivo) {
    string token;
    while (archivo >> token) {
        if (token == "LUZ") {
            archivo >> luzEscena.posicion.x >> luzEscena.posicion.y >> luzEscena.posicion.z
                    >> luzEscena.intensidadAmb >> luzEscena.intensidadDif >> luzEscena.intensidadEsp
                    >> luzEscena.colorLuz.r >> luzEscena.colorLuz.g >> luzEscena.colorLuz.b;
            actualizarLuzPhongEscena();
            continue;
        }

        if (token == "F") {
            int codigoTipo, numCarasGuardadas;
            float tamano, tX, tY, tZ, sX, sY, sZ, rX, rY, rZ;
            MaterialPhong mat;

            archivo >> codigoTipo >> tamano
                    >> tX >> tY >> tZ
                    >> sX >> sY >> sZ
                    >> rX >> rY >> rZ
                    >> mat.ka >> mat.kd >> mat.ks >> mat.shininess
                    >> mat.colorBase.r >> mat.colorBase.g >> mat.colorBase.b
                    >> numCarasGuardadas;

            Figura3D* fig = new Figura3D(codigoTipo, tamano, tX, tY, tZ);
            fig->setTransformaciones(sX, sY, sZ, rX, rY, rZ);
            fig->setMaterial(mat);

            for (int i = 0; i < numCarasGuardadas; i++) {
                ColorRGB col;
                string texToken;
                archivo >> col.r >> col.g >> col.b >> texToken;
                int texID = buscarIndiceTexturaPorNombre(texToken);
                fig->setDatosCara(i, col, texID);
            }
            return fig;
        }
        else if (token == "G") {
            int numHijos = 0, numLineas = 0;
            float tX, tY, tZ, sX, sY, sZ, rX, rY, rZ;
            archivo >> numHijos >> tX >> tY >> tZ >> sX >> sY >> sZ >> rX >> rY >> rZ >> numLineas;

            vector<LineaConectora> lineas;
            for (int i = 0; i < numLineas; i++) {
                string tagL;
                Vec3 a, b;
                archivo >> tagL >> a.x >> a.y >> a.z >> b.x >> b.y >> b.z;
                lineas.push_back(LineaConectora(a, b));
            }

            vector<Objeto3D*> hijos;
            for (int i = 0; i < numHijos; i++) {
                Objeto3D* hijo = cargarObjetoDesdeStream(archivo);
                if (hijo) hijos.push_back(hijo);
            }

            Grupo3D* grupo = new Grupo3D(hijos);
            grupo->tx = tX; grupo->ty = tY; grupo->tz = tZ;
            grupo->setTransformaciones(sX, sY, sZ, rX, rY, rZ);
            grupo->setLineasConectoras(lineas);
            return grupo;
        }
    }
    return nullptr;
}

inline void escanearArchivosLocales() {
    archivosGuardados.clear();
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("escena_*.txt", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            archivosGuardados.push_back(findFileData.cFileName);
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
}

inline void agruparYGuardarEscenaActual() {
    if (figuras.empty()) return;

    if (figuras.size() > 1) {
        vector<Objeto3D*> copiaHijos;
        for (size_t i = 0; i < figuras.size(); i++) {
            figuras[i]->setSeleccionada(false);
            figuras[i]->setCaraActiva(-1);
            copiaHijos.push_back(figuras[i]);
        }
        figuras.clear();
        Grupo3D* nuevoGrupo = new Grupo3D(copiaHijos);
        nuevoGrupo->setSeleccionada(true);
        figuras.push_back(nuevoGrupo);
        figActiva = 0;
    }

    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "escena_%Y%m%d_%H%M%S.txt", timeinfo);

    ofstream archivo(buffer);
    if (archivo.is_open()) {
        archivo << "LUZ "
                << luzEscena.posicion.x << " " << luzEscena.posicion.y << " " << luzEscena.posicion.z << " "
                << luzEscena.intensidadAmb << " " << luzEscena.intensidadDif << " " << luzEscena.intensidadEsp << " "
                << luzEscena.colorLuz.r << " " << luzEscena.colorLuz.g << " " << luzEscena.colorLuz.b << "\n";

        archivo << figuras[0]->serializar() << "\n";
        archivo.close();
    }

    escanearArchivosLocales();
}

#endif // ARCHIVOS_H
