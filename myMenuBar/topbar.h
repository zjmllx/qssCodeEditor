#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QPushButton>
#include <QMenu>

#include "../myEditor/codeeditor.h"

QT_BEGIN_NAMESPACE
class MyCustomStyle;
QT_END_NAMESPACE

class MyMenu : public QMenu
{
public:
    explicit MyMenu(QString name="", QWidget *parent = nullptr) : QMenu(parent){
        this->setTitle(name);
        this->setAttribute(Qt::WA_TranslucentBackground);
        this->setWindowFlags(Qt::Popup|Qt::NoDropShadowWindowHint|Qt::FramelessWindowHint);
    }
};


class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
    ~TopBar();

    void getEditor(CodeEditor *editor);
    void setTopBarBtnToMenuStyle(const QString &style);
    // void setSecondMenuStyle(const QString &style);

public:
    QPushButton* btn_file;
    QPushButton* btn_edit;
    QPushButton* btn_setting;
    QPushButton* btn_help;
    QPushButton* btn_about;

    MyMenu *menu_setTheme;
    MyCustomStyle *customstyle;

public:
    int spacing;
    int leftMargin;
    int topMargin;
    int btn_height;
    int btn_width;

public:
    MyMenu *menu_file;
    MyMenu *menu_edit;
    MyMenu *menu_setting;
    MyMenu *menu_help;
    MyMenu *menu_about;

private:
    void setBtnPosition(QPushButton *btn, MyMenu *menu, int mx, QString text, QString iconPath);

signals:
    void findBtnClicked();
    // QWidget interface
protected:
    CodeEditor *editor;
    // void resizeEvent(QResizeEvent *event) override;

private slots:
    void onSelectAll();  // 全选
    void onUndo();       // 撤销
    void onCopy();       // 复制
    void onPaste();      // 粘贴

    void onIncreaseFontSize();       // 增大字号
    void onDecreaseFontSize();       // 减小字号
    void loadDefaultText();
    void onResetDefaults();          // 恢复默认设置

    void onSelectSwanWhite();        // 天鹅白
    void onSelectInkBlack();        // 墨玉黑
    void onSelectBluePurplePink();  // 蓝紫粉
    void onSelectLightGray();       // 浅白灰
    void onSelectCustomTheme();    // 自定义
    void onFindSlots();

    void onUseGuide();
    void onAboutUs();
};

#endif // TOPBAR_H
