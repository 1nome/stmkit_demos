#ifndef RENDERER
#define RENDERER

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "uartDrawing.h"
#include "renderer_defines.h"

const float piFourths = 0.785398163398f;
const float piHalfs = 1.5707963268f;

// function declarations
float vectorCos(vertex3d v1, vertex3d v2);
vertex3d triangleNormal(vertex3d v1, vertex3d v2, vertex3d v3);

// funcs for rendering
float edgeF(vertex3d start, vertex3d end, vertex3d point)
{
    return (point.x - start.x) * (end.y - start.y) - (point.y - start.y) * (end.x - start.x);
}

float edgeFI(vertex3d start, vertex3d end, vertex2dI point)
{
    return (point.x - start.x) * (end.y - start.y) - (point.y - start.y) * (end.x - start.x);
}

vertex3d bbMin(vertex3d v1, vertex3d v2, vertex3d v3)
{
    vertex3d ret;
    ret.x = floorf(fminf(fminf(v1.x, v2.x), v3.x));
    ret.y = floorf(fminf(fminf(v1.y, v2.y), v3.y));
    ret.z = 0;
    return ret;
}

vertex3d bbMax(vertex3d v1, vertex3d v2, vertex3d v3)
{
    vertex3d ret;
    ret.x = ceilf(fmaxf(fmaxf(v1.x, v2.x), v3.x));
    ret.y = ceilf(fmaxf(fmaxf(v1.y, v2.y), v3.y));
    ret.z = 0;
    return ret;
}

vertex2dI bbMinI(vertex3d v1, vertex3d v2, vertex3d v3)
{
    vertex2dI ret;
    ret.x = floorf(fminf(fminf(v1.x, v2.x), v3.x));
    ret.y = floorf(fminf(fminf(v1.y, v2.y), v3.y));
    return ret;
}

vertex2dI bbMaxI(vertex3d v1, vertex3d v2, vertex3d v3)
{
    vertex2dI ret;
    ret.x = ceilf(fmaxf(fmaxf(v1.x, v2.x), v3.x));
    ret.y = ceilf(fmaxf(fmaxf(v1.y, v2.y), v3.y));
    return ret;
}

void drawZBuffer(float *zBuffer, int xRes, int yRes)
{
    float min = infinityf();
    float max = -infinityf();
    for (int i = 0; i < yRes * xRes; i++)
    {
        if (*(int *)(&zBuffer[i]) == 0x7F7F7F7F)
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
        set_grayscale(0xFF - 0xFF * (zBuffer[i] - min) / (max - min));
        move_to(i / xRes, i % xRes + 1);
        draw_pixel();
    }
}

float zBuffer[canvas_height * screenWidth];

// renders triangles into the terminal
// vertices have to be in screen space, else the object is cut away
void rasterize(triangle *triangles, int nTriangles, vertex3d *vertices, int xRes, int yRes)
{
    int zBufferSize = xRes * yRes * sizeof(float);
    memset(zBuffer, 0x7F, zBufferSize);

    for (int i = 0; i < nTriangles; i++)
    {
        vertex3d v1 = vertices[triangles[i].v1];
        vertex3d v2 = vertices[triangles[i].v2];
        vertex3d v3 = vertices[triangles[i].v3];
        float area = edgeF(v1, v2, v3);
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

        // set_color(triangles[i].color);

        vertex2dI pixel;
        for (pixel.x = bbul.x; pixel.x <= bblr.x; pixel.x++)
        {
            for (pixel.y = bbul.y; pixel.y <= bblr.y; pixel.y++)
            {
                // can be simplified to
                // vertex3d pixel = {x, y};
                // without sacrificing much quality
                // vertex3d pixel = {(float)x - 0.5f, (float)y - 0.5f};
                bool draw = 1;
                float w3 = edgeFI(v1, v2, pixel);
                float w1 = edgeFI(v2, v3, pixel);
                float w2 = edgeFI(v3, v1, pixel);
                draw &= w1 >= 0;
                draw &= w2 >= 0;
                draw &= w3 >= 0;
                if (draw)
                {
                    // w1 /= area;
                    // w2 /= area;
                    // w3 /= area;
                    // float z = 1.0f / (v1.z * w1 + v2.z * w2 + v3.z * w3);
                    float z = area / (v1.z * w1 + v2.z * w2 + v3.z * w3);
                    int idx = (pixel.y - 1) * xRes + pixel.x - 1;
                    if (z < zBuffer[idx])
                    {
                        // move_to(pixel.y, pixel.x);
                        // draw_pixel();
                        zBuffer[idx] = z;
                    }
                }
            }
        }
    }
    // drawZBuffer(zBuffer, xRes, yRes);
}

void vertexSwap(vertex3d *a, vertex3d *b)
{
    vertex3d temp = *a;
    *a = *b;
    *b = temp;
}

void intSwap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void sortByY(vertex3d *v1, vertex3d *v2, vertex3d *v3)
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

void segFill(vertex3d base, float width, float height, vertex3d start, vertex3d end, int xRes, int yRes)
{
    float segWidth = end.x - start.x;
    float segHeight = end.y - start.y;
    int minY = start.y < 0 ? 0 : start.y;
    int maxY = end.y > yRes ? yRes : end.y;
    for (int y = minY; y < maxY; y++)
    {
        int x1 = base.x + width * (y - base.y) / height;
        int x2 = start.x + segWidth * (y - start.y) / segHeight;
        if (x1 > x2)
        {
            intSwap(&x1, &x2);
        }
        if (x1 < 0)
        {
            x1 = 0;
        }
        if (x2 > xRes)
        {
            x2 = xRes;
        }
        move_to(y + 1, x1 + 1);
        for (int x = x1; x < x2; x++)
        {
            draw_pixel();
            int idx = y * xRes + x;
            zBuffer[idx] = 1;
        }
    }
}

void rasterize2(triangle *triangles, int nTriangles, vertex3d *vertices, int xRes, int yRes)
{
    int zBufferSize = xRes * yRes * sizeof(float);
    memset(zBuffer, 0x7F, zBufferSize);

    for (int i = 0; i < nTriangles; i++)
    {
        vertex3d v1 = vertices[triangles[i].v1];
        vertex3d v2 = vertices[triangles[i].v2];
        vertex3d v3 = vertices[triangles[i].v3];

        vertex3d normal = triangleNormal(v1, v2, v3);
        if(normal.z > 0){
            continue;
        }
/*
        float area = edgeF(v1, v2, v3);
        if (area < 0)
        {
            continue;
        }
*/
        sortByY(&v1, &v2, &v3);

        //set_color(triangles[i].color);

        float height = v3.y - v1.y;
        float topHeight = v2.y - v1.y;
        float bottomHeight = v3.y - v2.y;

        // bounding box
        int start = v1.y < 0 ? -v1.y : 0;
        int end = v3.y >= yRes ? v3.y - yRes : 0;

        float step = (v3.x - v1.x) / height;
        float topStep = (v2.x - v1.x) / topHeight;
        float bottomStep = (v3.x - v2.x) / bottomHeight;
        float x1f = v1.x + (float)(start - 1) * step;
        float x2f = v1.x + (float)(start - 1) * topStep;

        float depthStep = (v3.z - v1.z) / height;
        float topDepthStep = (v2.z - v1.z) / topHeight;
        float bottomDepthStep = (v3.z - v2.z) / bottomHeight;
        float z1 = v1.z + (float)(start - 1) * depthStep;
        float z2 = v1.z + (float)(start - 1) * topDepthStep;

        bool bottom = false;
        int rowIdx = ((int)v1.y + start) * xRes;

        for (int y = start; y < height - end; y++)
        {
            x1f += step;
            z1 += depthStep;
            if (y > topHeight || !topHeight)
            {
                x2f += bottomStep;
                z2 += bottomDepthStep;
                if(!bottom){
                    x2f = v2.x;
                    z2 = v2.z;
                    bottom = true;
                }
            }
            else
            {
                x2f += topStep;
                z2 += topDepthStep;
            }

            int x1 = x1f;
            int x2 = x2f;
            if (x1 > x2)
            {
                intSwap(&x1, &x2);
            }
            if (x1 < 0)
            {
                x1 = 0;
            }
            if (x2 > xRes)
            {
                x2 = xRes;
            }

            float zStep = (z2 - z1) / (x2 - x1);
            float z = z1;

            //move_to(y + v1.y + 1, x1 + 1);
            for (int x = x1; x <= x2; x++)
            {
                z = (-normal.x * x - normal.y * y - v1.z) / normal.z;
                if(zBuffer[rowIdx + x] > z){
                    //draw_pixel();
                    zBuffer[rowIdx + x] = z;
                }
                z += zStep;
            }

            rowIdx += xRes;
        }
    }
    drawZBuffer(zBuffer, xRes, yRes);
}

// vertex transformations

// uniform 3d vertex coords scaling
void scale3dU(vertex3d *v, float scale)
{
    v->x *= scale;
    v->y *= scale;
    v->z *= scale;
}

// per-axis 3d vertex coords scaling
void scale3d(vertex3d *v, float x, float y, float z)
{
    v->x *= x;
    v->y *= y;
    v->z *= z;
}

// per-axis 3d vertex coords translation
void translate3d(vertex3d *v, float x, float y, float z)
{
    v->x += x;
    v->y += y;
    v->z += z;
}

// 3d vertex coords rotation around x-axis
void rotate3dX(vertex3d *v, float theta)
{
    float temp = v->y;
    v->y = cosf(theta) * v->y - sinf(theta) * v->z;
    v->z = sinf(theta) * temp + cosf(theta) * v->z;
}

void rotate3dX_vec(vertex3d *v, int n, float theta)
{
    float cosine = cosf(theta);
    float sine = sinf(theta);
    for (int i = 0; i < n; i++)
    {
        float temp = v[i].y;
        v[i].y = cosine * v[i].y - sine * v[i].z;
        v[i].z = sine * temp + cosine * v[i].z;
    }
}

// 3d vertex coords rotation around y-axis
void rotate3dY(vertex3d *v, float theta)
{
    float temp = v->x;
    v->x = cosf(theta) * v->x + sinf(theta) * v->z;
    v->z = -sinf(theta) * temp + cosf(theta) * v->z;
}

void rotate3dY_vec(vertex3d *v, int n, float theta)
{
    float cosine = cosf(theta);
    float sine = sinf(theta);
    for (int i = 0; i < n; i++)
    {
        float temp = v[i].x;
        v[i].x = cosine * v[i].x + sine * v[i].z;
        v[i].z = -sine * temp + cosine * v[i].z;
    }
}

// 3d vertex coords rotation around z-axis
void rotate3dZ(vertex3d *v, float theta)
{
    float temp = v->x;
    v->x = cosf(theta) * v->x - sinf(theta) * v->y;
    v->y = sinf(theta) * temp + cosf(theta) * v->y;
}

void rotate3dZ_vec(vertex3d *v, int n, float theta)
{
    float cosine = cosf(theta);
    float sine = sinf(theta);
    for (int i = 0; i < n; i++)
    {
        float temp = v[i].x;
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
float vectorDot(vertex3d v1, vertex3d v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

float vectorLen(vertex3d v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

void vectorNormalize(vertex3d *v)
{
    float len = vectorLen(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

void vectorNegate(vertex3d *v)
{
    v->x *= -1;
    v->y *= -1;
    v->z *= -1;
}

float vectorCos(vertex3d v1, vertex3d v2)
{
    return vectorDot(v1, v2) / (vectorLen(v1) * vectorLen(v2));
}

vertex3d vectorCross(vertex3d v1, vertex3d v2)
{
    vertex3d ret;
    ret.x = v1.y * v2.z - v1.z * v2.y;
    ret.y = v1.z * v2.x - v1.x * v2.z;
    ret.z = v1.x * v2.y - v1.y * v2.x;
    return ret;
}

vertex3d vectorDiff(vertex3d base, vertex3d sub)
{
    base.x -= sub.x;
    base.y -= sub.y;
    base.z -= sub.z;
    return base;
}

vertex3d triangleNormal(vertex3d v1, vertex3d v2, vertex3d v3)
{
    vertex3d ret = vectorDiff(v2, v1);
    vertex3d temp = vectorDiff(v3, v1);
    ret = vectorCross(ret, temp);
    return ret;
}

void calculateLight(triangle *triangle, vertex3d *vertices, vertex3d skyDir)
{
    vertex3d normal = triangleNormal(vertices[triangle->v1], vertices[triangle->v2], vertices[triangle->v3]);
    vectorNormalize(&normal);
    float cos = vectorCos(normal, skyDir);
    // hard-baked value; light = 1
    if (cos < 0)
    {
        cos = 0;
    }
    triangle->color.red *= cos;
    triangle->color.green *= cos;
    triangle->color.blue *= cos;
}

void putObjectToWorld(object3d *obj, vertex3d *vertices, triangle *triangles, int vertexOffset, vertex3d skyDir)
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

bool makeWorld(object3d *objects, int nObjects, vertex3d *vertices, triangle *triangles, int *totalVertices, int *totalTriangles, int maxVertices, int maxTriangles, vertex3d skyDir)
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
void toMonitor(vertex3d *vertices, int nVertices, int x, int y)
{
    for (int i = 0; i < nVertices; i++)
    {
        vertices[i].x = (vertices[i].x + 1.0) / 2.0 * x;
        vertices[i].y = y - (vertices[i].y + 1.0) / 2.0 * y;
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
void multiplyVec(vertex3d *v, matrix *m)
{
    vertex3d t = *v;
    v->x = t.x * m->x11 + t.y * m->x12 + t.z * m->x13 + m->x14;
    v->y = t.x * m->x21 + t.y * m->x22 + t.z * m->x23 + m->x24;
    v->z = t.x * m->x31 + t.y * m->x32 + t.z * m->x33 + m->x34;
}

// converts vertices to camera space
void toCamera(vertex3d *vertices, int nVertices, camera cam)
{
    // r = d x up
    vertex3d d = {cam.targetX, cam.targetY, cam.targetZ};
    vertex3d up = {cam.upX, cam.upY, cam.upZ};
    vertex3d r = vectorCross(d, up);
    // u = r x d
    vertex3d u = vectorCross(r, d);

    // normalize vectors
    vectorNormalize(&d);
    vectorNormalize(&r);
    vectorNormalize(&u);

    vertex3d p = {-cam.posX, -cam.posY, -cam.posZ};

    float dotX = vectorDot(r, p);
    float dotY = vectorDot(u, p);
    vectorNegate(&p);
    float dotZ = vectorDot(d, p);

    matrix m = {r.x, r.y, r.z, dotX, u.x, u.y, u.z, dotY, -cam.targetX, -cam.targetY, -cam.targetZ, dotZ, 0, 0, 0, 1};

    for (int i = 0; i < nVertices; i++)
    {
        multiplyVec(vertices + i, &m);
        vertices[i].x = cam.near * vertices[i].x / vertices[i].z;
        vertices[i].y = cam.near * vertices[i].y / vertices[i].z;
    }
}
#endif
