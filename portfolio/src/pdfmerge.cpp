#include "pdfmerge.h"

PDFmerge::PDFmerge()
{    
    //document = new PdfMemDocument();
    //document->Load("pdf1.pdf");
    /*PdfMemDocument doc1("pdf3.pdf");
    PdfMemDocument doc2("pdf2.pdf");

    doc1.Append(doc2);
    doc1.Write("resultado.pdf");*/

    web = new QWebView();

    QObject::connect(this->web,SIGNAL(loadFinished(bool)),this,SLOT(htmlToPdf()));

    web->load(QUrl("http://www.google.com"));

    printer = new QPrinter();
    printer->setPageSize(QPrinter::A4);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("htmlToPdf.pdf");
}

void PDFmerge::htmlToPdf(){

    this->web->print(printer);
}
