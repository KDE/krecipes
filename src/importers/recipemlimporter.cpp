/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *   Richard Lärkäng                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "recipemlimporter.h"

#include <qfile.h>

#include <klocale.h>
#include <kdebug.h>

#include "recipe.h"
#include "mixednumber.h"

RecipeMLImporter::RecipeMLImporter( const QString& file ) : BaseImporter()
{
	QFile input( file );
	if ( input.open( IO_ReadOnly ) )
	{
		QDomDocument doc;
		QString error; int line; int column;
		if (!doc.setContent(&input,&error,&line,&column))
		{
			setErrorMsg( QString( i18n("\"%1\" at line %2, column %3.  This may not be a RecipeML file.") ).arg(error).arg(line).arg(column) );
			return;
		}

		QDomElement recipeml = doc.documentElement();

		if ( recipeml.tagName() != "recipeml" )
		{
			setErrorMsg( i18n("This file does not appear to be a valid RecipeML archive.") );
			return;
		}

		QDomNodeList l = recipeml.childNodes();

		for (unsigned i = 0 ; i < l.count(); i++)
		{
			QDomElement el = l.item(i).toElement();
			QString tagName = el.tagName();

			if (tagName == "meta")
				continue;
			else if (tagName == "recipe")
				readRecipemlRecipe(el);
			else if (tagName == "menu")
				readRecipemlMenu(el);
			else
				kdDebug()<<"Unknown tag within <recipeml>: "<<tagName<<endl;
		}
	}
	else
		setErrorMsg( i18n("Unable to open file.") );
}

RecipeMLImporter::~RecipeMLImporter()
{
}

void RecipeMLImporter::readRecipemlRecipe(const QDomElement& recipe_element)
{
	recipe = new Recipe();

	QDomNodeList l = recipe_element.childNodes();

	for (unsigned i = 0; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();
		QString tagName = el.tagName();

		if (tagName == "head")
			readRecipemlHead(el);
		else if (tagName == "ingredients")
			readRecipemlIngs(el);
		else if ( tagName == "description" )
			{}//TODO: what do we do with this?
		else if ( tagName == "equipment" )
			{}//TODO: what do we do with this?
		else if (tagName == "directions")
			readRecipemlDirections(el);
		else if ( tagName == "nutrition" )
			{}//TODO: what do we do with this?
		else if ( tagName == "diet-exchanges" )
			{}//TODO: what do we do with this?
		else
			kdDebug()<<"Unknown tag within <recipe>: "<<el.tagName()<<endl;
	}

	add(recipe);
}

void RecipeMLImporter::readRecipemlHead(const QDomElement& head)
{
	QDomNodeList l = head.childNodes();
	for (unsigned i = 0 ; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();
		QString tagName = el.tagName();

		if (tagName == "title")
			recipe->title = el.text().stripWhiteSpace();
		else if (tagName == "subtitle")
			recipe->title += ": " + el.text().stripWhiteSpace();
		else if ( tagName == "version" )
			{}//TODO: what do we do with this?
		else if (tagName == "source")
			readRecipemlSrcItems(el);
		else if (tagName == "categories")
		{
			QDomNodeList categories = el.childNodes();
			for (unsigned j=0; j< categories.count(); j++)
			{
				QDomElement c = categories.item(j).toElement();
				if (c.tagName() == "cat")
				{
					recipe->categoryList.append( new Element(c.text()) );
				}
			}
		}
		else if (tagName == "description")
			recipe->instructions += "\n\nDescription: "+el.text().stripWhiteSpace();
		else if (tagName == "preptime")
			// TODO check for "range, sep, timeunit" etc
			recipe->instructions += "\n\nPreparation time: "+el.text().stripWhiteSpace();
		else if (tagName == "yield")
			// TODO check for "range, sep, unit" etc
			recipe->persons = el.text().toInt();
		else
			kdDebug()<<"Unknown tag within <head>: "<<el.tagName()<<endl;
	}
}

void RecipeMLImporter::readRecipemlIngs(const QDomElement& ings)
{
	QDomNodeList l = ings.childNodes();
	for (unsigned i = 0 ; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();
		QString tagName = el.tagName();

		if (tagName == "ing")
			readRecipemlIng(el);
		else if (tagName == "ing-div") //NOTE: this can have the "type" attribute
		{
			// TODO Wouldn't this be better as a recursive function?
			QDomNodeList ingDiv = el.childNodes();
			for (unsigned j = 0; j < ingDiv.count(); j++)
			{
				QDomElement cEl = ingDiv.item(j).toElement();
				if (cEl.tagName() == "title")
				{
					QString name = cEl.text().stripWhiteSpace();
					if (!name.endsWith(":"))
						name += ":";

					recipe->ingList.append( new Ingredient( name, 0, "") );
				}
				else if (cEl.tagName() == "description" )
					{}//TODO: what do we do with this?
				else if (cEl.tagName() == "ing")
					readRecipemlIng(cEl);
				else if ( tagName == "note" )
					{}//TODO: what do we do with this?
				else
					kdDebug()<<"Unknown tag within <ing-div>: "<<cEl.tagName()<<endl;
			}
		}
		else if (tagName == "note" )
			{}//TODO: what do we do with this?
		else
			kdDebug()<<"Unknown tag within <ingredients>: "<<el.tagName()<<endl;
	}
}

void RecipeMLImporter::readRecipemlIng(const QDomElement& ing )
{
	QDomNodeList ingChilds = ing.childNodes();

	QString name, unit, size;
	double quantity = 1;

	for (unsigned j=0; j < ingChilds.count(); j++)
	{
		QDomElement ingChild = ingChilds.item(j).toElement();
		QString tagName = ingChild.tagName();

		if (tagName == "amt")
		{
			QDomNodeList amtChilds = ingChild.childNodes();

			for (unsigned k=0; k < amtChilds.count(); k++)
			{
				QDomElement amtChild = amtChilds.item(k).toElement();

				if (amtChild.tagName() == "qty")
					quantity = MixedNumber::fromString(amtChild.text()).toDouble();
				else if (amtChild.tagName() == "size")
					size = amtChild.text().stripWhiteSpace();
				else if (amtChild.tagName() == "unit")
					unit = amtChild.text().stripWhiteSpace();
				else
					kdDebug()<<"Unknown tag within <amt>: "<<amtChild.tagName()<<endl;
			}
		}
		else if (tagName == "item")
		{
			name = ingChild.text().stripWhiteSpace();
			if (ing.attribute( "optional", "no" ) == "yes" ) name += " (optional)";
		}
		else
			kdDebug()<<"Unknown tag within <ing>: "<<ingChild.tagName()<<endl;
	}

	if ( !size.isNull() )
		unit.prepend(size+" ");

	recipe->ingList.append( new Ingredient(name, quantity, unit) );
}

void RecipeMLImporter::readRecipemlDirections(const QDomElement& dirs)
{
	QDomNodeList l = dirs.childNodes();

	QStringList directions;

	for (unsigned i = 0 ; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();

		if (el.tagName()="step")
			directions.append(el.text().stripWhiteSpace());
		else
			kdDebug()<<"Unknown tag within <directions>: "<<el.tagName()<<endl;
	}

	QString directionsText;

	if (directions.count() > 1)
	{
		for (unsigned i=1; i <= directions.count(); i++)
		{
			if ( i != 1 ){directionsText += "\n\n";}

			QString sWith = QString("%1. ").arg(i);
			QString text = directions[i-1];
			if (!text.stripWhiteSpace().startsWith(sWith))
				directionsText += sWith;
			directionsText += text;
		}
	}
	else
		directionsText = directions[0];

	recipe->instructions = directionsText;
}

void RecipeMLImporter::readRecipemlMenu(const QDomElement& menu_el)
{
	QDomNodeList l = menu_el.childNodes();
	for (unsigned i = 0 ; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();
		QString tagName = el.tagName();

		if ( tagName == "head" )
			readRecipemlHead(el);
		else if ( tagName == "description" )
			{}//TODO: what do we do with this?
		else if ( tagName == "recipe" )
			readRecipemlRecipe(el);
		else
			kdDebug()<<"Unknown tag within <menu>: "<<tagName<<endl;
	}
}

void RecipeMLImporter::readRecipemlSrcItems(const QDomElement& sources)
{
	QDomNodeList l = sources.childNodes();
	for (unsigned i = 0 ; i < l.count(); i++)
	{
		QDomElement srcitem = l.item(i).toElement();
		QString tagName = srcitem.tagName();

		if ( tagName == "srcitem" )
			recipe->authorList.append( new Element( srcitem.text().stripWhiteSpace() ) );
		else
			kdDebug()<<"Unknown tag within <source>: "<<tagName<<endl;
	}
}
