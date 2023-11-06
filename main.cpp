#include <QCoreApplication>
#include <QTimer>
#include "blebenchmarkapplication.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BleBenchmarkApplication *application = new BleBenchmarkApplication(&a);

    QObject::connect(application, &BleBenchmarkApplication::destroyed, []() {
        QCoreApplication::exit();
    });

    QTimer::singleShot(0, application, &BleBenchmarkApplication::start);

    return a.exec();
}
