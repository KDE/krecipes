/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cookmlexporter.h"

#include <qdom.h>
#include <qpixmap.h>
#include <qfile.h>

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <ktempfile.h>
#include <kmdcodec.h>
#include <kglobal.h>
#include <kstandarddirs.h>

CookMLExporter::CookMLExporter( RecipeDB *db, const QString& filename, const QString format ) :
  BaseExporter( db, filename, format )
{
}


CookMLExporter::~CookMLExporter()
{
}

QString CookMLExporter::createContent( const RecipeList& recipes )
{
	QDomImplementation dom_imp;
	QDomDocument doc = dom_imp.createDocument( QString::null, "cookml", dom_imp.createDocumentType( "cookml", QString::null, "cookml.dtd") );

	QDomElement cookml_tag = doc.documentElement();
	cookml_tag.setAttribute( "version", "1.0.13" );
	cookml_tag.setAttribute( "prog", "Krecipes" );
	cookml_tag.setAttribute( "progver", krecipes_version() );

	doc.appendChild( cookml_tag );

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{
		QDomElement recipe_tag = doc.createElement("recipe");
		recipe_tag.setAttribute("lang",(KGlobal::locale())->language());

		cookml_tag.appendChild( recipe_tag );

			QDomElement head_tag = doc.createElement("head");
			head_tag.setAttribute("title",(*recipe_it).title);
			head_tag.setAttribute("servingqty",(*recipe_it).persons);
			head_tag.setAttribute("servingtype",i18n("Persons"));
			head_tag.setAttribute("rid",i18n("")); //FIXME:what's this...recipe ID??
			recipe_tag.appendChild( head_tag );

				QPtrListIterator<Element> cat_it( (*recipe_it).categoryList );
				Element *cat;
				while ( (cat = cat_it.current()) != 0 )
				{
					++cat_it;
					QDomElement cat_tag = doc.createElement("cat");
					cat_tag.appendChild( doc.createTextNode(cat->name) );
					head_tag.appendChild(cat_tag);
				}

				QPtrListIterator<Element> author_it( (*recipe_it).authorList );
				Element *author;
				while ( (author = author_it.current()) != 0 )
				{
					++author_it;
					QDomElement sourceline_tag = doc.createElement("sourceline");
					sourceline_tag.appendChild( doc.createTextNode(author->name) );
					head_tag.appendChild(sourceline_tag);
				}

				QDomElement picbin_tag = doc.createElement( "picbin");
				picbin_tag.setAttribute("format","JPG");

				KTempFile* fn = new KTempFile(locateLocal("tmp", "cml"), ".jpg", 0600);
				fn->setAutoDelete(true);
				(*recipe_it).photo.save(fn->name(), "JPEG");
				QByteArray data;
				if( fn )
				{
					data = (fn->file())->readAll();
					fn->close();
					delete fn;
				}
				picbin_tag.appendChild( doc.createTextNode( KCodecs::base64Encode(data, true) ) );
				head_tag.appendChild( picbin_tag );

			QDomElement part_tag = doc.createElement("part");
			QPtrListIterator<Ingredient> ing_it( (*recipe_it).ingList );
			Ingredient *ing;
			while ( (ing = ing_it.current()) != 0 )
			{
				++ing_it;

				QDomElement ingredient_tag = doc.createElement("ingredient");
				ingredient_tag.setAttribute("qty",QString::number(ing->amount));
				ingredient_tag.setAttribute("unit",ing->units);
				ingredient_tag.setAttribute("item",ing->name);
				part_tag.appendChild( ingredient_tag );
			}
			recipe_tag.appendChild( part_tag );

			QDomElement preparation_tag = doc.createElement("preparation");
			recipe_tag.appendChild( preparation_tag );

				QDomElement text_tag = doc.createElement("text");
				preparation_tag.appendChild( text_tag );
				text_tag.appendChild( doc.createTextNode((*recipe_it).instructions) );
	}

	QString ret = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	return ret.utf8() + doc.toString().utf8();
}
