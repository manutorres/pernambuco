#ifndef PDFMERGE_H
#define PDFMERGE_H

#include <stdio.h>
#include <podofo/podofo.h>
#include <QPrinter>
#include <QtWebKit/QtWebKit>
#include <QObject>
#include <QDir>

using namespace PoDoFo;

class PDFmerge:public QObject
{
    Q_OBJECT

public:

    PDFmerge();      

    void htmlToPdf(QString html, QString outputName);
    bool addPdf(QString file, QString &errorString);
    void writeOutput(QString outputName);

private:
    QWebView web;
    QPrinter printer;
    PdfMemDocument document;
    QDir dir;

private slots:
    void printHtmlToPdf();
};

#endif // PDFMERGE_H
