#ifndef RENDERER_DEFINES
#define RENDERER_DEFINES

typedef struct
{
    float x, y, z;
} vertex3d;

typedef struct
{
    int x, y;
} vertex2dI;

typedef struct{
    unsigned char red, green, blue;
} RGB;

typedef struct
{
    int v1, v2, v3;
    RGB color;
} triangle;

typedef struct
{
    const vertex3d *vertices;
    const triangle *triangles;
    int nVertices, nTriangles;
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