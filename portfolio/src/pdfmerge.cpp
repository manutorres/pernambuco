#include "pdfmerge.h"

PDFmerge::PDFmerge(){

    this->printer.setPaperSize(QPrinter::Letter);
    this->printer.setOutputFormat(QPrinter::PdfFormat);
    this->printer.setPageMargins(0.75, 1, 0.75, 0.75, QPrinter::Inch);
    this->dir.setNameFilters(QStringList() << "*.pdf");
    this->document = NULL;
    //QObject::connect(&this->web, SIGNAL(loadFinished(bool)), this, SLOT(printHtmlToPdf()));
}

void PDFmerge::setupDocument(){
    if(this->document)
        delete this->document;
    this->document = new PdfMemDocument;
}

//Convierte contenido html en un documento pdf
void PDFmerge::htmlToPdf(QString html, QString outputName){    
    //this->web.setHtml(html);//

    this->printer.setOutputFileName(outputName);
    QTextDocument *documentHTML = new QTextDocument();
    documentHTML->setPageSize(QSizeF(this->printer.width(), this->printer.height()));
    documentHTML->setHtml(html);
    documentHTML->print(&printer);
    delete documentHTML;
}

//Agrega el pdf file al final del documento de salida.
bool PDFmerge::addPdf(QString file, QString title, QString footNote, QString &errorString){

    PdfMemDocument doc;
    try{
        doc.CreateFont("Calibri");
        doc.Load(file.toStdString().data());
        try{
            if(title != "")
                addPageTitle(doc, title);
            if(footNote != "")
                addPageFootNote(doc, footNote);
            this->document->Append(doc);
        }catch(PoDoFo::PdfError){
            errorString = "The file couldn't be included.";
            return false;
        }
    }catch(PdfError){
        qDebug() << "Error al abrir el archivo" << file;
        errorString = "The file couldn't be opened.";
        return false;
    }
    return true;
}

void PDFmerge::addPageTitle(PdfMemDocument &doc, QString text){
    qDebug() << "PAGE TITLE, count:" << doc.GetPageCount();

    //PdfPainter is the class which is able to draw text and graphics directly on a PdfPage object.
    PdfPainter painter;
    //A PdfFont object is required to draw text on a PdfPage using a PdfPainter.
    //PoDoFo will find the font using fontconfig on your system and embedd truetype fonts automatically in the PDF file.
    PdfFont* pFont;
    //This pointer will hold the page object later. PdfSimpleWriter can write several PdfPage's to a PDF file.
    PdfPage* pPage = doc.GetPage(0);
    //Set the page as drawing target for the PdfPainter. Before the painter can draw, a page has to be set first.
    painter.SetPage( pPage );
    //Create a PdfFont object using the font "Arial". The font is found on the system using fontconfig and embedded into the
    //PDF file. If Arial is not available, a default font will be used. The created PdfFont will be deleted by the PdfDocument.
    pFont = doc.CreateFont( "Arial" );
    pFont->SetFontSize( 10.0 );
    //Set the font as default font for drawing. A font has to be set before you can draw text on a PdfPainter.
    painter.SetFont( pFont );
    painter.SetTransformationMatrix(1,0,0,-1,0,0);//Resuelve un bug. Si no esta el title se dibuja rotado en el eje Y unos 180 grados.
    //painter.DrawText( 10, 10, title.toStdString().data() );//Pone el title arriba del todo.
    painter.DrawText( 10, -925, text.toStdString().data() );//Pone el title abajo del todo (dentro de lo posible).
    //Tell PoDoFo that the page has been drawn completely. This required to optimize drawing operations inside in PoDoFo
    //and has to be done whenever you are done with drawing a page.
    painter.FinishPage();
}

void PDFmerge::addPageFootNote(PdfMemDocument &doc, QString text){
    for (int i = doc.GetPageCount() -1 ; i >= 0; i--){

        //PdfPainter is the class which is able to draw text and graphics directly on a PdfPage object.
        PdfPainter painter;
        //A PdfFont object is required to draw text on a PdfPage using a PdfPainter.
        //PoDoFo will find the font using fontconfig on your system and embedd truetype fonts automatically in the PDF file.
        PdfFont* pFont;
        //This pointer will hold the page object later. PdfSimpleWriter can write several PdfPage's to a PDF file.
        PdfPage* pPage = doc.GetPage(i);
        //Set the page as drawing target for the PdfPainter. Before the painter can draw, a page has to be set first.
        painter.SetPage( pPage );
        //Create a PdfFont object using the font "Arial". The font is found on the system using fontconfig and embedded into the
        //PDF file. If Arial is not available, a default font will be used. The created PdfFont will be deleted by the PdfDocument.
        pFont = doc.CreateFont( "Arial" );
        pFont->SetFontSize( 10.0 );
        //Set the font as default font for drawing. A font has to be set before you can draw text on a PdfPainter.
        painter.SetFont( pFont );
        painter.SetTransformationMatrix(1,0,0,-1,0,0);//Resuelve un bug. Si no esta el title se dibuja rotado en el eje Y unos 180 grados.
        //painter.DrawText( 10, 10, title.toStdString().data() );//Pone el title arriba del todo.
        painter.DrawText( 10, -925, text.toStdString().data() );//Pone el title abajo del todo (dentro de lo posible).
        //Tell PoDoFo that the page has been drawn completely. This required to optimize drawing operations inside in PoDoFo
        //and has to be done whenever you are done with drawing a page.
        painter.FinishPage();
    }
}

void PDFmerge::setOutputFileName(QString outputFile){
    this->outputFile = outputFile;
}

QString PDFmerge::getOutputFileName(){
    return this->outputFile;
}

//Imprime la salida en un archivo pdf
bool PDFmerge::writeOutput(QString outputFile){
    if(outputFile == "")
        outputFile = this->outputFile;
    try{
        QFile::remove(outputFile);
        this->document->Write(outputFile.toStdString().data());
    }catch(PoDoFo::PdfError){
        return false;
    }
    return true;
}

//Este método se ejecuta cuando el contenido html se termina de cargar en el objeto web
/*void PDFmerge::printHtmlToPdf(){
    this->web.print(&this->printer);
}*/

//Borra todas las páginas del documento
void PDFmerge::clearDocument(){
    this->document->DeletePages(0, this->document->GetPageCount());
}

//Agrega una página en blanco de tamaño A4 (eso es un parametro) para funcionar como separador
void PDFmerge::addPageSeparator(){
    this->document->CreatePage(this->document->GetPage(0)->GetPageSize());
}

void PDFmerge::addPageNeitherAssignmentsNorForumPost(QString tmpPath, QString studentName){

    QString message = "<body style='font: 18px arial, sans-serif;'><p align='center' style='font: 20px arial'>" +
                        studentName + " has not answered any assignments or any forum posts." + "</p></body>";
    QString tmpFileName = tmpPath + "/" + "NoAssigmentNoForumPost.pdf";
    htmlToPdf(message, tmpFileName);
    PdfMemDocument doc;
    doc.Load(tmpFileName.toStdString().data());
    this->document->Append(doc);
}
