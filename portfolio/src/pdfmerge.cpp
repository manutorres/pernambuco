#include "pdfmerge.h"

PDFmerge::PDFmerge()
{    
    this->printer.setPageSize(QPrinter::Letter);
    this->printer.setOutputFormat(QPrinter::PdfFormat);
    this->printer.setPageMargins(60, 80, 60, 80, QPrinter::DevicePixel);
    this->dir.setNameFilters(QStringList() << "*.pdf");
    QObject::connect(&this->web, SIGNAL(loadFinished(bool)), this, SLOT(printHtmlToPdf()));
}

//Convierte contenido html en un documento pdf
void PDFmerge::htmlToPdf(QString html, QString outputName){
    this->printer.setOutputFileName(outputName);
    //html = "<body style='font: 18px arial, sans-serif;'>" + html + "</body>";
    this->web.setHtml(html);
}

//Agrega el pdf file al final del documento de salida.
bool PDFmerge::addPdf(QString file, QString &errorString){
    //qDebug() << file;
    PdfMemDocument doc;
    try{
        doc.CreateFont("Calibri");
        doc.Load(file.toStdString().data());
        try{
            document.Append(doc);
        }catch(PoDoFo::PdfError){
            errorString = "The file couldn't be included.";
            return false;
        }
    }catch(PdfError){
        qDebug() << "Error al abrir el archivo" << file;
        //PdfError::ErrorMessage();
        errorString = "The file couldn't be opened.";
        return false;
    }
    return true;
}

void PDFmerge::setOutputFileName(QString outputFile){
    this->outputFile = outputFile;
}

QString PDFmerge::outputFileName(){
    return this->outputFile;
}

//Imprime la salida en un archivo pdf
bool PDFmerge::writeOutput(QString outputFile){
    if(outputFile == "")
        outputFile = this->outputFile;
    try{
        document.Write(outputFile.toStdString().data());
    }catch(PoDoFo::PdfError){
        //qDebug() << "Error en la escritura de la salida.";
        return false;
    }
    return true;
}

//Este método se ejecuta cuando el contenido html se termina de cargar en el objeto web
void PDFmerge::printHtmlToPdf(){
    this->web.print(&this->printer);
}
