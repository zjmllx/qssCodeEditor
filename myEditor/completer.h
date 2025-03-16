#ifndef COMPLETER_H
#define COMPLETER_H

#include <QCompleter>
#include <QAbstractItemView>
#include <QStringListModel>
#include <QListView>
#include <QTimer>

class MyCompleter : public QCompleter {
    Q_OBJECT

public:
    explicit MyCompleter(QObject *parent = nullptr);

    // 设置补全关键词
    QAbstractItemModel *modelFromFile(const QString& fileName);

private:
    QListView *listview;
    QString getCompleterlistQss();


};

#endif // COMPLETER_H

