#include "./interfaceUi/app.h"
// #include "mytolbtn.h"
// #include "customDialog/myCustomStyle/mycustomstyle.h"
// #include "./customDialog/myColorDialog/mycolordialog.h"
// #include "interfaceUi/viewgroup.h"
// #include "interfaceUi/inputgroup.h"
// #include "myCustomControl/myCustomBtn/jsonparse.h"
#include <QTranslator>
#include <QLibraryInfo>
#include <QStringListModel>
#include <QApplication>
#include <QListView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    App w;
    QTranslator translator;
    if (translator.load(QLocale::Chinese, "qtbase", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        a.installTranslator(&translator);
    };
    w.show();
    return a.exec();
}
