#ifndef RENDERER
#define RENDERER

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "tftDrawing2.h"
#include "renderer_defines.h"

const float piFourths = 0.785398163398f;
const float piHalfs = 1.5707963268f;

// function declarations
float vectorCos(vertex3d v1, vertex3d v2);
vertex3d triangleNormal(vertex3d v1, vertex3d v2, vertex3d v3);

// funcs for rendering
float edgeF(const vertex3d start, const vertex3d end, const vertex3d point)
{
    return (point.x - start.x) * (end.y - start.y) - (point.y - start.y) * (end.x - start.x);
}

float edgeFI(const vertex3d start, const vertex3d end, const vertex2dI point)
{
    return ((float)point.x - start.x) * (end.y - start.y) - ((float)point.y - start.y) * (end.x - start.x);
}

vertex3d bbMin(const vertex3d v1, const vertex3d v2, const vertex3d v3)
{
    vertex3d ret;
    ret.x = floorf(fminf(fminf(v1.x, v2.x), v3.x));
    ret.y = floorf(fminf(fminf(v1.y, v2.y), v3.y));
    ret.z = 0;
    return ret;
}

vertex3d bbMax(const vertex3d v1, const vertex3d v2, const vertex3d v3)
{
    vertex3d ret;
    ret.x = ceilf(fmaxf(fmaxf(v1.x, v2.x), v3.x));
    ret.y = ceilf(fmaxf(fmaxf(v1.y, v2.y), v3.y));
    ret.z = 0;
    return ret;
}

vertex2dI bbMinI(const vertex3d v1, const vertex3d v2, const vertex3d v3)
{
    vertex2dI ret;
    ret.x = (int)floorf(fminf(fminf(v1.x, v2.x), v3.x));
    ret.y = (int)floorf(fminf(fminf(v1.y, v2.y), v3.y));
    return ret;
}

vertex2dI bbMaxI(const vertex3d v1, const vertex3d v2, const vertex3d v3)
{
    vertex2dI ret;
    ret.x = (int)ceilf(fmaxf(fmaxf(v1.x, v2.x), v3.x));
    ret.y = (int)ceilf(fmaxf(fmaxf(v1.y, v2.y), v3.y));
    return ret;
}

void drawZBuffer(const float* zBuffer, const int xRes, const int yRes)
{
    float min = infinityf();
    float max = -infinityf();
    for (int i = 0; i < yRes * xRes; i++)
    {
        if (*(int*)(&zBuffer[i]) == 0x7F7F7F7F)
        {
            continue;
        }
        if (zBuffer[i] > max)
        {
            max = zBuffer[i];
        }
        if (zBuffer[i] < min)
        {
            min = zBuffer[i];
        }
    }
    for (int i = 0; i < yRes * xRes; i++)
    {
        if (zBuffer[i] > max)
        {
            continue;
        }
        set_grayscale((uint8_t)(0xFF - 0xFF * (zBuffer[i] - min) / (max - min)));
        draw_pixel(i % xRes + 1, i / xRes);
    }
}

float zBuffer[canvas_height * screenWidth];

// renders triangles into the terminal
// vertices have to be in screen space, else the object is cut away
void rasterize(const triangle* triangles, const int nTriangles, const vertex3d* vertices, const int xRes,
               const int yRes)
{
    const int zBufferSize = xRes * yRes * (int)sizeof(float);
    memset(zBuffer, 0x7F, zBufferSize);

    for (int i = 0; i < nTriangles; i++)
    {
        const vertex3d v1 = vertices[triangles[i].v1];
        const vertex3d v2 = vertices[triangles[i].v2];
        const vertex3d v3 = vertices[triangles[i].v3];
        const float area = edgeF(v1, v2, v3);
        if (area < 0)
        {
            continue;
        }

        vertex2dI bbul = bbMinI(v1, v2, v3);
        if (bbul.x < 1)
        {
            bbul.x = 1;
        }
        if (bbul.y < 1)
        {
            bbul.y = 1;
        }
        vertex2dI bblr = bbMaxI(v1, v2, v3);
        if (bblr.x > xRes)
        {
            bblr.x = xRes;
        }
        if (bblr.y > yRes)
        {
            bblr.y = yRes;
        }

        set_color(triangles[i].color);

        vertex2dI pixel;
        for (pixel.x = bbul.x; pixel.x <= bblr.x; pixel.x++)
        {
            for (pixel.y = bbul.y; pixel.y <= bblr.y; pixel.y++)
            {
                // vertex3d pixel = {(float)x - 0.5f, (float)y - 0.5f};
                const float w3 = edgeFI(v1, v2, pixel);
                const float w1 = edgeFI(v2, v3, pixel);
                const float w2 = edgeFI(v3, v1, pixel);
                if (w1 >= 0 && w2 >= 0 && w3 >= 0)
                {
                    // w1 /= area;
                    // w2 /= area;
                    // w3 /= area;
                    // float z = 1.0f / (v1.z * w1 + v2.z * w2 + v3.z * w3);
                    const float z = area / (v1.z * w1 + v2.z * w2 + v3.z * w3);
                    const int idx = (pixel.y - 1) * xRes + pixel.x - 1;
                    if (z < zBuffer[idx])
                    {
                        draw_pixel(pixel.x, pixel.y);
                        zBuffer[idx] = z;
                    }
                }
            }
        }
    }
    // drawZBuffer(zBuffer, xRes, yRes);
}

void vertexSwap(vertex3d* a, vertex3d* b)
{
    const vertex3d temp = *a;
    *a = *b;
    *b = temp;
}

void intSwap(int* a, int* b)
{
    const int temp = *a;
    *a = *b;
    *b = temp;
}

void floatSwap(float* a, float* b)
{
    const float temp = *a;
    *a = *b;
    *b = temp;
}

void sortByY(vertex3d* v1, vertex3d* v2, vertex3d* v3)
{
    if (v1->y > v2->y)
    {
        vertexSwap(v1, v2);
    }
    if (v1->y > v3->y)
    {
        vertexSwap(v1, v3);
    }
    if (v2->y > v3->y)
    {
        vertexSwap(v2, v3);
    }
}

void triangle_rasterize(const vertex3d v1, const vertex3d v2, const vertex3d v3, const int xRes, const int yRes)
{
    bool bottom = false;

    const float height = v3.y - v1.y;
    if (height == 0)
    {
        return;
    }
    const float topHeight = v2.y - v1.y;
    const float bottomHeight = v3.y - v2.y;

    const float step = (v3.x - v1.x) / height;
    const float topStep = (v2.x - v1.x) / topHeight;
    const float bottomStep = (v3.x - v2.x) / bottomHeight;

    const float depthStep = (v3.z - v1.z) / height;
    const float topDepthStep = (v2.z - v1.z) / topHeight;
    const float bottomDepthStep = (v3.z - v2.z) / bottomHeight;

    const float y1 = v1.y < 0 ? 0 : v1.y;
    const float y2 = v3.y > (float)yRes ? (float)yRes : v3.y;
    float y = roundf(y1) + 0.5f;
    int iy = (int)roundf(y1);
    while (y <= y2)
    {
        if (y >= v2.y)
        {
            bottom = true;
        }
        float x1 = v1.x + (y - v1.y) * step;
        float x2 = bottom ? v2.x + (y - v2.y) * bottomStep : v1.x + (y - v1.y) * topStep;
        float z1 = v1.z + (y - v1.y) * depthStep;
        float z2 = bottom ? v2.z + (y - v2.y) * bottomDepthStep : v1.z + (y - v1.y) * topDepthStep;

        if (x1 > x2)
        {
            floatSwap(&x1, &x2);
            floatSwap(&z1, &z2);
        }
        if (x1 < 0)
        {
            x1 = 0;
        }
        if (x2 > (float)xRes)
        {
            x2 = (float)xRes;
        }

        const float zStep = (z2 - z1) / (x2 - x1);

        // float z = (-normal.x * x - normal.y * y - v1.z) / normal.z * 10;
        float x = roundf(x1) + 0.5f;
        int ix = (int)roundf(x1);
        while (x <= x2)
        {
            const float z = 1.0f / (z1 + (x - x1) * zStep);
            if (z < zBuffer[ix + iy * xRes])
            {
                draw_pixel(ix, iy);
                zBuffer[ix + iy * xRes] = z;
            }
            x += 1;
            ix++;
        }

        y += 1;
        iy++;
    }
}

void rasterize2(const triangle* triangles, const int nTriangles, const vertex3d* vertices, const int xRes,
                const int yRes)
{
    const int zBufferSize = xRes * yRes * (int)sizeof(float);
    memset(zBuffer, 0x7F, zBufferSize);

    for (int i = 0; i < nTriangles; i++)
    {
        vertex3d v1 = vertices[triangles[i].v1];
        vertex3d v2 = vertices[triangles[i].v2];
        vertex3d v3 = vertices[triangles[i].v3];

        // const vertex3d normal = triangleNormal(v1, v2, v3);
        // if(normal.z > 0){
        //     continue;
        // }


        const float area = edgeF(v1, v2, v3);
        if (area < 0)
        {
            continue;
        }


        set_color(triangles[i].color);
        sortByY(&v1, &v2, &v3);

        triangle_rasterize(v1, v2, v3, xRes, yRes);
    }
    flush();
    // drawZBuffer(zBuffer, xRes, yRes);
}


// vertex transformations

// uniform 3d vertex coords scaling
void scale3dU(vertex3d* v, const float scale)
{
    v->x *= scale;
    v->y *= scale;
    v->z *= scale;
}

// per-axis 3d vertex coords scaling
void scale3d(vertex3d* v, const float x, const float y, const float z)
{
    v->x *= x;
    v->y *= y;
    v->z *= z;
}

// per-axis 3d vertex coords translation
void translate3d(vertex3d* v, const float x, const float y, const float z)
{
    v->x += x;
    v->y += y;
    v->z += z;
}

// 3d vertex coords rotation around x-axis
void rotate3dX(vertex3d* v, const float theta)
{
    const float temp = v->y;
    v->y = cosf(theta) * v->y - sinf(theta) * v->z;
    v->z = sinf(theta) * temp + cosf(theta) * v->z;
}

void rotate3dX_vec(vertex3d* v, const int n, const float theta)
{
    const float cosine = cosf(theta);
    const float sine = sinf(theta);
    for (int i = 0; i < n; i++)
    {
        const float temp = v[i].y;
        v[i].y = cosine * v[i].y - sine * v[i].z;
        v[i].z = sine * temp + cosine * v[i].z;
    }
}

// 3d vertex coords rotation around y-axis
void rotate3dY(vertex3d* v, const float theta)
{
    const float temp = v->x;
    v->x = cosf(theta) * v->x + sinf(theta) * v->z;
    v->z = -sinf(theta) * temp + cosf(theta) * v->z;
}

void rotate3dY_vec(vertex3d* v, const int n, const float theta)
{
    const float cosine = cosf(theta);
    const float sine = sinf(theta);
    for (int i = 0; i < n; i++)
    {
        const float temp = v[i].x;
        v[i].x = cosine * v[i].x + sine * v[i].z;
        v[i].z = -sine * temp + cosine * v[i].z;
    }
}

// 3d vertex coords rotation around z-axis
void rotate3dZ(vertex3d* v, const float theta)
{
    const float temp = v->x;
    v->x = cosf(theta) * v->x - sinf(theta) * v->y;
    v->y = sinf(theta) * temp + cosf(theta) * v->y;
}

void rotate3dZ_vec(vertex3d* v, const int n, const float theta)
{
    const float cosine = cosf(theta);
    const float sine = sinf(theta);
    for (int i = 0; i < n; i++)
    {
        const float temp = v[i].x;
        v[i].x = cosine * v[i].x - sine * v[i].y;
        v[i].y = sine * temp + cosine * v[i].y;
    }
}

// working with objects
/*
void objectAllocateMesh(object3d *obj, vertex3d *vertices, triangle *triangles, int nVertices, int nTriangles)
{
    obj->vertices = calloc(nVertices, sizeof(vertex3d));
    obj->nVertices = nVertices;
    memcpy(obj->vertices, vertices, nVertices * sizeof(vertex3d));
    obj->triangles = calloc(nTriangles, sizeof(triangle));
    obj->nTriangles = nTriangles;
    memcpy(obj->triangles, triangles, nTriangles * sizeof(triangle));
}

void objectFreeMesh(object3d *obj)
{
    free(obj->vertices);
    free(obj->triangles);
    obj->nTriangles = 0;
    obj->nVertices = 0;
    obj->vertices = NULL;
    obj->triangles = NULL;
}

void objectCopy(object3d *obj, const object3d *src)
{
    *obj = *src;
    objectAllocateMesh(obj, src->vertices, src->triangles, src->nVertices, src->nTriangles);
}


// funcs to make the main loop more readable

void objectArrFreeMesh(object3d *obj, int nObj)
{
    for (int i = 0; i < nObj; i++)
    {
        objectFreeMesh(obj + i);
    }
}
*/

// operations with vertices as if they were vectors (they are)
float vectorDot(const vertex3d v1, const vertex3d v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

float vectorLen(const vertex3d v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

void vectorNormalize(vertex3d* v)
{
    const float len = vectorLen(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

void vectorNegate(vertex3d* v)
{
    v->x *= -1;
    v->y *= -1;
    v->z *= -1;
}

float vectorCos(const vertex3d v1, const vertex3d v2)
{
    return vectorDot(v1, v2) / (vectorLen(v1) * vectorLen(v2));
}

vertex3d vectorCross(const vertex3d v1, const vertex3d v2)
{
    vertex3d ret;
    ret.x = v1.y * v2.z - v1.z * v2.y;
    ret.y = v1.z * v2.x - v1.x * v2.z;
    ret.z = v1.x * v2.y - v1.y * v2.x;
    return ret;
}

vertex3d vectorDiff(vertex3d base, const vertex3d sub)
{
    base.x -= sub.x;
    base.y -= sub.y;
    base.z -= sub.z;
    return base;
}

vertex3d triangleNormal(const vertex3d v1, const vertex3d v2, const vertex3d v3)
{
    vertex3d ret = vectorDiff(v2, v1);
    const vertex3d temp = vectorDiff(v3, v1);
    ret = vectorCross(ret, temp);
    return ret;
}

void calculateLight(triangle* triangle, const vertex3d* vertices, const vertex3d skyDir)
{
    vertex3d normal = triangleNormal(vertices[triangle->v1], vertices[triangle->v2], vertices[triangle->v3]);
    vectorNormalize(&normal);
    float cos = vectorCos(normal, skyDir);
    // hard-baked value; light = 1
    if (cos < 0)
    {
        cos = 0;
    }
    triangle->color.red = (uint8_t)((float)triangle->color.red * cos);
    triangle->color.green = (uint8_t)((float)triangle->color.green * cos);
    triangle->color.blue = (uint8_t)((float)triangle->color.blue * cos);
}

void putObjectToWorld(const object3d* obj, vertex3d* vertices, triangle* triangles, const int vertexOffset,
                      const vertex3d skyDir)
{
    // int start = Ticks;
    memcpy(vertices, obj->vertices, obj->nVertices * sizeof(vertex3d));
    memcpy(triangles, obj->triangles, obj->nTriangles * sizeof(triangle));
    // int copied = Ticks;

    rotate3dX_vec(vertices, obj->nVertices, obj->rotX);
    rotate3dY_vec(vertices, obj->nVertices, obj->rotY);
    rotate3dZ_vec(vertices, obj->nVertices, obj->rotZ);

    for (int i = 0; i < obj->nVertices; i++)
    {
        scale3d(vertices + i, obj->scaleX, obj->scaleY, obj->scaleZ);
        translate3d(vertices + i, obj->posX, obj->posY, obj->posZ);
    }
    // int moved = Ticks;

    for (int i = 0; i < obj->nTriangles; i++)
    {
        calculateLight(triangles + i, vertices, skyDir);
        triangles[i].v1 += vertexOffset;
        triangles[i].v2 += vertexOffset;
        triangles[i].v3 += vertexOffset;
    }
    /*
    int lit = Ticks;
    char str[50];
    snprintf(str, 50, "Copied in %dms\n\r", (copied - start) / 10);
    USART3_write(str, strlen(str));
    snprintf(str, 50, "Moved in %dms\n\r", (moved - copied) / 10);
    USART3_write(str, strlen(str));
    snprintf(str, 50, "Lit in %dms\n\r", (lit - moved) / 10);
    USART3_write(str, strlen(str));
    */
}

bool makeWorld(const object3d* objects, const int nObjects, vertex3d* vertices, triangle* triangles, int* totalVertices,
               int* totalTriangles, const int maxVertices, const int maxTriangles, const vertex3d skyDir)
{
    *totalTriangles = 0;
    *totalVertices = 0;
    for (int i = 0; i < nObjects; i++)
    {
        if ((objects + i)->nTriangles + *totalTriangles > maxTriangles)
        {
            return false;
        }
        if ((objects + i)->nVertices + *totalVertices > maxVertices)
        {
            return false;
        }
        putObjectToWorld(objects + i, vertices + *totalVertices, triangles + *totalTriangles, *totalVertices, skyDir);
        *totalVertices += (objects + i)->nVertices;
        *totalTriangles += (objects + i)->nTriangles;
    }
    return true;
}

// move vertices to screen space
void toMonitor(vertex3d* vertices, const int nVertices, const int x, const int y)
{
    for (int i = 0; i < nVertices; i++)
    {
        vertices[i].x = (vertices[i].x + 1.0f) / 2.0f * (float)x;
        vertices[i].y = (float)y - (vertices[i].y + 1.0f) / 2.0f * (float)y;
    }
}

typedef struct
{
    float x11, x12, x13, x14;
    float x21, x22, x23, x24;
    float x31, x32, x33, x34;
    float x41, x42, x43, x44;
} matrix;

// multiplies a vector (vertex) by a matrix
void multiplyVec(vertex3d* v, const matrix* m)
{
    const vertex3d t = *v;
    v->x = t.x * m->x11 + t.y * m->x12 + t.z * m->x13 + m->x14;
    v->y = t.x * m->x21 + t.y * m->x22 + t.z * m->x23 + m->x24;
    v->z = t.x * m->x31 + t.y * m->x32 + t.z * m->x33 + m->x34;
}

// converts vertices to camera space
void toCamera(vertex3d* vertices, const int nVertices, const camera cam)
{
    // r = d x up
    vertex3d d = {cam.targetX, cam.targetY, cam.targetZ};
    const vertex3d up = {cam.upX, cam.upY, cam.upZ};
    vertex3d r = vectorCross(d, up);
    // u = r x d
    vertex3d u = vectorCross(r, d);

    // normalize vectors
    vectorNormalize(&d);
    vectorNormalize(&r);
    vectorNormalize(&u);

    vertex3d p = {-cam.posX, -cam.posY, -cam.posZ};

    const float dotX = vectorDot(r, p);
    const float dotY = vectorDot(u, p);
    vectorNegate(&p);
    const float dotZ = vectorDot(d, p);

    const matrix m = {
        r.x, r.y, r.z, dotX, u.x, u.y, u.z, dotY, -cam.targetX, -cam.targetY, -cam.targetZ, dotZ, 0, 0, 0, 1
    };

    for (int i = 0; i < nVertices; i++)
    {
        multiplyVec(vertices + i, &m);
        vertices[i].x = cam.near * vertices[i].x / vertices[i].z;
        vertices[i].y = cam.near * vertices[i].y / vertices[i].z;
    }
}
#endif
