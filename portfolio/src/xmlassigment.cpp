#include "xmlassigment.h"

//Constructor
XMLAssigment::XMLAssigment(){
    //DO NOTHING
}

//Abre un archivo de XML de assignments en un objeto QDomDocument
void XMLAssigment::openFile(QString fileName){

    xmlDocument = QDomDocument();
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    if (!xmlDocument.setContent(&file)) {
        file.close();
        return;
    }

    file.close();
}

//Guarda en un archivo el XML de assignments generado
void XMLAssigment::saveFile(QString fileName){

    QFile file(fileName);
    if( !file.open(QIODevice::WriteOnly))
        return;

    QTextStream ts(&file);
    ts << xmlDocument.toString();

    file.close();
}

//Crea un objeto QDomDocument vacío
void XMLAssigment::newAssignmentXML(){

    xmlDocument = QDomDocument();

    root = xmlDocument.createElement("assigmentList");
    xmlDocument.appendChild(root);
}

//Crea un nuevo elemento dentro del XML de assignmets
void XMLAssigment::addAssignmentElement(QString type, QString title, QString date, QString assignmentId){

    QDomElement element = xmlDocument.createElement("doc");

    element.setAttribute("type",type);
    element.setAttribute("title",title);
    element.setAttribute("date",date);
    element.setAttribute("assignmentId",assignmentId);

    root.appendChild(element);
}
