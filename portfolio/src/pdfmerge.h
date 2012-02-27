#ifndef PDFMERGE_H
#define PDFMERGE_H

#include "stdio.h"
#include <podofo/podofo.h>
#include "QPrinter"
#include "QtWebKit/QtWebKit"
#include "QObject"

using namespace PoDoFo;

class PDFmerge:public QObject
{
    Q_OBJECT

public:

    PDFmerge();

    QWebView *web;
    QPrinter *printer;

    //PdfMemDocument *document;

public slots:
    void htmlToPdf();
};

#endif // PDFMERGE_H
