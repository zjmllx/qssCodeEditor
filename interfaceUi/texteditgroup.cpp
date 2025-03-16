#include "texteditgroup.h"
#include "ui_texteditgroup.h"

TextEditGroup::TextEditGroup(QPlainTextEdit *editor, QWidget *parent):
    QWidget(parent), editor(editor),
    ui(new Ui::TextEditGroup)
{
    ui->setupUi(this);
    this->setStyleFromEditor();
    connect(editor, &QPlainTextEdit::textChanged, this, &TextEditGroup::setStyleFromEditor);

}

TextEditGroup::~TextEditGroup()
{
    delete ui;
}

void TextEditGroup::setStyleFromEditor()
{
    QString styStr = editor->toPlainText();
    if(styStr.isEmpty()) return;

    ui->calendarWidget->setStyleSheet(styStr);
    ui->plainTextEdit->setStyleSheet(styStr);

    ui->textEdit->setStyleSheet(styStr);
    ui->lcdNumber->setStyleSheet(styStr);

    ui->progressBar->setStyleSheet(styStr);
    ui->textBrowser->setStyleSheet(styStr);
}
