/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recipemlexporter.h"

#include <qdom.h>

#include <kdebug.h>
#include <klocale.h>

RecipeMLExporter::RecipeMLExporter( RecipeDB *db, const QString& filename, const QString format ) :
  BaseExporter( db, filename, format )
{
}


RecipeMLExporter::~RecipeMLExporter()
{
}

QString RecipeMLExporter::createContent( const RecipeList& recipes )
{
	QDomImplementation dom_imp;
	QDomDocument doc = dom_imp.createDocument( QString::null, "recipeml", dom_imp.createDocumentType( "recipeml", "-//FormatData//DTD RecipeML 0.5//EN", "http://www.formatdata.com/recipeml/recipeml.dtd") );

	QDomElement recipeml_tag = doc.documentElement();
	recipeml_tag.setAttribute( "version", 0.5 );
	recipeml_tag.setAttribute( "generator", QString("Krecipes v%1").arg(krecipes_version()) );
	doc.appendChild( recipeml_tag );

	QDomElement recipe_root = recipeml_tag;
	#if 0
	//I might use this later....
	if ( recipes.count() > 1 )
	{
		QDomElement menu_tag = doc.createElement("menu");
		recipeml_tag.appendChild( menu_tag );

			QDomElement description_tag = doc.createElement("description");
			description_tag.appendChild( doc.createTextNode( QString(i18n("Recipes in the category \"%1\"")).arg(filename) ) );
			menu_tag.appendChild(description_tag);

		recipe_root = menu_tag;
	}
	#endif

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{
		QDomElement recipe_tag = doc.createElement("recipe");

		recipe_root.appendChild( recipe_tag ); //will append to either <menu> if exists or else <recipeml>

			QDomElement head_tag = doc.createElement("head");
			recipe_tag.appendChild( head_tag );

				QDomElement title_tag = doc.createElement("title");
				title_tag.appendChild( doc.createTextNode((*recipe_it).title) );
				head_tag.appendChild( title_tag );

				QDomElement source_tag = doc.createElement("source");
				for ( ElementList::const_iterator author_it = (*recipe_it).authorList.begin(); author_it != (*recipe_it).authorList.end(); ++author_it )
				{
					QDomElement srcitem_tag = doc.createElement("srcitem");
					srcitem_tag.appendChild( doc.createTextNode((*author_it).name) );
					source_tag.appendChild(srcitem_tag);
				}
				head_tag.appendChild( source_tag );

				QDomElement categories_tag = doc.createElement("categories");
				for ( ElementList::const_iterator cat_it = (*recipe_it).categoryList.begin(); cat_it != (*recipe_it).categoryList.end(); ++cat_it )
				{
					QDomElement cat_tag = doc.createElement("cat");
					cat_tag.appendChild( doc.createTextNode( (*cat_it).name) );
					categories_tag.appendChild(cat_tag);
				}
				head_tag.appendChild(categories_tag);

				QDomElement yield_tag = doc.createElement("yield");
				yield_tag.appendChild( doc.createTextNode(QString::number((*recipe_it).persons)) );
				head_tag.appendChild( yield_tag );

			QDomElement ingredients_tag = doc.createElement("ingredients");
			for ( IngredientList::const_iterator ing_it = (*recipe_it).ingList.begin(); ing_it != (*recipe_it).ingList.end(); ++ing_it )
			{
				QDomElement ing_tag = doc.createElement("ing");
				ingredients_tag.appendChild( ing_tag );

					QDomElement amt_tag = doc.createElement("amt");
					ing_tag.appendChild( amt_tag );

						QDomElement qty_tag = doc.createElement("qty");
						amt_tag.appendChild(qty_tag);
						qty_tag.appendChild( doc.createTextNode(QString::number((*ing_it).amount)) );

						QDomElement unit_tag = doc.createElement("unit");
						amt_tag.appendChild(unit_tag);
						unit_tag.appendChild( doc.createTextNode((*ing_it).units) );

					QDomElement item_tag = doc.createElement("item");
					item_tag.appendChild( doc.createTextNode((*ing_it).name ) );
					ing_tag.appendChild( item_tag );


			}
			recipe_tag.appendChild( ingredients_tag );

			QDomElement directions_tag = doc.createElement("directions");
			recipe_tag.appendChild( directions_tag );

				QDomElement step_tag = doc.createElement("step"); //we've just got everything in one step
				directions_tag.appendChild( step_tag );
				step_tag.appendChild( doc.createTextNode((*recipe_it).instructions) );
	}

	QString ret = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	return ret.utf8() + doc.toString().utf8();
}
