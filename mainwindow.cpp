#include "mainwindow.h"

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glMatrixMode(GL_PROJECTION); // устанавливаем матрицу
    glLoadIdentity(); // загружаем матрицу
    glOrtho(0, 600, 0, 600, -1, 2); // подготавливаем плоскости для матрицы
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
    std::vector<Dot> &dots = abstructObject->getDots();

    for (size_t i = 0; i < dots.size(); i++)
    {
        dots[i].x++;
        dots[i].y++;
    }

    updateGL();
}

AbstructObject::AbstructObject()
{
    dots.reserve(4);
    Dot dot;
    dot.x = GLfloat(20);
    dot.y = GLfloat(30);
    dots.push_back(dot);
    dot.x = GLfloat(520);
    dot.y = GLfloat(50);
    dots.push_back(dot);
    dot.x = GLfloat(430);
    dot.y = GLfloat(300);
    dots.push_back(dot);
    dot.x = GLfloat(40);
    dot.y = GLfloat(350);
    dots.push_back(dot);
    lines.reserve(4);
    Line line;
    line.dotIndex1 = 0;
    line.dotIndex2 = 1;
    lines.push_back(line);
    line.dotIndex1 = 1;
    line.dotIndex2 = 2;
    lines.push_back(line);
    line.dotIndex1 = 2;
    line.dotIndex2 = 3;
    lines.push_back(line);
    line.dotIndex1 = 3;
    line.dotIndex2 = 0;
    lines.push_back(line);
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

}

void AbstructObject::resizeObject(int widht, int height)
{
    (void) widht;
    (void) height;
}
