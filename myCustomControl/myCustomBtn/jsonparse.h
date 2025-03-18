#ifndef JSONPARSE_H
#define JSONPARSE_H

#include <QString>
#include <QColor>
#include <QJsonObject>

struct HighlighterData {
    QByteArray jsonData;
    QMap<QString, QColor> highlighterColors;
};
struct ParseThemeData{
    QString themeJsonData;
    QString themeQssData;
};
struct LineNumberColor{
    QString jsonData;
    QColor selectedColor;
    QColor noSelectedColor;
    QColor backgroundColor;
};
class JsonParse
{
public:
    JsonParse();

    static JsonParse *getJsonParse();
    QString parseThemeJsontoQss(const QString &jsonConten);
    ParseThemeData parseThemeJsonFromFile(const QString &jsonConten);
    QString qssToJson(const QString &qss);

    QColor parseHighlightWKJson(const QString &filePath, const QString &colorType);

    QJsonObject getJsonOBJFromFile(const QString &filePath);
    LineNumberColor parseLineNumberJsonGetAllColor(const QString &filePath);
    HighlighterData parseHighlighterColorsFromFile(const QString &filePath);


};

#endif // JSONPARSE_H
