#include <QtCore>
#include <QtXml>
#include "codegenerator.h"
#include "abstypes.h"

CodeGenerator::CodeGenerator(QObject *parent) : QObject(parent)
{
    domDocument = QDomDocument();
    codeString = QString();
    containerPath = QString("LST_Planung_Projekt/LST_Planung_Gruppe/LST_Planung_Einzel/LST_Zustand_Ziel/Container");
}

void CodeGenerator::setContainerPath(QString cpath)
{
    containerPath = cpath;
}

bool CodeGenerator::loadInputFile(const QString& filename)
{
    QFile dataFile(filename);
    if (!dataFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return false;
    }
    else
    {
        domDocument = QDomDocument("objectlist");

        if(!domDocument.setContent(&dataFile))
        {
            dataFile.close();
            return false;
        }
    }
    dataFile.close();
    codeString = QString();
    return true;
}

void CodeGenerator::setInputDomDocument(QDomDocument doc)
{
    domDocument = doc;
    codeString = QString();
}

QString CodeGenerator::getCodeString()
{
    return codeString;
}

bool CodeGenerator::writeOutputFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      return false;
    }
    else
    {
      QTextStream ts(&file);
      ts.setCodec("UTF-8");
      ts << codeString;
      file.close();
    }

    return true;
}

bool CodeGenerator::generateCode(QString etcsLevel, QStringList addList, QStringList removeList, bool skipDefault)
{
    QDomElement containerElement = domDocument.documentElement();
    QStringList domPath = containerPath.split("/");
    for(int i = 0; i < domPath.size(); i++)
    {
        containerElement = containerElement.firstChildElement(domPath.at(i));
    }

    codeString =
    QString("module Test;\n\n") +
    QString("export *;\n") +
    QString("import * from Metaobject;\n") +
    QString("import * from BasisTypen;\n") +
    QString("import * from Basisobjekte;\n") +
    QString("import * from Geodaten;\n") +
    QString("import * from Signale;\n") +
    QString("import * from ETCS;\n") +
    QString("import * from Block;\n") +
    QString("import * from Util;\n") +
    QString("import * from XmlWriter;\n") +
    QString("import * from Constructor;\n\n") +
    QString("interface Test {\n") +
    QString("  Unit doNothing();\n") +
    QString("}\n\n") +
    QString("class TestImpl implements Test {\n") +
    QString("  Unit doNothing() {\n") +
    QString("    println(\"nix\");\n") +
    QString("  }\n\n") +
    QString("  Unit run() {\n") +
    QString("    XmlWriter xmlw = new local XmlWriter();\n") +
    QString("    Constructor constructor = new local Constructor();\n") +
    QString("    Map<TGUID, PlanProObject> l = map(Nil);\n") +
    QString("    List<Punkt_Objekt> entrylist = Nil;\n") +
    QString("    List<Punkt_Objekt> exitlist = Nil;\n\n");

    int variableIndex = 1;
    QDomNode n = containerElement.firstChild();
    while (!n.isNull())
    {
        if (n.isElement())
        {
            QDomElement e = n.toElement();
            QString name = e.tagName();
            if(name == "Anhang" && (addList.contains("Anhang") || (!skipDefault && !removeList.contains("Anhang"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Anhang o%1 = new local AnhangImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                QDomElement anhangAllgElement = e.firstChildElement("Anhang_Allg");
                QDomElement anhangArtElement = anhangAllgElement.firstChildElement("Anhang_Art");
                QDomElement anhangArtWertElement = anhangArtElement.firstChildElement("Wert");
                QString anhangArtText = anhangArtWertElement.text();
                QDomElement dateinameElement = anhangAllgElement.firstChildElement("Dateiname");
                QDomElement dateinameWertElement = dateinameElement.firstChildElement("Wert");
                QString dateinameText = dateinameWertElement.text();
                QDomElement dateitypElement = anhangAllgElement.firstChildElement("Dateityp");
                QDomElement dateitypWertElement = dateitypElement.firstChildElement("Wert");
                QString dateitypText = dateitypWertElement.text();
                QDomElement datenElement = anhangAllgElement.firstChildElement("Daten");
                QDomElement datenWertElement = datenElement.firstChildElement("Wert");
                QString datenText = datenWertElement.text();
                codeString += QString("    o%1.setAnhang_Allg_Anhang_Art(ENUMAnhang_Art_%2);\n").arg(variableIndex).arg(anhangArtText);
                codeString += QString("    o%1.setAnhang_Allg_Dateiname(\"%2\");\n").arg(variableIndex).arg(dateinameText);
                codeString += QString("    o%1.setAnhang_Allg_Dateityp(ENUMDateityp_%2);\n").arg(variableIndex).arg(dateitypText);
                codeString += QString("    o%1.setAnhang_Allg_Daten(\"%2\");\n").arg(variableIndex).arg(datenText);
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Anhang(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Bearbeitungsvermerk" && (addList.contains("Bearbeitungsvermerk") || (!skipDefault && !removeList.contains("Bearbeitungsvermerk"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Bearbeitungsvermerk o%1 = new local BearbeitungsvermerkImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                QDomElement idAnhangElement = e.firstChildElement("ID_Anhang");
                if(!idAnhangElement.isNull())
                {
                    QDomElement idAnhangWertElement = idAnhangElement.firstChildElement("Wert");
                    QString idAnhangText = idAnhangWertElement.text();
                    codeString += QString("    o%1.setID_Anhang(Just(\"%2\"));\n").arg(variableIndex).arg(idAnhangText);
                }
                QDomElement bearbVermerkAllgElement = e.firstChildElement("Bearbeitungsvermerk_Allg");
                QDomElement rolleElement = bearbVermerkAllgElement.firstChildElement("Bearbeitungsvermerk_Rolle");
                QDomElement rolleWertElement = rolleElement.firstChildElement("Wert");
                QString rolleText = rolleWertElement.text();
                QDomElement kommentarElement = bearbVermerkAllgElement.firstChildElement("Kommentar");
                QDomElement kommentarWertElement = kommentarElement.firstChildElement("Wert");
                QString kommentarText = kommentarWertElement.text();
                QDomElement zeitElement = bearbVermerkAllgElement.firstChildElement("Zeit_Bearbeitungsvermerk");
                QDomElement zeitWertElement = zeitElement.firstChildElement("Wert");
                QString zeitText = zeitWertElement.text();
                codeString += QString("    o%1.setBearbeitungsvermerk_Allg_Bearbeitungsvermerk_Rolle(ENUMRolle_%2);\n").arg(variableIndex).arg(rolleText);
                codeString += QString("    o%1.setBearbeitungsvermerk_Allg_Kommentar(\"%2\");\n").arg(variableIndex).arg(kommentarText);
                codeString += QString("    o%1.setBearbeitungsvermerk_Allg_Zeit_Bearbeitungsvermerk(\"%2\");\n").arg(variableIndex).arg(zeitText);
                QDomElement kennungElement = bearbVermerkAllgElement.firstChildElement("Bearbeitungsvermerk_Kennung");
                if(!kennungElement.isNull())
                {
                    QDomElement kennungWertElement = kennungElement.firstChildElement("Wert");
                    QString kennungText = kennungWertElement.text();
                    codeString += QString("    o%1.setBearbeitungsvermerk_Allg_Bearbeitungsvermerk_Kennung(Just(\"%2\"));\n").arg(variableIndex).arg(kennungText);
                }
                QDomElement kurztextElement = bearbVermerkAllgElement.firstChildElement("Kurztext");
                if(!kurztextElement.isNull())
                {
                    QDomElement kurztextWertElement = kurztextElement.firstChildElement("Wert");
                    QString kurztextText = kurztextWertElement.text();
                    codeString += QString("    o%1.setBearbeitungsvermerk_Allg_Kurztext(Just(\"%2\"));\n").arg(variableIndex).arg(kurztextText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Bearbeitungsvermerk(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Proxy_Objekt" && (addList.contains("Proxy_Objekt") || (!skipDefault && !removeList.contains("Proxy_Objekt"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Proxy_Objekt o%1 = new local Proxy_ObjektImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                QDomElement nameLstObjektElement = e.firstChildElement("Name_LST_Objekt");
                QDomElement nameLstObjektWertElement = nameLstObjektElement.firstChildElement("Wert");
                QString nameLstObjektText = nameLstObjektWertElement.text();
                codeString += QString("    o%1.setName_LST_Objekt(ENUMName_LST_Objekt_%2);\n").arg(variableIndex).arg(nameLstObjektText);
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Proxy_Objekt(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "GEO_Kante" && (addList.contains("GEO_Kante") || (!skipDefault && !removeList.contains("GEO_Kante"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    GEO_Kante o%1 = new local GEO_KanteImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idGeoKnotenAElement = e.firstChildElement("ID_GEO_Knoten_A");
                QDomElement idGeoKnotenAWertElement = idGeoKnotenAElement.firstChildElement("Wert");
                QString idknotena = idGeoKnotenAWertElement.text();
                QDomElement idGeoKnotenBElement = e.firstChildElement("ID_GEO_Knoten_B");
                QDomElement idGeoKnotenBWertElement = idGeoKnotenBElement.firstChildElement("Wert");
                QString idknotenb = idGeoKnotenBWertElement.text();
                QDomElement idGeoArtElement = e.firstChildElement("ID_GEO_Art");
                QDomElement idGeoArtWertElement = idGeoArtElement.firstChildElement("Wert");
                QString idart = idGeoArtWertElement.text();
                codeString += QString("    o%1.setID_GEO_Knoten_A(\"%2\");\n").arg(variableIndex).arg(idknotena);
                codeString += QString("    o%1.setID_GEO_Knoten_B(\"%2\");\n").arg(variableIndex).arg(idknotenb);
                codeString += QString("    o%1.setID_GEO_Art(\"%2\");\n").arg(variableIndex).arg(idart);
                QDomElement kanteAllgElement = e.firstChildElement("GEO_Kante_Allg");
                QDomElement planQuelleElement = kanteAllgElement.firstChildElement("Plan_Quelle");
                QDomElement planQuelleWertElement = planQuelleElement.firstChildElement("Wert");
                QString planQuelleText = planQuelleWertElement.text();
                QDomElement laengeElement = kanteAllgElement.firstChildElement("GEO_Laenge");
                QDomElement laengeWertElement = laengeElement.firstChildElement("Wert");
                QString laenge = laengeWertElement.text();
                Rat laengeRat(laenge);
                codeString += QString("    o%1.setGEO_Kante_Allg_Plan_Quelle(ENUMPlan_Quelle_%2);\n").arg(variableIndex).arg(planQuelleText);
                codeString += QString("    o%1.setGEO_Kante_Allg_GEO_Laenge(%2);\n").arg(variableIndex).arg(laengeRat.toRatString());
                QDomElement radiusAElement = kanteAllgElement.firstChildElement("GEO_Radius_A");
                if(!radiusAElement.isNull())
                {
                    QDomElement radiusAWertElement = radiusAElement.firstChildElement("Wert");
                    QString radiusAText = radiusAWertElement.text();
                    Rat radiusARat(radiusAText);
                    codeString += QString("    o%1.setGEO_Kante_Allg_GEO_Radius_A(Just(%2));\n").arg(variableIndex).arg(radiusARat.toRatString());
                }
                QDomElement radiusBElement = kanteAllgElement.firstChildElement("GEO_Radius_B");
                if(!radiusBElement.isNull())
                {
                    QDomElement radiusBWertElement = radiusBElement.firstChildElement("Wert");
                    QString radiusBText = radiusBWertElement.text();
                    Rat radiusBRat(radiusBText);
                    codeString += QString("    o%1.setGEO_Kante_Allg_GEO_Radius_B(Just(%2));\n").arg(variableIndex).arg(radiusBRat.toRatString());
                }
                QDomElement richtungswinkelElement = kanteAllgElement.firstChildElement("GEO_Richtungswinkel");
                if(!richtungswinkelElement.isNull())
                {
                    QDomElement richtungswinkelWertElement = richtungswinkelElement.firstChildElement("Wert");
                    QString richtungswinkelText = richtungswinkelWertElement.text();
                    Rat richtungswinkelRat(richtungswinkelText);
                    codeString += QString("    o%1.setGEO_Kante_Allg_GEO_Richtungswinkel(Just(%2));\n").arg(variableIndex).arg(richtungswinkelRat.toRatString());
                }
                QDomElement geoFormElement = kanteAllgElement.firstChildElement("GEO_Form");
                if(!geoFormElement.isNull())
                {
                    QDomElement geoFormWertElement = geoFormElement.firstChildElement("Wert");
                    QString geoFormText = geoFormWertElement.text();
                    codeString += QString("    o%1.setGEO_Kante_Allg_GEO_Form(Just(ENUMGEO_Form_%2));\n").arg(variableIndex).arg(geoFormText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_GEO_Kante(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "GEO_Knoten" && (addList.contains("GEO_Knoten") || (!skipDefault && !removeList.contains("GEO_Knoten"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    GEO_Knoten o%1 = new local GEO_KnotenImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement geoPadElement = e.firstChildElement("GEO_PAD");
                if(!geoPadElement.isNull())
                {
                    QDomElement geoPadWertElement = geoPadElement.firstChildElement("Wert");
                    QString geoPadText = geoPadWertElement.text();
                    codeString += QString("    o%1.setGEO_PAD(Just(\"%2\"));\n").arg(variableIndex).arg(geoPadText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_GEO_Knoten(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "GEO_Punkt" && (addList.contains("GEO_Punkt") || (!skipDefault && !removeList.contains("GEO_Punkt"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    GEO_Punkt o%1 = new local GEO_PunktImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idGeoKnotenElement = e.firstChildElement("ID_GEO_Knoten");
                if(!idGeoKnotenElement.isNull())
                {
                    QDomElement idGeoKnotenWertElement = idGeoKnotenElement.firstChildElement("Wert");
                    QString idknoten = idGeoKnotenWertElement.text();
                    codeString += QString("    o%1.setID_GEO_Knoten(Just(\"%2\"));\n").arg(variableIndex).arg(idknoten);
                }
                QDomElement punktAllgElement = e.firstChildElement("GEO_Punkt_Allg");
                QDomElement planQuelleElement = punktAllgElement.firstChildElement("Plan_Quelle");
                QDomElement planQuelleWertElement = planQuelleElement.firstChildElement("Wert");
                QString planQuelleText = planQuelleWertElement.text();
                QDomElement gkxElement = punktAllgElement.firstChildElement("GK_X");
                QDomElement gkxWertElement = gkxElement.firstChildElement("Wert");
                QString x = gkxWertElement.text();
                Rat xRat(x);
                QDomElement gkyElement = punktAllgElement.firstChildElement("GK_Y");
                QDomElement gkyWertElement = gkyElement.firstChildElement("Wert");
                QString y = gkyWertElement.text();
                Rat yRat(y);
                codeString += QString("    o%1.setGEO_Punkt_Allg_Plan_Quelle(ENUMPlan_Quelle_%2);\n").arg(variableIndex).arg(planQuelleText);
                codeString += QString("    o%1.setGEO_Punkt_Allg_GK_X(%2);\n").arg(variableIndex).arg(xRat.toRatString());
                codeString += QString("    o%1.setGEO_Punkt_Allg_GK_Y(%2);\n").arg(variableIndex).arg(yRat.toRatString());
                QDomElement gkzElement = punktAllgElement.firstChildElement("GK_Z");
                if(!gkzElement.isNull())
                {
                    QDomElement gkzWertElement = gkzElement.firstChildElement("Wert");
                    QString z = gkzWertElement.text();
                    Rat zRat(z);
                    codeString += QString("    o%1.setGEO_Punkt_Allg_GK_Z(Just(%2));\n").arg(variableIndex).arg(zRat.toRatString());
                }
                QDomElement koorLsysElement = punktAllgElement.firstChildElement("GEO_KoordinatenSystem_LSys");
                if(!koorLsysElement.isNull())
                {
                    QDomElement koorLsysWertElement = koorLsysElement.firstChildElement("Wert");
                    QString koorLsysText = koorLsysWertElement.text();
                    codeString += QString("    o%1.setGEO_Punkt_Allg_GEO_KoordinatenSystem_LSys(Just(\"%2\"));\n").arg(variableIndex).arg(koorLsysText);
                }
                QDomElement koorSonstElement = punktAllgElement.firstChildElement("GEO_KoordinatenSystem_Sonstige");
                if(!koorSonstElement.isNull())
                {
                    QDomElement koorSonstWertElement = koorSonstElement.firstChildElement("Wert");
                    QString koorSonstText = koorSonstWertElement.text();
                    codeString += QString("    o%1.setGEO_Punkt_Allg_GEO_KoordinatenSystem_Sonstige(Just(\"%2\"));\n").arg(variableIndex).arg(koorSonstText);
                }
                codeString +=  QString("    l = insert(l, Pair(\"%1\", Objecttype_GEO_Punkt(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Geschwindigkeitsprofil" && (addList.contains("Geschwindigkeitsprofil") || (!skipDefault && !removeList.contains("Geschwindigkeitsprofil"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Geschwindigkeitsprofil o%1 = new local GeschwindigkeitsprofilImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generateBereichObjektCode(e, variableIndex);
                QDomElement geschwProfilAllgElement = e.firstChildElement("Geschwindigkeitsprofil_Allg");
                QDomElement geschwindigkeitElement = geschwProfilAllgElement.firstChildElement("Geschwindigkeit");
                QDomElement geschwindigkeitWertElement = geschwindigkeitElement.firstChildElement("Wert");
                QString geschwindigkeitText = geschwindigkeitWertElement.text();
                QDomElement profilArtElement = geschwProfilAllgElement.firstChildElement("V_Profil_Art");
                QDomElement profilArtWertElement = profilArtElement.firstChildElement("Wert");
                QString profilArtText = profilArtWertElement.text();
                QDomElement wirkrichtungElement = geschwProfilAllgElement.firstChildElement("Wirkrichtung");
                QDomElement wirkrichtungWertElement = wirkrichtungElement.firstChildElement("Wert");
                QString wirkrichtungText = wirkrichtungWertElement.text();
                codeString += QString("    o%1.setGeschwindigkeitsprofil_Allg_Geschwindigkeit(%2);\n").arg(variableIndex).arg(geschwindigkeitText);
                codeString += QString("    o%1.setGeschwindigkeitsprofil_Allg_V_Profil_Art(ENUMV_Profil_Art_%2);\n").arg(variableIndex).arg(profilArtText);
                codeString += QString("    o%1.setGeschwindigkeitsprofil_Allg_Wirkrichtung(ENUMWirkrichtung_%2);\n").arg(variableIndex).arg(wirkrichtungText);
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Geschwindigkeitsprofil(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Hoehenpunkt" && (addList.contains("Hoehenpunkt") || (!skipDefault && !removeList.contains("Hoehenpunkt"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Hoehenpunkt o%1 = new local HoehenpunktImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generatePunktObjektCode(e, variableIndex);
                QDomElement hoehenpunktAllgElement = e.firstChildElement("Hoehenpunkt_Allg");
                QDomElement hoeheElement = hoehenpunktAllgElement.firstChildElement("Hoehenpunkt_Hoehe");
                QDomElement hoeheWertElement = hoeheElement.firstChildElement("Wert");
                QString hoeheText = hoeheWertElement.text();
                Rat hoeheRat(hoeheText);
                codeString += QString("    o%1.setHoehenpunkt_Allg_Hoehenpunkt_Hoehe(%2);\n").arg(variableIndex).arg(hoeheRat.toRatString());
                QDomElement geoPadElement = hoehenpunktAllgElement.firstChildElement("GEO_PAD");
                if(!geoPadElement.isNull())
                {
                    QDomElement geoPadWertElement = geoPadElement.firstChildElement("Wert");
                    QString geoPadText = geoPadWertElement.text();
                    codeString += QString("    o%1.setHoehenpunkt_Allg_GEO_PAD(Just(\"%2\"));\n").arg(variableIndex).arg(geoPadText);
                }
                QDomElement datumElement = hoehenpunktAllgElement.firstChildElement("Hoehenpunkt_Datum");
                if(!datumElement.isNull())
                {
                    QDomElement datumWertElement = datumElement.firstChildElement("Wert");
                    QString datumText = datumWertElement.text();
                    codeString += QString("    o%1.setHoehenpunkt_Allg_Hoehenpunkt_Datum(Just(\"%2\"));\n").arg(variableIndex).arg(datumText);
                }
                QDomElement hSystemElement = hoehenpunktAllgElement.firstChildElement("HSystem");
                if(!hSystemElement.isNull())
                {
                    QDomElement hSystemWertElement = hSystemElement.firstChildElement("Wert");
                    QString hSystemText = hSystemWertElement.text();
                    codeString += QString("    o%1.setHoehenpunkt_Allg_HSystem(Just(ENUMHSystem_%2));\n").arg(variableIndex).arg(hSystemText);
                }
                QDomElement planQuelleElement = hoehenpunktAllgElement.firstChildElement("Plan_Quelle");
                if(!planQuelleElement.isNull())
                {
                    QDomElement planQuelleWertElement = planQuelleElement.firstChildElement("Wert");
                    QString planQuelleText = planQuelleWertElement.text();
                    codeString += QString("    o%1.setHoehenpunkt_Allg_Plan_Quelle(Just(ENUMPlan_Quelle_%2));\n").arg(variableIndex).arg(planQuelleText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Hoehenpunkt(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Oertlichkeit" && (addList.contains("Oertlichkeit") || (!skipDefault && !removeList.contains("Oertlichkeit"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Oertlichkeit o%1 = new local OertlichkeitImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idOertlichkeitElement = e.firstChildElement("ID_Oertlichkeit");
                if(!idOertlichkeitElement.isNull())
                {
                    QDomElement idOertlichkeitWertElement = idOertlichkeitElement.firstChildElement("Wert");
                    QString idOertlichkeitText = idOertlichkeitWertElement.text();
                    codeString += QString("    o%1.setID_Oertlichkeit(Just(\"%2\"));\n").arg(variableIndex).arg(idOertlichkeitText);
                }
                codeString += QString("    List<TGUID> splist_o%1 = Nil;\n").arg(variableIndex);
                QDomElement idStreckePunktElement = e.firstChildElement("ID_Strecke_Punkt");
                while(!idStreckePunktElement.isNull())
                {
                    QDomElement idStreckePunktWertElement = idStreckePunktElement.firstChildElement("Wert");
                    QString  idStreckePunktText = idStreckePunktWertElement.text();
                    codeString += QString("    splist_o%1 = appendright(splist_o%1, \"%2\");\n").arg(variableIndex).arg(idStreckePunktText);

                    idStreckePunktElement = idStreckePunktElement.nextSiblingElement("ID_Strecke_Punkt");
                }
                codeString += QString("    o%1.setID_Strecke_Punkt(splist_o%1);\n").arg(variableIndex);
                QDomElement oertlichkeitAllgElement = e.firstChildElement("Oertlichkeit_Allg");
                QDomElement langnameElement = oertlichkeitAllgElement.firstChildElement("Oertlichkeit_Langname");
                QDomElement langnameWertElement = langnameElement.firstChildElement("Wert");
                QString langnameText = langnameWertElement.text();
                codeString += QString("    o%1.setOertlichkeit_Allg_Oertlichkeit_Langname(\"%2\");\n").arg(variableIndex).arg(langnameText);
                QDomElement abkuerzungElement = oertlichkeitAllgElement.firstChildElement("Oertlichkeit_Abkuerzung");
                if(!abkuerzungElement.isNull())
                {
                    QDomElement abkuerzungWertElement = abkuerzungElement.firstChildElement("Wert");
                    QString abkuerzungText = abkuerzungWertElement.text();
                    codeString += QString("    o%1.setOertlichkeit_Allg_Oertlichkeit_Abkuerzung(Just(\"%2\"));\n").arg(variableIndex).arg(abkuerzungText);
                }
                QDomElement gueltigAbElement = oertlichkeitAllgElement.firstChildElement("Oertlichkeit_Gueltig_Ab");
                if(!gueltigAbElement.isNull())
                {
                    QDomElement gueltigAbWertElement = gueltigAbElement.firstChildElement("Wert");
                    QString gueltigAbText = gueltigAbWertElement.text();
                    codeString += QString("    o%1.setOertlichkeit_Allg_Oertlichkeit_Gueltig_Ab(Just(\"%2\"));\n").arg(variableIndex).arg(gueltigAbText);
                }
                QDomElement gueltigBisElement = oertlichkeitAllgElement.firstChildElement("Oertlichkeit_Gueltig_Bis");
                if(!gueltigBisElement.isNull())
                {
                    QDomElement gueltigBisWertElement = gueltigBisElement.firstChildElement("Wert");
                    QString gueltigBisText = gueltigBisWertElement.text();
                    codeString += QString("    o%1.setOertlichkeit_Allg_Oertlichkeit_Gueltig_Bis(Just(\"%2\"));\n").arg(variableIndex).arg(gueltigBisText);
                }
                QDomElement kurznameElement = oertlichkeitAllgElement.firstChildElement("Oertlichkeit_Kurzname");
                if(!kurznameElement.isNull())
                {
                    QDomElement kurznameWertElement = kurznameElement.firstChildElement("Wert");
                    QString kurznameText = kurznameWertElement.text();
                    codeString += QString("    o%1.setOertlichkeit_Allg_Oertlichkeit_Kurzname(Just(\"%2\"));\n").arg(variableIndex).arg(kurznameText);
                }
                QDomElement oertlichkeitArtElement = oertlichkeitAllgElement.firstChildElement("Oertlichkeit_Art");
                if(!oertlichkeitArtElement.isNull())
                {
                    QDomElement oertlichkeitArtWertElement = oertlichkeitArtElement.firstChildElement("Wert");
                    QString oertlichkeitArtText = oertlichkeitArtWertElement.text();
                    codeString += QString("    o%1.setOertlichkeit_Allg_Oertlichkeit_Art(Just(ENUMOertlichkeit_Art_%2));\n").arg(variableIndex).arg(oertlichkeitArtText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Oertlichkeit(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Strecke" && (addList.contains("Strecke") || (!skipDefault && !removeList.contains("Strecke"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Strecke o%1 = new local StreckeImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generateBereichObjektCode(e, variableIndex);
                QDomElement bezeichnungElement = e.firstChildElement("Bezeichnung");
                QDomElement bezStreckeElement = bezeichnungElement.firstChildElement("Bezeichnung_Strecke");
                QDomElement bezStreckeWertElement = bezStreckeElement.firstChildElement("Wert");
                QString bezStreckeText = bezStreckeWertElement.text();
                codeString += QString("    o%1.setBezeichnung_Bezeichnung_Strecke(\"%2\");\n").arg(variableIndex).arg(bezStreckeText);
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Strecke(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Strecke_Punkt" && (addList.contains("Strecke_Punkt") || (!skipDefault && !removeList.contains("Strecke_Punkt"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Strecke_Punkt o%1 = new local Strecke_PunktImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idStreckeElement = e.firstChildElement("ID_Strecke");
                QDomElement idStreckeWertElement = idStreckeElement.firstChildElement("Wert");
                QString idStreckeText = idStreckeWertElement.text();
                QDomElement streckeMeterElement = e.firstChildElement("Strecke_Meter");
                QDomElement streckeMeterWertElement = streckeMeterElement.firstChildElement("Wert");
                QString streckeMeterText = streckeMeterWertElement.text();
                Rat streckeMeterRat(streckeMeterText);
                codeString += QString("    o%1.setID_Strecke(\"%2\");\n").arg(variableIndex).arg(idStreckeText);
                codeString += QString("    o%1.setStrecke_Meter(%2);\n").arg(variableIndex).arg(streckeMeterRat.toRatString());
                QDomElement idGeoKnotenElement = e.firstChildElement("ID_GEO_Knoten");
                if(!idGeoKnotenElement.isNull())
                {
                    QDomElement idGeoKnotenWertElement = idGeoKnotenElement.firstChildElement("Wert");
                    QString idGeoKnotenText = idGeoKnotenWertElement.text();
                    codeString += QString("    o%1.setID_GEO_Knoten(Just(\"%2\"));\n").arg(variableIndex).arg(idGeoKnotenText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Strecke_Punkt(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Technischer_Bereich" && (addList.contains("Technischer_Bereich") || (!skipDefault && !removeList.contains("Technischer_Bereich"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Technischer_Bereich o%1 = new local Technischer_BereichImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generateBereichObjektCode(e, variableIndex);
                QDomElement beschreibungElement = e.firstChildElement("TB_Beschreibung");
                QDomElement beschreibungWertElement = beschreibungElement.firstChildElement("Wert");
                QString beschreibungText = beschreibungWertElement.text();
                codeString += QString("    o%1.setTB_Beschreibung(\"%2\");\n").arg(variableIndex).arg(beschreibungText);
                QDomElement artElement = e.firstChildElement("TB_Art");
                if(!artElement.isNull())
                {
                    QDomElement artWertElement = artElement.firstChildElement("Wert");
                    QString artText = artWertElement.text();
                    codeString += QString("    o%1.setTB_Art(Just(ENUMTB_Art_%2));\n").arg(variableIndex).arg(artText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Technischer_Bereich(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Technischer_Punkt" && (addList.contains("Technischer_Punkt") || (!skipDefault && !removeList.contains("Technischer_Punkt"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Technischer_Punkt o%1 = new local Technischer_PunktImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generatePunktObjektCode(e, variableIndex);
                QDomElement beschreibungElement = e.firstChildElement("TP_Beschreibung");
                QDomElement beschreibungWertElement = beschreibungElement.firstChildElement("Wert");
                QString beschreibungText = beschreibungWertElement.text();
                codeString += QString("    o%1.setTP_Beschreibung(\"%2\");\n").arg(variableIndex).arg(beschreibungText);
                QDomElement artElement = e.firstChildElement("TP_Art");
                if(!artElement.isNull())
                {
                    QDomElement artWertElement = artElement.firstChildElement("Wert");
                    QString artText = artWertElement.text();
                    codeString += QString("    o%1.setTP_Art(Just(ENUMTP_Art_%2));\n").arg(variableIndex).arg(artText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Technischer_Punkt(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "TOP_Knoten" && (addList.contains("TOP_Knoten") || (!skipDefault && !removeList.contains("TOP_Knoten"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    TOP_Knoten o%1 = new local TOP_KnotenImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idGeoKnotenElement = e.firstChildElement("ID_GEO_Knoten");
                QDomElement idGeoKnotenWertElement = idGeoKnotenElement.firstChildElement("Wert");
                QString idknoten = idGeoKnotenWertElement.text();
                codeString += QString("    o%1.setID_GEO_Knoten(\"%2\");\n").arg(variableIndex).arg(idknoten);
                QDomElement topKnotenAllgElement = e.firstChildElement("TOP_Knoten_Allg");
                if(!topKnotenAllgElement.isNull())
                {
                    QDomElement artBesondersElement = topKnotenAllgElement.firstChildElement("Art_Besonders");
                    if(!artBesondersElement.isNull())
                    {
                        QDomElement artBesondersWertElement = artBesondersElement.firstChildElement("Wert");
                        QString artBesondersText = artBesondersWertElement.text();
                        codeString += QString("    o%1.setTOP_Knoten_Allg_Art_Besonders(Just(ENUMArt_Besonders_%2));\n").arg(variableIndex).arg(artBesondersText);
                    }
                    QDomElement knotennameElement = topKnotenAllgElement.firstChildElement("Knotenname");
                    if(!knotennameElement.isNull())
                    {
                        QDomElement knotennameWertElement = knotennameElement.firstChildElement("Wert");
                        QString knotennameText = knotennameWertElement.text();
                        codeString += QString("    o%1.setTOP_Knoten_Allg_Knotenname(Just(\"%2\"));\n").arg(variableIndex).arg(knotennameText);
                    }
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_TOP_Knoten(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "TOP_Kante" && (addList.contains("TOP_Kante") || (!skipDefault && !removeList.contains("TOP_Kante"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    TOP_Kante o%1 = new local TOP_KanteImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idTopKnotenAElement = e.firstChildElement("ID_TOP_Knoten_A");
                QDomElement idTopKnotenAWertElement = idTopKnotenAElement.firstChildElement("Wert");
                QString idknotena = idTopKnotenAWertElement.text();
                QDomElement idTopKnotenBElement = e.firstChildElement("ID_TOP_Knoten_B");
                QDomElement idTopKnotenBWertElement = idTopKnotenBElement.firstChildElement("Wert");
                QString idknotenb = idTopKnotenBWertElement.text();
                QDomElement kanteAllgElement = e.firstChildElement("TOP_Kante_Allg");
                QDomElement anschlussAElement = kanteAllgElement.firstChildElement("TOP_Anschluss_A");
                QDomElement anschlussAWertElement = anschlussAElement.firstChildElement("Wert");
                QString anschlussa = anschlussAWertElement.text();
                QDomElement anschlussBElement = kanteAllgElement.firstChildElement("TOP_Anschluss_B");
                QDomElement anschlussBWertElement = anschlussBElement.firstChildElement("Wert");
                QString anschlussb = anschlussBWertElement.text();
                QDomElement laengeElement = kanteAllgElement.firstChildElement("TOP_Laenge");
                QDomElement laengeWertElement = laengeElement.firstChildElement("Wert");
                QString laenge = laengeWertElement.text();
                Rat laengeRat(laenge);
                codeString += QString("    o%1.setID_TOP_Knoten_A(\"%2\");\n").arg(variableIndex).arg(idknotena);
                codeString += QString("    o%1.setID_TOP_Knoten_B(\"%2\");\n").arg(variableIndex).arg(idknotenb);
                codeString += QString("    o%1.setTOP_Kante_Allg_TOP_Anschluss_A(ENUMTOP_Anschluss_%2);\n").arg(variableIndex).arg(anschlussa);
                codeString += QString("    o%1.setTOP_Kante_Allg_TOP_Anschluss_B(ENUMTOP_Anschluss_%2);\n").arg(variableIndex).arg(anschlussb);
                codeString += QString("    o%1.setTOP_Kante_Allg_TOP_Laenge(%2);\n").arg(variableIndex).arg(laengeRat.toRatString());
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_TOP_Kante(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Signal" && (addList.contains("Signal") || (!skipDefault && !removeList.contains("Signal"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Signal o%1 = new local SignalImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generatePunktObjektCode(e, variableIndex);
                QDomElement bezeichnungElement = e.firstChildElement("Bezeichnung");
                if(!bezeichnungElement.isNull())
                {
                    QDomElement bezeichnungLageplanKurzElement = bezeichnungElement.firstChildElement("Bezeichnung_Lageplan_Kurz");
                    QDomElement bezeichnungLageplanKurzWertElement = bezeichnungLageplanKurzElement.firstChildElement("Wert");
                    QString bezeichnunglageplankurztext = bezeichnungLageplanKurzWertElement.text();
                    QDomElement bezeichnungLageplanLangElement = bezeichnungElement.firstChildElement("Bezeichnung_Lageplan_Lang");
                    QDomElement bezeichnungLageplanLangWertElement = bezeichnungLageplanLangElement.firstChildElement("Wert");
                    QString bezeichnunglageplanlangtext = bezeichnungLageplanLangWertElement.text();
                    QDomElement bezeichnungTabelleElement = bezeichnungElement.firstChildElement("Bezeichnung_Tabelle");
                    QDomElement bezeichnungTabelleWertElement = bezeichnungTabelleElement.firstChildElement("Wert");
                    QString bezeichnungtabelletext = bezeichnungTabelleWertElement.text();
                    QDomElement bezeichnungAussenanlageElement = bezeichnungElement.firstChildElement("Bezeichnung_Aussenanlage");
                    QDomElement bezeichnungAussenanlageWertElement = bezeichnungAussenanlageElement.firstChildElement("Wert");
                    QString bezeichnungaussenanlagetext = bezeichnungAussenanlageWertElement.text();
                    codeString += QString("    CBezeichnung_Element bez_o%1 = new local CBezeichnung_Element();\n").arg(variableIndex);
                    codeString += QString("    bez_o%1.setBezeichnung_Lageplan_Kurz(\"%2\");\n").arg(variableIndex).arg(bezeichnunglageplankurztext);
                    codeString += QString("    bez_o%1.setBezeichnung_Lageplan_Lang(\"%2\");\n").arg(variableIndex).arg(bezeichnunglageplanlangtext);
                    codeString += QString("    bez_o%1.setBezeichnung_Tabelle(\"%2\");\n").arg(variableIndex).arg(bezeichnungtabelletext);
                    codeString += QString("    bez_o%1.setBezeichnung_Aussenanlage(\"%2\");\n").arg(variableIndex).arg(bezeichnungaussenanlagetext);
                    QDomElement kennzahlElement = bezeichnungElement.firstChildElement("Kennzahl");
                    if(!kennzahlElement.isNull())
                    {
                        QDomElement kennzahlWertElement = kennzahlElement.firstChildElement("Wert");
                        QString kennzahlText = kennzahlWertElement.text();
                        codeString += QString("    bez_o%1.setKennzahl(\"%2\");\n").arg(variableIndex).arg(kennzahlText); // No Just() constructor (empty string as default)
                    }
                    QDomElement elemBezeichnerElement = bezeichnungElement.firstChildElement("Oertlicher_Elementbezeichner");
                    if(!elemBezeichnerElement.isNull())
                    {
                        QDomElement elemBezeichnerWertElement = elemBezeichnerElement.firstChildElement("Wert");
                        QString elemBezeichnerText = elemBezeichnerWertElement.text();
                        codeString += QString("    bez_o%1.setOertlicher_Elementbezeichner(\"%2\");\n").arg(variableIndex).arg(elemBezeichnerText); // No Just() constructor (empty string as default)
                    }
                    codeString += QString("    o%1.setBezeichnung(Just(bez_o%1));\n").arg(variableIndex);
                }
                QDomElement fstrInselgleisElement = e.firstChildElement("Signal_Fstr_Aus_Inselgleis");
                if(!fstrInselgleisElement.isNull())
                {
                    QDomElement raVerbotElement = fstrInselgleisElement.firstChildElement("ID_RaFahrt_Gleichzeitig_Verbot");
                    if(!raVerbotElement.isNull())
                    {
                        QDomElement raVerbotWertElement = raVerbotElement.firstChildElement("Wert");
                        QString raVerbotText = raVerbotWertElement.text();
                        codeString += QString("    o%1.setSignal_Fstr_Aus_Inselgleis_ID_RaFahrt_Gleichzeitig_Verbot(Just(\"%2\"));\n").arg(variableIndex).arg(raVerbotText);
                    }
                    QDomElement zgVerbotElement = fstrInselgleisElement.firstChildElement("ID_ZgFahrt_Gleichzeitig_Verbot");
                    if(!zgVerbotElement.isNull())
                    {
                        QDomElement zgVerbotWertElement = zgVerbotElement.firstChildElement("Wert");
                        QString zgVerbotText = zgVerbotWertElement.text();
                        codeString += QString("    o%1.setSignal_Fstr_Aus_Inselgleis_ID_ZgFahrt_Gleichzeitig_Verbot(Just(\"%2\"));\n").arg(variableIndex).arg(zgVerbotText);
                    }
                }
                QDomElement fstrSElement = e.firstChildElement("Signal_Fstr_S");
                if(!fstrSElement.isNull())
                {
                    QDomElement gegengleisElement = fstrSElement.firstChildElement("Gegengleis");
                    if(!gegengleisElement.isNull())
                    {
                        QDomElement gegengleisWertElement = gegengleisElement.firstChildElement("Wert");
                        QString gegengleisText = gegengleisWertElement.text();
                        QString gegengleisBoolString = QString();
                        if(gegengleisText == "True" || gegengleisText == "true")
                        {
                            gegengleisBoolString = "True";
                        }
                        else
                        {
                            gegengleisBoolString = "False";
                        }
                        codeString += QString("    o%1.setSignal_Fstr_S_Gegengleis(Just(%2));\n").arg(variableIndex).arg(gegengleisBoolString);
                    }
                    QDomElement anrueckverschlussElement = fstrSElement.firstChildElement("ID_Anrueckverschluss");
                    if(!anrueckverschlussElement.isNull())
                    {
                        QDomElement anrueckverschlussWertElement = anrueckverschlussElement.firstChildElement("Wert");
                        QString anrueckverschlussText = anrueckverschlussWertElement.text();
                        codeString += QString("    o%1.setSignal_Fstr_S_ID_Anrueckverschluss(Just(\"%2\"));\n").arg(variableIndex).arg(anrueckverschlussText);
                    }
                }
                QDomElement fstrElement = e.firstChildElement("Signal_Fstr");
                if(!fstrElement.isNull())
                {
                    QDomElement daManuellElement = fstrElement.firstChildElement("DA_Manuell");
                    if(!daManuellElement.isNull())
                    {
                        QDomElement daManuellWertElement = daManuellElement.firstChildElement("Wert");
                        QString daManuellText = daManuellWertElement.text();
                        QString daManuellBoolString = QString();
                        if(daManuellText == "True" || daManuellText == "true")
                        {
                            daManuellBoolString = "True";
                        }
                        else
                        {
                            daManuellBoolString = "False";
                        }
                        codeString += QString("    o%1.setSignal_Fstr_DA_Manuell(Just(%2));\n").arg(variableIndex).arg(daManuellBoolString);
                    }
                    QDomElement durchfahrtElement = fstrElement.firstChildElement("Durchfahrt");
                    if(!durchfahrtElement.isNull())
                    {
                        QDomElement durchfahrtWertElement = durchfahrtElement.firstChildElement("Wert");
                        QString durchfahrtText = durchfahrtWertElement.text();
                        codeString += QString("    o%1.setSignal_Fstr_Durchfahrt(Just(ENUMDurchfahrt_%2));\n").arg(variableIndex).arg(durchfahrtText);
                    }
                    QDomElement raZielElement = fstrElement.firstChildElement("ID_RaZiel_Erlaubnisabhaengig");
                    if(!raZielElement.isNull())
                    {
                        QDomElement raZielWertElement = raZielElement.firstChildElement("Wert");
                        QString raZielText = raZielWertElement.text();
                        codeString += QString("    o%1.setSignal_Fstr_ID_RaZiel_Erlaubnisabhaengig(Just(\"%2\"));\n").arg(variableIndex).arg(raZielText);
                    }
                    QDomElement restaufloesungElement = fstrElement.firstChildElement("Rangierstrasse_Restaufloesung");
                    if(!restaufloesungElement.isNull())
                    {
                        QDomElement restaufloesungWertElement = restaufloesungElement.firstChildElement("Wert");
                        QString restaufloesungText = restaufloesungWertElement.text();
                        QString restaufloesungBoolString = QString();
                        if(restaufloesungText == "True" || restaufloesungText == "true")
                        {
                            restaufloesungBoolString = "True";
                        }
                        else
                        {
                            restaufloesungBoolString = "False";
                        }
                        codeString += QString("    o%1.setSignal_Fstr_Rangierstrasse_Restaufloesung(Just(%2));\n").arg(variableIndex).arg(restaufloesungBoolString);
                    }
                }
                QDomElement signalPzbElement = e.firstChildElement("Signal_PZB");
                if(!signalPzbElement.isNull())
                {
                    QDomElement schutzstreckeSollElement = signalPzbElement.firstChildElement("PZB_Schutzstrecke_Soll");
                    if(!schutzstreckeSollElement.isNull())
                    {
                        QDomElement schutzstreckeSollWertElement = schutzstreckeSollElement.firstChildElement("Wert");
                        QString schutzstreckeSollText = schutzstreckeSollWertElement.text();
                        Rat schutzstreckeSollRat(schutzstreckeSollText);
                        codeString += QString("    o%1.setSignal_PZB_PZB_Schutzstrecke_Soll(Just(%2));\n").arg(variableIndex).arg(schutzstreckeSollRat.toRatString());
                    }
                }

                QDomElement signalFiktivElement = e.firstChildElement("Signal_Fiktiv");
                if(!signalFiktivElement.isNull())
                {
                    QDomElement fiktivesSignalFunktionElement = signalFiktivElement.firstChildElement("Fiktives_Signal_Funktion");
                    QDomElement fiktivesSignalFunktionWertElement = fiktivesSignalFunktionElement.firstChildElement("Wert");
                    QString fiktivesSignalFunktionText = fiktivesSignalFunktionWertElement.text();
                    codeString += QString("    Signal_Fiktiv sigfik_o%1 = new local Signal_Fiktiv();\n").arg(variableIndex);
                    codeString += QString("    sigfik_o%1.setFiktives_Signal_Funktion(ENUMFiktives_Signal_Funktion_%2);\n").arg(variableIndex).arg(fiktivesSignalFunktionText);
                    QDomElement autoEinstellungElement = signalFiktivElement.firstChildElement("Auto_Einstellung");
                    if(!autoEinstellungElement.isNull())
                    {
                        QDomElement autoEinstellungWertElement = autoEinstellungElement.firstChildElement("Wert");
                        QString autoEinstellungText = autoEinstellungWertElement.text();
                        codeString += QString("    sigfik_o%1.setAuto_Einstellung(Just(ENUMAuto_Einstellung_%2));\n").arg(variableIndex).arg(autoEinstellungText);
                    }
                    codeString += QString("    SignalFiktivReal sfikreal_o%1 = SignalFiktiv(sigfik_o%1);\n").arg(variableIndex);
                    codeString += QString("    o%1.setSignalFiktivReal(sfikreal_o%1);\n").arg(variableIndex);
                }
                QDomElement signalRealElement = e.firstChildElement("Signal_Real");
                if(!signalRealElement.isNull())
                {
                    QDomElement signalBefestigungsartElement = signalRealElement.firstChildElement("Signal_Befestigungsart");
                    QDomElement signalBefestigungsartWertElement = signalBefestigungsartElement.firstChildElement("Wert");
                    QString signalBefestigungsartText = signalBefestigungsartWertElement.text();
                    codeString += QString("    Signal_Real sigreal_o%1 = new local Signal_Real();\n").arg(variableIndex);
                    codeString += QString("    sigreal_o%1.setSignal_Befestigungsart(ENUMSignal_Befestigungsart_%2);\n").arg(variableIndex).arg(signalBefestigungsartText);
                    QDomElement signalRealAktivSchirmElement = signalRealElement.firstChildElement("Signal_Real_Aktiv_Schirm");
                    if(!signalRealAktivSchirmElement.isNull())
                    {
                        QDomElement bedienartElement = signalRealAktivSchirmElement.firstChildElement("Bedienart");
                        if(!bedienartElement.isNull())
                        {
                            QDomElement bedienartWertElement = bedienartElement.firstChildElement("Wert");
                            QString bedienartText = bedienartWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Bedienart(Just(ENUMBedienart_%2));\n").arg(variableIndex).arg(bedienartText);
                        }
                        QDomElement richtpunktElement = signalRealAktivSchirmElement.firstChildElement("Richtpunkt");
                        if(!richtpunktElement.isNull())
                        {
                            QDomElement richtpunktWertElement = richtpunktElement.firstChildElement("Wert");
                            QString richtpunktText = richtpunktWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Richtpunkt(Just(\"%2\"));\n").arg(variableIndex).arg(richtpunktText);
                        }
                        QDomElement dunkelschaltungElement = signalRealAktivSchirmElement.firstChildElement("Dunkelschaltung");
                        if(!dunkelschaltungElement.isNull())
                        {
                            QDomElement dunkelschaltungWertElement = dunkelschaltungElement.firstChildElement("Wert");
                            QString dunkelschaltungText = dunkelschaltungWertElement.text();
                            QString dunkelschaltungBoolString = QString();
                            if(dunkelschaltungText == "True" || dunkelschaltungText == "true")
                            {
                                dunkelschaltungBoolString = "True";
                            }
                            else
                            {
                                dunkelschaltungBoolString = "False";
                            }
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Dunkelschaltung(Just(%2));\n").arg(variableIndex).arg(dunkelschaltungBoolString);
                        }
                        QDomElement richtpunktentfernungElement = signalRealAktivSchirmElement.firstChildElement("Richtpunktentfernung");
                        if(!richtpunktentfernungElement.isNull())
                        {
                            QDomElement richtpunktentfernungWertElement = richtpunktentfernungElement.firstChildElement("Wert");
                            QString richtpunktentfernungText = richtpunktentfernungWertElement.text();
                            Rat richtpunktentfernungRat(richtpunktentfernungText);
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Richtpunktentfernung(Just(%2));\n").arg(variableIndex).arg(richtpunktentfernungRat.toRatString());
                        }
                        QDomElement signalsystemElement = signalRealAktivSchirmElement.firstChildElement("Signalsystem");
                        if(!signalsystemElement.isNull())
                        {
                            QDomElement signalsystemWertElement = signalsystemElement.firstChildElement("Wert");
                            QString signalsystemText = signalsystemWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Signalsystem(Just(ENUMSignalsystem_%2));\n").arg(variableIndex).arg(signalsystemText);
                        }
                        QDomElement streuscheibeArtElement = signalRealAktivSchirmElement.firstChildElement("Streuscheibe_Art");
                        if(!streuscheibeArtElement.isNull())
                        {
                            QDomElement streuscheibeArtWertElement = streuscheibeArtElement.firstChildElement("Wert");
                            QString streuscheibeArtText = streuscheibeArtWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Streuscheibe_Art(Just(ENUMStreuscheibe_Art_%2));\n").arg(variableIndex).arg(streuscheibeArtText);
                        }
                        QDomElement streuscheibeBetriebsstellungElement = signalRealAktivSchirmElement.firstChildElement("Streuscheibe_Betriebsstellung");
                        if(!streuscheibeBetriebsstellungElement.isNull())
                        {
                            QDomElement streuscheibeBetriebsstellungWertElement = streuscheibeBetriebsstellungElement.firstChildElement("Wert");
                            QString streuscheibeBetriebsstellungText = streuscheibeBetriebsstellungWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Streuscheibe_Betriebsstellung(Just(ENUMStreuscheibe_Betriebsstellung_%2));\n").arg(variableIndex).arg(streuscheibeBetriebsstellungText);
                        }
                        QDomElement signalArtElement = signalRealAktivSchirmElement.firstChildElement("Signal_Art");
                        if(!signalArtElement.isNull())
                        {
                            QDomElement signalArtWertElement = signalArtElement.firstChildElement("Wert");
                            QString signalArtText = signalArtWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Schirm_Signal_Art(Just(ENUMSignal_Art_%2));\n").arg(variableIndex).arg(signalArtText);
                        }

                    }
                    QDomElement signalRealAktivElement = signalRealElement.firstChildElement("Signal_Real_Aktiv");
                    if(!signalRealAktivElement.isNull())
                    {
                        QDomElement idStellelementElement = signalRealAktivElement.firstChildElement("ID_Stellelement");
                        if(!idStellelementElement.isNull())
                        {
                            QDomElement idStellelementWertElement = idStellelementElement.firstChildElement("Wert");
                            QString idStellelementText = idStellelementWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_ID_Stellelement(Just(\"%2\"));\n").arg(variableIndex).arg(idStellelementText);
                        }
                        QDomElement signalsichtMindestElement = signalRealAktivElement.firstChildElement("Signalsicht_Mindest");
                        if(!signalsichtMindestElement.isNull())
                        {
                            QDomElement signalsichtMindestWertElement = signalsichtMindestElement.firstChildElement("Wert");
                            QString signalsichtMindestText = signalsichtMindestWertElement.text();
                            Rat signalsichtMindestRat(signalsichtMindestText);
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Signalsicht_Mindest(Just(%2));\n").arg(variableIndex).arg(signalsichtMindestRat.toRatString());
                        }
                        QDomElement signalsichtSollElement = signalRealAktivElement.firstChildElement("Signalsicht_Soll");
                        if(!signalsichtSollElement.isNull())
                        {
                            QDomElement signalsichtSollWertElement = signalsichtSollElement.firstChildElement("Wert");
                            QString signalsichtSollText = signalsichtSollWertElement.text();
                            Rat signalsichtSollRat(signalsichtSollText);
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Signalsicht_Soll(Just(%2));\n").arg(variableIndex).arg(signalsichtSollRat.toRatString());
                        }
                        QDomElement signalsichtErreichbarElement = signalRealAktivElement.firstChildElement("Signalsicht_Erreichbar");
                        if(!signalsichtErreichbarElement.isNull())
                        {
                            QDomElement signalsichtErreichbarWertElement = signalsichtErreichbarElement.firstChildElement("Wert");
                            QString signalsichtErreichbarText = signalsichtErreichbarWertElement.text();
                            Rat signalsichtErreichbarRat(signalsichtErreichbarText);
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Signalsicht_Erreichbar(Just(%2));\n").arg(variableIndex).arg(signalsichtErreichbarRat.toRatString());
                        }
                        QDomElement tunnelsignalElement = signalRealAktivElement.firstChildElement("Tunnelsignal");
                        if(!tunnelsignalElement.isNull())
                        {
                            QDomElement tunnelsignalWertElement = tunnelsignalElement.firstChildElement("Wert");
                            QString tunnelsignalText = tunnelsignalWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Tunnelsignal(Just(ENUMTunnelsignal_%2));\n").arg(variableIndex).arg(tunnelsignalText);
                        }
                        QDomElement signalFunktionElement = signalRealAktivElement.firstChildElement("Signal_Funktion");
                        if(!signalFunktionElement.isNull())
                        {
                            QDomElement signalFunktionWertElement = signalFunktionElement.firstChildElement("Wert");
                            QString signalFunktionText = signalFunktionWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Signal_Funktion(Just(ENUMSignal_Funktion_%2));\n").arg(variableIndex).arg(signalFunktionText);
                        }
                        QDomElement autoEinstellungElement = signalRealAktivElement.firstChildElement("Auto_Einstellung");
                        if(!autoEinstellungElement.isNull())
                        {
                            QDomElement autoEinstellungWertElement = autoEinstellungElement.firstChildElement("Wert");
                            QString autoEinstellungText = autoEinstellungWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Auto_Einstellung(Just(ENUMAuto_Einstellung_%2));\n").arg(variableIndex).arg(autoEinstellungText);
                        }
                        QDomElement sonstZulAnordnungElement = signalRealAktivElement.firstChildElement("Sonstige_Zulaessige_Anordnung");
                        if(!sonstZulAnordnungElement.isNull())
                        {
                            QDomElement sonstZulAnordnungWertElement = sonstZulAnordnungElement.firstChildElement("Wert");
                            QString sonstZulAnordnungText = sonstZulAnordnungWertElement.text();
                            codeString += QString("    sigreal_o%1.setSignal_Real_Aktiv_Sonstige_Zulaessige_Anordnung(Just(ENUMSonstige_Zulaessige_Anordnung_%2));\n").arg(variableIndex).arg(sonstZulAnordnungText);
                        }
                    }
                    codeString += QString("    SignalFiktivReal sfikreal_o%1 = SignalReal(sigreal_o%1);\n").arg(variableIndex);
                    codeString += QString("    o%1.setSignalFiktivReal(sfikreal_o%1);\n").arg(variableIndex);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Signal(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Signal_Befestigung" && (addList.contains("Signal_Befestigung") || (!skipDefault && !removeList.contains("Signal_Befestigung"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Signal_Befestigung o%1 = new local Signal_BefestigungImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                codeString += generatePunktObjektCode(e, variableIndex);
                QDomElement idSigBefestElement = e.firstChildElement("ID_Signal_Befestigung");
                if(!idSigBefestElement.isNull())
                {
                    QDomElement idSigBefestWertElement = idSigBefestElement.firstChildElement("Wert");
                    QString idSigBefestText = idSigBefestWertElement.text();
                    codeString += QString("    o%1.setID_Signal_Befestigung(Just(\"%2\"));\n").arg(variableIndex).arg(idSigBefestText);
                }
                codeString += QString("    List<TGUID> rzlist_o%1 = Nil;\n").arg(variableIndex);
                QDomElement idRegelzeichnungElement = e.firstChildElement("ID_Regelzeichnung");
                while(!idRegelzeichnungElement.isNull())
                {
                    QDomElement idRegelzeichnungWertElement = idRegelzeichnungElement.firstChildElement("Wert");
                    QString  idRegelzeichnungText = idRegelzeichnungWertElement.text();
                    codeString += QString("    rzlist_o%1 = appendright(rzlist_o%1, \"%2\");\n").arg(variableIndex).arg(idRegelzeichnungText);

                    idRegelzeichnungElement = idRegelzeichnungElement.nextSiblingElement("ID_Regelzeichnung");
                }
                codeString += QString("    o%1.setID_Regelzeichnung(rzlist_o%1);\n").arg(variableIndex);
                QDomElement sigBefestAllgElement = e.firstChildElement("Signal_Befestigung_Allg");
                QDomElement befestArtElement = sigBefestAllgElement.firstChildElement("Befestigung_Art");
                QDomElement befestArtWertElement = befestArtElement.firstChildElement("Wert");
                QString befestArtText = befestArtWertElement.text();
                codeString += QString("    o%1.setSignal_Befestigung_Allg_Befestigung_Art(ENUMBefestigung_Art_%2);\n").arg(variableIndex).arg(befestArtText);
                QDomElement hoeheFundamentElement = sigBefestAllgElement.firstChildElement("Hoehe_Fundamentoberkante");
                if(!hoeheFundamentElement.isNull())
                {
                    QDomElement hoeheFundamentWertElement = hoeheFundamentElement.firstChildElement("Wert");
                    QString hoeheFundamentText = hoeheFundamentWertElement.text();
                    Rat hoeheFundamentRat(hoeheFundamentText);
                    codeString += QString("    o%1.setSignal_Befestigung_Allg_Hoehe_Fundamentoberkante(Just(%2));\n").arg(variableIndex).arg(hoeheFundamentRat.toRatString());
                }
                QDomElement obereLichtpunkthoeheElement = sigBefestAllgElement.firstChildElement("Obere_Lichtpunkthoehe");
                if(!obereLichtpunkthoeheElement.isNull())
                {
                    QDomElement obereLichtpunkthoeheWertElement = obereLichtpunkthoeheElement.firstChildElement("Wert");
                    QString obereLichtpunkthoeheText = obereLichtpunkthoeheWertElement.text();
                    Rat obereLichtpunkthoeheRat(obereLichtpunkthoeheText);
                    codeString += QString("    o%1.setSignal_Befestigung_Allg_Obere_Lichtpunkthoehe(Just(%2));\n").arg(variableIndex).arg(obereLichtpunkthoeheRat.toRatString());
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Signal_Befestigung(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Signal_Rahmen" && (addList.contains("Signal_Rahmen") || (!skipDefault && !removeList.contains("Signal_Rahmen"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Signal_Rahmen o%1 = new local Signal_RahmenImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idSigBefestElement = e.firstChildElement("ID_Signal_Befestigung");
                QDomElement idSigBefestWertElement = idSigBefestElement.firstChildElement("Wert");
                QString idSigBefestText = idSigBefestWertElement.text();
                QDomElement rahmenArtElement = e.firstChildElement("Rahmen_Art");
                QDomElement rahmenArtWertElement = rahmenArtElement.firstChildElement("Wert");
                QString rahmenArtText = rahmenArtWertElement.text();
                QDomElement idSignalElement = e.firstChildElement("ID_Signal");
                QDomElement idSignalWertElement = idSignalElement.firstChildElement("Wert");
                QString idSignalText = idSignalWertElement.text();
                codeString += QString("    o%1.setID_Signal_Befestigung(\"%2\");\n").arg(variableIndex).arg(idSigBefestText);
                codeString += QString("    o%1.setRahmen_Art(ENUMRahmen_Art_%2);\n").arg(variableIndex).arg(rahmenArtText);
                codeString += QString("    o%1.setID_Signal(\"%2\");\n").arg(variableIndex).arg(idSignalText);
                QDomElement idSigNachordElement = e.firstChildElement("ID_Signal_Nachordnung");
                if(!idSigNachordElement.isNull())
                {
                    QDomElement idSigNachordWertElement = idSigNachordElement.firstChildElement("Wert");
                    QString idSigNachordText = idSigNachordWertElement.text();
                    codeString += QString("    o%1.setID_Signal_Nachordnung(Just(\"%2\"));\n").arg(variableIndex).arg(idSigNachordText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Signal_Rahmen(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Signal_Signalbegriff" && (addList.contains("Signal_Signalbegriff") || (!skipDefault && !removeList.contains("Signal_Signalbegriff"))))
            {
                QString id = getIdentitaet(e);

                codeString += QString("    Signal_Signalbegriff o%1 = new local Signal_SignalbegriffImpl();\n").arg(variableIndex);
                codeString += generateUrObjektCode(e, variableIndex);
                codeString += generateBasisObjektCode(e, variableIndex);
                QDomElement idSigRahmenElement = e.firstChildElement("ID_Signal_Rahmen");
                QDomElement idSigRahmenWertElement = idSigRahmenElement.firstChildElement("Wert");
                QString idSigRahmenText = idSigRahmenWertElement.text();
                codeString += QString("    o%1.setID_Signal_Rahmen(\"%2\");\n").arg(variableIndex).arg(idSigRahmenText);
                QDomElement signalbegriffIdElement = e.firstChildElement("Signalbegriff_ID");
                QString signalbegriffIdAttribute = signalbegriffIdElement.attribute("xsi:type");
                QString signalbegriffIdText = QString();
                QDomElement signalbegriffIdElementSymbolElement = signalbegriffIdElement.firstChildElement("Symbol");
                if(!signalbegriffIdElementSymbolElement.isNull())
                {
                    signalbegriffIdText = signalbegriffIdElementSymbolElement.text();
                }
                codeString += QString("    o%1.setSignalbegriff_ID(\"%2(%3)\");\n").arg(variableIndex).arg(signalbegriffIdAttribute).arg(signalbegriffIdText);
                QDomElement sigbegriffAllgElement = e.firstChildElement("Signal_Signalbegriff_Allg");
                QDomElement geschaltetElement = sigbegriffAllgElement.firstChildElement("Geschaltet");
                if(!geschaltetElement.isNull())
                {
                    QDomElement geschaltetWertElement = geschaltetElement.firstChildElement("Wert");
                    QString geschaltetText = geschaltetWertElement.text();
                    QString geschaltetBoolString = QString();
                    if(geschaltetText == "True" || geschaltetText == "true")
                    {
                        geschaltetBoolString = "True";
                    }
                    else
                    {
                        geschaltetBoolString = "False";
                    }
                    codeString += QString("    o%1.setSignal_Signalbegriff_Allg_Geschaltet(Just(%2));\n").arg(variableIndex).arg(geschaltetBoolString);
                }
                QDomElement beleuchtetElement = sigbegriffAllgElement.firstChildElement("Beleuchtet");
                if(!beleuchtetElement.isNull())
                {
                    QDomElement beleuchtetWertElement = beleuchtetElement.firstChildElement("Wert");
                    QString beleuchtetText = beleuchtetWertElement.text();
                    codeString += QString("    o%1.setSignal_Signalbegriff_Allg_Beleuchtet(Just(ENUMBeleuchtet_%2));\n").arg(variableIndex).arg(beleuchtetText);
                }
                QDomElement anschaltdauerElement = sigbegriffAllgElement.firstChildElement("Anschaltdauer");
                if(!anschaltdauerElement.isNull())
                {
                    QDomElement anschaltdauerWertElement = anschaltdauerElement.firstChildElement("Wert");
                    QString anschaltdauerText = anschaltdauerWertElement.text();
                    codeString += QString("    o%1.setSignal_Signalbegriff_Allg_Anschaltdauer(Just(ENUMAnschaltdauer_%2));\n").arg(variableIndex).arg(anschaltdauerText);
                }
                codeString += QString("    l = insert(l, Pair(\"%1\", Objecttype_Signal_Signalbegriff(o%2)));\n\n").arg(id).arg(variableIndex);
            }
            else if(name == "Datenpunkt" && (addList.contains("Datenpunkt") || (!skipDefault && !removeList.contains("Datenpunkt"))))
            {
                //QDomElement datenpunktAllgemeinElement = e.firstChildElement("Datenpunkt_Allg");
                //QDomElement datenpunktBeschreibungElement = datenpunktAllgemeinElement.firstChildElement("Datenpunkt_Beschreibung");
                //QDomElement datenpunktBeschreibungWertElement = datenpunktBeschreibungElement.firstChildElement("Wert");
                //QString id = getIdentitaet(e);
                //QString idtopkante = getPunkt_Objekt_ID_TOP_Kante(e);
                //QString abstand = getPunkt_Objekt_Abstand(e);
                //QString seitlicherabstand = getPunkt_Objekt_Seitlicher_Abstand(e);
                //QString seitlichelage = getPunkt_Objekt_Seitliche_Lage(e);
                //QString wirkrichtung = getPunkt_Objekt_Wirkrichtung(e);
                //QString datenpunktbeschreibungtext = datenpunktBeschreibungWertElement.text();

                //QString message = id + ": Datenpunkt, ID_TOP_Kante=" + idtopkante + ", Abstand=" + abstand + ", Seitlicher_Abstand=" + seitlicherabstand + ", Beschreibung=" + datenpunktbeschreibungtext;
                //std::cout << message.toLatin1().constData() << std::endl;
            }

            if(!e.attribute("entry").isEmpty())
            {
                codeString +=
                QString("    entrylist = appendright(entrylist, o%1);\n\n").arg(variableIndex);
            }
            if(!e.attribute("exit").isEmpty())
            {
                codeString +=
                QString("    exitlist = appendright(exitlist, o%1);\n\n").arg(variableIndex);
            }
        }
        variableIndex++;
        n = n.nextSibling();
    }

    codeString +=
    QString("    constructor.setEntryList(entrylist);\n") +
    QString("    constructor.setExitList(exitlist);\n") +
    QString("    l = constructor.constructEtcsL%1Line(l);\n").arg(etcsLevel) +
    QString("    xmlw.printXmlInfo(l);\n") +
    QString("  }\n") +
    QString("}\n\n") +
    QString("{\n") +
    QString("  new local TestImpl();\n") +
    QString("}\n\n");

    return true;
}


QString CodeGenerator::getIdentitaet(QDomElement e)
{
    QDomElement identitaetElement = e.firstChildElement("Identitaet");
    QDomElement identitaetWertElement = identitaetElement.firstChildElement("Wert");
    return identitaetWertElement.text();
}

QString CodeGenerator::generateUrObjektCode(QDomElement e, int varIndex)
{
    QDomElement identitaetElement = e.firstChildElement("Identitaet");
    QDomElement identitaetWertElement = identitaetElement.firstChildElement("Wert");

    QString codePart = QString("    o%1.setIdentitaet(\"%2\");\n").arg(varIndex).arg(identitaetWertElement.text());

    return codePart;
}

QString CodeGenerator::generateBasisObjektCode(QDomElement e, int varIndex)
{
    QString codePart = QString();

    codePart += QString("    List<TGUID> bvlist_o%1 = Nil;\n").arg(varIndex);
    QDomElement bearbeitungsvermerkElement = e.firstChildElement("ID_Bearbeitungsvermerk");
    while(!bearbeitungsvermerkElement.isNull())
    {
        QDomElement bearbeitungsvermerkWertElement = bearbeitungsvermerkElement.firstChildElement("Wert");
        QString  bearbeitungsvermerkText = bearbeitungsvermerkWertElement.text();
        codePart += QString("    bvlist_o%1 = appendright(bvlist_o%1, \"%2\");\n").arg(varIndex).arg(bearbeitungsvermerkText);

        bearbeitungsvermerkElement = bearbeitungsvermerkElement.nextSiblingElement("ID_Bearbeitungsvermerk");
    }
    codePart += QString("    o%1.setID_Bearbeitungsvermerk(bvlist_o%1);\n").arg(varIndex);

    QDomElement objektreferenzenElement = e.firstChildElement("Objektreferenzen");
    if(!objektreferenzenElement.isNull())
    {
        QDomElement gdiReferenzElement = objektreferenzenElement.firstChildElement("DB_GDI_Referenz");
        if(!gdiReferenzElement.isNull())
        {
            QDomElement gdiReferenzWertElement = gdiReferenzElement.firstChildElement("Wert");
            QString gdiReferenzText =  gdiReferenzWertElement.text();
            codePart += QString("    o%1.setObjektreferenzenDB_GDI_Referenz(Just(\"%2\"));\n").arg(varIndex).arg(gdiReferenzText);
        }
        QDomElement techPlatzElement = objektreferenzenElement.firstChildElement("Technischer_Platz");
        if(!techPlatzElement.isNull())
        {
            QDomElement techPlatzWertElement = techPlatzElement.firstChildElement("Wert");
            QString techPlatzText =  techPlatzWertElement.text();
            codePart += QString("    o%1.setObjektreferenzenTechnischer_Platz(Just(\"%2\"));\n").arg(varIndex).arg(techPlatzText);
        }
    }
    QDomElement basisObjektAllgElement = e.firstChildElement("Basis_Objekt_Allg");
    if(!basisObjektAllgElement.isNull())
    {
        QDomElement zustandBesondersElement = basisObjektAllgElement.firstChildElement("Objektzustand_Besonders");
        if(!zustandBesondersElement.isNull())
        {
            QDomElement zustandBesondersWertElement = zustandBesondersElement.firstChildElement("Wert");
            QString zustandBesondersText =  zustandBesondersWertElement.text();
            codePart += QString("    o%1.setBasis_Objekt_Allg_Objektzustand_Besonders(Just(ENUMObjektzustand_Besonders_%2));\n").arg(varIndex).arg(zustandBesondersText);
        }
        QDomElement regelwerkBesondersElement = basisObjektAllgElement.firstChildElement("ID_Anhang_Regelwerk_Besonders");
        if(!regelwerkBesondersElement.isNull())
        {
            QDomElement regelwerkBesondersWertElement = regelwerkBesondersElement.firstChildElement("Wert");
            QString regelwerkBesondersText =  regelwerkBesondersWertElement.text();
            codePart += QString("    o%1.setBasis_Objekt_Allg_ID_Anhang_Regelwerk_Besonders(Just(\"%2\"));\n").arg(varIndex).arg(regelwerkBesondersText);
        }
        QDomElement datumRegelwerkElement = basisObjektAllgElement.firstChildElement("Datum_Regelwerk");
        if(!datumRegelwerkElement.isNull())
        {
            QDomElement datumRegelwerkWertElement = datumRegelwerkElement.firstChildElement("Wert");
            QString datumRegelwerkText =  datumRegelwerkWertElement.text();
            codePart += QString("    o%1.setBasis_Objekt_Allg_Datum_Regelwerk(Just(\"%2\"));\n").arg(varIndex).arg(datumRegelwerkText);
        }
    }

    return codePart;
}

QString CodeGenerator::generatePunktObjektCode(QDomElement e, int varIndex)
{
    QString codePart = QString();

    codePart += QString("    List<Punkt_Objekt_Strecke> stgrouplist_o%1 = Nil;\n").arg(varIndex);
    QDomElement streckeElement = e.firstChildElement("Punkt_Objekt_Strecke");
    int subIndex = 0;
    while(!streckeElement.isNull())
    {
        codePart += QString("    Punkt_Objekt_Strecke stgroup_o%1_%2 = new local Punkt_Objekt_Strecke();\n").arg(varIndex).arg(subIndex);
        QDomElement idStreckeElement = streckeElement.firstChildElement("ID_Strecke");
        QDomElement idStreckeWertElement = idStreckeElement.firstChildElement("Wert");
        QString idstrecke = idStreckeWertElement.text();
        QDomElement streckeKmElement = streckeElement.firstChildElement("Strecke_Km");
        QDomElement streckeKmWertElement = streckeKmElement.firstChildElement("Wert");
        QString streckekm = streckeKmWertElement.text();
        codePart += QString("    stgroup_o%1_%2.setID_Strecke(\"%3\");\n").arg(varIndex).arg(subIndex).arg(idstrecke);
        codePart += QString("    stgroup_o%1_%2.setStrecke_Km(\"%3\");\n").arg(varIndex).arg(subIndex).arg(streckekm);
        codePart += QString("    stgrouplist_o%1 = appendright(stgrouplist_o%1, stgroup_o%1_%2);\n").arg(varIndex).arg(subIndex);

        streckeElement = streckeElement.nextSiblingElement("Punkt_Objekt_Strecke");
        subIndex++;
    }
    codePart += QString("    o%1.setPunkt_Objekt_Strecke(stgrouplist_o%1);\n").arg(varIndex);

    codePart += QString("    List<Punkt_Objekt_TOP_Kante> tkgrouplist_o%1 = Nil;\n").arg(varIndex);
    QDomElement topKanteElement = e.firstChildElement("Punkt_Objekt_TOP_Kante");
    subIndex = 0;
    while(!topKanteElement.isNull())
    {
        codePart += QString("    Punkt_Objekt_TOP_Kante tkgroup_o%1_%2 = new local Punkt_Objekt_TOP_Kante();\n").arg(varIndex).arg(subIndex);
        QDomElement idTopKanteElement = topKanteElement.firstChildElement("ID_TOP_Kante");
        QDomElement idTopKanteWertElement = idTopKanteElement.firstChildElement("Wert");
        QString idtopkante = idTopKanteWertElement.text();
        QDomElement abstandElement = topKanteElement.firstChildElement("Abstand");
        QDomElement abstandWertElement = abstandElement.firstChildElement("Wert");
        QString abstand = abstandWertElement.text();
        Rat abstandRat(abstand);
        codePart += QString("    tkgroup_o%1_%2.setID_TOP_Kante(\"%3\");\n").arg(varIndex).arg(subIndex).arg(idtopkante);
        codePart += QString("    tkgroup_o%1_%2.setAbstand(%3);\n").arg(varIndex).arg(subIndex).arg(abstandRat.toRatString());
        QDomElement seitlicherAbstandElement = topKanteElement.firstChildElement("Seitlicher_Abstand");
        if(!seitlicherAbstandElement.isNull())
        {
            QDomElement seitlicherAbstandWertElement = seitlicherAbstandElement.firstChildElement("Wert");
            QString seitlicherabstand = seitlicherAbstandWertElement.text();
            Rat seitlicherabstandRat(seitlicherabstand);
            codePart += QString("    tkgroup_o%1_%2.setSeitlicher_Abstand(Just(%3));\n").arg(varIndex).arg(subIndex).arg(seitlicherabstandRat.toRatString());
        }
        QDomElement seitlicheLageElement = topKanteElement.firstChildElement("Seitliche_Lage");
        if(!seitlicheLageElement.isNull())
        {
            QDomElement seitlicheLageWertElement = seitlicheLageElement.firstChildElement("Wert");
            QString seitlichelage = seitlicheLageWertElement.text();
            codePart += QString("    tkgroup_o%1_%2.setSeitliche_Lage(Just(ENUMLinksRechts_%3));\n").arg(varIndex).arg(subIndex).arg(seitlichelage);
        }
        QDomElement wirkrichtungElement = topKanteElement.firstChildElement("Wirkrichtung");
        if(!wirkrichtungElement.isNull())
        {
            QDomElement wirkrichtungWertElement = wirkrichtungElement.firstChildElement("Wert");
            QString wirkrichtung = wirkrichtungWertElement.text();
            codePart += QString("    tkgroup_o%1_%2.setWirkrichtung(Just(ENUMWirkrichtung_%3));\n").arg(varIndex).arg(subIndex).arg(wirkrichtung);
        }
        codePart += QString("    tkgrouplist_o%1 = appendright(tkgrouplist_o%1, tkgroup_o%1_%2);\n").arg(varIndex).arg(subIndex);

        topKanteElement = topKanteElement.nextSiblingElement("Punkt_Objekt_TOP_Kante");
        subIndex++;
    }
    codePart += QString("    o%1.setPunkt_Objekt_TOP_Kante(tkgrouplist_o%1);\n").arg(varIndex);

    return codePart;
}

QString CodeGenerator::generateBereichObjektCode(QDomElement e, int varIndex)
{
    QString codePart = QString();

    codePart += QString("    List<Bereich_Objekt_Teilbereich> tbgrouplist_o%1 = Nil;\n").arg(varIndex);
    QDomElement teilbereichElement = e.firstChildElement("Bereich_Objekt_Teilbereich");
    int subIndex = 0;
    while(!teilbereichElement.isNull())
    {
        codePart += QString("    Bereich_Objekt_Teilbereich tbgroup_o%1_%2 = new local Bereich_Objekt_Teilbereich();\n").arg(varIndex).arg(subIndex);
        QDomElement idTopKanteElement = teilbereichElement.firstChildElement("ID_TOP_Kante");
        QDomElement idTopKanteWertElement = idTopKanteElement.firstChildElement("Wert");
        QString idtopkante = idTopKanteWertElement.text();
        QDomElement begrenzungAElement = teilbereichElement.firstChildElement("Begrenzung_A");
        QDomElement begrenzungAWertElement = begrenzungAElement.firstChildElement("Wert");
        QString begrenzungAText = begrenzungAWertElement.text();
        Rat begrenzungARat(begrenzungAText);
        QDomElement begrenzungBElement = teilbereichElement.firstChildElement("Begrenzung_B");
        QDomElement begrenzungBWertElement = begrenzungBElement.firstChildElement("Wert");
        QString begrenzungBText = begrenzungBWertElement.text();
        Rat begrenzungBRat(begrenzungBText);
        codePart += QString("    tbgroup_o%1_%2.setID_TOP_Kante(\"%3\");\n").arg(varIndex).arg(subIndex).arg(idtopkante);
        codePart += QString("    tbgroup_o%1_%2.setBegrenzung_A(%3);\n").arg(varIndex).arg(subIndex).arg(begrenzungARat.toRatString());
        codePart += QString("    tbgroup_o%1_%2.setBegrenzung_B(%3);\n").arg(varIndex).arg(subIndex).arg(begrenzungBRat.toRatString());
        QDomElement richtungsbezugElement = teilbereichElement.firstChildElement("Richtungsbezug");
        if(!richtungsbezugElement.isNull())
        {
            QDomElement richtungsbezugWertElement = richtungsbezugElement.firstChildElement("Wert");
            QString richtungsbezugText = richtungsbezugWertElement.text();
            codePart += QString("    tbgroup_o%1_%2.setRichtungsbezug(Just(ENUMWirkrichtung_%3));\n").arg(varIndex).arg(subIndex).arg(richtungsbezugText);
        }
        codePart += QString("    tbgrouplist_o%1 = appendright(tbgrouplist_o%1, tbgroup_o%1_%2);\n").arg(varIndex).arg(subIndex);

        teilbereichElement = teilbereichElement.nextSiblingElement("Bereich_Objekt_Teilbereich");
        subIndex++;
    }
    codePart += QString("    o%1.setBereich_Objekt_Teilbereich(tbgrouplist_o%1);\n").arg(varIndex);

    return codePart;
}
