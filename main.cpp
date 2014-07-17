#include <QDebug>

#include <QGuiApplication>
#include <QDataStream>
#include <QFile>

#include "c_main_window.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    CMainWindow w;

    w.show();
    return a.exec();
}
