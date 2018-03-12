#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef SCREEN_SAVER
#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>
#else
#include <windows.h>
#include <gl/gl.h>
#include <string>
#include <vector>
#endif /* SCREEN_SAVER */

#define DEFAULT_SCREEN_WIDHT 680
#define DEFAULT_SCREEN_HEIGHT 600

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

struct Config
{
    int widht;
    int height;
    size_t dotsNum;
    size_t addDotsMaxNum;
    size_t addDotsMinNum;
    size_t coordMinNum;
    int maxIters;
    int minIters;
    int addDotProbability;
    int coordProbability;
    int stapsToCheck;
    int smoothLine;
};

class AbstructObject
{
private:
    Config config;

    std::vector<Dot> dots;
    std::vector<Line> lines;
    std::vector<size_t> addDotIndexes;
    std::vector<Coord> coordinates;

    int backCounter;

    void loadConfig();

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
    int getSmoothSetting();

    void modifyObject();
    void resizeObject(int widht, int height);

};


class MainWindow
#ifndef SCREEN_SAVER
    : public QGLWidget
{
    Q_OBJECT
#else
{
#endif /* SCREEN_SAVER */
private:
    int screenWidht;
    int screenHeight;

    AbstructObject *abstructObject;

    void drawBackground();
    void drawAbstructObject();

#ifndef SCREEN_SAVER
protected:
#else
public:
#endif /* SCREEN_SAVER */
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

public:
    ~MainWindow();
#ifndef SCREEN_SAVER
    MainWindow(QWidget *parent = 0);

protected slots:
#else
    MainWindow();
#endif /* SCREEN_SAVER */
    void timerDrawScene();
};

#endif // MAINWINDOW_H
