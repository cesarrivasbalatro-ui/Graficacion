

#ifndef OBJETOS3D_H
#define OBJETOS3D_H

#include "Globales.h"

class Objeto3D {
public:
    float tx, ty, tz;
    float sx, sy, sz;
    float rx, ry, rz;
    bool seleccionada;
    bool esValido;
    int caraActiva;

    Objeto3D() : tx(0), ty(0), tz(0), sx(1), sy(1), sz(1), rx(0), ry(0), rz(0), seleccionada(false), esValido(true), caraActiva(-1) {}
    virtual ~Objeto3D() {}

    virtual Objeto3D* clonar() = 0;
    virtual void dibujar() = 0;
    virtual string serializar() = 0;
    virtual void ciclarCaraActiva() = 0;
    virtual void setCaraActiva(int id) = 0;
    virtual void setTexturaCaraActiva(int texID) = 0;
    virtual void setColorBaseGlobal(float r, float g, float b) = 0;

    void trasladar(float dx, float dy, float dz) { if (esValido) { tx += dx; ty += dy; tz += dz; } }
    void escalar(float ex, float ey, float ez) { if (esValido) { sx *= ex; sy *= ey; sz *= ez; } }
    void escalarX(float factor) { if (esValido) { sx *= factor; } }
    void escalarY(float factor) { if (esValido) { sy *= factor; } }
    void escalarZ(float factor) { if (esValido) { sz *= factor; } }
    void rotar(float ax, float ay, float az) { if (esValido) { rx += ax; ry += ay; rz += az; } }

    virtual void setSeleccionada(bool s) { if (esValido) seleccionada = s; }
    bool isValid() { return esValido; }

    void setTransformaciones(float _sx, float _sy, float _sz, float _rx, float _ry, float _rz) {
        sx = _sx; sy = _sy; sz = _sz;
        rx = _rx; ry = _ry; rz = _rz;
    }
};

class Figura3D : public Objeto3D {
private:
    int totalCaras;
    float tamano;
    vector<float> verticesX;
    vector<float> verticesY;
    vector<float> verticesZ;
    vector<vector<int>> carasVertices;
    vector<ColorRGB> coloresCaras;
    vector<int> texturasCaras;

    void addVertice(float x, float y, float z) {
        verticesX.push_back(x * tamano);
        verticesY.push_back(y * tamano);
        verticesZ.push_back(z * tamano);
    }

    void generarGeometria() {
        switch (totalCaras) {
            case 4: {
                float sq2 = sqrt(2.0f);
                addVertice(1, 0, -1/sq2); addVertice(-1, 0, -1/sq2);
                addVertice(0, 1, 1/sq2); addVertice(0, -1, 1/sq2);
                carasVertices = {{0, 1, 2}, {1, 0, 3}, {0, 2, 3}, {2, 1, 3}};
                break;
            }
            case 6: {
                addVertice(1, 1, 1); addVertice(-1, 1, 1); addVertice(-1, -1, 1); addVertice(1, -1, 1);
                addVertice(1, 1, -1); addVertice(-1, 1, -1); addVertice(-1, -1, -1); addVertice(1, -1, -1);
                carasVertices = {
                    {0, 1, 2, 3}, {5, 4, 7, 6}, {4, 0, 3, 7},
                    {1, 5, 6, 2}, {4, 5, 1, 0}, {3, 2, 6, 7}
                };
                break;
            }
            case 8: {
                addVertice(1, 0, 0); addVertice(-1, 0, 0);
                addVertice(0, 1, 0); addVertice(0, -1, 0);
                addVertice(0, 0, 1); addVertice(0, 0, -1);
                carasVertices = {
                    {4, 0, 2}, {4, 2, 1}, {4, 1, 3}, {4, 3, 0},
                    {5, 2, 0}, {5, 1, 2}, {5, 3, 1}, {5, 0, 3}
                };
                break;
            }
            case 20: {
                float t = (1.0f + sqrt(5.0f)) / 2.0f;
                addVertice(-1, t, 0); addVertice(1, t, 0); addVertice(-1, -t, 0); addVertice(1, -t, 0);
                addVertice(0, -1, t); addVertice(0, 1, t); addVertice(0, -1, -t); addVertice(0, 1, -t);
                addVertice(t, 0, -1); addVertice(t, 0, 1); addVertice(-t, 0, -1); addVertice(-t, 0, 1);
                carasVertices = {
                    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
                    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
                    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
                    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
                };
                break;
            }
        }
    }

    void generarColoresAleatorios() {
        coloresCaras.resize(totalCaras);
        texturasCaras.resize(totalCaras, -1);
        for(int i = 0; i < totalCaras; i++) {
            coloresCaras[i].r = (rand() % 100) / 100.0f;
            coloresCaras[i].g = (rand() % 100) / 100.0f;
            coloresCaras[i].b = (rand() % 100) / 100.0f;
        }
    }

    void calcularNormal(float* v1, float* v2, float* v3, float* normal) {
        float u[3] = {v2[0]-v1[0], v2[1]-v1[1], v2[2]-v1[2]};
        float v[3] = {v3[0]-v1[0], v3[1]-v1[1], v3[2]-v1[2]};
        normal[0] = u[1]*v[2] - u[2]*v[1];
        normal[1] = u[2]*v[0] - u[0]*v[2];
        normal[2] = u[0]*v[1] - u[1]*v[0];
        float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
        if (len > 0) { normal[0]/=len; normal[1]/=len; normal[2]/=len; }
    }

public:
    Figura3D(int carasReq, float size, float x, float y, float z) {
        if (carasReq != 4 && carasReq != 6 && carasReq != 8 && carasReq != 20) {
            esValido = false; totalCaras = 0; return;
        }
        esValido = true;
        totalCaras = carasReq;
        tamano = size;
        tx = x; ty = y; tz = z;
        generarGeometria();
        generarColoresAleatorios();
    }

    Objeto3D* clonar() override {
        if (!esValido) return nullptr;
        Figura3D* copia = new Figura3D(totalCaras, tamano, tx, ty, tz);
        copia->sx = sx; copia->sy = sy; copia->sz = sz;
        copia->rx = rx; copia->ry = ry; copia->rz = rz;
        copia->coloresCaras = coloresCaras;
        copia->texturasCaras = texturasCaras;
        return copia;
    }

    string serializar() override {
        stringstream ss;
        ss << "F " << totalCaras << " " << tamano << " "
           << tx << " " << ty << " " << tz << " "
           << sx << " " << sy << " " << sz << " "
           << rx << " " << ry << " " << rz;
        for (int texID : texturasCaras) ss << " " << texID;
        return ss.str();
    }

    void ciclarCaraActiva() override {
        caraActiva++;
        if (caraActiva >= totalCaras) caraActiva = -1;
    }

    void setCaraActiva(int id) override { caraActiva = id; }

    void setTexturaCaraActiva(int texID) override {
        if (caraActiva >= 0 && caraActiva < totalCaras) texturasCaras[caraActiva] = texID;
    }

    void setTexturaEnCara(int cara, int texID) {
        if (cara >= 0 && cara < totalCaras) texturasCaras[cara] = texID;
    }

    void setColorBaseGlobal(float r, float g, float b) override {
        for(int i = 0; i < totalCaras; i++) {
            coloresCaras[i].r = r; coloresCaras[i].g = g; coloresCaras[i].b = b;
        }
    }

    void dibujar() override {
        if (!esValido) return;

        glPushMatrix();
        glTranslatef(tx, ty, tz);
        glRotatef(rx, 1.0f, 0.0f, 0.0f);
        glRotatef(ry, 0.0f, 1.0f, 0.0f);
        glRotatef(rz, 0.0f, 0.0f, 1.0f);
        glScalef(sx, sy, sz);

        for (size_t i = 0; i < carasVertices.size(); i++) {
            bool usaTextura = (texturasCaras[i] != -1 && texturasGlobales[texturasCaras[i]] != 0);

            if (usaTextura) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texturasGlobales[texturasCaras[i]]);
                glColor3f(1.0f, 1.0f, 1.0f);
            } else {
                glDisable(GL_TEXTURE_2D);
                float r = coloresCaras[i].r; float g = coloresCaras[i].g; float b = coloresCaras[i].b;
                if (seleccionada && caraActiva == -1)
                    glColor3f(r + (1.0f - r) * 0.5f, g + (1.0f - g) * 0.5f, b + (1.0f - b) * 0.5f);
                else glColor3f(r, g, b);
            }

            float v1[3] = {verticesX[carasVertices[i][0]], verticesY[carasVertices[i][0]], verticesZ[carasVertices[i][0]]};
            float v2[3] = {verticesX[carasVertices[i][1]], verticesY[carasVertices[i][1]], verticesZ[carasVertices[i][1]]};
            float v3[3] = {verticesX[carasVertices[i][2]], verticesY[carasVertices[i][2]], verticesZ[carasVertices[i][2]]};
            float normal[3];
            calcularNormal(v1, v2, v3, normal);

            glBegin(GL_POLYGON);
            glNormal3f(normal[0], normal[1], normal[2]);

            for (size_t j = 0; j < carasVertices[i].size(); j++) {
                if(j==0) glTexCoord2f(0.0f, 0.0f);
                else if(j==1) glTexCoord2f(1.0f, 0.0f);
                else if(j==2) glTexCoord2f(1.0f, 1.0f);
                else if(j==3) glTexCoord2f(0.0f, 1.0f);

                int index = carasVertices[i][j];
                glVertex3f(verticesX[index], verticesY[index], verticesZ[index]);
            }
            glEnd();

            glDisable(GL_TEXTURE_2D);

            if (seleccionada && caraActiva == i) {
                glLineWidth(4.0f);
                glColor3f(1.0f, 0.0f, 0.0f);
            } else {
                glLineWidth(1.0f);
                glColor3f(0.0f, 0.0f, 0.0f);
            }
            glBegin(GL_LINE_LOOP);
            for (int index : carasVertices[i]) glVertex3f(verticesX[index], verticesY[index], verticesZ[index]);
            glEnd();
            glLineWidth(1.0f);
        }
        glPopMatrix();
    }
};

class Grupo3D : public Objeto3D {
private:
    vector<Objeto3D*> componentes;
public:
    Grupo3D(const vector<Objeto3D*>& hijos) {
        esValido = true;
        for (auto h : hijos) componentes.push_back(h);
    }
    ~Grupo3D() { for (auto h : componentes) delete h; }

    Objeto3D* clonar() override {
        vector<Objeto3D*> hijosClonados;
        for (auto h : componentes) hijosClonados.push_back(h->clonar());
        Grupo3D* copia = new Grupo3D(hijosClonados);
        copia->tx = tx; copia->ty = ty; copia->tz = tz;
        copia->sx = sx; copia->sy = sy; copia->sz = sz;
        copia->rx = rx; copia->ry = ry; copia->rz = rz;
        return copia;
    }

    string serializar() override {
        stringstream ss;
        ss << "G " << componentes.size() << " "
           << tx << " " << ty << " " << tz << " "
           << sx << " " << sy << " " << sz << " "
           << rx << " " << ry << " " << rz << "\n";
        for (auto h : componentes) ss << h->serializar() << "\n";
        return ss.str();
    }

    void setSeleccionada(bool s) override {
        seleccionada = s;
        for (auto h : componentes) h->setSeleccionada(s);
    }

    void ciclarCaraActiva() override {}
    void setCaraActiva(int id) override {}
    void setTexturaCaraActiva(int texID) override {}
    void setColorBaseGlobal(float r, float g, float b) override {
        for (auto h : componentes) h->setColorBaseGlobal(r, g, b);
    }

    void dibujar() override {
        if (!esValido) return;
        glPushMatrix();
        glTranslatef(tx, ty, tz);
        glRotatef(rx, 1.0f, 0.0f, 0.0f);
        glRotatef(ry, 0.0f, 1.0f, 0.0f);
        glRotatef(rz, 0.0f, 0.0f, 1.0f);
        glScalef(sx, sy, sz);
        for (auto h : componentes) h->dibujar();
        glPopMatrix();
    }
};

#endif
