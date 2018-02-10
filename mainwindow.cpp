#include "mainwindow.h"
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QGLWidget(parent)
{
    screenHeight = 600;
    screenWidht = 600;

    abstructObject = new AbstructObject;

    setFormat(QGLFormat(QGL::DoubleBuffer));
    glDepthFunc(GL_LEQUAL);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerDrawScene()));
    timer->start(10);

    srand(time(NULL));
}

MainWindow::~MainWindow()
{
    delete abstructObject;
}

void MainWindow::initializeGL()
{
    setAutoBufferSwap(false);
    qglClearColor(Qt::white);
}

void MainWindow::resizeGL(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLint)width, (GLint)height);

    screenHeight = height;
    screenWidht = width;
}

void MainWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 600, 0, 600, -1, 2);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawBackground();
    drawAbstructObject();

    swapBuffers();
}

void MainWindow::drawBackground()
{
    glBegin(GL_QUADS);
    glColor3f(GLfloat(1.0), GLfloat(1.0), GLfloat(1.0));
    glVertex3f(GLfloat(0), GLfloat(300), GLfloat(0));
    glColor3f(GLfloat(0.7), GLfloat(0.7), GLfloat(0.7));
    glVertex3f(GLfloat(0), GLfloat(0), GLfloat(0));
    glVertex3f(GLfloat(600), GLfloat(0), GLfloat(0));
    glColor3f(GLfloat(1.0), GLfloat(1.0), GLfloat(1.0));
    glVertex3f(GLfloat(600), GLfloat(300), GLfloat(0));
    glEnd();
}

void MainWindow::drawAbstructObject()
{
    std::vector<Line> &objectLines = abstructObject->getLines();

    for (size_t i = 0; i < objectLines.size(); i++)
    {
        Dot &dot1 = abstructObject->getDot(objectLines[i].dotIndex1);
        Dot &dot2 = abstructObject->getDot(objectLines[i].dotIndex2);

        glLineWidth(GLfloat(1));
        glColor3f(GLfloat(0.2), GLfloat(0.2), GLfloat(0.2));
        glBegin(GL_LINES);
        glVertex2f(dot1.x, dot1.y);
        glVertex2f(dot2.x, dot2.y);
        glEnd();
    }
}

void MainWindow::timerDrawScene()
{
    abstructObject->modifyObject();

    updateGL();
}

AbstructObject::AbstructObject()
{
    backCounter = 0;

    dots.reserve(7);
    lines.reserve(7);
    addDotIndexes.reserve(3);

    for (int i = 0; i < 7; i++)
    {
        createRandomDot();
    }

    Line line;
    for (int i = 0; i < 6; i++)
    {
        line.dotIndex1 = i;
        line.dotIndex2 = i + 1;
        lines.push_back(line);
    }
    line.dotIndex1 = 6;
    line.dotIndex2 = 0;
    lines.push_back(line);

    randomiseLoop();
}

std::vector<Dot> &AbstructObject::getDots()
{
    return dots;
}

Dot &AbstructObject::getDot(size_t index)
{
    return dots[index];
}

std::vector<Line> &AbstructObject::getLines()
{
    return lines;
}

Line &AbstructObject::getLine(size_t index)
{
    return lines[index];
}

void AbstructObject::modifyObject()
{
    for (size_t i = 0; i < dots.size(); i ++)
    {
        if (dots[i].iter == 0)
        {
            randomizeDot(&dots[i]);
        }

        dots[i].x += dots[i].vx;
        dots[i].y += dots[i].vy;
        dots[i].iter--;
    }

    randomiseLoop();
}

void AbstructObject::resizeObject(int widht, int height)
{
    (void) widht;
    (void) height;
}

void AbstructObject::randomizeDot(Dot *dot)
{
    int randCoord = rand() % 599 + 1;
    dot->iter = rand() % 200 + 100;
    dot->vx = static_cast <GLfloat>(static_cast <GLfloat>(randCoord) - dot->x) /
            static_cast <GLfloat>(dot->iter);

    randCoord = rand() % 599 + 1;
    dot->vy = static_cast <GLfloat>(static_cast <GLfloat>(randCoord) - dot->y) /
            static_cast <GLfloat>(dot->iter);
}

void  AbstructObject::randomiseLoop()
{
    if(backCounter == 0)
    {
        int randNum = rand() % 4;

        switch (randNum)
        {
        case 1:
            if (addDotIndexes.size() < 3 && addDotIndexes.size() >= 0)
            {
                createRandomDot();

                size_t newDotIndex = dots.size() - 1;
                addDotIndexes.push_back(newDotIndex);

                Line newLine;
                newLine.dotIndex1 = newDotIndex;
                newLine.dotIndex2 = rand() % 7;

                lines.push_back(newLine);

                int secondIndex = rand() % 6;
                if (secondIndex >= newLine.dotIndex2)
                {
                    secondIndex++;
                }
                newLine.dotIndex2 =secondIndex;

                lines.push_back(newLine);
            }
            break;
        case 2:
            if (addDotIndexes.size() <= 3 && addDotIndexes.size() > 0)
            {
                size_t deletedDotIndex = addDotIndexes[addDotIndexes.size() - 1];

                addDotIndexes.erase(addDotIndexes.end() - 1);

                for (int i = 0; i < 2; i++)
                {
                    std::vector<Line>::iterator lineToDel = lines.end() - 1;

                    for (size_t j = (deletedDotIndex + 1) * 2 - i; j > 0; j--)
                    {
                        if (lineToDel->dotIndex1 == deletedDotIndex ||
                                lineToDel->dotIndex2 == deletedDotIndex)
                        {
                            lines.erase(lineToDel);
                            break;
                        }

                        lineToDel--;
                    }
                }
            }
            break;
        }

        backCounter = 50;
    }

    backCounter--;
}

void AbstructObject::createRandomDot()
{
    Dot dot;

    dot.x =rand() % 599 + 1;
    dot.y = rand() % 599 + 1;

    randomizeDot(&dot);

    dots.push_back(dot);
}
