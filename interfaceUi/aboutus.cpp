#include "aboutus.h"
#include "ui_aboutus.h"
#include "../config.h"

AboutUs::AboutUs(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AboutUs)
{
    ui->setupUi(this);
    this->setStyleSheet(Config::config()->codeEditorJson_qss.themeQssData);
}

AboutUs::~AboutUs()
{
    delete ui;
}
