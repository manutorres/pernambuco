#include "pdfmerge.h"

PDFmerge::PDFmerge()
{    
    this->printer.setPageSize(QPrinter::A4);
    this->printer.setOutputFormat(QPrinter::PdfFormat);
    this->dir.setNameFilters(QStringList() << "*.pdf");

    QObject::connect(&this->web, SIGNAL(loadFinished(bool)), this, SLOT(printHtmlToPdf()));
}

//Convierte contenido html en un documento pdf
void PDFmerge::htmlToPdf(QString html, QString outputName){
    this->printer.setOutputFileName(outputName);
    this->web.setStyleSheet("p {font: 18px arial, sans-serif;}");
    html = "<body style='font: 18px arial, sans-serif;'>" + html + "</body>";
    this->web.setHtml(html);
    qDebug() << this->web.styleSheet();
    qDebug() << this->web.page()->mainFrame()->toHtml();
}

//Mezcla todos los documentos pdf contenidos en un dado directorio
bool PDFmerge::addPdf(QString file, QString &errorString){
    qDebug() << file;
    PdfMemDocument doc;
    try{
        doc.Load(file.toStdString().data());
        try{
            document.Append(doc);
        }catch(PoDoFo::PdfError){
            errorString = "The file couldn't be merged: " + file;
            return false;
        }
    }catch(PoDoFo::PdfError){
        qDebug() << "Error al cargar el archivo:" << file;
        errorString = "The file couldn't be loaded: " + file;
        return false;
    }
    return true;
}

//Imprime la salida en un archivo pdf
bool PDFmerge::writeOutput(QString outputName){
    try{
        document.Write(outputName.toStdString().data());
    }catch(PoDoFo::PdfError){
        qDebug() << "Error en la escritura de la salida.";
        return false;
    }
    return true;
}

//Este método se ejecuta cuando el contenido html se termina de cargar en el objeto web
void PDFmerge::printHtmlToPdf(){
    this->web.print(&this->printer);
}
