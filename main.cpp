#include "dialog.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont fonts=a.font();
    fonts.setPixelSize(16);
    a.setFont(fonts);
    Dialog w;
    w.show();

    return a.exec();
}
