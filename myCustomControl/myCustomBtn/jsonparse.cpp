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
        QJsonObject style = jsonObj[key].toObject();
        qss += key + " {\n";
        foreach (const QString &attr , style.keys()) {
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
        // qWarning() << "无法打开 JSON 文件：" << jsonFilePath;
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
    static const QRegularExpression re(R"((\S+)\s*\{([^}]*)\})");
    static const QRegularExpression reProperty(R"((\S+)\s*:\s*([^;]+);?)");

    QJsonObject jsonObj;
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
        // qWarning() << "颜色类型不存在:" << colorType;
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
        // qWarning() << "无法打开文件:" << filePath;
        return QJsonObject();  // 返回空对象
    }

    QByteArray jsonData = file.readAll();
    file.close();

    // 解析 JSON 数据
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        // qWarning() << "JSON 解析错误:" << parseError.errorString();
        return QJsonObject();
    }

    // 返回 JSON 对象
    return doc.object();
}

LineNumberColor JsonParse::parseLineNumberJsonGetAllColor(const QString &filePath)
{
    LineNumberColor color;

    // 从文件读取 JSON 数据
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // qWarning() << "无法打开文件:" << filePath;
        return color;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        // qWarning() << "JSON解析错误:" << parseError.errorString();
        return color;
    }

    QJsonObject rootObj = jsonDoc.object();

    // 辅助lambda函数：从 JSON 对象中提取 QColor，如果缺少 alpha，默认设为255
    auto getColorFromJson = [](const QJsonObject &obj, const QString &key) -> QColor {
        if (obj.contains(key) && obj[key].isObject()) {
            QJsonObject colorObj = obj[key].toObject();
            int r = colorObj["r"].toInt(0);
            int g = colorObj["g"].toInt(0);
            int b = colorObj["b"].toInt(0);
            int a = colorObj.contains("a") ? colorObj["a"].toInt(255) : 255;
            return QColor(r, g, b, a);
        }
        return QColor();
    };

    // 将整个 JSON 文档以格式化字符串保存到结构体中（可选）
    color.jsonData = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Indented));

    // 提取对应的颜色
    color.selectedColor = getColorFromJson(rootObj, "selectedColor");
    color.noSelectedColor = getColorFromJson(rootObj, "noSelectedColor");
    color.backgroundColor = getColorFromJson(rootObj, "backgroundColor");

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

