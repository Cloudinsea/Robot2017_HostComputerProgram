#include "usartassistant.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    UsartAssistant w;
    w.show();

    return a.exec();
}
