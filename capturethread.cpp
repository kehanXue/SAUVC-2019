#include "JHCap.h"
#include "capturethread.h"
#include "imageutil.h"
#include <opencv2/opencv.hpp>

#include <QDebug>

CaptureThread::CaptureThread(QObject *parent) :
    QThread(parent)
{
    work = false;
    term = false;
    cnt_assert = 0;
    index=0;

    for(int i = 0; i < 256; i++)
    {
       grayColourTable.append(qRgb(i, i, i));
    }
}

void CaptureThread::run()
{
    forever
    {
        int width, height, len;

        if(work)
        {

            CameraGetImageSize(index,&width, &height);
            CameraGetImageBufferSize(index,&len, CAMERA_IMAGE_RGB24);
            /// while(CameraOnePushWB(0) != API_OK );


            unsigned char *buffer = new unsigned char[len];
            IplImage *image = cvCreateImage(cvSize(width, height), 8, 3);

            int ret = API_OK;
            if((ret = CameraQueryImage(0, (unsigned char*)image->imageData, &len, CAMERA_IMAGE_BMP))==API_OK) {
                if(term) break;

                cnt_assert ++;
                if(cnt_assert > 30) {
                    qDebug() << "Start Save img to buffer.";
                    cnt_assert = 0;
                }
              // QImage img(buffer, width, height, QImage::Format_Indexed8);
               // img.setColorTable(grayColourTable);


                QImage img(buffer, width, height, QImage::Format_RGB888);

                emit captured(img, image);
            }
            else {
                delete[] buffer;
                // qDebug() << "Start Delete " << ret;
                usleep(1000);

            }
        } else {
            usleep(1000);
        }
        if(term) break;
    }
}

void CaptureThread::stream()
{
    work = true;
}
void CaptureThread::setIndex(int index)
{
    this->index=index;
}

void CaptureThread::pause()
{
    work = false;
}

void CaptureThread::stop()
{
    work = false;
    term = true;
}

bool CaptureThread::isWorking()
{
    return this -> work;
}
