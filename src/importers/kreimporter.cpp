/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kreimporter.h"

#include <klocale.h>

#include <qfile.h>
#include <qstringlist.h>

#include "recipe.h"


KreImporter::KreImporter(const QString& filename)
{
  QFile* file = 0;
  bool unlink = false;
	qDebug("loading file: %s",filename.latin1());

  if(filename.right(6) == ".kreml"){
	  file = new QFile( filename );
  }
  else if(filename.right(4) == ".kre"){
    qDebug("file is an archive");
    KTar* kre = new KTar(filename, "application/x-gzip");
    kre->open( IO_ReadOnly );
    const KArchiveDirectory* dir = kre->directory();
    QString name;
    QStringList fileList = dir->entries();
    for ( QStringList::Iterator it = fileList.begin(); it != fileList.end(); ++it ) {
        if( (*it).right(6) == ".kreml" ){
          name = *it;
        }
    }
    if(name == QString::null){
			qDebug("error: Archive doesn't contain a valid krecipes file");
			setErrorMsg( i18n("error: Archive doesn't contain a valid krecipes file") );
			return;
    }
    dir->copyTo("/tmp/");
	  file = new QFile( "/tmp/"+name );
    kre->close();
    unlink = true; //remove file after import
  }

	if ( file->open( IO_ReadOnly ) )
	{
		qDebug("file opened");
		QDomDocument doc;
		QString error; int line; int column;
		if (!doc.setContent(file,&error,&line,&column))
		{
			qDebug("error: \"%s\" at line %d, column %d",error.latin1(),line,column);
			setErrorMsg( QString( i18n("\"%1\" at line %2, column %3") ).arg(error).arg(line).arg(column) );
			return;
		}

		QDomElement kreml = doc.documentElement();

		if (kreml.tagName() != "krecipes-recipe" /*|| kreml.attribute("source") != "MasterCook 5.0"*/)
		{
			setErrorMsg( i18n("This file doesn't appear to be a *.kreml file") );
			return;
		}

		// TODO Check if there are changes between versions
    QString version = kreml.attribute("version");

		QDomNodeList l = kreml.childNodes();
    QDomElement el;

    Recipe *recipe = new Recipe;
		for (unsigned i = 0; i < l.count(); i++)
		{
		  el = l.item(i).toElement();
			if (el.tagName() == "krecipes-description"){
        readDescription(el.childNodes(), recipe);
      }
			if (el.tagName() == "krecipes-ingredients"){
        readIngredients(el.childNodes(), recipe);
      }
			if (el.tagName() == "krecipes-instructions"){
        recipe->instructions = el.text();
      }
		}
    add(recipe);
	}
  if(unlink){
    file->remove();
  }
}

KreImporter::~KreImporter()
{

}

void KreImporter::readDescription(const QDomNodeList& l, Recipe *recipe)
{
  ElementList authors;
  ElementList categoryList;
  for (unsigned i = 0; i < l.count(); i++)
  {
    QDomElement el = l.item(i).toElement();
    if (el.tagName() == "title"){
      recipe->title = el.text();
      qDebug("Found title: %s",recipe->title.latin1());
    }
    else if (el.tagName() == "author"){
      Element author;
      author.name = el.text();
      qDebug("Found author: %s",author.name.latin1());
      authors.add(author);
    }
		else if (el.tagName() == "serving"){
			recipe->persons = el.text().toInt();
    }
		else if (el.tagName() == "category"){
			QDomNodeList categories = el.childNodes();
			for (unsigned j=0; j < categories.count(); j++)
			{
				QDomElement c = categories.item(j).toElement();
				if (c.tagName() == "cat"){
						Element new_cat;
						new_cat.name = QString(c.text()).stripWhiteSpace();
						qDebug("Found category: %s", new_cat.name.latin1() );
						categoryList.add( new_cat );
				}
			}
		}
		else if (el.tagName() == "pictures"){
			if (el.hasChildNodes())
			{
				QDomNodeList pictures = el.childNodes();
				for (unsigned j=0; j < pictures.count(); j++){
					QDomElement pic = pictures.item(j).toElement();
          QCString decodedPic;
					if (pic.tagName() == "pic")
            qDebug("Found photo");
            QPixmap pix;
            KCodecs::base64Decode(QCString(pic.text()), decodedPic);
            int len = decodedPic.size();
            QByteArray picData(len);
            memcpy(picData.data(),decodedPic.data(),len);
            bool ok = pix.loadFromData(picData, "JPEG");
            if(ok){
              recipe->photo = pix;
            }
				}
			}
		}
  }
  recipe->categoryList = categoryList;
  recipe->authorList = authors;
}

void KreImporter::readIngredients(const QDomNodeList& l, Recipe *recipe)
{
	IngredientList ingredientList;
	for (unsigned i=0; i < l.count(); i++){
    QDomElement el = l.item(i).toElement();
		if (el.tagName() == "ingredient"){
	    QDomNodeList ingredient = el.childNodes();
      Ingredient new_ing;
			for (unsigned j=0; j < ingredient.count(); j++){
				QDomElement ing = ingredient.item(j).toElement();
				if (ing.tagName() == "name"){
						new_ing.name = QString(ing.text()).stripWhiteSpace();
						qDebug("Found ingredient: %s", new_ing.name.latin1() );
				}
				else if (ing.tagName() == "amount"){
						new_ing.amount = (QString(ing.text()).stripWhiteSpace()).toDouble();
				}
				else if (ing.tagName() == "unit"){
						new_ing.units = QString(ing.text()).stripWhiteSpace();
				}
      }
		  ingredientList.add( new_ing );
    }
  }
	recipe->ingList = ingredientList;
}


