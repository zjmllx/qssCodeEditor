#include "inputgroup.h"
#include "ui_inputgroup.h"
#include <QPushButton>
#include <QListWidgetItem>

InputGroup::InputGroup(QPlainTextEdit *editor, QWidget *parent):
    QWidget(parent), editor(editor),
    ui(new Ui::InputGroup)
{
    ui->setupUi(this);
    this->setStyleFromEditor();
    connect(editor, &QPlainTextEdit::textChanged, this, &InputGroup::setStyleFromEditor);
}

InputGroup::~InputGroup()
{
    delete ui;
}

void InputGroup::setStyleFromEditor()
{
    QString styStr = editor->toPlainText();
    if(styStr.isEmpty()) return;

    ui->groupBox->setStyleSheet(styStr);
    ui->groupBox_2->setStyleSheet(styStr);

    ui->comboBox->setStyleSheet(styStr);
    ui->dateEdit->setStyleSheet(styStr);
    ui->dateTimeEdit->setStyleSheet(styStr);
    ui->doubleSpinBox->setStyleSheet(styStr);
    ui->fontComboBox->setStyleSheet(styStr);

    ui->lineEdit->setStyleSheet(styStr);
    ui->spinBox->setStyleSheet(styStr);
    ui->timeEdit->setStyleSheet(styStr);

    ui->dial->setStyleSheet(styStr);

    ui->horizontalSlider->setStyleSheet(styStr);
    ui->verticalScrollBar->setStyleSheet(styStr);
    ui->verticalSlider->setStyleSheet(styStr);

    ui->horizontalScrollBar->setStyleSheet(R"(
                    QScrollBar::handle:vertical {
                        background-color: lightgray;                  // 滑块颜色
                        min-height: 10px;                   // 滑块的最小高度
                        border-radius: 4px;               // 滑块的圆角
                    }
                    QScrollBar::add-line:vertical {
                        background: none;      // 向下滚动按钮
                        height: 0px;          // 隐藏按钮
                    }
                    QScrollBar::sub-line:vertical {
                        background: none;  // 向上滚动按钮
                        height: 0px;     // 隐藏按钮
                    }
                    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
                        background: none;   // 滑块周围的背景（空白部分）
                    }

                    /* QDial 样式 */
                    QDial {                         /* 旋钮控件 */
                        background-color: #ffffff; /* 背景颜色 */
                        border: 2px solid #cccccc; /* 边框颜色 */
                        border-radius: 16px;        /* 圆角（圆形） */
                    }

                    QDial::handle {
                        width: 2px;                /* 旋钮手柄宽度 */
                        height: 2px;               /* 旋钮手柄高度 */
                        background-color: pink; /* 手柄颜色 */
                    })");
}







