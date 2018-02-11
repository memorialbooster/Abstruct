#include "mainwindow.h"
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>

#define DEFAULT_DOTS_NUM 7
#define DEFAULT_ADD_DOTS_MAX_NUM 3
#define DEFAULT_ADD_DOTS_MIN_NUM 0
#define DEFAULT_COORD_MIN_NUM 2
#define COORD_MAX_NUM 16
#define DEFAULT_MIN_ITERS 100
#define DEFAULT_MAX_ITERS 300
#define DEFAULT_DOT_PROBABILITY 4
#define DEFAULT_COORD_PROBABILITY 4
#define DEFAULT_STEPS 50

#define COORD_DIAG_LENGHT GLfloat(15)
#define COORD_LINE_LENGHT GLfloat(37)
#define CTEST_Y_SCALE GLfloat(20)
#define BORDER_X 60
#define BORDER_Y 20
#define OBJECT_Y GLfloat(1)

MainWindow::MainWindow(QWidget *parent)
    : QGLWidget(parent)
{
    screenHeight = DEFAULT_SCREEN_HEIGHT;
    screenWidht = DEFAULT_SCREEN_WIDHT;

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
    glOrtho(0, 680, 0, 600, -1, 2);
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
    glVertex3f(GLfloat(0), GLfloat(DEFAULT_SCREEN_HEIGHT / 2), GLfloat(0));
    glColor3f(GLfloat(0.7), GLfloat(0.7), GLfloat(0.7));
    glVertex3f(GLfloat(0), GLfloat(0), GLfloat(0));
    glVertex3f(GLfloat(DEFAULT_SCREEN_WIDHT), GLfloat(0), GLfloat(0));
    glColor3f(GLfloat(1.0), GLfloat(1.0), GLfloat(1.0));
    glVertex3f(GLfloat(DEFAULT_SCREEN_WIDHT), GLfloat(DEFAULT_SCREEN_HEIGHT / 2), GLfloat(0));
    glEnd();
}

void MainWindow::drawAbstructObject()
{
    std::vector<Line> &objectLines = abstructObject->getLines();

    glLineWidth(GLfloat(1));
    glColor3f(GLfloat(0.2), GLfloat(0.2), GLfloat(0.2));

    for (size_t i = 0; i < objectLines.size(); i++)
    {
        Dot &dot1 = abstructObject->getDot(objectLines[i].dotIndex1);
        Dot &dot2 = abstructObject->getDot(objectLines[i].dotIndex2);

        glBegin(GL_LINES);
        glVertex3f(dot1.x, dot1.y, OBJECT_Y);
        glVertex3f(dot2.x, dot2.y, OBJECT_Y);
        glEnd();
    }

    std::vector<Coord> coords = abstructObject->getCoordinates();

    for (size_t i = 0; i < coords.size(); i++)
    {
        Dot &dot = abstructObject->getDot(coords[i].dotIndex);

        glBegin(GL_LINES);
        glVertex3f(dot.x, dot.y, OBJECT_Y);
        glVertex3f(dot.x + COORD_DIAG_LENGHT, dot.y + COORD_DIAG_LENGHT, OBJECT_Y);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(dot.x + COORD_LINE_LENGHT, dot.y + COORD_DIAG_LENGHT, OBJECT_Y);
        glVertex3f(dot.x + COORD_DIAG_LENGHT, dot.y + COORD_DIAG_LENGHT, OBJECT_Y);
        glEnd();

        renderText(dot.x + COORD_DIAG_LENGHT, dot.y + CTEST_Y_SCALE, OBJECT_Y,
                   QString::fromUtf8(coords[i].coordString.c_str()), QFont());
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

    loadConfig();

    dots.reserve(config.dotsNum + config.addDotsMaxNum);
    lines.reserve(config.dotsNum + config.addDotsMaxNum * 2);
    addDotIndexes.reserve(config.addDotsMaxNum);
    coordinates.reserve(config.dotsNum + config.addDotsMaxNum - 1);

    for (int i = 0; i < config.dotsNum; i++)
    {
        createRandomDot();
    }

    Line line;
    for (int i = 0; i < config.dotsNum - 1; i++)
    {
        line.dotIndex1 = i;
        line.dotIndex2 = i + 1;
        lines.push_back(line);
    }
    line.dotIndex1 = config.dotsNum - 1;
    line.dotIndex2 = 0;
    lines.push_back(line);

    randomiseLoop();
    createCoord();
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

std::vector<Coord> &AbstructObject::getCoordinates()
{
    return coordinates;
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
    randomiseCoord();

    for (size_t i = 0; i < coordinates.size(); i ++)
    {
        std::stringstream coodrdString;
        coodrdString << "[";
        int intCoord = dots[coordinates[i].dotIndex].x / 1;
        coodrdString << intCoord;
        coodrdString << ":";
        intCoord = dots[coordinates[i].dotIndex].y / 1;
        coodrdString << intCoord;
        coodrdString << "]";
        coordinates[i].coordString = coodrdString.str();
    }
}

void AbstructObject::resizeObject(int widht, int height)
{
    (void) widht;
    (void) height;
}

void AbstructObject::randomizeDot(Dot *dot)
{
    int randCoord = rand() % (config.widht - 1 - BORDER_X * 2) + 1 + BORDER_X;
    dot->iter = rand() % (config.maxIters - config.minIters) + config.minIters;
    dot->vx = static_cast <GLfloat>(static_cast <GLfloat>(randCoord) - dot->x) /
            static_cast <GLfloat>(dot->iter);

    randCoord = rand() % (config.height - 1 - BORDER_Y * 2) + 1 + BORDER_Y;
    dot->vy = static_cast <GLfloat>(static_cast <GLfloat>(randCoord) - dot->y) /
            static_cast <GLfloat>(dot->iter);
}

void  AbstructObject::randomiseLoop()
{
    if(backCounter == 0)
    {
        int randNum = rand() % config.addDotProbability;

        switch (randNum)
        {
        case 1:
            if (addDotIndexes.size() < config.addDotsMaxNum)
            {
                createRandomDot();

                size_t newDotIndex = dots.size() - 1;
                addDotIndexes.push_back(newDotIndex);

                Line newLine;
                newLine.dotIndex1 = newDotIndex;
                newLine.dotIndex2 = rand() % config.dotsNum;

                lines.push_back(newLine);

                int secondIndex = rand() % (config.dotsNum - 1);
                if (secondIndex >= newLine.dotIndex2)
                {
                    secondIndex++;
                }
                newLine.dotIndex2 =secondIndex;

                lines.push_back(newLine);
            }
            break;
        case 2:
            if (addDotIndexes.size() <= config.addDotsMaxNum &&
                    addDotIndexes.size() > config.addDotsMinNum)
            {
                size_t deletedDotIndex = addDotIndexes[addDotIndexes.size() - 1];

                addDotIndexes.erase(addDotIndexes.end() - 1);
                dots.erase(dots.begin() + deletedDotIndex);

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

                removeCoord(deletedDotIndex);
            }
            break;
        }

        backCounter = config.stapsToCheck;
    }

    backCounter--;
}

void AbstructObject::createRandomDot()
{
    Dot dot;

    dot.x = rand() % (config.widht - 1) + 1;
    dot.y = rand() % (config.height - 1) + 1;

    randomizeDot(&dot);

    dots.push_back(dot);
}

void AbstructObject::randomiseCoord()
{
    if(backCounter == config.stapsToCheck / 2)
    {
        int randNum = rand() % config.coordProbability;

        switch (randNum)
        {
        case 1:
            if (coordinates.size() < dots.size() && coordinates.size() < COORD_MAX_NUM)
            {
                createCoord();
            }
        case 2:
            if (coordinates.size() < dots.size() && coordinates.size() > config.coordMinNum)
            {
                randNum = rand() % dots.size();
                // Looks pretty good but incorrect.
                // addDotds should be there instead of dots.
                removeCoord(randNum);
            }
        }
    }
}

void  AbstructObject::createCoord()
{
    Coord coord;

    std::vector<size_t> freeIndexses;

    for(size_t i = 0; i < dots.size(); i++)
    {
        size_t j = 0;
        for (; j < coordinates.size(); j++)
        {
            if (coordinates[j].dotIndex == i)
            {
                break;
            }
        }

        if(j == coordinates.size())
        {
            freeIndexses.push_back(i);
        }
    }

    size_t index = freeIndexses[rand() % freeIndexses.size()];
    coord.dotIndex =  index;

    std::stringstream coodrdString;
    coodrdString << "[";
    int intCoord = dots[index].x / 1;
    coodrdString << intCoord;
    coodrdString << ":";
    intCoord = dots[index].y / 1;
    coodrdString << intCoord;
    coodrdString << "]";

    coord.coordString = coodrdString.str();
    coordinates.push_back(coord);
}

void AbstructObject::removeCoord(size_t dotIndex)
{
    std::vector<Coord>::iterator coord = coordinates.begin();

    for (size_t i = 0; i < coordinates.size(); i++)
    {
        if (coord->dotIndex == dotIndex)
        {
            coordinates.erase(coord);

            break;
        }

        coord++;
    }
}

void AbstructObject::loadConfig()
{
    config.widht = DEFAULT_SCREEN_WIDHT;
    config.height = DEFAULT_SCREEN_HEIGHT;
    config.dotsNum = DEFAULT_DOTS_NUM;
    config.addDotsMaxNum = DEFAULT_ADD_DOTS_MAX_NUM;
    config.addDotsMinNum = DEFAULT_ADD_DOTS_MIN_NUM;
    config.coordMinNum = DEFAULT_COORD_MIN_NUM;
    config.minIters = DEFAULT_MIN_ITERS;
    config.maxIters = DEFAULT_MAX_ITERS;
    config.addDotProbability = DEFAULT_DOT_PROBABILITY;
    config.coordProbability = DEFAULT_COORD_PROBABILITY;
    config.stapsToCheck = DEFAULT_STEPS;

    std::string fileLine;
    //std::ifstream configFile("settings.cfg");
    std::ifstream configFile("E:\\Projects\\Abstruct_2\\build-Abstruct_2-Desktop_Qt_5_7_1_MSVC2015_64bit-Debug\\debug\\settings.cfg");

    while (getline(configFile, fileLine))
    {
        if (fileLine[0] == '#' || fileLine[0] == '\n')
        {
            continue;
        }

        std::string key;

        for(size_t i = 0; fileLine[i] != ' ' && i < fileLine.size(); i++)
        {
            key.push_back(fileLine[i]);
        }

        if (fileLine.size() - key.size() < 2)
        {
            continue;
        }

        fileLine.erase(0, key.size() + 1);
        std::stringstream buf;
        buf << fileLine;
        int num;
        buf >> num;

        if (key == "dotsNum")
        {
            if (num < 3)
            {
                num = 3;
            }
            if (num > 100)
            {
                num = 100;
            }
            config.dotsNum = num;
            continue;
        }

        if (key == "addDotsMaxNum")
        {
            if (num < 2)
            {
                num = 2;
            }
            if (num > 5)
            {
                num = 5;
            }
            config.addDotsMaxNum = num;
            continue;
        }

        if (key == "addDotsMinNum")
        {
            if (num < 0)
            {
                num = 0;
            }
            if (num > config.addDotsMaxNum - 1)
            {
                num = static_cast<int>(config.addDotsMaxNum) - 1;
            }
            config.addDotsMinNum = num;
            continue;
        }

        if (key == "coordMinNum")
        {
            if (num < 0)
            {
                num = 0;
            }
            if (num > 15)
            {
                num = 15;
            }
            config.addDotsMinNum = num;
            continue;
        }

        if (key == "maxIters")
        {
            if (num > 4000)
            {
                num = 4000;
            }
            if (num < 201)
            {
                num = 201;
            }
            config.maxIters = num / 10;

            if (config.maxIters <= config.minIters)
            {
                config.minIters = config.maxIters - 1;
            }

            continue;
        }

        if (key == "minIters")
        {
            if (num < 200)
            {
                num = 200;
            }
            if (num > config.maxIters)
            {
                num = static_cast<int>(config.maxIters) * 10 - 1;
            }

            config.minIters = num / 10;

            if (config.minIters >= config.maxIters)
            {
                config.maxIters = config.minIters + 1;
            }

            continue;
        }

        if (key == "addDotProbability")
        {
            if (num < 1)
            {
                num = 1;
            }
            if (num > 3)
            {
                num = 3;
            }
            config.addDotProbability = 6 - num;
            continue;
        }

        if (key == "coordProbability")
        {
            if (num < 1)
            {
                num = 1;
            }
            if (num > 3)
            {
                num = 3;
            }
            config.coordProbability = 6 - num;
            continue;
        }

        if (key == "stapsToCheck")
        {
            if (num < 50)
            {
                num = 50;
            }
            if (num > 2000)
            {
                num = 2000;
            }
            config.stapsToCheck = num / 10;
            continue;
        }
    }

    configFile.close();
}
