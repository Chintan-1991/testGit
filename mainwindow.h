#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QGLWidget>

namespace Ui {
class MainWindow;
}

class GLWidget : public QOpenGLWidget , protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget();
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setMaxX(int maxValue){maxX = maxValue;}
    int getMaxX() {return maxX;}

    void setMaxY(int maxValue){maxY = maxValue;}
    int getMaxY() {return maxY;}

    void setMinX(int minValue){minX = minValue;}
    int getMinX() {return minX;}

    void setMinY(int minValue){minY = minValue;}
    int getMinY() {return minY;}

    void drawGrid();
    void drawCurve();

    QVector <float> depths;
    QVector <float> values;

private:
    int minX;
    int maxX;
    int minY;
    int maxY;

    float scaleFactor;

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

    //void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    //void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    //void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    //void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QPushButton *browsBtn;
    QLineEdit *fileNameEdit;

    GLWidget *glWidget_;

    QVector <float> depths;
    QVector <float> values;

private slots:
    void browseClicked();
};




#endif // MAINWINDOW_H
