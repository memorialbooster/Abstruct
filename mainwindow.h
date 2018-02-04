#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>

class MainWindow : public QGLWidget
{
    Q_OBJECT

private:
    int screenWidht;
    int screenHeight;

    void drawBackground();

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
