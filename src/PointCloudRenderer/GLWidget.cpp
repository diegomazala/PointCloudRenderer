
#include "GLWidget.h"
#include <QMouseEvent>
#include <QCoreApplication>
#include <QMessageBox>
#include <math.h>




GLWidget::GLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

GLWidget::~GLWidget()
{
	cleanup();
}

void GLWidget::loadMesh(QString fileName)
{
	if (!mesh.load(fileName.toStdString()))
	{
		QMessageBox::critical(this, tr("Point Cloud Renderer"),
			QString("Could not load mesh file.\n<" + fileName + ">"),
			QMessageBox::Ok);
	}
	else
	{
		QMessageBox::warning(this, tr("Point Cloud Renderer"),
			QString("Loaded mesh file.\n<" + fileName + ">"),
			QMessageBox::Ok);
	}
}


void GLWidget::cleanup()
{
	makeCurrent();
	m_vbo.destroy();
	doneCurrent();
}


void GLWidget::initializeGL()
{
	this->initializeOpenGLFunctions();

	// get context opengl-version
	qDebug() << "Widget OpenGl: " << format().majorVersion() << "." << format().minorVersion();
	qDebug() << "Context valid: " << context()->isValid();
	qDebug() << "Really used OpenGl: " << context()->format().majorVersion() << "." << context()->format().minorVersion();
	qDebug() << "OpenGl information: VENDOR:       " << (const char*)glGetString(GL_VENDOR);
	qDebug() << "                    RENDERDER:    " << (const char*)glGetString(GL_RENDERER);
	qDebug() << "                    VERSION:      " << (const char*)glGetString(GL_VERSION);
	qDebug() << "                    GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CW for clock-wise

	initShaders();
	initTextures();

	mesh.init();
}



void GLWidget::initShaders()
{
	QString vertexShaderFile = "../../../shaders/vshader.glsl";
	QString fragmentShaderFile = "../../../shaders/fshader.glsl";


	// Compile vertex shader
	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile))
		close();

	// Compile fragment shader
	if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile))
		close();

	// Link shader pipeline
	if (!program.link())
		close();

	// Bind shader pipeline for use
	if (!program.bind())
		close();
}



void GLWidget::initTextures()
{
	QString imageFile = "../../../data/cube.png";

	// Load cube.png image
	texture = new QOpenGLTexture(QImage(imageFile).mirrored());

	// Set nearest filtering mode for texture minification
	texture->setMinificationFilter(QOpenGLTexture::Nearest);

	// Set bilinear filtering mode for texture magnification
	texture->setMagnificationFilter(QOpenGLTexture::Linear);

	// Wrap texture coordinates by repeating
	// f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
	texture->setWrapMode(QOpenGLTexture::Repeat);
}


void GLWidget::paintGL()
{
	// Clear color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	texture->bind();

	QMatrix4x4 cameraView;
	cameraView.lookAt(QVector3D(3.0, 2.0, -5.0), QVector3D(0.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0));

	// Calculate model view transformation
	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();

	// Set modelview-projection matrix
	program.setUniformValue("mvp", projection * cameraView * modelMatrix);

	// Use texture unit 0 which contains cube.png
	program.setUniformValue("texture", 0);

	// Draw cube geometry
	mesh.draw(&program);
}


void GLWidget::resizeGL(int w, int h)
{
	// Calculate aspect ratio
	qreal aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 0.1, far plane to 32.0, field of view 45 degrees
	const qreal zNear = 0.1, zFar = 32.0, fov = 45.0;

	// Reset projection
	projection.setToIdentity();

	// Set perspective projection
	projection.perspective(fov, aspect, zNear, zFar);
}



void GLWidget::mousePressEvent(QMouseEvent *event)
{
}



void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
}
