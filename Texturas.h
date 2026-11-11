/*
 * ============================================================================
 * Archivo: Texturas.h
 * Descripción: Cargador nativo de archivos de imagen BMP (24 bits) y escáner
 * automático del directorio local 'texturas/' con mapeo por nombre e índice.
 * ============================================================================
 */

#ifndef TEXTURAS_H
#define TEXTURAS_H

#include "Globales.h"

// Cargador nativo de texturas BMP de 24 bits
inline GLuint loadBMP_custom(const char* imagepath) {
    FILE* file = fopen(imagepath, "rb");
    if (!file) return 0;

    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        fclose(file);
        return 0;
    }
    if (header[0] != 'B' || header[1] != 'M') {
        fclose(file);
        return 0;
    }

    unsigned int dataPos   = *(int*)&(header[0x0A]);
    unsigned int imageSize = *(int*)&(header[0x22]);
    unsigned int width     = *(int*)&(header[0x12]);
    unsigned int height    = *(int*)&(header[0x16]);

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0)   dataPos = 54;

    unsigned char* data = new unsigned char[imageSize];
    fseek(file, dataPos, SEEK_SET);
    fread(data, 1, imageSize, file);
    fclose(file);

    // Convertir canales BGR (estándar BMP) a RGB
    for (unsigned int i = 0; i + 2 < imageSize; i += 3) {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return textureID;
}

// Escanea dinámicamente los archivos .bmp en la carpeta texturas/
inline void cargarTodasLasTexturas() {
    nombresTexturas.clear();
    for (int i = 0; i < 10; i++) texturasGlobales[i] = 0;

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA("texturas/*.bmp", &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        int i = 0;
        do {
            if (i > 9) break; // Límite de 10 texturas mapeadas a las teclas 0-9
            nombresTexturas.push_back(findFileData.cFileName);
            string path = "texturas/" + string(findFileData.cFileName);
            texturasGlobales[i] = loadBMP_custom(path.c_str());
            i++;
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
}

// Obtiene el índice de textura a partir del nombre del archivo BMP
inline int buscarIndiceTexturaPorNombre(const string& nombreArchivo) {
    if (nombreArchivo == "NONE" || nombreArchivo == "-1") return -1;
    for (size_t i = 0; i < nombresTexturas.size(); i++) {
        if (nombresTexturas[i] == nombreArchivo) return (int)i;
    }
    // Compatibilidad hacia atrás si en el archivo viene un número del 0 al 9
    if (nombreArchivo.size() == 1 && isdigit(nombreArchivo[0])) {
        int id = nombreArchivo[0] - '0';
        if (id >= 0 && id < (int)nombresTexturas.size()) return id;
    }
    return -1;
}

#endif // TEXTURAS_H
