#ifndef XMLASSIGMENT_H
#define XMLASSIGMENT_H

#include "QString"
#include "QtXml/QDomDocument"
#include "QFile"
#include "QTextStream"

class XMLAssigment
{
public:
    XMLAssigment();

    QDomDocument xmlDocument;
    QDomElement root;

    void saveFile(QString fileName="assigmentXml.xml");
    void openFile(QString fileName);
    void newAssignmentXML();
    void addAssignmentElement(QString type, QString title, QString date, QString assignmentId);
};

#endif // XMLASSIGMENT_H
