#include "camera.h"
#include "ui_camera.h"
#include <QTimer>

Camera::Camera(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Camera)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    cam = NULL;

    connect(timer, SIGNAL(timeout()), this, SLOT(cameraopen()));
}

Camera::~Camera()
{
    delete ui;
    cvReleaseCapture(&cam);
}

void Camera::on_cancelButton_clicked()
{
    timer->stop();
    cvReleaseCapture(&cam);
    emit quit();
}

void Camera::cameraopen()
{
    pFrame = cvQueryFrame(cam);

    QImage image((const uchar*)pFrame->imageData, pFrame->width, pFrame->height,QImage::Format_RGB888);

    QGraphicsScene *scene = new QGraphicsScene;


    scene->addPixmap(QPixmap::fromImage(image));


    ui->graphicsView->setScene(scene);

    ui->graphicsView->show();

}

void Camera::on_shotButton_clicked()
{
    cam = cvCreateCameraCapture(0);


    timer->start(33);

}
