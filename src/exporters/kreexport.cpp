/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kreexport.h"

KreExporter::KreExporter( RecipeDB *db, const int recipeId, const QString& filename = QString::null, const QString format = QString::null )
{
  database = db;
  recipe = new Recipe();
  recipe->recipeID = -1;
  database->loadRecipe( recipe, recipeId );
  if(filename != QString::null){
    if(filename.contains(".kre") || filename.contains(".kreml")){
      file = new QFile(filename);
    }
    else{
      QString form = format;
      form = form.remove('*');
      if(form != ".kre" && form != ".kreml"){
        form = ".kre";
      }
      file = new QFile(filename+form);
    }
    QFileInfo fi(*file);
    type = fi.extension();
    name = (fi.fileName()).remove("."+type);
    exportEnable = true;
  }
  else{
    exportEnable = false;
  }
}


KreExporter::~KreExporter()
{
}




/*!
    \fn KreExporter::export()
 * export recipe to XML and save it to a file
 */
void KreExporter::exporter()
{
  if(exportEnable){
    bool fileExists = file->exists();
    int overwrite;
    if(fileExists){
      overwrite = KMessageBox::questionYesNo( 0,i18n("File ")+file->name()+i18n(" exists. Would you like to overwrite it?"),i18n("Saving recipe") );
    }
    if(!fileExists || overwrite == KMessageBox::Yes){
      if(type == "kreml"){
        if ( file->open( IO_WriteOnly ) ) {
          QTextStream stream( file );
          stream << createKRE();
          file->close();
        }
      }
      else{
        // create a temporary .kre file
        QString kreml = createKRE();
        int size = kreml.length();
        // compress and save file
        KTar* kre = new KTar(file->name(), "application/x-gzip");
        kre->open( IO_WriteOnly );
        kre->writeFile(name+".kreml",getenv( "LOGNAME" ), "", size, kreml);
        kre->close();
      }
    }
  }
  else{
    qDebug("no output file have been selected for export.");
  }
}


/*!
    \fn KreManager::createKRE()
 * return a QString containing XML encoded recipe
 */
QString KreExporter::createKRE()
{
    QString xml;
    xml = "<krecipes-recipe version=\"0.1\" lang=\""+(KGlobal::locale())->country()+"\" >\n";
    xml += "<krecipes-description>\n";
    xml += "<title>"+recipe->title.utf8()+"</title>\n";
    for (Element *el = (recipe->authorList).getFirst(); el; el= (recipe->authorList).getNext()){
      xml += "<author>"+el->name.utf8()+"</author>\n";
    }
    xml += "<pictures>\n";
    xml += "<pic format=\"JPEG\" id=\"1\"><![CDATA["; //fixed id until we implement multiple photos ability
    KTempFile* fn = new KTempFile (locateLocal("tmp", "kre"), ".jpg", 0600);
    recipe->photo.save(fn->name(), "JPEG");
    QByteArray data;
    if( fn ){
      data = (fn->file())->readAll();
      fn->close();
      xml += KCodecs::base64Encode(data, true);
    }
    xml += "]]></pic>\n";
    xml += "</pictures>\n";
    xml += "<category>\n";
    for (Element *ct = (recipe->categoryList).getFirst(); ct; ct= (recipe->categoryList).getNext()){
      xml += "<cat>"+ct->name.utf8()+"</cat>\n";
    }
    xml += "</category>\n";
    xml += "<serving>";
    xml += QString::number(recipe->persons);
    xml += "</serving>\n";
    xml += "</krecipes-description>\n";
    xml += "<krecipes-ingredients>\n";
    for (Ingredient* in = (recipe->ingList).getFirst(); in; in= (recipe->ingList).getNext()){
      xml += "<ingredient>\n";
      xml += "<name>"+in->name.utf8()+"</name>\n";
      xml += "<amount>";
      xml += QString::number(in->amount);
      xml += "</amount>\n";
      xml += "<unit>"+in->units.utf8()+"</unit>\n";
      xml += "</ingredient>\n";
      /// @todo add ingredient properties
    }
    xml += "</krecipes-ingredients>\n";
    xml += "<krecipes-instructions>\n";
    xml += recipe->instructions.utf8();
    xml += "</krecipes-instructions>\n";
    xml += "</krecipes-recipe>\n";
    return xml;
}
