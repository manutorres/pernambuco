#include "pdfmerge.h"

PDFmerge::PDFmerge()
{    
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    dir.setNameFilters(QStringList() << "*.pdf");

    QObject::connect(&this->web,SIGNAL(loadFinished(bool)),this,SLOT(printHtmlToPdf()));
}

//Convierte contenido html en un documento pdf
void PDFmerge::htmlToPdf(QString html, QString outputName){
    this->printer.setOutputFileName(outputName);
    this->web.setHtml(html);
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
            errorString = "The file could not be appended: " + file;
            return false;
        }
    }catch(PoDoFo::PdfError){
        qDebug() << "Error al cargar el archivo:" << file;
        errorString = "The file could not be loaded: " + file;
        return false;
    }
    return true;
}

//Imprime la salida en un archivo pdf
void PDFmerge::writeOutput(QString outputName){
    document.Write(outputName.toStdString().data());
}

//Este método se ejecuta cuando el contenido html se termina de cargar en el objeto web
void PDFmerge::printHtmlToPdf(){
    this->web.print(&this->printer);
}
