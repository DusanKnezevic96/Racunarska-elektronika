#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.setWindowTitle("Merenje distance sa ultrazvucnim senzorom");
    w.show();
    return a.exec();
}
