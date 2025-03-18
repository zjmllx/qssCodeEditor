#include "app.h"
#include "../myEditor/highlighter.h"
#include "../myMenuBar/findwidget.h"
#include "../config.h"
#include "../customDialog/myCustomStyle/mycustomstyle.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QCompleter>
#include <QFile>
#include <QStringListModel>
#include <QGuiApplication>


App::App(QWidget *parent) :
    QWidget(parent),
    findWidget(nullptr)
{
    this->setWindowTitle("QSS编辑器");
    this->setWindowIcon(QIcon(":/img/images/app.png"));

    resize(800, 600);
    tolBarH = 26;
    tolBar = new TopBar(this);
    Config::config()->getTopBar(tolBar);

    // 创建两个 QFrame：文本编辑区域和按钮组控制区域
    fra_text = new QFrame;
    fra_control = new QFrame;
    Config::config()->getRightFrame(fra_control);

    // 设置 QFrame 的最小宽度，防止过度缩小
    fra_text->setMinimumWidth(400);      // 文本编辑区域最小宽度
    fra_control->setMinimumWidth(350);   // 控制区域最小宽度

    // 在现有代码中新增以下控件
    QLabel *lblPageInfo = new QLabel("1 / 4");  // 初始显示为"当前页/总页数"
    QPushButton *btnPrev = new QPushButton("<-");
    QPushButton *btnNext = new QPushButton("->");

    // 创建导航栏布局容器
    QHBoxLayout *navLayout = new QHBoxLayout;
    navLayout->setContentsMargins(0, 0, 0, 0);  // 底部留5像素间距
    navLayout->addWidget(btnPrev);
    navLayout->addWidget(lblPageInfo);
    navLayout->addWidget(btnNext);
    navLayout->addStretch();

    // 可选：设置按钮固定尺寸
    lblPageInfo->setAlignment(Qt::AlignCenter);
    lblPageInfo->setFixedSize(40, 25);
    btnPrev->setFixedSize(30, 25);
    btnNext->setFixedSize(30, 25);

    // 创建编辑器和按钮组
    editor = new CodeEditor;
    completer = new MyCompleter(this);
    Config::config()->getCodeEditor(editor);

    editor->setCompleter(completer);
    editor->setPlainText("QWidget, QPushButton");
    highlighter = new Highlighter(editor->document());
    stackedWidget = new QStackedWidget;
    Config::config()->getMainHighlighter(highlighter);

    btnGroup = new ButtonGroup(editor);
    inputGroup = new InputGroup(editor);
    textEditGroup = new TextEditGroup(editor);
    viewGroup = new ViewGroup(editor);

    stackedWidget->addWidget(btnGroup);
    stackedWidget->addWidget(inputGroup);
    stackedWidget->addWidget(textEditGroup);
    stackedWidget->addWidget(viewGroup);

    QVBoxLayout* lay_editor = new QVBoxLayout(fra_text);
    lay_editor->setContentsMargins(0 ,0, 0, 0);
    lay_editor->addWidget(editor);

    QVBoxLayout* vlay_btngroup = new QVBoxLayout(fra_control);
    vlay_btngroup->setContentsMargins(20,5,0,0);
    vlay_btngroup->addLayout(navLayout);
    vlay_btngroup->addWidget(stackedWidget);

    editor->setContentsMargins(1, 0, 0, 10);

    // 查找信号连接部分
    connect(tolBar, &TopBar::findBtnClicked, this, [=](){
        // 如果已存在则先删除
        if(findWidget) {
            lay_editor->removeWidget(findWidget);
            delete findWidget;
            findWidget = nullptr;
        }
        // 创建查找控件
        findWidget = new FindWidget(editor);
        lay_editor->addWidget(findWidget);
    });

    // 创建 QSplitter，用于分割两个 QFrame
    splitter = new QSplitter(Qt::Horizontal,this);  // 横向分割
    splitter->addWidget(fra_text);
    splitter->addWidget(fra_control);

    Config::config()->getSplitter(splitter);

    // 实现循环翻页
    connect(btnPrev, &QPushButton::clicked, this, [=](){
        const int total = stackedWidget->count();
        const int current = stackedWidget->currentIndex();
        // 正确计算前一页索引
        const int newIndex = (current - 1 + total) % total;
        stackedWidget->setCurrentIndex(newIndex);
        // 显示页码时索引+1（人类可读格式）
        lblPageInfo->setText(QString("%1 / %2").arg(newIndex + 1).arg(total));
    });

    connect(btnNext, &QPushButton::clicked, this, [=](){
        const int total = stackedWidget->count();
        const int current = stackedWidget->currentIndex();
        // 计算下一页索引
        const int newIndex = (current + 1) % total;
        stackedWidget->setCurrentIndex(newIndex);
        lblPageInfo->setText(QString("%1 / %2").arg(newIndex + 1).arg(total));
    });

    splitter->setMinimumWidth(fra_text->minimumWidth()+fra_control->minimumWidth());

    // 禁止两个区域完全折叠
    splitter->setCollapsible(0, false);  // 禁止左侧完全折叠
    splitter->setCollapsible(1, false);  // 禁止右侧完全折叠

    // 设置初始比例及伸缩优先级
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    //传递editor对象，实现tolbar功能
    tolBar->getEditor(editor);
    Config::config()->updateCurrentThemeStyle();
    Config::config()->resumeDefaultText();
}

// 析构函数
App::~App() {}

void App::resizeEvent(QResizeEvent *event)
{
    // 更新工具栏和 splitter 的尺寸和位置
    tolBar->setFixedSize(this->width(), tolBarH);
    splitter->resize(this->width(), this->height() - (tolBarH + tolBar->y()));
    splitter->move(0, tolBar->y() + tolBar->height());

    fra_text->setMinimumHeight(splitter->height());
    fra_control->setMinimumHeight(splitter->height());

    QWidget::resizeEvent(event);  // 调用父类的事件处理
}

void App::closeEvent(QCloseEvent *event)
{
    if(tolBar->customstyle){
        tolBar->customstyle->close();
        tolBar->customstyle->deleteLater();

    }
    QWidget::closeEvent(event);
}


