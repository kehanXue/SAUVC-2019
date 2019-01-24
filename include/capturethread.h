#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QImage>
#include <opencv2/opencv.hpp>

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread(QObject *parent = 0);

public:
    void run();
    void setIndex(int index);
    void stream();
    void pause();
    void stop();
    bool isWorking();
signals:
    void captured(QImage img, IplImage* buffer);
private:
    int index;
    bool work;
    bool term;


    QVector<QRgb> grayColourTable;

public slots:

};

#endif // CAPTURETHREAD_H
