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

    void htmlToPdf(QString outputName, QString html);
    void mergePdfs(QString path, QString outputName);

private:
    QWebView web;
    QPrinter printer;
    PdfMemDocument document;
    QDir dir;

private slots:
    void printHtmlToPdf();
};

#endif // PDFMERGE_H
