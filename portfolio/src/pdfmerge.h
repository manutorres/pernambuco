#ifndef PDFMERGE_H
#define PDFMERGE_H

#include <QPrinter>
#include <QtWebKit/QtWebKit>
#include <QObject>
#include <QDir>
#include <stdio.h>
#include <podofo/podofo.h>


using namespace PoDoFo;

class PDFmerge:public QObject
{
    Q_OBJECT

public:

    PDFmerge();      

    void htmlToPdf(QString html, QString outputName);
    void setupDocument();
    bool addPdf(QString file, QString studentName, QString &errorString);
    void setOutputFileName(QString outputFile);
    QString getOutputFileName();
    bool writeOutput(QString outputFile = "");
    void clearDocument();
    void addPageSeparator();

private:
    QWebView web;
    QPrinter printer;
    PdfMemDocument* document;
    QString outputFile;
    QDir dir;

    void addPageTitle(PdfMemDocument &doc, QString title);

private slots:
    void printHtmlToPdf();
};

#endif // PDFMERGE_H
