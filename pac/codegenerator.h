#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDomDocument>
#include <QDomElement>

class CodeGenerator : public QObject
{
    Q_OBJECT
public:
    explicit CodeGenerator(QObject *parent = nullptr);
    bool loadInputFile(const QString& filename);
    bool generateCode(QString etcsLevel, QStringList addList = QStringList(), QStringList removeList = QStringList(), bool skipDefault = false);
    bool writeOutputFile(const QString& filename);
    void setInputDomDocument(QDomDocument doc);
    void setContainerPath(QString cpath);
    QString getCodeString();

signals:

public slots:

private:
    QString getIdentitaet(QDomElement e);

    QString generateUrObjektCode(QDomElement e, int varIndex);
    QString generateBasisObjektCode(QDomElement e, int varIndex);
    QString generatePunktObjektCode(QDomElement e, int varIndex);
    QString generateBereichObjektCode(QDomElement e, int varIndex);

    QDomDocument domDocument;
    QString codeString;
    QString containerPath;
};

#endif // CODEGENERATOR_H
