#include "completer.h"

#include <QMessageBox>
#include <QFile>
#include <QGuiApplication>

MyCompleter::MyCompleter(QObject *parent)
    : QCompleter(parent)
{
    this->setModel(modelFromFile(":/config/wordlist.txt"));
    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setFilterMode(Qt::MatchStartsWith);
    this->setWrapAround(true);

    listview = new QListView;
    listview->setStyleSheet(getCompleterlistQss());

    this->setPopup(listview);

    // 计算最长项的宽度
    setCompletionMode(QCompleter::PopupCompletion); // 确保设置了显示模式
}

QAbstractItemModel *MyCompleter::modelFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(nullptr, "错误", "关键字列表数据加载错误！");
        qWarning() << "Cannot open file:" << fileName;
        return new QStringListModel(this);  // 返回一个空模型
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    QStringList words;
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        // 使用 trimmed() 去除前后空白，若转换后不为空则加入列表
        QString word = QString::fromUtf8(line).trimmed();
        if (!word.isEmpty())
            words << word;
    }

    // 删除列表中所有空字符串（如有）
    words.removeAll("");

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, this);
}

QString MyCompleter::getCompleterlistQss()
{
    QString qss = R"(
        QListView {
            outline: none;
            background: #474747;
            border: 1px solid #313438;
        }

        QListView::item {
            padding: 4px;
            color: rgb(255, 255, 255);
        }

        QListView::item:selected {
            background: #8BA19F;
            color: yellow;
        }

        QListView::item:hover {
            background: #657574;
        }

        QScrollBar:vertical {
            border: none;
            background: #E0E0E0;
            width: 8px;
            margin: 0px;
            border-radius: 3px;
        }

        QScrollBar::handle:vertical {
            background-color: #BDBDBD;
            min-height: 20px;
            border-radius: 3px;
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical,
        QScrollBar::add-page:vertical,
        QScrollBar::sub-page:vertical {
            background: none;
        }

        QScrollBar:horizontal {
            border: none;
            background: #E0E0E0;
            height: 8px;
            margin: 0px;
            border-radius: 3px;
        }

        QScrollBar::handle:horizontal {
            background-color: #BDBDBD;
            min-width: 20px;
            border-radius: 3px;
        }

        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal,
        QScrollBar::add-page:horizontal,
        QScrollBar::sub-page:horizontal {
            background: none;
        }
    )";
    return qss;
}

