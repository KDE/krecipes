/***************************************************************************
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kreexporter.h"

#include <QBuffer>
#include <QImage>
#include <QImageWriter>
#include <QTextDocument>

#include <kdebug.h>
#include <klocale.h>
#include <kcodecs.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kstandarddirs.h>

#include "backends/recipedb.h"

KreExporter::KreExporter( CategoryTree *_categories, const QString& filename, const QString &format, bool compatibleNumbers ) :
		BaseExporter( filename, format ), categories( _categories )
{
	if ( format == "*.kre" ) {
		setCompressed(true);
	}
	
	if ( !compatibleNumbers ) {
		KConfigGroup config = KGlobal::config()->group("Formatting");
		m_number_format = ( config.readEntry( "Fraction" , false ) ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;
		m_locale_aware_numbers = true;
	} else {
		m_number_format = MixedNumber::DecimalFormat;
		m_locale_aware_numbers = false;
	}
}


KreExporter::~KreExporter()
{
	delete categories;
}

int KreExporter::supportedItems() const
{
	return RecipeDB::All;
}

int KreExporter::headerFlags() const
{
	if (categories)
		return RecipeDB::Categories;
	else
		return 0;
}

QString KreExporter::createHeader( const RecipeList& recipes )
{
	QString xml;

	xml += "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
	xml += "<krecipes version=\"" + krecipes_version() + "\" lang=\"" + ( KGlobal::locale() )->language() + "\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"krecipes.xsd\">\n";

	createCategoryStructure( xml, recipes );

	return xml;
}

QString KreExporter::createFooter()
{
	return "</krecipes>\n";
}

QString KreExporter::generateIngredient( const IngredientData &ing )
{
	QString xml;

	xml += "<name>" + Qt::escape( ( ing ).name ) + "</name>\n";
	xml += "<amount>";
	if ( ing.amount_offset < 1e-10 ) {
		xml += MixedNumber( ing.amount ).toString( m_number_format, m_locale_aware_numbers );
	}
	else {
		xml += "<min>"+MixedNumber( ing.amount ).toString( m_number_format, m_locale_aware_numbers )+"</min>";
		xml += "<max>"+MixedNumber( ing.amount + ing.amount_offset ).toString( m_number_format, m_locale_aware_numbers )+"</max>";
	}
	xml += "</amount>\n";

	KConfigGroup config(KGlobal::config(),"Formatting");

	bool useAbbreviations = config.readEntry("AbbreviateUnits" , false );
	QString unit = ing.units.determineName( ing.amount + ing.amount_offset, useAbbreviations );
	xml += "<unit>" + Qt::escape( unit ) + "</unit>\n";

	if ( ing.prepMethodList.count() > 0 )
		xml += "<prep>" + Qt::escape( ing.prepMethodList.join(",") ) + "</prep>\n";

	return xml;
}

//TODO: use QDOM (see recipemlexporter.cpp)?
QString KreExporter::createContent( const RecipeList& recipes )
{
	QString xml;

	RecipeList::const_iterator recipe_it;
	for ( recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {

		xml += "<krecipes-recipe id=\""+QString::number((*recipe_it).recipeID)+"\">\n";
		xml += "<krecipes-description>\n";
		xml += "<title>" + Qt::escape( ( *recipe_it ).title ) + "</title>\n";

		for ( ElementList::const_iterator author_it = ( *recipe_it ).authorList.begin(); author_it != ( *recipe_it ).authorList.end(); ++author_it )
			xml += "<author>" + Qt::escape( ( *author_it ).name ) + "</author>\n";

		xml += "<pictures>\n";
		if ( !( *recipe_it ).photo.isNull() ) {
			xml += "<pic format=\"JPEG\" id=\"1\"><![CDATA["; //fixed id until we implement multiple photos ability
			QByteArray data;
			QBuffer buffer( &data );
			buffer.open( QIODevice::WriteOnly );
			QImageWriter iio( &buffer, "JPEG" );
			iio.write( ( *recipe_it ).photo.toImage() );
			
			//( *recipe_it ).photo.save( &buffer, "JPEG" ); don't need QImageIO in QT 3.2

			xml += KCodecs::base64Encode( data, true );

			xml += "]]></pic>\n";
		}
		xml += "</pictures>\n";
		xml += "<category>\n";

		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it )
			xml += "<cat>" + Qt::escape( ( *cat_it ).name ) + "</cat>\n";

		xml += "</category>\n";
		xml += "<yield>";
		xml += "<amount>";
		if ( ( *recipe_it ).yield.amountOffset() < 1e-10 ) {
			xml += MixedNumber( ( *recipe_it ).yield.amount() ).toString( m_number_format, m_locale_aware_numbers );
		}
		else {
			xml += "<min>"+MixedNumber( ( *recipe_it ).yield.amount() ).toString( m_number_format, m_locale_aware_numbers )+"</min>";
			xml += "<max>"+MixedNumber( ( *recipe_it ).yield.amount() + ( *recipe_it ).yield.amountOffset() ).toString( m_number_format, m_locale_aware_numbers )+"</max>";
		}
		xml += "</amount>";
		xml += "<type>"+( *recipe_it ).yield.type()+"</type>";
		xml += "</yield>\n";
		xml += "<preparation-time>";
		xml += ( *recipe_it ).prepTime.toString( "hh:mm" );
		xml += "</preparation-time>\n";
		xml += "</krecipes-description>\n";
		xml += "<krecipes-ingredients>\n";

		IngredientList list_copy = ( *recipe_it ).ingList;
		for ( IngredientList group_list = list_copy.firstGroup(); group_list.count() != 0; group_list = list_copy.nextGroup() ) {
			QString group = group_list[ 0 ].group; //just use the first's name... they're all the same
			if ( !group.isEmpty() )
				xml += "<ingredient-group name=\"" + Qt::escape(group) + "\">\n";

			for ( IngredientList::const_iterator ing_it = group_list.begin(); ing_it != group_list.end(); ++ing_it ) {
				xml += "<ingredient>\n";

				xml += generateIngredient(*ing_it);

				if ( (*ing_it).substitutes.count() > 0 ) {
					xml += "<substitutes>\n";
					for ( Ingredient::SubstitutesList::const_iterator sub_it = (*ing_it).substitutes.begin(); sub_it != (*ing_it).substitutes.end(); ++sub_it ) {
						xml += "<ingredient>\n";
						xml += generateIngredient(*sub_it);
						xml += "</ingredient>\n";
					}
					xml += "</substitutes>\n";
				}

				xml += "</ingredient>\n";
			}

			if ( !group.isEmpty() )
				xml += "</ingredient-group>\n";
		}
	
		xml += "</krecipes-ingredients>\n";

		KConfigGroup config(KGlobal::config(),"Formatting"); 
		QStringList hiddenList = config.readEntry("HiddenProperties" , QStringList() );
		if (( *recipe_it ).properties.count() > 0) {
			xml += "<krecipes-properties>\n";
			for ( IngredientPropertyList::const_iterator prop_it = ( *recipe_it ).properties.begin(); prop_it != ( *recipe_it ).properties.end(); ++prop_it ) {
            if ( hiddenList.indexOf((*prop_it).name) == -1 ) {
					xml += "<property>\n";
				} else {
					xml += "<property hidden=\"true\">\n";
				}
				QString amount_str;

				xml += "<name>";
				xml += Qt::escape( (*prop_it).name );
				xml += "</name>\n";

				double prop_amount = (*prop_it).amount;
				if ( prop_amount > 0 ) { //TODO: make the precision configuratble
					prop_amount = double( qRound( prop_amount * 10.0 ) ) / 10.0; //not a "chemistry experiment" ;)  Let's only have one decimal place
					if ( m_locale_aware_numbers ) {
						amount_str = beautify( KGlobal::locale() ->formatNumber( prop_amount, 5 ) );
					} else {
						amount_str = beautify( QString::number(prop_amount) );
					}
				}
				else
					amount_str = '0';

				xml += "<amount>";
				xml += amount_str;
				xml += "</amount>\n";

				xml += "<units>";
				xml += Qt::escape( (*prop_it).units );
				xml += "</units>\n";

				xml += "</property>\n";
			}
			xml += "</krecipes-properties>\n";
		}

		xml += "<krecipes-instructions>\n";
		xml += Qt::escape( ( *recipe_it ).instructions );
		xml += "</krecipes-instructions>\n";

		//ratings
		xml += "<krecipes-ratings>";
		for ( RatingList::const_iterator rating_it = (*recipe_it).ratingList.begin(); rating_it != (*recipe_it).ratingList.end(); ++rating_it ) {
			xml += "<rating>";
			xml += "<comment>"+Qt::escape( ( *rating_it ).comment() )+"</comment>";
			xml += "<rater>"+Qt::escape( ( *rating_it ).rater() )+"</rater>";

			xml += "<criterion>";
			foreach ( RatingCriteria rc, (*rating_it).ratingCriterias() ) {
				xml += "<criteria>";
				xml += "<name>"+rc.name()+"</name>";
				xml += "<stars>"+QString::number(rc.stars())+"</stars>";
				xml += "</criteria>";
			}
			xml += "</criterion>";
			xml += "</rating>";
		}
		xml += "</krecipes-ratings>";

		xml += "</krecipes-recipe>\n";
	}

	return xml;
}

void KreExporter::createCategoryStructure( QString &xml, const RecipeList &recipes )
{
	if (!categories) return;

	QList<int> categoriesUsed;
	for ( RecipeList::const_iterator recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it ) {
		for ( ElementList::const_iterator cat_it = ( *recipe_it ).categoryList.begin(); cat_it != ( *recipe_it ).categoryList.end(); ++cat_it ) {
			if ( categoriesUsed.indexOf( ( *cat_it ).id ) == -1 )
				categoriesUsed << ( *cat_it ).id;
		}
	}

	if ( !categoriesUsed.empty() ) {
		//only keep the relevant category structure
		removeIfUnused( categoriesUsed, categories );
	
		xml += "<krecipes-category-structure>\n";
		writeCategoryStructure( xml, categories );
		xml += "</krecipes-category-structure>\n";
	}
}

bool KreExporter::removeIfUnused( const QList<int> &cat_ids, CategoryTree *parent, bool parent_should_show )
{
	for ( CategoryTree * it = parent->firstChild(); it; it = it->nextSibling() ) {
		if ( cat_ids.indexOf( it->category.id ) != -1 ) {
			parent_should_show = true;
			removeIfUnused( cat_ids, it, true ); //still recurse, but doesn't affect 'parent'
		}
		else {
			bool result = removeIfUnused( cat_ids, it );
			if ( parent_should_show == false )
				parent_should_show = result;
		}
	}

	if ( !parent_should_show && parent->category.id != -1 ) {
		//FIXME: CategoryTree is broken when deleting items
		//delete parent;

		parent->category.id = -2; //temporary workaround
	}

	return parent_should_show;
}

void KreExporter::writeCategoryStructure( QString &xml, const CategoryTree *categoryTree )
{
	if ( categoryTree->category.id != -2 ) {
		if ( categoryTree->category.id != -1 )
			xml += "<category name=\"" + Qt::escape( categoryTree->category.name ).replace("\"","&quot;") + "\">\n";
	
		for ( CategoryTree * child_it = categoryTree->firstChild(); child_it; child_it = child_it->nextSibling() ) {
			writeCategoryStructure( xml, child_it );
		}
	
		if ( categoryTree->category.id != -1 )
			xml += "</category>\n";
	}
}

