/*
 * ============================================================================
 * Archivo: Objetos3D.h
 * Descripción: Define la jerarquía de clases para los modelos 3D:
 *   - Objeto3D (Clase base abstracta con transformaciones y MaterialPhong).
 *   - Figura3D (Soporta Tetraedro, Cubo, Octaedro, Icosaedro, Cilindro y Esfera).
 *   - Grupo3D  (Agrupa figuras incluso separadas uniéndolas mediante líneas
 *               conectoras invisibles para manipularlas y guardarlas juntas).
 * ============================================================================
 */

#ifndef OBJETOS3D_H
#define OBJETOS3D_H

#include "Globales.h"
#include "IluminacionPhong.h"
#include "Texturas.h"

// Representa la línea invisible que conecta figuras separadas dentro de un grupo
struct LineaConectora {
    Vec3 inicio;
    Vec3 fin;
    LineaConectora(Vec3 a = Vec3(), Vec3 b = Vec3()) : inicio(a), fin(b) {}
};

// =========================================================================
// CLASE BASE ABSTRACTA: Objeto3D
// =========================================================================
class Objeto3D {
public:
    float tx, ty, tz;
    float sx, sy, sz;
    float rx, ry, rz;
    bool seleccionada;
    bool esValido;
    int caraActiva;
    MaterialPhong material;

    Objeto3D()
        : tx(0), ty(0), tz(0),
          sx(1), sy(1), sz(1),
          rx(0), ry(0), rz(0),
          seleccionada(false), esValido(true), caraActiva(-1) {}

    virtual ~Objeto3D() {}

    virtual Objeto3D* clonar() = 0;
    virtual void dibujar() = 0;
    virtual string serializar() = 0;
    virtual void ciclarCaraActiva() = 0;
    virtual void setCaraActiva(int id) = 0;
    virtual void setTexturaCaraActiva(int texID) = 0;
    virtual void setColorBaseGlobal(float r, float g, float b) = 0;
    virtual MaterialPhong& getMaterial() { return material; }
    virtual void setMaterial(const MaterialPhong& m) { material = m; }

    void trasladar(float dx, float dy, float dz) {
        if (esValido) { tx += dx; ty += dy; tz += dz; }
    }
    void escalar(float ex, float ey, float ez) {
        if (esValido) { sx *= ex; sy *= ey; sz *= ez; }
    }
    void escalarX(float factor) { if (esValido) sx *= factor; }
    void escalarY(float factor) { if (esValido) sy *= factor; }
    void escalarZ(float factor) { if (esValido) sz *= factor; }
    void rotar(float ax, float ay, float az) {
        if (esValido) { rx += ax; ry += ay; rz += az; }
    }

    virtual void setSeleccionada(bool s) { if (esValido) seleccionada = s; }
    bool isValid() const { return esValido; }

    void setTransformaciones(float _sx, float _sy, float _sz, float _rx, float _ry, float _rz) {
        sx = _sx; sy = _sy; sz = _sz;
        rx = _rx; ry = _ry; rz = _rz;
    }
};

// =========================================================================
// CLASE FIGURA 3D (Poliedros Regulares + Cilindro + Esfera)
// =========================================================================
class Figura3D : public Objeto3D {
private:
    int codigoTipo; // 4, 6, 8, 20, 100 (Cilindro), 200 (Esfera)
    int totalCaras;
    float tamano;

    vector<Vec3> vertices;
    vector<Vec3> normalesVertices;
    vector<vector<int>> carasVertices;
    vector<vector<Vec2>> carasUV;
    vector<ColorRGB> coloresCaras;
    vector<int> texturasCaras;

    void addVertice(float x, float y, float z) {
        vertices.push_back(Vec3(x * tamano, y * tamano, z * tamano));
        normalesVertices.push_back(normalizarVec3(Vec3(x, y, z)));
    }

    void asignarUVPorDefectoPoliedros() {
        carasUV.clear();
        carasUV.resize(carasVertices.size());
        for (size_t i = 0; i < carasVertices.size(); i++) {
            size_t n = carasVertices[i].size();
            if (n == 3) {
                carasUV[i] = { Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.5f, 1.0f) };
            } else {
                for (size_t j = 0; j < n; j++) {
                    if (j == 0)      carasUV[i].push_back(Vec2(0.0f, 0.0f));
                    else if (j == 1) carasUV[i].push_back(Vec2(1.0f, 0.0f));
                    else if (j == 2) carasUV[i].push_back(Vec2(1.0f, 1.0f));
                    else             carasUV[i].push_back(Vec2(0.0f, 1.0f));
                }
            }
        }
    }

    void generarCilindro() {
        const int slices = 18;
        float radio = 0.8f;
        float mitadAltura = 1.0f;

        // Vértice central superior (índice 0) e inferior (índice 1)
        addVertice(0.0f,  mitadAltura, 0.0f);
        normalesVertices.back() = Vec3(0.0f, 1.0f, 0.0f);

        addVertice(0.0f, -mitadAltura, 0.0f);
        normalesVertices.back() = Vec3(0.0f, -1.0f, 0.0f);

        // Anillo superior (índices 2 .. 2 + slices - 1)
        for (int i = 0; i < slices; i++) {
            float ang = 2.0f * M_PI * i / slices;
            float x = radio * cos(ang);
            float z = radio * sin(ang);
            addVertice(x, mitadAltura, z);
            normalesVertices.back() = normalizarVec3(Vec3(cos(ang), 0.3f, sin(ang)));
        }

        // Anillo inferior (índices 2 + slices .. 2 + 2*slices - 1)
        for (int i = 0; i < slices; i++) {
            float ang = 2.0f * M_PI * i / slices;
            float x = radio * cos(ang);
            float z = radio * sin(ang);
            addVertice(x, -mitadAltura, z);
            normalesVertices.back() = normalizarVec3(Vec3(cos(ang), -0.3f, sin(ang)));
        }

        // Caras laterales (quads)
        for (int i = 0; i < slices; i++) {
            int next = (i + 1) % slices;
            int top1 = 2 + i;
            int top2 = 2 + next;
            int bot1 = 2 + slices + i;
            int bot2 = 2 + slices + next;

            carasVertices.push_back({ bot1, bot2, top2, top1 });
            float u1 = (float)i / slices;
            float u2 = (float)(i + 1) / slices;
            carasUV.push_back({ Vec2(u1, 0.0f), Vec2(u2, 0.0f), Vec2(u2, 1.0f), Vec2(u1, 1.0f) });
        }

        // Tapa superior
        for (int i = 0; i < slices; i++) {
            int next = (i + 1) % slices;
            float ang1 = 2.0f * M_PI * i / slices;
            float ang2 = 2.0f * M_PI * next / slices;
            carasVertices.push_back({ 0, 2 + next, 2 + i });
            carasUV.push_back({
                Vec2(0.5f, 0.5f),
                Vec2(0.5f + 0.5f * cos(ang2), 0.5f + 0.5f * sin(ang2)),
                Vec2(0.5f + 0.5f * cos(ang1), 0.5f + 0.5f * sin(ang1))
            });
        }

        // Tapa inferior
        for (int i = 0; i < slices; i++) {
            int next = (i + 1) % slices;
            float ang1 = 2.0f * M_PI * i / slices;
            float ang2 = 2.0f * M_PI * next / slices;
            carasVertices.push_back({ 1, 2 + slices + i, 2 + slices + next });
            carasUV.push_back({
                Vec2(0.5f, 0.5f),
                Vec2(0.5f + 0.5f * cos(ang1), 0.5f + 0.5f * sin(ang1)),
                Vec2(0.5f + 0.5f * cos(ang2), 0.5f + 0.5f * sin(ang2))
            });
        }

        totalCaras = (int)carasVertices.size();
    }

    void generarEsfera() {
        const int stacks = 12;
        const int slices = 16;

        for (int i = 0; i <= stacks; i++) {
            float phi = M_PI * i / stacks; // 0 a PI
            float y = cos(phi);
            float r = sin(phi);

            for (int j = 0; j <= slices; j++) {
                float theta = 2.0f * M_PI * j / slices; // 0 a 2*PI
                float x = r * cos(theta);
                float z = r * sin(theta);
                addVertice(x, y, z);
                normalesVertices.back() = normalizarVec3(Vec3(x, y, z));
            }
        }

        for (int i = 0; i < stacks; i++) {
            for (int j = 0; j < slices; j++) {
                int v1 = i * (slices + 1) + j;
                int v2 = v1 + 1;
                int v3 = (i + 1) * (slices + 1) + j + 1;
                int v4 = (i + 1) * (slices + 1) + j;

                carasVertices.push_back({ v1, v4, v3, v2 });

                float u1 = (float)j / slices;
                float u2 = (float)(j + 1) / slices;
                float vt1 = 1.0f - (float)i / stacks;
                float vt2 = 1.0f - (float)(i + 1) / stacks;

                carasUV.push_back({
                    Vec2(u1, vt1),
                    Vec2(u1, vt2),
                    Vec2(u2, vt2),
                    Vec2(u2, vt1)
                });
            }
        }

        totalCaras = (int)carasVertices.size();
    }

    void generarGeometria() {
        vertices.clear();
        normalesVertices.clear();
        carasVertices.clear();
        carasUV.clear();

        switch (codigoTipo) {
            case FIG_TETRAEDRO: {
                float sq2 = sqrt(2.0f);
                addVertice(1, 0, -1 / sq2); addVertice(-1, 0, -1 / sq2);
                addVertice(0, 1,  1 / sq2); addVertice(0, -1,  1 / sq2);
                carasVertices = { {0, 1, 2}, {1, 0, 3}, {0, 2, 3}, {2, 1, 3} };
                totalCaras = 4;
                asignarUVPorDefectoPoliedros();
                break;
            }
            case FIG_CUBO: {
                addVertice(1, 1, 1);  addVertice(-1, 1, 1);  addVertice(-1, -1, 1);  addVertice(1, -1, 1);
                addVertice(1, 1, -1); addVertice(-1, 1, -1); addVertice(-1, -1, -1); addVertice(1, -1, -1);
                carasVertices = {
                    {0, 1, 2, 3}, {5, 4, 7, 6}, {4, 0, 3, 7},
                    {1, 5, 6, 2}, {4, 5, 1, 0}, {3, 2, 6, 7}
                };
                totalCaras = 6;
                asignarUVPorDefectoPoliedros();
                break;
            }
            case FIG_OCTAEDRO: {
                addVertice(1, 0, 0); addVertice(-1, 0, 0);
                addVertice(0, 1, 0); addVertice(0, -1, 0);
                addVertice(0, 0, 1); addVertice(0, 0, -1);
                carasVertices = {
                    {4, 0, 2}, {4, 2, 1}, {4, 1, 3}, {4, 3, 0},
                    {5, 2, 0}, {5, 1, 2}, {5, 3, 1}, {5, 0, 3}
                };
                totalCaras = 8;
                asignarUVPorDefectoPoliedros();
                break;
            }
            case FIG_ICOSAEDRO: {
                float t = (1.0f + sqrt(5.0f)) / 2.0f;
                addVertice(-1,  t, 0); addVertice(1,  t, 0); addVertice(-1, -t, 0); addVertice(1, -t, 0);
                addVertice(0, -1,  t); addVertice(0,  1, t); addVertice(0, -1, -t); addVertice(0,  1, -t);
                addVertice(t, 0, -1);  addVertice(t, 0,  1); addVertice(-t, 0, -1); addVertice(-t, 0,  1);
                carasVertices = {
                    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
                    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
                    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
                    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
                };
                totalCaras = 20;
                asignarUVPorDefectoPoliedros();
                break;
            }
            case FIG_CILINDRO:
                generarCilindro();
                break;
            case FIG_ESFERA:
                generarEsfera();
                break;
        }
    }

    void inicializarColoresYTexturas() {
        coloresCaras.resize(totalCaras);
        texturasCaras.assign(totalCaras, -1);

        if (codigoTipo == FIG_CILINDRO || codigoTipo == FIG_ESFERA) {
            ColorRGB c(
                0.35f + (rand() % 55) / 100.0f,
                0.35f + (rand() % 55) / 100.0f,
                0.35f + (rand() % 55) / 100.0f
            );
            material.colorBase = c;
            for (int i = 0; i < totalCaras; i++) coloresCaras[i] = c;
        } else {
            for (int i = 0; i < totalCaras; i++) {
                coloresCaras[i].r = 0.25f + (rand() % 70) / 100.0f;
                coloresCaras[i].g = 0.25f + (rand() % 70) / 100.0f;
                coloresCaras[i].b = 0.25f + (rand() % 70) / 100.0f;
            }
            if (!coloresCaras.empty()) material.colorBase = coloresCaras[0];
        }
    }

    Vec3 calcularNormalCara(size_t indiceCara) const {
        const vector<int>& c = carasVertices[indiceCara];
        if (c.size() < 3) return Vec3(0, 1, 0);
        Vec3 v1 = vertices[c[0]];
        Vec3 v2 = vertices[c[1]];
        Vec3 v3 = vertices[c[2]];
        Vec3 u(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        Vec3 v(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        return normalizarVec3(productoCruz(u, v));
    }

public:
    Figura3D(int tipoReq, float size, float x, float y, float z) {
        if (tipoReq != FIG_TETRAEDRO && tipoReq != FIG_CUBO &&
            tipoReq != FIG_OCTAEDRO  && tipoReq != FIG_ICOSAEDRO &&
            tipoReq != FIG_CILINDRO  && tipoReq != FIG_ESFERA) {
            esValido = false;
            codigoTipo = 0;
            totalCaras = 0;
            return;
        }
        esValido = true;
        codigoTipo = tipoReq;
        tamano = size;
        tx = x; ty = y; tz = z;
        generarGeometria();
        inicializarColoresYTexturas();
    }

    int getCodigoTipo() const { return codigoTipo; }
    int getTotalCaras() const { return totalCaras; }

    void setDatosCara(int cara, const ColorRGB& col, int texID) {
        if (cara >= 0 && cara < totalCaras) {
            coloresCaras[cara] = col;
            texturasCaras[cara] = texID;
        }
    }

    Objeto3D* clonar() override {
        if (!esValido) return nullptr;
        Figura3D* copia = new Figura3D(codigoTipo, tamano, tx, ty, tz);
        copia->sx = sx; copia->sy = sy; copia->sz = sz;
        copia->rx = rx; copia->ry = ry; copia->rz = rz;
        copia->material = material;
        copia->coloresCaras = coloresCaras;
        copia->texturasCaras = texturasCaras;
        return copia;
    }

    string serializar() override {
        stringstream ss;
        ss << "F " << codigoTipo << " " << tamano << " "
           << tx << " " << ty << " " << tz << " "
           << sx << " " << sy << " " << sz << " "
           << rx << " " << ry << " " << rz << " "
           << material.ka << " " << material.kd << " " << material.ks << " " << material.shininess << " "
           << material.colorBase.r << " " << material.colorBase.g << " " << material.colorBase.b << " "
           << totalCaras;

        for (int i = 0; i < totalCaras; i++) {
            string nombreTex = "NONE";
            int id = texturasCaras[i];
            if (id >= 0 && id < (int)nombresTexturas.size()) {
                nombreTex = nombresTexturas[id];
            }
            ss << " " << coloresCaras[i].r << " " << coloresCaras[i].g << " " << coloresCaras[i].b
               << " " << nombreTex;
        }
        return ss.str();
    }

    void ciclarCaraActiva() override {
        caraActiva++;
        if (caraActiva >= totalCaras) caraActiva = -1;
    }

    void setCaraActiva(int id) override { caraActiva = id; }

    // Si hay una cara activa, aplica la textura a esa cara; si es -1, la aplica a toda la figura
    void setTexturaCaraActiva(int texID) override {
        if (caraActiva >= 0 && caraActiva < totalCaras) {
            texturasCaras[caraActiva] = texID;
        } else {
            for (int i = 0; i < totalCaras; i++) texturasCaras[i] = texID;
        }
    }

    void setColorBaseGlobal(float r, float g, float b) override {
        material.colorBase = ColorRGB(r, g, b);
        if (caraActiva >= 0 && caraActiva < totalCaras) {
            coloresCaras[caraActiva] = ColorRGB(r, g, b);
        } else {
            for (int i = 0; i < totalCaras; i++) {
                coloresCaras[i] = ColorRGB(r, g, b);
            }
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

        // Desactivamos GL_LIGHTING fijo durante el dibujado de caras para aplicar
        // nuestra ecuación paramétrica exacta de Phong vértice por vértice
        glDisable(GL_LIGHTING);

        bool usarSuavizado = (codigoTipo == FIG_CILINDRO || codigoTipo == FIG_ESFERA);

        for (size_t i = 0; i < carasVertices.size(); i++) {
            bool usaTextura = (texturasCaras[i] >= 0 &&
                               texturasCaras[i] < 10 &&
                               texturasGlobales[texturasCaras[i]] != 0);

            if (usaTextura) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texturasGlobales[texturasCaras[i]]);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            } else {
                glDisable(GL_TEXTURE_2D);
            }

            Vec3 normalCaraLocal = calcularNormalCara(i);

            glBegin(GL_POLYGON);
            for (size_t j = 0; j < carasVertices[i].size(); j++) {
                int idx = carasVertices[i][j];
                Vec3 vLocal = vertices[idx];
                Vec3 nLocal = usarSuavizado ? normalesVertices[idx] : normalCaraLocal;

                // Posición y normal del vértice en el espacio del mundo
                Vec3 vEscalado(vLocal.x * sx, vLocal.y * sy, vLocal.z * sz);
                Vec3 vRotado = rotarVectorModelo(vEscalado, rx, ry, rz);
                Vec3 posMundo(vRotado.x + tx, vRotado.y + ty, vRotado.z + tz);
                Vec3 normalMundo = normalizarVec3(rotarVectorModelo(nLocal, rx, ry, rz));

                // Color base de la superficie (blanco si tiene textura para modular con luz Phong)
                ColorRGB colBase = usaTextura ? ColorRGB(1.0f, 1.0f, 1.0f) : coloresCaras[i];
                if (!usaTextura && seleccionada && caraActiva == -1) {
                    colBase.r = min(1.0f, colBase.r + 0.15f);
                    colBase.g = min(1.0f, colBase.g + 0.15f);
                    colBase.b = min(1.0f, colBase.b + 0.15f);
                }

                // Evaluar la función paramétrica de Phong en este vértice
                ColorRGB colorPhong = calcularColorPhongParametrico(posMundo, normalMundo, colBase, material);
                glColor3f(colorPhong.r, colorPhong.g, colorPhong.b);

                if (j < carasUV[i].size()) {
                    glTexCoord2f(carasUV[i][j].u, carasUV[i][j].v);
                }
                glVertex3f(vLocal.x, vLocal.y, vLocal.z);
            }
            glEnd();

            glDisable(GL_TEXTURE_2D);

            // Delineado de aristas y resaltado de selección
            if (seleccionada && caraActiva == (int)i) {
                glLineWidth(4.0f);
                glColor3f(1.0f, 0.1f, 0.1f); // Cara activa en rojo intenso
                glBegin(GL_LINE_LOOP);
                for (int idx : carasVertices[i]) glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
                glEnd();
                glLineWidth(1.0f);
            } else if (!usarSuavizado || seleccionada) {
                glLineWidth(1.0f);
                if (seleccionada && caraActiva == -1) glColor3f(0.9f, 0.9f, 0.2f);
                else glColor3f(0.05f, 0.05f, 0.05f);
                glBegin(GL_LINE_LOOP);
                for (int idx : carasVertices[i]) glVertex3f(vertices[idx].x, vertices[idx].y, vertices[idx].z);
                glEnd();
            }
        }

        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
};

// =========================================================================
// CLASE GRUPO COMPUESTO CON LÍNEAS CONECTORAS INVISIBLES
// =========================================================================
class Grupo3D : public Objeto3D {
private:
    vector<Objeto3D*> componentes;
    vector<LineaConectora> lineasInvisibles; // Líneas que conectan figuras separadas

public:
    Grupo3D(const vector<Objeto3D*>& hijos) {
        esValido = true;
        for (auto h : hijos) componentes.push_back(h);
        generarLineasConectoras();
    }

    ~Grupo3D() {
        for (auto h : componentes) delete h;
    }

    // Crea la red de líneas invisibles que une los centros de las figuras separadas
    void generarLineasConectoras() {
        lineasInvisibles.clear();
        if (componentes.size() < 2) return;
        for (size_t i = 0; i + 1 < componentes.size(); i++) {
            Vec3 p1(componentes[i]->tx, componentes[i]->ty, componentes[i]->tz);
            Vec3 p2(componentes[i + 1]->tx, componentes[i + 1]->ty, componentes[i + 1]->tz);
            lineasInvisibles.push_back(LineaConectora(p1, p2));
        }
    }

    void setLineasConectoras(const vector<LineaConectora>& lineas) {
        lineasInvisibles = lineas;
    }

    Objeto3D* clonar() override {
        vector<Objeto3D*> hijosClonados;
        for (auto h : componentes) hijosClonados.push_back(h->clonar());
        Grupo3D* copia = new Grupo3D(hijosClonados);
        copia->tx = tx; copia->ty = ty; copia->tz = tz;
        copia->sx = sx; copia->sy = sy; copia->sz = sz;
        copia->rx = rx; copia->ry = ry; copia->rz = rz;
        copia->material = material;
        copia->lineasInvisibles = lineasInvisibles;
        return copia;
    }

    string serializar() override {
        generarLineasConectoras();
        stringstream ss;
        ss << "G " << componentes.size() << " "
           << tx << " " << ty << " " << tz << " "
           << sx << " " << sy << " " << sz << " "
           << rx << " " << ry << " " << rz << " "
           << lineasInvisibles.size() << "\n";

        // Guardar las líneas invisibles conectoras que enlazan las figuras separadas
        for (const auto& l : lineasInvisibles) {
            ss << "L " << l.inicio.x << " " << l.inicio.y << " " << l.inicio.z << " "
               << l.fin.x << " " << l.fin.y << " " << l.fin.z << "\n";
        }

        // Guardar cada sub-figura del grupo
        for (auto h : componentes) {
            ss << h->serializar() << "\n";
        }
        return ss.str();
    }

    void setSeleccionada(bool s) override {
        seleccionada = s;
        for (auto h : componentes) h->setSeleccionada(s);
    }

    void ciclarCaraActiva() override {}
    void setCaraActiva(int id) override {}

    void setTexturaCaraActiva(int texID) override {
        for (auto h : componentes) h->setTexturaCaraActiva(texID);
    }

    void setColorBaseGlobal(float r, float g, float b) override {
        material.colorBase = ColorRGB(r, g, b);
        for (auto h : componentes) h->setColorBaseGlobal(r, g, b);
    }

    void setMaterial(const MaterialPhong& m) override {
        material = m;
        for (auto h : componentes) h->setMaterial(m);
    }

    void dibujar() override {
        if (!esValido) return;
        glPushMatrix();
        glTranslatef(tx, ty, tz);
        glRotatef(rx, 1.0f, 0.0f, 0.0f);
        glRotatef(ry, 0.0f, 1.0f, 0.0f);
        glRotatef(rz, 0.0f, 0.0f, 1.0f);
        glScalef(sx, sy, sz);

        // Trazado de las líneas invisibles conectoras que agrupan las figuras separadas
        glDisable(GL_LIGHTING);
        if (!mostrarLineasUnion) {
            // Línea invisible (no escribe en el buffer de color a menos que se active la vista de conectores)
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        } else {
            glLineWidth(2.0f);
            glColor3f(0.0f, 1.0f, 1.0f); // Cian punteado si el usuario activa ver conectores con 'h'
        }

        glBegin(GL_LINES);
        for (const auto& l : lineasInvisibles) {
            glVertex3f(l.inicio.x, l.inicio.y, l.inicio.z);
            glVertex3f(l.fin.x, l.fin.y, l.fin.z);
        }
        glEnd();

        if (!mostrarLineasUnion) {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        } else {
            glLineWidth(1.0f);
        }
        glEnable(GL_LIGHTING);

        // Dibujar todos los componentes del grupo
        for (auto h : componentes) h->dibujar();
        glPopMatrix();
    }
};

#endif // OBJETOS3D_H
