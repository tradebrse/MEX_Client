#include "mex_login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MEX_Login w;
    w.show();

    return a.exec();
}
