/*
Copyright (C) 2003 Richard Lärkäng and Jason Kivlighn

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

#include "mx2importer.h"

#include <klocale.h>
#include <kdebug.h>

#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include "recipe.h"


MX2Importer::MX2Importer(const QString& filename)
{
	QFile file( filename );
	kdDebug()<<"loading file: "<<filename<<endl;
	if ( file.open( IO_ReadOnly ) )
	{
		kdDebug()<<"file opened"<<endl;
		QDomDocument doc;

		//hopefully a temporary hack, since MasterCook creates invalid xml declarations
		QTextStream stream(&file);
		QString all_data = stream.read();
		if ( all_data.startsWith("<?xml") )
			all_data.remove(0,all_data.find("?>")+2);

		QString error; int line; int column;
		if (!doc.setContent(all_data,&error,&line,&column))
		{
			kdDebug()<<QString("error: \"%1\" at line %2, column %3").arg(error).arg(line).arg(column)<<endl;
			setErrorMsg( QString( i18n("\"%1\" at line %2, column %3.  This may not be a *.mx2 file.") ).arg(error).arg(line).arg(column) );
			return;
		}

		QDomElement mx2 = doc.documentElement();

		// TODO Check if there are changes between versions
		if (mx2.tagName() != "mx2" /*|| mx2.attribute("source") != "MasterCook 5.0"*/)
		{
			setErrorMsg( i18n("This file doesn't appear to be a *.mx2 file") );
			return;
		}

		QDomNodeList l = mx2.childNodes();

		for (unsigned i = 0; i < l.count(); i++)
		{
			QDomElement el = l.item(i).toElement();

			if (el.tagName() == "RcpE")
			{
				Recipe *recipe = new Recipe;
				recipe->title = el.attribute("name");

				Element author( el.attribute("author") );
				recipe->authorList.add( author );

				readRecipe( el.childNodes(), recipe );
				add(recipe);
			}
		}
	}
	else
		setErrorMsg(i18n("Unable to open file."));
}

MX2Importer::~MX2Importer()
{

}

void MX2Importer::readRecipe(const QDomNodeList& l, Recipe *recipe)
{
	for (unsigned i=0; i < l.count(); i++)
	{
		QDomElement el = l.item(i).toElement();

		QString tagName = el.tagName();
		if (tagName == "Serv")
			recipe->persons = el.attribute("qty").toInt();
		else if (tagName == "PrpT")
			;//recipe->prep_time = el.attrubute("elapsed");
		else if (tagName == "CatS")
		{
			QDomNodeList categories = el.childNodes();
			for (unsigned j=0; j < categories.count(); j++)
			{
				QDomElement c = categories.item(j).toElement();
				if (c.tagName() == "CatT")
				{
					if (c.text().length() > 0)
					{
						Element cat( c.text().stripWhiteSpace() );
						recipe->categoryList.add( cat );
					}
				}
			}
		}
		else if (tagName == "IngR")
		{
			Ingredient new_ing( el.attribute("name"),
			  el.attribute("qty").toDouble(),
			  el.attribute("unit") );
			if (el.hasChildNodes())
			{
				QDomNodeList iChilds = el.childNodes();
				for (unsigned j=0; j < iChilds.count(); j++)
				{
					QDomElement iChild = iChilds.item(j).toElement();
					if (iChild.tagName() == "IPrp")
						new_ing.name += " -- " + iChild.text().stripWhiteSpace();
					else if (iChild.tagName() == "INtI")
						; // TODO: What does it mean?... ingredient nutrient info?
				}
			}
			recipe->ingList.add(new_ing);
		}
		else if (tagName == "DirS")
		{
			QStringList directions;
			QDomNodeList dirs = el.childNodes();
			for (unsigned j=0; j < dirs.count(); j++)
			{
				QDomElement dir = dirs.item(j).toElement();
				if (dir.tagName() == "DirT")
					directions.append(dir.text().stripWhiteSpace());
			}
			QString directionsText;

			// TODO This is copied from RecipeML, maybe a QStringList
			//	for directions in Recipe instead?
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
		else if (tagName == "SrvI")
		{
			// Don't know what to do with it, for now add it to directions
			// btw lets hope this is read after the directions
			recipe->instructions += "\n\n"+el.text().stripWhiteSpace();
		}
		else if (tagName == "Note")
		{
			// Don't know what to do with it, for now add it to directions
			// btw lets hope this is read after the directions
			recipe->instructions += "\n\n"+el.text().stripWhiteSpace();
		}
		else if (tagName == "Nutr")
		{
			//example: <Nutr>Per Serving (excluding unknown items): 51 Calories; 6g Fat (99.5% calories from fat); trace Protein; trace Carbohydrate; 0g Dietary Fiber; 16mg Cholesterol; 137mg Sodium.  Exchanges: 1 Fat.</Nutr>
			// Don't know what to do with it, for now add it to directions
			// btw lets hope this is read after the directions
			recipe->instructions += "\n\n"+el.text().stripWhiteSpace();
		}
		/* tags to check for (example follows:
		<Srce>SARA&apos;S SECRETS with Sara Moulton - (Show # SS-1B43) - from the TV FOOD NETWORK</Srce>
		<AltS label="Formatted for MC7" source="07-11-2003  by Joe Comiskey - Mad&apos;s Recipe Emporium"/>
		*/
		// TODO Have i missed some tag?
	}
}

