/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kreexport.h"

#include <qfile.h>
#include <qstylesheet.h>
#include <qbuffer.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmdcodec.h>
#include <ktar.h>
#include <ktempfile.h>
#include <kglobal.h>
#include <kstandarddirs.h>

KreExporter::KreExporter( CategoryTree *_categories, const QString& filename, const QString &format ) :
  BaseExporter( filename, format ), categories(_categories)
{
}


KreExporter::~KreExporter()
{
	delete categories;
}

void KreExporter::saveToFile( const RecipeList& recipes )
{
	if(format == "kreml")
	{
        	if ( file->open( IO_WriteOnly ) )
		{
			QTextStream stream( file );
			stream << createContent(recipes);
			file->close();
		}
	}
	else{
        // create a temporary .kre file
        QString kreml = createContent(recipes);
        int size = kreml.length();
        // compress and save file
        KTar* kre = new KTar(file->name(), "application/x-gzip");
        kre->open( IO_WriteOnly );
        kre->writeFile(filename+".kreml",getenv( "LOGNAME" ), "", size, kreml.latin1());
        kre->close();
	delete kre;
      }
}

/*!
    \fn KreManager::createContent()
 * return a QString containing XML encoded recipe
 */
 //TODO: use QDOM (see recipemlexporter.cpp)?
QString KreExporter::createContent( const RecipeList& recipes )
{
    QString xml;

    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    xml += "<krecipes version=\""+krecipes_version()+"\" lang=\""+(KGlobal::locale())->country()+"\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"krecipes.xsd\">\n";

    createCategoryStructure(xml,recipes);
    
	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it )
	{

    xml +="<krecipes-recipe>\n";
    xml += "<krecipes-description>\n";
    xml += "<title>"+QStyleSheet::escape((*recipe_it).title.utf8())+"</title>\n";

	for ( ElementList::const_iterator author_it = (*recipe_it).authorList.begin(); author_it != (*recipe_it).authorList.end(); ++author_it )
		xml += "<author>"+QStyleSheet::escape((*author_it).name.utf8())+"</author>\n";

    xml += "<pictures>\n";
    if ( !(*recipe_it).photo.isNull() ) {
    xml += "<pic format=\"JPEG\" id=\"1\"><![CDATA["; //fixed id until we implement multiple photos ability
	QByteArray data;
	QBuffer buffer( data );
	buffer.open( IO_WriteOnly );
	(*recipe_it).photo.save( &buffer, "JPEG" );
	
	xml += KCodecs::base64Encode(data, true);

    xml += "]]></pic>\n";
    }
    xml += "</pictures>\n";
    xml += "<category>\n";

	for ( ElementList::const_iterator cat_it = (*recipe_it).categoryList.begin(); cat_it != (*recipe_it).categoryList.end(); ++cat_it )
		xml += "<cat>"+QStyleSheet::escape((*cat_it).name.utf8())+"</cat>\n";

    xml += "</category>\n";
    xml += "<serving>";
    xml += QString::number((*recipe_it).persons);
    xml += "</serving>\n";
    xml += "<preparation-time>";
    xml += (*recipe_it).prepTime.toString("hh:mm");
    xml += "</preparation-time>\n";
    xml += "</krecipes-description>\n";
    xml += "<krecipes-ingredients>\n";

	IngredientList list_copy = (*recipe_it).ingList;
	for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() )
	{
		QString group = group_list[0].group; //just use the first's name... they're all the same
		if ( !group.isEmpty() )
			xml += "<ingredient-group name=\""+group+"\">\n";

		for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
			xml += "<ingredient>\n";
			xml += "<name>"+QStyleSheet::escape((*ing_it).name.utf8())+"</name>\n";
			xml += "<amount>";
			xml += QString::number((*ing_it).amount);
			xml += "</amount>\n";
			QString unit_str = ((*ing_it).amount>1)?(*ing_it).units.plural:(*ing_it).units.name;
			xml += "<unit>"+QStyleSheet::escape(unit_str.utf8())+"</unit>\n";
			if ( !(*ing_it).prepMethod.isEmpty() ) xml += "<prep>"+QStyleSheet::escape((*ing_it).prepMethod.utf8())+"</prep>\n";
			xml += "</ingredient>\n";
		}

		if ( !group.isEmpty() )
			xml += "</ingredient-group>\n";
	}

      /// @todo add ingredient properties

    xml += "</krecipes-ingredients>\n";
    xml += "<krecipes-instructions>\n";
    xml += QStyleSheet::escape((*recipe_it).instructions.utf8());
    xml += "</krecipes-instructions>\n";
    xml += "</krecipes-recipe>\n";
    
    if ( progressBarCancelled() ) return QString::null;
    advanceProgressBar();
    }
    
    xml += "</krecipes>\n";

    return xml;
}

void KreExporter::createCategoryStructure( QString &xml, const RecipeList &recipes )
{
	QValueList<int> categoriesUsed;
	for ( RecipeList::const_iterator recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		for ( ElementList::const_iterator cat_it = (*recipe_it).categoryList.begin(); cat_it != (*recipe_it).categoryList.end(); ++cat_it ) {
			if ( categoriesUsed.find( (*cat_it).id ) == categoriesUsed.end() )
				categoriesUsed << (*cat_it).id;
		}
	}

	//only keep the relevant category structure
	removeIfUnused(categoriesUsed,categories);


	xml += "<krecipes-category-structure>\n";
	writeCategoryStructure(xml,categories);
	xml += "</krecipes-category-structure>\n";
}

bool KreExporter::removeIfUnused( const QValueList<int> &cat_ids, CategoryTree *parent, bool parent_should_show )
{
	for ( CategoryTree *it = parent->firstChild(); it; it = it->nextSibling() ) {
		if ( cat_ids.find(it->category.id) != cat_ids.end() ) {
			parent_should_show = true;
			removeIfUnused(cat_ids,it,true); //still recurse, but doesn't affect 'parent'
		}
		else {
			bool result = removeIfUnused(cat_ids,it);
			if ( parent_should_show == false )
				parent_should_show = result;
		}
	}
	
	if ( !parent_should_show ) {
		delete parent;
	}

	return parent_should_show;
}

void KreExporter::writeCategoryStructure(QString &xml, const CategoryTree *categoryTree )
{
	if ( categoryTree->category.id != -1 )
		xml += "<category name=\""+QStyleSheet::escape(categoryTree->category.name.utf8())+"\">\n";

	const CategoryTreeChildren *children = categoryTree->children();
	for ( CategoryTreeChildren::const_iterator child_it = children->begin(); child_it != children->end(); ++child_it ) {
		writeCategoryStructure( xml, *child_it );
	}

	if ( categoryTree->category.id != -1 )
		xml += "</category>\n";

}

