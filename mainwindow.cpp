#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <GL/glu.h>
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Opengl 2D plot");

    glWidget_ = new GLWidget;

    QWidget *widget = new QWidget(this);

    QHBoxLayout *h0 = new QHBoxLayout(this);

    fileNameEdit = new QLineEdit(this);
    browsBtn = new QPushButton("Browse",this);

    h0->addWidget(new QLabel("FileName :",this));
    h0->addWidget(fileNameEdit);
    h0->addWidget(browsBtn);

    QVBoxLayout *v0 = new QVBoxLayout(this);
    v0->addLayout(h0);
    v0->addWidget(glWidget_);

    widget->setLayout(v0);

    this->setCentralWidget(widget);

    glWidget_->setMaxX(100);
    glWidget_->setMaxY(100);

    connect(browsBtn,SIGNAL(clicked(bool)),this,SLOT(browseClicked()));
}

MainWindow::~MainWindow()
{
    delete glWidget_;
    delete ui;
}

void MainWindow::browseClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Select Log file","","*.txt *.*");
    if(filename.isEmpty()) return;

    fileNameEdit->setText(filename);

    QFile file_(filename);

    if(!file_.open(QIODevice::ReadOnly)) return;

    QTextStream text(&file_);

    depths.clear();
    values.clear();

    float maxX = 0, minX = 999999;
    float maxY = 0, minY = 999999;

    while(!text.atEnd())
    {
        float d = 0;
        float v = 0;

        text >> d >> v ;

        if(d > 0)
        {
            depths.append(d);
            values.append(v);

            if(d > maxY) maxY = d; if(d < minY) minY = d;
            if(v > maxX) maxX = v; if(v < minX) minX = v;
        }
    }

    file_.close();

    if(depths.size() > 0)
    {
        //maxX = ceil(maxX); maxY = ceil(maxY);
        //minX = floor(minX); minY =floor(minY);

        maxX = maxX + (10 - int(maxX) % 10);
        maxY = maxY + (10 - int(maxY) % 10);

        minX = minX - int(minX) % 10;
        minY = minY - int(minY) % 10;

        glWidget_->setMaxX(maxX);
        glWidget_->setMaxY(maxY);
        glWidget_->setMinX(minX);
        glWidget_->setMinY(minY);

        glWidget_->depths = depths;
        glWidget_->values = values;

        glWidget_->update();
    }
}

GLWidget::GLWidget()
{
    this->setFocusPolicy(Qt::StrongFocus);

    minX = minY = 0;
    maxX = 100;
    maxY = 100;

    scaleFactor = 1.0;
}

GLWidget::~GLWidget()
{

}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(240,480);
}

QSize GLWidget::sizeHint() const
{
    return QSize(720,720);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black
    glMatrixMode(GL_PROJECTION);            // To operate on the Projection matrix
    glLoadIdentity();                       // Reset
    glOrtho(minX-10,maxX+10,minY+10,maxY-10,-1.0,1.0); //set coordinate range
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glScalef(scaleFactor,scaleFactor,scaleFactor); //for zoom in zoom out

    //grid drawing function
    drawGrid();
    //Log curve drawing function
    drawCurve();
}

void GLWidget::drawGrid()
{
    //glLineWidth(1.0);


    glMatrixMode(GL_PROJECTION);            // To operate on the Projection matrix
    glLoadIdentity();                       // Reset
    glOrtho(minX - 1.0,maxX + 1.0,minY - 1.0,maxY + 1.0,-1.0,1.0); //set coordinate range
    glMatrixMode(GL_MODELVIEW);

    //outer grid box
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINE_LOOP);
        glVertex2d(minX,minY);
        glVertex2d(maxX,minY);
        glVertex2d(maxX,maxY);
        glVertex2d(minX,maxY);
    glEnd();

    //horizontal line
    glColor4f(0.8,0.8,0.8,0.5);
    glBegin(GL_LINES);
        for(int i=minY+1;i<maxY;i++)
        {
            if( i % 10 == 0){
                glVertex2d(minX,i); glVertex2d(maxX,i);
            }
        }
    glEnd();

    //vertical line
    glColor4f(0.8,0.8,0.8,0.5);
    glBegin(GL_LINES);
        for(int i=minX+1;i<maxX;i++)
        {
            if( i % 10 == 0){
                glVertex2d(i,minY); glVertex2d(i,maxY);
            }
        }
    glEnd();
}

void GLWidget::drawCurve()
{
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_LINES);
        for(int i=0;i<depths.size()-1;i++)
        {
            float x1 = values.at(i); float x2 = values.at(i+1);
            float y1 = depths.at(i); float y2 = depths.at(i+1);

            if(y1 > 0 && y2 > 0)
            {
                glLineWidth(2.0);
                glVertex2d(x1,y1); glVertex2d(x2,y2);
            }
        }
    glEnd();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);        // Set the viewport to cover the new window
    /*glMatrixMode(GL_PROJECTION);            // To operate on the Projection matrix
    glLoadIdentity();                       // Reset
    glOrtho(minX-10,maxX+10,minY+10,maxY-10,-1.0,1.0); //set coordinate range
    glMatrixMode(GL_MODELVIEW);*/
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);

    if(event->delta() > 0) {
       // scaleFactor = scaleFactor + 0.1;
    }
    else {
       // scaleFactor = scaleFactor - 0.1;
    }

    this->update();
}

/*
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W) {
        qDebug()<<"W";
        yTrns = yTrns + 1;
    }
    if(event->key() == Qt::Key_S) {
        qDebug()<<"S";
        yTrns = yTrns - 1;
    }
    if(event->key() == Qt::Key_A) {
        qDebug()<<"A";
        xTrns = xTrns + 1;
    }
    if(event->key() == Qt::Key_D) {
        qDebug()<<"D";
        xTrns = xTrns - 1;
    }
    this->update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        lastPos = event->pos();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseMoveEvent(event);

    dx = (event->x() - lastPos.x()) / 1.0f;
    dy = (event->y() - lastPos.y())/ 1.0f;

    this->update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseReleaseEvent(event);

    lastPos = event->pos();
}
*/
