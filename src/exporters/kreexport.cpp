/***************************************************************************
 *   Copyright (C) 2003 by Cyril Bosselut (bosselut@b1project.com)         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "kreexport.h"

#include <qfile.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmdcodec.h>
#include <ktar.h>
#include <ktempfile.h>
#include <kglobal.h>
#include <kstandarddirs.h>

KreExporter::KreExporter( RecipeDB *db, const QString& filename, const QString format ) :
  BaseExporter( db, filename, format )
{
	possible_formats << ".kre" << ".kreml";
}


KreExporter::~KreExporter()
{
}

void KreExporter::saveToFile( const QPtrList<Recipe>& recipes )
{
	if(format == "kreml")
	{
        	if ( file->open( IO_WriteOnly ) )
		{
			QTextStream stream( file );
			stream << createKRE(recipes);
			file->close();
		}
	}
	else{
        // create a temporary .kre file
        QString kreml = createKRE(recipes);
        int size = kreml.length();
        // compress and save file
        KTar* kre = new KTar(file->name(), "application/x-gzip");
        kre->open( IO_WriteOnly );
        kre->writeFile(filename+".kreml",getenv( "LOGNAME" ), "", size, kreml);
        kre->close();
	delete kre;
      }
}

/*!
    \fn KreManager::createKRE()
 * return a QString containing XML encoded recipe
 */
QString KreExporter::createKRE( const QPtrList<Recipe>& recipes )
{
    QString xml;

    xml = "<krecipes version=\"0.4\" lang=\""+(KGlobal::locale())->country()+"\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"krecipes.xsd\">\n";

	QPtrListIterator<Recipe> recipes_it( recipes );
	Recipe *recipe;
	while ( (recipe = recipes_it.current()) != 0 )
	{
	++recipes_it;

    xml +="<krecipes-recipe>\n";
    xml += "<krecipes-description>\n";
    xml += "<title>"+recipe->title.utf8()+"</title>\n";

	QPtrListIterator<Element> author_it( recipe->authorList );
	Element *author;
	while ( (author = author_it.current()) != 0 )
	{
		++author_it;
		xml += "<author>"+author->name.utf8()+"</author>\n";
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
    delete fn;

    xml += "]]></pic>\n";
    xml += "</pictures>\n";
    xml += "<category>\n";

	QPtrListIterator<Element> cat_it( recipe->categoryList );
	Element *cat;
	while ( (cat = cat_it.current()) != 0 )
	{
		++cat_it;
		xml += "<cat>"+cat->name.utf8()+"</cat>\n";
	}

    xml += "</category>\n";
    xml += "<serving>";
    xml += QString::number(recipe->persons);
    xml += "</serving>\n";
    xml += "</krecipes-description>\n";
    xml += "<krecipes-ingredients>\n";

	QPtrListIterator<Ingredient> ing_it( recipe->ingList );
	Ingredient *ing;
	while ( (ing = ing_it.current()) != 0 )
	{
		++ing_it;
      xml += "<ingredient>\n";
      xml += "<name>"+ing->name.utf8()+"</name>\n";
      xml += "<amount>";
      xml += QString::number(ing->amount);
      xml += "</amount>\n";
      xml += "<unit>"+ing->units.utf8()+"</unit>\n";
      xml += "</ingredient>\n";
      /// @todo add ingredient properties
	}

    xml += "</krecipes-ingredients>\n";
    xml += "<krecipes-instructions>\n";
    xml += recipe->instructions.utf8();
    xml += "</krecipes-instructions>\n";
    xml += "</krecipes-recipe>\n";
    }
    
    xml += "</krecipes>\n";

    return xml;
}

