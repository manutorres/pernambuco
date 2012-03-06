#include "pdfmerge.h"

PDFmerge::PDFmerge()
{    
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    dir.setNameFilters(QStringList() << "*.pdf");

    QObject::connect(&this->web,SIGNAL(loadFinished(bool)),this,SLOT(printHtmlToPdf()));
}

//Convierte contenido html en un documento pdf
void PDFmerge::htmlToPdf(QString outputName, QString html){
    qDebug() << "HtmlToPdf 1";
    this->printer.setOutputFileName(outputName + ".pdf");
    qDebug() << "HtmlToPdf 2 - Output name:" << outputName;
    this->web.setHtml(html);
}

//Mezcla todos los documentos pdf contenidos en un dado directorio
void PDFmerge::mergePdfs(QString path, QString outputName){

    dir.setPath(path);
    QStringList files = dir.entryList(QDir::Files | QDir::NoSymLinks);
    qDebug() << files;
    foreach (QString file, files){
        qDebug() << file;
        PdfMemDocument doc;
        doc.Load((path + "/" + file).toStdString().data());
        document.Append(doc);
    }

    document.Write(outputName.toStdString().data());
}

//Este método se ejecuta cuando el contenido html se termina de cargar en el objeto web
void PDFmerge::printHtmlToPdf(){
    this->web.print(&this->printer);
}
