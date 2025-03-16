#include "buttongroup.h"
#include "ui_buttongroup.h"

ButtonGroup::ButtonGroup(QPlainTextEdit *editor, QWidget *parent):
    QWidget(parent), editor(editor),
    ui_buttonGroup(new Ui::ButtonGroup)
{
    ui_buttonGroup->setupUi(this);
    this->setStyleFromEditor();
    connect(editor, &QPlainTextEdit::textChanged, this, &ButtonGroup::setStyleFromEditor);
}

ButtonGroup::~ButtonGroup()
{
    delete ui_buttonGroup;
}

void ButtonGroup::setStyleFromEditor()
{
    QString styStr = editor->toPlainText();
    if(styStr.isEmpty()) return;

    ui_buttonGroup->groupBox->setStyleSheet(styStr);
    ui_buttonGroup->pushButton->setStyleSheet(styStr);
    ui_buttonGroup->radioButton->setStyleSheet(styStr);
    ui_buttonGroup->radioButton_2->setStyleSheet(styStr);
    ui_buttonGroup->radioButton_3->setStyleSheet(styStr);
    ui_buttonGroup->toolButton->setStyleSheet(styStr);
    ui_buttonGroup->checkBox->setStyleSheet(styStr);
    ui_buttonGroup->checkBox_2->setStyleSheet(styStr);
    ui_buttonGroup->checkBox_3->setStyleSheet(styStr);
    ui_buttonGroup->frame->setStyleSheet(styStr);
    ui_buttonGroup->lineEdit->setStyleSheet(styStr);
}
