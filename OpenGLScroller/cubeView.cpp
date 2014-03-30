/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cubeView.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QTime>

#include <qmath.h>

//! [7]
CubeView::CubeView()
    : m_program(0)
    , m_t(0)
    , m_thread_t(0)
    , minimums(0.1f, 0.1f, 0.1f, -1.0f, -1.0f, -1.0f, -0.5f, 0.0f)
    , maximums(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 3.14f * 2)
    , deltas(0.02f, 0.018f, 0.016f, 0.01f, 0.02f, 0.03f, 0.01f, 0.0f)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}
//! [7]

//! [8]
void CubeView::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}
//! [8]

void CubeView::setScrollOffsetX(qreal scrollOffsetX)
{
    if (scrollOffsetX == m_scrollOffsetX)
        return;
    m_scrollOffsetX = scrollOffsetX;
    emit scrollOffsetXChanged();
    if (window())
        window()->update();
}

//! [1]
void CubeView::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
//! [1]
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
//! [2]
        connect(win, SIGNAL(beforeRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
//! [2]

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
//! [3]
        win->setClearBeforeRendering(false);
    }
}

//! [3]

void CubeView::handleLittleCubeTap(int x)
{
    int index = qFloor((x)/LittleCubeWidth);
    littleCube[index] = bigCube;
}

//! //! [4]
void CubeView::paint()
{
    if (!m_program) {
        GLfloat gCubeVertexData[] =
        {
            // Data layout for each line below is:
            // positionX, positionY, positionZ,     normalX, normalY, normalZ,
            0.5f, -0.5f, -0.5f,        1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,         1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,         1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,          1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,          0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,        0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,         0.0f, 1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f,        -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,         -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,       -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,        -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,       0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,        0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,        0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,         0.0f, -1.0f, 0.0f,

            0.5f, 0.5f, 0.5f,          0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,         0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,        0.0f, 0.0f, 1.0f,

            0.5f, -0.5f, -0.5f,        0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f,         0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,       0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f,        0.0f, 0.0f, -1.0f
        };

        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 posAttr;\n"
                                           "uniform lowp vec4 colAttr;\n"
                                           "varying lowp vec4 col;\n"
                                           "uniform highp mat4 matrix;\n"
                                           "void main() {\n"
                                           "   col = colAttr;\n"
                                           "   gl_Position = matrix * posAttr;\n"
                                           "}\n");
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "varying lowp vec4 col;\n"
                                           "void main() {\n"
                                           "   gl_FragColor = col;\n"
                                           "}\n");
        m_program->link();
        m_posAttr = m_program->attributeLocation("posAttr");
        m_colAttr = m_program->uniformLocation("colAttr");
       // GLuint normAttr = m_program->attributeLocation("normAttr");
        m_matrixUniform = m_program->uniformLocation("matrix");

    //http://www.kdab.com/opengl-in-qt-5-1-part-2/
    //http://stackoverflow.com/questions/20634627/usage-of-qopenglvertexarrayobject
        vertexArray = new QOpenGLVertexArrayObject();//this);
        vertexArray->create();
        vertexArray->bind();

        vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        vertexBuffer->create();
        vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vertexBuffer->bind();

        vertexBuffer->allocate(gCubeVertexData, (12*18 * sizeof (GLfloat)));

        m_program->enableAttributeArray(m_posAttr);
        m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3, 24);

        vertexArray->release();

        qsrand(1);
        randomizeBigCube ();

        for (int i = 0; i< NumLittleCubes; i++)
            littleCube[i] = bigCube;

        connect(window()->openglContext(), SIGNAL(aboutToBeDestroyed()),
                this, SLOT(cleanup()), Qt::DirectConnection);
    }
//! [4] //! [5]
    UpdateCubes ();

    m_program->bind();

    qreal ratio = window()->devicePixelRatio();
    int w = int(ratio * window()->width());
    int h = int(ratio * window()->height());
    glViewport(0, 0, w, h);

            //glEnable(GL_DEPTH_TEST);

    glClearColor (0.15f, 0.15f, 0.15f, 1);
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT

    for (int i = 0; i < NumLittleCubes; i++) {
        const CubeInfo& cube = littleCube [i];

        float translationX = ((i - 1) * UnitLittleCubeWidth) - (m_scrollOffsetX * UnitLittleCubeWidth / LittleCubeWidth);
        QMatrix4x4 cubeMatrix;
        cubeMatrix.perspective(65, 0.9*w/h, 0.1f, 100.0);
        cubeMatrix.translate (translationX, 2.8f, -7);
        cubeMatrix.rotate(cube.RotationRadians*360/3.14, cube.XAxis, cube.YAxis, cube.ZAxis);

        m_program->setUniformValue(m_colAttr, QVector4D( cube.Red, cube.Green, cube.Blue, 1.0f));
        m_program->setUniformValue(m_matrixUniform, cubeMatrix);

        vertexArray->bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        vertexArray->release();
    }

    QMatrix4x4 bigCubeMatrix;
    bigCubeMatrix.perspective(65, 0.9*w/h, 0.1f, 100.0);
    bigCubeMatrix.translate (0, -0.5f, -3);
    bigCubeMatrix.rotate(bigCube.RotationRadians*360/3.14, bigCube.XAxis, bigCube.YAxis, bigCube.ZAxis);

    m_program->setUniformValue(m_colAttr, QVector4D( bigCube.Red, bigCube.Green, bigCube.Blue, 0.5f));
    m_program->setUniformValue(m_matrixUniform, bigCubeMatrix);

    vertexArray->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vertexArray->release();

    m_program->release();
}
//! [5]

//! [6]
void CubeView::cleanup()
{
    if (m_program) {
        delete m_program;
        m_program = 0;
    }
}
//! [6]

//! [9]
void CubeView::sync()
{
    m_thread_t = m_t;
}
//! [9]


float PositiveOrNegative ()
{
            bool temp = qrand() % 2 == 0;
            return temp ? 1.0f : -1.0f;
}

void CubeView::randomizeBigCube()
{
    bigCube.Red = (minimums.Red + ((float) qrand() / RAND_MAX * (maximums.Red - minimums.Red)));
    bigCube.Green = (minimums.Green + ((float) qrand() / RAND_MAX * (maximums.Green - minimums.Green)));
    bigCube.Blue = (minimums.Blue + ((float) qrand() / RAND_MAX * (maximums.Blue - minimums.Blue)));
    bigCube.XAxis = (minimums.XAxis + ((float) qrand() / RAND_MAX * (maximums.XAxis - minimums.XAxis)));
    bigCube.YAxis = (minimums.YAxis + ((float) qrand() / RAND_MAX * (maximums.YAxis - minimums.YAxis)));
    bigCube.ZAxis = (minimums.ZAxis + ((float) qrand() / RAND_MAX * (maximums.ZAxis - minimums.ZAxis)));
    bigCube.Speed = (minimums.Speed + ((float) qrand() / RAND_MAX * (maximums.Speed - minimums.Speed)));
    bigCube.RotationRadians = (minimums.RotationRadians + ((float) qrand() / RAND_MAX * (maximums.RotationRadians - minimums.RotationRadians)));

    bigCubeDirections.Red = PositiveOrNegative();
    bigCubeDirections.Green = PositiveOrNegative();
    bigCubeDirections.Blue = PositiveOrNegative();
    bigCubeDirections.XAxis = PositiveOrNegative();
    bigCubeDirections.YAxis = PositiveOrNegative();
    bigCubeDirections.ZAxis = PositiveOrNegative();
    bigCubeDirections.Speed = PositiveOrNegative();
    bigCubeDirections.RotationRadians = 0.0f;
}

void CubeView::UpdateCubes ()
{
    double elapsedTime = (QTime::currentTime().msecsSinceStartOfDay() - timeOfLastRenderedFrame)/1000;

    bigCube.Red = bigCube.Red + (bigCubeDirections.Red * deltas.Red * (float) elapsedTime);
    if (bigCube.Red < minimums.Red || bigCube.Red > maximums.Red) {
        bigCube.Red = qMax (minimums.Red, qMin (maximums.Red, bigCube.Red));
        bigCubeDirections.Red *= -1;
    }

    bigCube.Green = bigCube.Green + (bigCubeDirections.Green * deltas.Green * (float) elapsedTime);
    if (bigCube.Green < minimums.Green || bigCube.Green > maximums.Green) {
        bigCube.Green = qMax (minimums.Green, qMin (maximums.Green, bigCube.Green));
        bigCubeDirections.Green *= -1;
    }

    bigCube.Blue = bigCube.Blue + (bigCubeDirections.Blue * deltas.Blue * (float) elapsedTime);
    if (bigCube.Blue < minimums.Blue || bigCube.Blue > maximums.Blue) {
        bigCube.Blue = qMax (minimums.Blue, qMin (maximums.Blue, bigCube.Blue));
        bigCubeDirections.Blue *= -1;
    }

    bigCube.XAxis = bigCube.XAxis + (bigCubeDirections.XAxis * deltas.XAxis * (float) elapsedTime);
    if (bigCube.XAxis < minimums.XAxis || bigCube.XAxis > maximums.XAxis) {
        bigCube.XAxis = qMax (minimums.XAxis, qMin (maximums.XAxis, bigCube.XAxis));
        bigCubeDirections.XAxis *= -1;
    }

    bigCube.YAxis = bigCube.YAxis + (bigCubeDirections.YAxis * deltas.YAxis * (float) elapsedTime);
    if (bigCube.YAxis < minimums.YAxis || bigCube.YAxis > maximums.YAxis) {
        bigCube.YAxis = qMax (minimums.YAxis, qMin (maximums.YAxis, bigCube.YAxis));
        bigCubeDirections.YAxis *= -1;
    }

    bigCube.ZAxis = bigCube.ZAxis + (bigCubeDirections.ZAxis * deltas.ZAxis * (float) elapsedTime);
    if (bigCube.ZAxis < minimums.ZAxis || bigCube.Red > maximums.ZAxis) {
        bigCube.ZAxis = qMax (minimums.ZAxis, qMin (maximums.ZAxis, bigCube.ZAxis));
        bigCubeDirections.ZAxis *= -1;
    }

    bigCube.Speed = bigCube.Speed + (bigCubeDirections.Speed * deltas.Speed * (float) elapsedTime);
    if (bigCube.Speed < minimums.Speed || bigCube.Speed > maximums.Speed) {
        bigCube.Speed = qMax (minimums.Speed, qMin (maximums.Speed, bigCube.Speed));
        bigCubeDirections.Speed *= -1;
    }

    bigCube.RotationRadians = UpdatedRotationRadians (bigCube.RotationRadians, bigCube.Speed, elapsedTime);

    for (int i = 0; i < NumLittleCubes; i++)
        littleCube [i].RotationRadians = UpdatedRotationRadians (littleCube [i].RotationRadians, littleCube [i].Speed, elapsedTime);

    timeOfLastRenderedFrame = QTime::currentTime().msecsSinceStartOfDay();
}

float CubeView::UpdatedRotationRadians (float radians, float speed, double elapsedTime)
{
    float speedInRadians = speed * (float) 3.14 * 2;
    float radiansDelta = speedInRadians * (float) elapsedTime;
    return fmod((radians + radiansDelta) , ((float) 3.14 * 2));
}


