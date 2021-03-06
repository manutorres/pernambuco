#ifndef PDFMERGE_H
#define PDFMERGE_H

#include <QPrinter>
//#include <QtWebKit/QtWebKit>
#include <QObject>
#include <QDir>
#include <stdio.h>
#include <podofo/podofo.h>
#include <QTextDocument>
#include <QDebug>



using namespace PoDoFo;

class PDFmerge:public QObject
{
    Q_OBJECT

public:

    PDFmerge();      

    void htmlToPdf(QString html, QString outputName);
    void setupDocument();
    bool addPdf(QString file, QString title, QString footNote, QString &errorString);
    void setOutputFileName(QString outputFile);
    QString getOutputFileName();
    bool writeOutput(QString outputFile = "");
    void clearDocument();
    void addPageSeparator();
    //Se agrega una p�gina indicando que el estudiante X no tiene ni assignments ni forum post
    void addPageNeitherAssignmentsNorForumPost(QString tmpPath, QString studentName);

private:
    //QWebView web;
    QPrinter printer;
    PdfMemDocument* document;
    QString outputFile;
    QDir dir;

    void addPageTitle(PdfMemDocument &doc, QString text);
    void addPageFootNote(PdfMemDocument &doc, QString text);

//private slots:
  //  void printHtmlToPdf();
};

#endif // PDFMERGE_H
