
#ifndef _CUBE_MESH_H_
#define _CUBE_MESH_H_

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class CubeMesh : protected QOpenGLFunctions
{
public:
    CubeMesh();
    virtual ~CubeMesh();

    void draw(QOpenGLShaderProgram *program);

private:
    void init();

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

#endif // _CUBE_MESH_H_
