

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "Globales.h"
#include "Objetos3D.h"

inline Objeto3D* cargarObjeto(ifstream& archivo) {
    string tipo;
    if (!(archivo >> tipo)) return nullptr;

    if (tipo == "F") {
        int caras; float tamano, tX, tY, tZ, sX, sY, sZ, rX, rY, rZ;
        archivo >> caras >> tamano >> tX >> tY >> tZ >> sX >> sY >> sZ >> rX >> rY >> rZ;
        Figura3D* fig = new Figura3D(caras, tamano, tX, tY, tZ);
        fig->setTransformaciones(sX, sY, sZ, rX, rY, rZ);

        for (int i = 0; i < caras; i++) {
            int texID;
            if (archivo >> texID) fig->setTexturaEnCara(i, texID);
        }
        return fig;
    }
    else if (tipo == "G") {
        int numHijos; float tX, tY, tZ, sX, sY, sZ, rX, rY, rZ;
        archivo >> numHijos >> tX >> tY >> tZ >> sX >> sY >> sZ >> rX >> rY >> rZ;
        vector<Objeto3D*> hijos;
        for (int i = 0; i < numHijos; i++) {
            Objeto3D* hijo = cargarObjeto(archivo);
            if (hijo) hijos.push_back(hijo);
        }
        Grupo3D* grupo = new Grupo3D(hijos);
        grupo->setTransformaciones(sX, sY, sZ, rX, rY, rZ);
        return grupo;
    }
    return nullptr;
}

inline void escanearArchivosLocales() {
    archivosGuardados.clear();
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("escena_*.txt", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do { archivosGuardados.push_back(findFileData.cFileName); } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
}

#endif
