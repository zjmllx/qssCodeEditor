#include "useguide.h"
#include "ui_useguide.h"
#include "../config.h"

UseGuide::UseGuide(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UseGuide)
{
    ui->setupUi(this);
    this->setStyleSheet(Config::config()->codeEditorJson_qss.themeQssData);

}

UseGuide::~UseGuide()
{
    delete ui;
}
