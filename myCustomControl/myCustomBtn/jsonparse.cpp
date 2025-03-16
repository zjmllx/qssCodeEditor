#include "jsonparse.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

JsonParse::JsonParse() {}

JsonParse *JsonParse::getJsonParse()
{
    static JsonParse* jp = nullptr;
    if(!jp){
        jp =new JsonParse;
    }
    return jp;
}

QString JsonParse::parseThemeJsontoQss(const QString &jsonConten)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonConten.toUtf8());
    // 解析 JSON
    if (jsonDoc.isNull()) {
        qWarning() << "JSON 解析失败";
        return QString();
    }

    QJsonObject jsonObj = jsonDoc.object();
    QString qss;

    // 遍历 JSON 键值对，构建 QSS
    for (QString &key : jsonObj.keys()) {
        QJsonObject style = jsonObj[key/*.remove(QRegularExpression("\\d"))*/].toObject();
        qss += key + " {\n";
        for (const QString &attr : style.keys()) {
            qss += "    " + attr + ": " + style[attr].toString() + ";\n";
        }
        qss += "}\n";
    }
    return qss;
}

ParseThemeData JsonParse::parseThemeJsonFromFile(const QString &jsonFilePath)
{
    // 读取 JSON 文件
    ParseThemeData themeData;
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开 JSON 文件：" << jsonFilePath;
        return themeData;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QString qss = this->parseThemeJsontoQss(jsonData);
    themeData.themeJsonData = jsonData;
    themeData.themeQssData = qss;
    return themeData;
}

QString JsonParse::qssToJson(const QString &qss)
{
    QJsonObject jsonObj;
    QRegularExpression re(R"((\S+)\s*\{([^}]*)\})");
    QRegularExpression reProperty(R"((\S+)\s*:\s*([^;]+);?)");

    QRegularExpressionMatchIterator i = re.globalMatch(qss);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString selector = match.captured(1).trimmed();
        QString properties = match.captured(2).trimmed();

        QJsonObject propertiesObj;
        QRegularExpressionMatchIterator j = reProperty.globalMatch(properties);
        while (j.hasNext()) {
            QRegularExpressionMatch propertyMatch = j.next();
            QString property = propertyMatch.captured(1).trimmed();
            QString value = propertyMatch.captured(2).trimmed();
            propertiesObj[property] = value;
        }

        jsonObj[selector] = propertiesObj;
    }

    QJsonDocument jsonDoc(jsonObj);
    return QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Indented));
}

QColor JsonParse::parseHighlightWKJson(const QString &filePath, const QString &colorType)
{
    // 获取 JSON 根对象
    QJsonObject rootObj = this->getJsonOBJFromFile(filePath);

    // 检查是否存在所需的颜色类型
    if (!rootObj.contains(colorType) || !rootObj[colorType].isObject()) {
        qWarning() << "颜色类型不存在:" << colorType;
        return QColor();
    }

    // 获取颜色对象
    QJsonObject colorObj = rootObj[colorType].toObject();

    // 解析 r, g, b, a 值
    int r = colorObj.value("r").toInt(0);
    int g = colorObj.value("g").toInt(0);
    int b = colorObj.value("b").toInt(0);
    int a = colorObj.value("a").toInt(255);  // 默认 alpha 值为 255（不透明）

    // 返回 QColor
    return QColor(r, g, b, a);
}

QJsonObject JsonParse::getJsonOBJFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件:" << filePath;
        return QJsonObject();  // 返回空对象
    }

    QByteArray jsonData = file.readAll();
    file.close();

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON 解析错误:" << parseError.errorString();
        return QJsonObject();
    }

    // 返回 JSON 对象
    return doc.object();
}

LineNumberColor JsonParse::parseLineNumberJsonGetAllColor(const QString &filePath, const QString &colorSet)
{
    LineNumberColor color;

    QJsonObject rootObj = this->getJsonOBJFromFile(filePath);

    // 检查是否包含指定的颜色集
    if (!rootObj.contains(colorSet) || !rootObj[colorSet].isObject()) {
        return color;  // 返回默认的 LineNumberColor 结构体（未找到指定颜色集）
    }

    QJsonObject colorObj = rootObj[colorSet].toObject();

    // 辅助函数：从 JSON 对象中提取 QColor
    auto getColorFromJson = [](const QJsonObject &obj, const QString &key) -> QColor {
        if (obj.contains(key) && obj[key].isObject()) {
            QJsonObject colorValue = obj[key].toObject();
            int r = colorValue["r"].toInt(0);
            int g = colorValue["g"].toInt(0);
            int b = colorValue["b"].toInt(0);
            int a = colorValue["a"].toInt(255);  // 默认 alpha 值为 255
            return QColor(r, g, b, a);
        }
        return QColor();  // 返回默认颜色（透明）
    };
    // 设置结构体中的颜色
    QJsonDocument jsonDoc(rootObj);
    color.jsonData = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Indented));
    color.selectedColor = getColorFromJson(colorObj, "selectedColor");
    color.noSelectedColor = getColorFromJson(colorObj, "noSelectedColor");
    color.backgroundColor = getColorFromJson(colorObj, "backgroundColor");

    return color;
}

HighlighterData JsonParse::parseHighlighterColorsFromFile(const QString &filePath)
{
    QJsonObject jsonObj = this->getJsonOBJFromFile(filePath);
    HighlighterData data;

    if (jsonObj.isEmpty()) {
        return data;
    }

    data.jsonData = QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);

    // 遍历 JSON 对象的键值对
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        if (it.value().isObject()) {
            QJsonObject colorObj = it.value().toObject();
            int r = colorObj["r"].toInt();
            int g = colorObj["g"].toInt();
            int b = colorObj["b"].toInt();
            int a = colorObj["a"].toInt();
            QColor color(r, g, b, a);
            data.highlighterColors.insert(it.key(), color);
        }
    }

    return data;
}

