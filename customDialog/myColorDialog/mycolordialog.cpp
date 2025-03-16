#include "mycolordialog.h"
#include "ui_mycolordialog.h"

#include <QPalette>
#include <QValidator>
#include <QMessageBox>
#include <QPixmap>

MyColorDialog::MyColorDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MyColorDialog)
{
    ui->setupUi(this);

    this->colorAlpha = 255;
    this->setWindowIcon(QIcon(":/img/images/platte.png"));
    this->setWindowTitle("选择颜色");
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::Window); // 保证独立窗口

    // 添加转换项
    ui->comb_RGBToHSV->addItem("RGB");
    ui->comb_RGBToHSV->addItem("HSV");

    const QColor baseColors[] = {
        // 第一行：基础色系（纯色光谱）
        QColor(255, 0, 0),       // 1. 纯红       HEX:#FF0000  RGB(255,0,0)     - 三原色
        QColor(0, 255, 0),       // 2. 纯绿       HEX:#00FF00  RGB(0,255,0)     - 三原色
        QColor(0, 0, 255),       // 3. 纯蓝       HEX:#0000FF  RGB(0,0,255)     - 三原色
        QColor(255, 255, 0),     // 4. 明黄       HEX:#FFFF00  RGB(255,255,0)   - 补色：蓝
        QColor(255, 0, 255),     // 5. 品红       HEX:#FF00FF  RGB(255,0,255)   - 非光谱色
        QColor(0, 255, 255),     // 6. 青色       HEX:#00FFFF  RGB(0,255,255)   - 印刷三原色
        QColor(128, 0, 0),       // 7. 深红       HEX:#800000  RGB(128,0,0)     - 低明度红色
        QColor(0, 128, 0),       // 8. 深绿       HEX:#008000  RGB(0,128,0)     - 森林色系
        QColor(0, 0, 128),       // 9. 深蓝       HEX:#000080  RGB(0,0,128)     - 海军蓝

        // 第二行：扩展色系（混合中间色）
        QColor(255, 165, 0),     // 10. 橙色      HEX:#FFA500  RGB(255,165,0)   - 暖色系代表
        QColor(128, 0, 128),     // 11. 紫色      HEX:#800080  RGB(128,0,128)   - 皇室象征色
        QColor(255, 192, 203),   // 12. 粉红      HEX:#FFC0CB  RGB(255,192,203) - 孟菲斯风格
        QColor(144, 238, 144),   // 13. 浅绿      HEX:#90EE90  RGB(144,238,144) - 薄荷色调
        QColor(135, 206, 235),   // 14. 天蓝      HEX:#87CEEB  RGB(135,206,235) - 潘通年度色
        QColor(255, 127, 80),    // 15. 珊瑚      HEX:#FF7F50  RGB(255,127,80)  - 活力暖色
        QColor(128, 128, 0),     // 16. 橄榄绿    HEX:#808000  RGB(128,128,0)   - 军事风格
        QColor(75, 0, 130),      // 17. 靛蓝      HEX:#4B0082  RGB(75,0,130)    - 传统染料色
        QColor(210, 180, 140),   // 18. 茶色      HEX:#D2B48C  RGB(210,180,140) - 大地色系

        // 第三行：灰度色系（明度渐变）
        QColor(0, 0, 0),         // 19. 纯黑      HEX:#000000  RGB(0,0,0)       - K=100%印刷色
        QColor(64, 64, 64),      // 20. 炭灰      HEX:#404040  RGB(64,64,64)    - 深背景色
        QColor(96, 96, 96),      // 21. 石板灰    HEX:#606060  RGB(96,96,96)    - 中等对比度
        QColor(128, 128, 128),   // 22. 中灰      HEX:#808080  RGB(128,128,128) - 标准中性灰
        QColor(160, 160, 160),   // 23. 银灰      HEX:#A0A0A0  RGB(160,160,160) - 浅色文本背景
        QColor(192, 192, 192),   // 24. 浅灰      HEX:#C0C0C0  RGB(192,192,192) - 系统默认灰
        QColor(224, 224, 224),   // 25. 白灰      HEX:#E0E0E0  RGB(224,224,224) - 极浅背景
        QColor(240, 240, 240),   // 26. 珍珠白    HEX:#F0F0F0  RGB(240,240,240) - 纸张底色
        QColor(255, 255, 255)    // 27. 纯白      HEX:#FFFFFF  RGB(255,255,255) - 印刷留白
    };

    // 添加基本色块
    QVector<CustomButton*>baseColorBlock;
    int left = 5;
    int top = 25;
    for(int i=0; i<3; i++){
        for(int j=0; j<9; j++){
            CustomButton *btn = new CustomButton(ui->gBox_baseColor);
            btn->color = baseColors[i*9+j];
            btn->resize(30, 30);
            btn->move(left+j*(5+30), top);
            baseColorBlock.append(btn);
            // connect(btn, &QPushButton::clicked, [=]() {
            //     QColor color = btn->color;
            //     qDebug().noquote() << QString("(%1, %2, %3, %4)")
            //                               .arg(color.red())     // 红色通道 0-255
            //                               .arg(color.green())   // 绿色通道 0-255
            //                               .arg(color.blue())    // 蓝色通道 0-255
            //                               .arg(color.alpha());  // 透明度通道 0-255
            // });
            this->colorBlockchecked(btn);
        }
        top += 30+5;
    }

    m_buttonGroup = new QButtonGroup(this);
    const int btn_size = 30;   // 按钮尺寸
    const int spaces = 5;        // 按钮间距
    const int start_x = 10;       // 起始X坐标
    const int start_y = 25;       // 固定Y坐标
    // 初始化单选组
    m_buttonGroup->setExclusive(true);

    connect(ui->btn_addColor, &QPushButton::clicked, this, [this](){
        if(m_buttons.length()>=21){
            QMessageBox msgBox;
            QFont font = msgBox.font();
            font.setPointSize(12);
            msgBox.setWindowTitle("提示");
            msgBox.setText("自定义色块已满，请先移除一些不常用的色块！");
            msgBox.setFont(font);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::Yes);
            msgBox.exec();
            return;
        }

        // 创建自定义按钮
        CustomButton *customColorBtn = new CustomButton(ui->gBox_custColBlo);
        customColorBtn->color = this->currentColor;
        customColorBtn->setCheckable(true);
        customColorBtn->setFixedSize(btn_size, btn_size);

        // 添加按钮时加入按钮组
        m_buttonGroup->addButton(customColorBtn);
        m_buttons.append(customColorBtn);

        int curBtnNum = m_buttons.length()-1;  // 使得下标从0开始
        int curRow = curBtnNum/8;
        int curCol = curBtnNum%8;
        customColorBtn->index = curBtnNum;
        customColorBtn->move(start_x+curCol*(btn_size+spaces), start_y+curRow*(btn_size+spaces));

        // 强制刷新UI，避免布局问题
        customColorBtn->show();
        ui->gBox_custColBlo->update();
        this->colorBlockchecked(customColorBtn);
    });

    connect(ui->btn_removeColor, &QPushButton::clicked, this, [&](){
        if (QAbstractButton *selected = m_buttonGroup->checkedButton()) {
            if (auto *btn = dynamic_cast<CustomButton*>(selected)) {
                int currentIndex = btn->index;
                // 从所有容器移除
                m_buttonGroup->removeButton(btn);
                m_buttons.removeOne(btn);

                // 销毁对象
                btn->deleteLater();

                // 重新排列剩余按钮
                int curBtnNum = m_buttons.length();
                for(int i=currentIndex; i<curBtnNum; i++){
                    CustomButton *btn = m_buttons[i];
                    btn->index = i;
                    int curRow = i/8;
                    int curCol = i%8;
                    btn->move(start_x+curCol*(30+5), start_y+curRow*(30+5));
                }
            }
        }
    });

    // 初始化调色板
    hueSaSel = new HueSaturationSelector(ui->lab_HueSaSel);
    hueSaSel->resize(ui->lab_HueSaSel->size());

    myVSlider = new MyVSlider(ui->lab_ligntness);
    myVSlider->resize(ui->lab_ligntness->size());

    line_R = new MyLineEdit(ui->lab_R);
    line_G = new MyLineEdit(ui->lab_G);
    line_B = new MyLineEdit(ui->lab_B);
    line_A = new MyLineEdit(ui->lab_A);

    line_R->resize(ui->lab_R->size());
    line_G->resize(ui->lab_G->size());
    line_B->resize(ui->lab_B->size());
    line_A->resize(ui->lab_A->size());

    // 初始化颜色值
    colorValue = myVSlider->getValue();
    ui->lab_lightValue->setText("%" + QString::number(colorValue*100 / 255));  // 转化为百分数显示

    this->setStyleSheet("background-color: rgb(38,38,38);");

    connect(ui->comb_RGBToHSV, &QComboBox::currentIndexChanged, this, [&](){
        if(ui->comb_RGBToHSV->currentText() == "RGB")
        {
            ui->lab_RText->setText("红 色");
            ui->lab_GText->setText("绿 色");
            ui->lab_BText->setText("蓝 色");
        }
        else
        {
            ui->lab_RText->setText("色 调");
            ui->lab_GText->setText("饱和度");
            ui->lab_BText->setText("明亮度");
        }
        changeCurrentColor();
        qDebug() << ui->comb_RGBToHSV->currentText();
    });
    connect(hueSaSel, &HueSaturationSelector::colorSelected, this, &MyColorDialog::setColorFromHSVSelector);
    connect(myVSlider, &MyVSlider::valueChanged, this, &MyColorDialog::updateBrightness);

    connect(line_R, &MyLineEdit::textEdited, this, &MyColorDialog::setRValue);
    connect(line_G, &MyLineEdit::textEdited, this, &MyColorDialog::setGValue);
    connect(line_B, &MyLineEdit::textEdited, this, &MyColorDialog::setBValue);
    connect(line_A, &MyLineEdit::textEdited, this, &MyColorDialog::setAValue);

    connect(line_R, &MyLineEdit::valueChanged, this, &MyColorDialog::setRValue);
    connect(line_G, &MyLineEdit::valueChanged, this, &MyColorDialog::setGValue);
    connect(line_B, &MyLineEdit::valueChanged, this, &MyColorDialog::setBValue);
    connect(line_A, &MyLineEdit::valueChanged, this, &MyColorDialog::setAValue);

    // 移除默认的 OK 按钮
    ui->buttonBox->setStandardButtons(QDialogButtonBox::NoButton);

    // 如果需要自定义按钮，可以像这样添加
    QPushButton *okButton = new QPushButton("确定");
    ui->buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    okButton->setFixedSize(80, 30);
    okButton->setStyleSheet("QPushButton {"
                            "background-color: #BEC2CF;"
                            "color: black;"
                            "border-radius: 5px;"
                            "border: 1px solid #8F929A;"
                            "text-align: center;"
                            "font: bold 14px;"
                            "}"
                            "QPushButton:hover {"
                            "background-color: #A8ABB3;"
                            "}"
                            "QPushButton:pressed {"
                            "background-color: #92959E;"
                            "}"
                            );

    QPushButton *cancelButton = new QPushButton("取消");
    ui->buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    cancelButton->setFixedSize(80, 30);
    cancelButton->setStyleSheet("QPushButton {"
                            "background-color: #BEC2CF;"
                            "color: black;"
                            "border-radius: 5px;"
                            "border: 1px solid #8F929A;"
                            "text-align: center;"
                            "font: bold 14px;"
                            "}"
                            "QPushButton:hover {"
                            "background-color: #A8ABB3;"
                            "}"
                            "QPushButton:pressed {"
                            "background-color: #92959E;"
                            "}"
                            );
}

MyColorDialog::~MyColorDialog()
{
    delete ui;
}

QColor MyColorDialog::getCurrentColor(QColor preColor, QWidget *parent)
{
    MyColorDialog colorDialog(parent);
    colorDialog.currentColor = preColor;
    if(colorDialog.exec() == QDialog::Accepted){
        return colorDialog.currentColor;
    }
    return preColor;
}

void MyColorDialog::setColorHue(int hue)
{
    this->colorHue = hue;
}

void MyColorDialog::setColorSaturation(int saturation)
{
    this->colorSaturation = saturation;
}

void MyColorDialog::changeCurrentColor()
{
    // 从色板得到的明亮度为255，这里需要转化为当前垂直滑动条的值
    if(currentColor.hue() == -1)
        currentColor.setHsv(0, currentColor.saturation(), colorValue, this->colorAlpha);
    else
        currentColor.setHsv(currentColor.hue(), currentColor.saturation(), colorValue, this->colorAlpha);

    this->updataShowLabColor();

    // 设置文本值
    ui->lab_lightValue->setText("%" + QString::number(colorValue*100 / 255));
    ui->lineEdit_colHStr->setText(currentColor.name(QColor::HexRgb).toUpper());

    this->setShowText();

    emit onCurrentColorChanged(currentColor);
}

void MyColorDialog::colorBlockchecked(CustomButton *btn)
{
    connect(btn, &QPushButton::clicked, this, [=](){
        this->currentColor = btn->color;
        this->colorValue = btn->color.value();
        this->myVSlider->changeStartColor(currentColor);
        this->myVSlider->setValueFromExternal(currentColor.value());
        this->hueSaSel->updatePositionFromColor(currentColor);
        this->changeCurrentColor();
    });
}

void MyColorDialog::setColorFromHSVSelector(const QColor &color)
{
    this->currentColor = color;
    this->currentColor.setAlpha(this->colorAlpha);
    this->myVSlider->changeStartColor(color);
    this->changeCurrentColor();
}

void MyColorDialog::updateBrightness(int brightness)
{
    this->colorValue = brightness;
    this->changeCurrentColor();
}

void MyColorDialog::setRValue(QString red)
{
    if (ui->comb_RGBToHSV->currentText() == "RGB") {
        this->currentColor.setRed(red.toInt());
    } else {
        // 获取当前颜色的 RGB 值
        int r = red.toInt();
        int g = currentColor.green();
        int b = currentColor.blue();

        // 将 RGB 转换为 HSV
        float h, s, v;
        RGBtoHSV(r / 255.0f, g / 255.0f, b / 255.0f, h, s, v);

        // 设置新的 HSV 值
        this->currentColor.setHsv(h, s * 255, v * 255);
    }

    this->myVSlider->changeStartColor(currentColor);
    this->myVSlider->setValueFromExternal(currentColor.value());
    this->hueSaSel->updatePositionFromColor(currentColor);

    this->updataShowLabColor(); // 更新显示的颜色
}

void MyColorDialog::setGValue(QString green)
{
    if (ui->comb_RGBToHSV->currentText() == "RGB") {
        this->currentColor.setGreen(green.toInt());
    } else {
        // 获取当前颜色的 RGB 值
        int r = currentColor.red();
        int g = green.toInt();
        int b = currentColor.blue();

        // 将 RGB 转换为 HSV
        float h, s, v;
        RGBtoHSV(r / 255.0f, g / 255.0f, b / 255.0f, h, s, v);

        // 设置新的 HSV 值
        this->currentColor.setHsv(h, s * 255, v * 255);
    }

    this->myVSlider->changeStartColor(currentColor);
    this->myVSlider->setValueFromExternal(currentColor.value());
    this->hueSaSel->updatePositionFromColor(currentColor);

    this->updataShowLabColor();
}

void MyColorDialog::setBValue(QString blue)
{
    if (ui->comb_RGBToHSV->currentText() == "RGB") {
        this->currentColor.setBlue(blue.toInt());
    } else {
        // 获取当前颜色的 RGB 值
        int r = currentColor.red();
        int g = currentColor.green();
        int b = blue.toInt();

        // 将 RGB 转换为 HSV
        float h, s, v;
        RGBtoHSV(r / 255.0f, g / 255.0f, b / 255.0f, h, s, v);
        qDebug()<<h<<s<<v;
        // 设置新的 HSV 值
        this->currentColor.setHsv(h, s * 255, v * 255);
    }

    this->myVSlider->changeStartColor(currentColor);
    this->myVSlider->setValueFromExternal(currentColor.value());
    this->hueSaSel->updatePositionFromColor(currentColor);

    this->updataShowLabColor();
}

void MyColorDialog::setAValue(QString alpha)
{
    this->currentColor.setAlpha(alpha.toInt());
    this->colorAlpha = alpha.toInt();
    this->updataShowLabColor();
}
void MyColorDialog::RGBtoHSV(float r, float g, float b, float &h, float &s, float &v)
{
    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    v = max; // 明度

    float delta = max - min;
    if (max != 0) {
        s = delta / max; // 饱和度
    } else {
        // r = g = b = 0
        s = 0;
        h = 0;
        qDebug() << "内部" << h << s << v;
        return;
    }

    if (r == max) {
        h = (g - b) / delta; // 色相
    } else if (g == max) {
        h = 2 + (b - r) / delta;
    } else {
        h = 4 + (r - g) / delta;
    }

    h *= 60; // 转换为角度
    if (h < 0) {
        h += 360;
    }
}

void MyColorDialog::updataShowLabColor()
{
    ui->lab_showColor->setStyleSheet(QString("background-color: %1;"
                                             "border-radius: 5px;").arg(currentColor.name(QColor::HexArgb)));
}

void MyColorDialog::setShowText()
{
    if(ui->comb_RGBToHSV->currentText() == "RGB")
    {
        line_R->setText(QString::number(currentColor.red()));
        line_G->setText(QString::number(currentColor.green()));
        line_B->setText(QString::number(currentColor.blue()));
    }
    else
    {
        line_R->setText(QString::number(currentColor.hue()));
        line_G->setText(QString::number(currentColor.saturation()));
        line_B->setText(QString::number(currentColor.value()));
    }
    line_A->setText(QString::number(currentColor.alpha()));
    // qDebug() << "hsv = " << this->currentColor.hue() << this->currentColor.saturation()
    //          << this->currentColor.value() << this->currentColor.alpha();
}
