/*
Copyright (C) 2003 by
   Richard Lärkäng
   Jason Kivlighn <mizunoami44@users.sourceforge.net>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "recipemlimporter.h"

#include <qfile.h>

#include <klocale.h>

#include "recipe.h"

RecipeMLImporter::RecipeMLImporter( const QString& file ) : BaseImporter()
{
	QFile input( file );
	if ( input.open( IO_ReadOnly ) )
	{
		recipe = new Recipe();
		QDomDocument doc;
		QString error; int line; int column;
		if (!doc.setContent(&input,&error,&line,&column))
		{
			setErrorMsg( QString( i18n("\"%1\" at line %2, column %3.  This may not be a *.mx2 file.") ).arg(error).arg(line).arg(column) );
			return;
		}

	//	if (doc.doctype().name() != "recipeml") {
	//		return false;
	//	}

		QDomElement recipeml = doc.documentElement();

		QDomNodeList l = recipeml.childNodes();

		for (unsigned i = 0 ; i < l.count(); i++)
		{
			QDomElement el = l.item(i).toElement();
			QString tagName = el.tagName();

			if (tagName == "meta")
				continue;

			if (tagName == "recipe")
				readRecipemlRecipe(el);
		}

		add(recipe);
	}
	else
		error_code = FileOpenError;
}

RecipeMLImporter::~RecipeMLImporter()
{

}

void RecipeMLImporter::readRecipemlRecipe(const QDomElement& recipe)
{
	QDomNodeList l = recipe.childNodes();

	for (unsigned i = 0; i < l.count(); i++) {
		QDomElement el = l.item(i).toElement();
		QString tagName = el.tagName();

		if (tagName == "head")
			readRecipemlHead(el);
		else if (tagName == "ingredients")
			readRecipemlIngs(el);
		else if (tagName == "directions")
			readRecipemlDirections(el);
	}
}

void RecipeMLImporter::readRecipemlHead(const QDomElement& head)
{
	QDomNodeList l = head.childNodes();
	for (unsigned i = 0 ; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();
		QString tagName = el.tagName();

		if (tagName == "title")
			recipe->title = el.text();
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
			recipe->instructions += "\n\nDescription: "+el.text();
		else if (tagName == "preptime")
			// TODO check for "range, sep, timeunit" etc
			recipe->instructions += "\n\nPreparation time: "+el.text();
		else if (tagName == "yield")
			// TODO check for "range, sep, unit" etc
			recipe->persons = el.text().toInt();
		else if (tagName == "source")
		{
			// TODO check for "sourceitem's"
			recipe->authorList.append( new Element(el.text()) );
		}
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
		{
			readRecipemlIng(el);
		}
		else if (tagName == "ing-div")
		{
			// TODO Wouldn't this be better as a recursive function?
			QDomNodeList ingDiv = el.childNodes();
			for (unsigned j = 0; j < ingDiv.count(); j++)
			{
				QDomElement cEl = ingDiv.item(j).toElement();
				if (cEl.tagName() == "title")
				{
					QString name = cEl.text();
					if (!name.endsWith(":"))
						name += ":";

					recipe->ingList.append( new Ingredient( name, 0, "") );
				}
				else if (cEl.tagName() == "ing")
				{
					readRecipemlIng(cEl);
				}
			}
		}
	}
}

void RecipeMLImporter::readRecipemlIng(const QDomElement& ing )
{
	QDomNodeList ingChilds = ing.childNodes();

	QString name, unit;
	double quantity = 0;

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
					quantity = amtChild.text().toDouble();
				else if (amtChild.tagName() == "unit")
					unit = amtChild.text();
			}
		}
		else if (tagName == "item")
			name = ingChild.text();
	}
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
		{
			directions.append(el.text());
		}
	}

	QString directionsText;

	if (directions.count() > 1)
	{
		for (unsigned i=1; i <= directions.count(); i++)
		{
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
