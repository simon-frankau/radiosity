////////////////////////////////////////////////////////////////////////
//
// rendering.cpp: Put the calculated polys on the screen
//
// Copyright (c) Simon Frankau 2018
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>

#include <png.h>

#include "geom.h"

static const int WIDTH = 512;
static const int HEIGHT = 512;

static std::vector<Quad> faces;
static std::vector<Vertex> vertices;

static void screenshotPNG(const char *filename)
{
    const size_t format_nchannels = 4;
    size_t nvals = format_nchannels * WIDTH * HEIGHT;

    // Assuming a GLubyte is a png_byte...
    std::vector<png_byte> pixels(nvals);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    std::vector<png_byte *> png_rows(HEIGHT);
    for (size_t i = 0; i < HEIGHT; i++) {
        png_rows[HEIGHT - i - 1] = &pixels[i * WIDTH * format_nchannels];
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                              NULL, NULL, NULL);
    if (png == NULL) {
        throw std::runtime_error("png_create_write_struct failed");
    }

    png_infop info = png_create_info_struct(png);
    if (info == NULL) {
        throw std::runtime_error("png_create_info_struct failed");
    }

    if (setjmp(png_jmpbuf(png))) {
        throw std::runtime_error("Something failed in libpng");
    }

    FILE *f = fopen(filename, "wb");
    png_init_io(png, f);
    png_set_IHDR(png, info, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
    png_write_image(png, &png_rows[0]);
    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(f);
}

static void drawScene(void)
{
    for (std::vector<Quad>::const_iterator iter = faces.begin(),
             end = faces.end(); iter != end; ++iter) {
        iter->render(vertices);
    }
}

static void display(void)
{
    static bool first = true;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    if (first) {
        screenshotPNG("png/scene.png");
        first = false;
    }
    glutSwapBuffers();
}

static void initGL(void)
{
    // Flat shading.
    glEnable(GL_COLOR_MATERIAL);
    // Use depth buffering for hidden surface elimination.
    glEnable(GL_DEPTH_TEST);
    // Back-face culling.
    glEnable(GL_CULL_FACE);

    // Setup the view of the cube. Will become a view from inside the
    // cube.
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0,  // Field of view in degrees
                   1.0,   // Aspect ratio
                   1.0,   // Z near
                   10.0); // Z far
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0.0, 0.0, -3.0, // Eye position
              0.0, 0.0,  0.0, // Looking at
              0.0, 1.0,  0.); // Up is in positive Y direction
}

void render(std::vector<Quad> f, std::vector<Vertex> v)
{
    faces = f;
    vertices = v;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Radiosity demo");
    glutDisplayFunc(display);
    initGL();
    glutMainLoop();
}
