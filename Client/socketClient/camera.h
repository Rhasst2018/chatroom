#ifndef CAMERA_H
#define CAMERA_H

#include <QDialog>
#include "opencv2/opencv.hpp"
#include "cv.h"
#include "highgui.h"
#include <QTime>

using namespace cv;
namespace Ui {
class Camera;
}

class Camera : public QDialog
{
    Q_OBJECT

public:
    explicit Camera(QWidget *parent = 0);
    ~Camera();

private slots:
    void on_cancelButton_clicked();
    void cameraopen();

    void on_shotButton_clicked();

signals:
    void quit();

private:
    Ui::Camera *ui;

    IplImage* pFrame;
    QTimer *timer;
    CvCapture *cam;

};

#endif // CAMERA_H
