#include <iostream>
#include <cmath>
#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"

using namespace std;

#define PI 3.14159265358

struct VertexAttribute {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat texcoord[2];
};

GLsizei width = 600, height = 500;

GLuint program;

// VBO
GLuint VBO;

// maps
GLuint earthTex;

// total vertices
const int vertices_length = 65341;

void init();
void reshape( GLsizei _w, GLsizei _h );
void display();
void idle();
void initTextures();
void initShaders();
void loadBuffer();
VertexAttribute *drawEarth();

int main( int argc, char *argv[] ) {

    glutInit( &argc, argv );

    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowPosition( 100, 100 );
    glutInitWindowSize( width, height );
    glutCreateWindow( "ICG Hw3 0513328" );

    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutIdleFunc( idle );

    glutMainLoop();

    return 0;
}

void init() {
    glewInit();
    initTextures();
    initShaders();
    loadBuffer();
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
}

void reshape( GLsizei _w, GLsizei _h ) {
    width = width < 1 ? 1 : _w;
    height = height < 1 ? 1 : _h;
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45, (double)width / (double)height, 0.1, 1000 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void display() {
    //Clear the buffer
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // viewport transformation
    glViewport( 0, 0, width, height );
    // projection transformation
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 45, (double)width / (double)height, 0.1, 1000 );
    // viewing and modeling transformation
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluLookAt( 0.0f, 0.0f, 3.0f,// eye
               0.0f, 0.0f, 0.0f,// center
               0.0f, 1.0f, 0.0f );// up

    GLint texLoc = glGetUniformLocation( program, "Texture" );
    GLfloat pmtx[16];
    GLfloat mmtx[16];
    glGetFloatv( GL_PROJECTION_MATRIX, pmtx );
    glGetFloatv( GL_MODELVIEW_MATRIX, mmtx );
    GLint pmatLoc = glGetUniformLocation( program, "Projection" );
    GLint mmatLoc = glGetUniformLocation( program, "ModelView" );

    // make earth
    glUseProgram( program );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, earthTex );
    glUniform1i( texLoc, 0 );
    glUniformMatrix4fv( pmatLoc, 1, GL_FALSE, pmtx );
    glUniformMatrix4fv( mmatLoc, 1, GL_FALSE, mmtx );
    glDrawArrays( GL_TRIANGLES, 0, vertices_length );
    glBindTexture( GL_TEXTURE_2D, NULL );

    glUseProgram( 0 );

    glPopMatrix();
    glutSwapBuffers();
}

void idle() {
    glutPostRedisplay();
}

void initTextures() {
    // bind earth texture map
    glGenTextures( 1, &earthTex );
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename( "earth_texture_map.jpg" );
    FIBITMAP *pImage = FreeImage_Load( fif, "earth_texture_map.jpg", 0 );
    FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits( pImage );
    FreeImage_Unload( pImage );
    int iWidth = FreeImage_GetWidth( p32BitsImage );
    int iHeight = FreeImage_GetHeight( p32BitsImage );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, earthTex );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA,
                  GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits( p32BitsImage ) );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void initShaders() {
    GLuint vert = createShader( "Shaders/hw3.vert", "vertex" );
    GLuint geom = createShader( "Shaders/hw3.geom", "geometry" );
    GLuint frag = createShader( "Shaders/hw3.frag", "fragment" );
    program = createProgram( vert, geom, frag );
}

void loadBuffer() {
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );

    glEnableVertexAttribArray( 0 );     // position
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( VertexAttribute ), (void*)(offsetof( VertexAttribute, position )) );

    glEnableVertexAttribArray( 1 );     // normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( VertexAttribute ), (void*)(offsetof( VertexAttribute, normal )) );

    glEnableVertexAttribArray( 2 );     // texture coordinate(u,v)
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( VertexAttribute ), (void*)(offsetof( VertexAttribute, texcoord )) );
    VertexAttribute *vertices = drawEarth();
    glBufferData( GL_ARRAY_BUFFER, sizeof( VertexAttribute ) * vertices_length, vertices, GL_STATIC_DRAW );
}

VertexAttribute *drawEarth() {
    VertexAttribute *vertices;
    vertices = new VertexAttribute[vertices_length];

    int radius = 1;
    int slice = 5;
    int stack = 3;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / slice;
    float stackStep = PI / stack;
    float sectorAngle, stackAngle;

    int k = 0;
    for (int i = 0; i <= stack; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf( stackAngle );             // r * cos(u)
        z = radius * sinf( stackAngle );              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= slice; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf( sectorAngle );             // r * cos(u) * cos(v)
            y = xy * sinf( sectorAngle );             // r * cos(u) * sin(v)
            vertices[k].position[0] = x;
            vertices[k].position[1] = y;
            vertices[k].position[2] = z;

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertices[k].normal[0] = nx;
            vertices[k].normal[1] = ny;
            vertices[k].normal[2] = nz;

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / slice;
            t = (float)i / stack;
            vertices[k].texcoord[0] = s;
            vertices[k].texcoord[1] = t;
            k++;
        }
    }

    return vertices;
}