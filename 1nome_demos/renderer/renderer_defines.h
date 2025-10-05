#ifndef RENDERER_DEFINES
#define RENDERER_DEFINES

typedef struct
{
    float x, y, z;
} vertex3d;

typedef struct
{
    float x, y;
} vertex2d;

typedef struct
{
    int x, y;
} vertex2dI;

typedef struct{
    unsigned char red, green, blue;
} RGB;

typedef struct
{
    uint16_t v1, v2, v3;
    uint8_t mat;
} triangle;

typedef struct
{
    uint16_t v1, v2, v3;
    uint16_t n1, n2, n3;
    uint16_t t1, t2, t3;
} triangle3;

typedef struct
{
    float se, tr, ni;
    RGB a, d, s;
    uint8_t illum;
} material;

typedef struct
{
    const vertex3d *vertices;
    const triangle *triangles;
    const material *materials;
    int nVertices, nTriangles, nMaterials;
    float posX, posY, posZ;
    float scaleX, scaleY, scaleZ;
    float rotX, rotY, rotZ;
} object3d;

typedef struct
{
    float posX, posY, posZ;
    float targetX, targetY, targetZ;
    float upX, upY, upZ;
    float near;
} camera;

#endif