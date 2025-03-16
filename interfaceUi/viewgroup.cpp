#include "viewgroup.h"
#include "ui_viewgroup.h"
#include <QFileDevice>
#include <QFileSystemModel>
#include <QStandardItemModel>

ViewGroup::ViewGroup(QPlainTextEdit *editor, QWidget *parent):
    QWidget(parent), editor(editor),
    ui_viewGroup(new Ui::ViewGroup)
{
    ui_viewGroup->setupUi(this);

    // 文件系统模型，用于展示驱动器和文件内容
    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath(QDir::rootPath());  // 设置根路径

    ui_viewGroup->treeView->setModel(model);
    ui_viewGroup->listView->setModel(model);
    ui_viewGroup->tableView->setModel(model);
    ui_viewGroup->columnView->setModel(model);

    connect(ui_viewGroup->treeView, SIGNAL(clicked(QModelIndex)),
            ui_viewGroup->listView,SLOT(setRootIndex(QModelIndex)));
    connect(ui_viewGroup->treeView, SIGNAL(clicked(QModelIndex)),
            ui_viewGroup->tableView,SLOT(setRootIndex(QModelIndex)));
    connect(ui_viewGroup->treeView, SIGNAL(clicked(QModelIndex)),
            ui_viewGroup->columnView,SLOT(setRootIndex(QModelIndex)));

    this->setStyleFromEditor();
    connect(editor, &QPlainTextEdit::textChanged, this, &ViewGroup::setStyleFromEditor);

}

// 析构函数，释放动态分配的内存
ViewGroup::~ViewGroup()
{
    delete ui_viewGroup;
}

void ViewGroup::setStyleFromEditor()
{
    QString styStr = editor->toPlainText();
    if(styStr.isEmpty()) return;

    ui_viewGroup->treeView->setStyleSheet(styStr);
    ui_viewGroup->columnView->setStyleSheet(styStr);
    ui_viewGroup->tableView->setStyleSheet(styStr);
    ui_viewGroup->listView->setStyleSheet(styStr);
}
