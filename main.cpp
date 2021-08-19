#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("HashCheck");
    app.setApplicationVersion("1.0");
    Widget window;
    window.show();

    return app.exec();
}
