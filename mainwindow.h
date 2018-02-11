#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>

struct Dot
{
    GLfloat x;
    GLfloat y;
    GLfloat vx;
    GLfloat vy;
    int iter;
};

struct Line
{
    size_t dotIndex1;
    size_t dotIndex2;
};

struct Coord
{
    size_t dotIndex;
    std::string coordString;
};

class AbstructObject
{
private:
    std::vector<Dot> dots;
    std::vector<Line> lines;
    std::vector<size_t> addDotIndexes;
    std::vector<Coord> coordinates;

    int backCounter;

    void createRandomDot();
    void createCoord();
    void removeCoord(size_t dotIndex);
    void randomizeDot(Dot *dot);
    void randomiseLoop();
    void randomiseCoord();

public:
    AbstructObject();

    std::vector<Dot> &getDots();
    Dot &getDot(size_t index);
    std::vector<Line> &getLines();
    Line &getLine(size_t index);
    std::vector<Coord> &getCoordinates();

    void modifyObject();
    void resizeObject(int widht, int height);

};


class MainWindow : public QGLWidget
{
    Q_OBJECT

private:
    int screenWidht;
    int screenHeight;

    AbstructObject *abstructObject;

    void drawBackground();
    void drawAbstructObject();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void timerDrawScene();
};

#endif // MAINWINDOW_H
