/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "shoppinglistviewdialog.h"
#include "datablocks/ingredientlist.h"
#include "datablocks/mixednumber.h"

#include <qpushbutton.h>

#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>

ShoppingListViewDialog::ShoppingListViewDialog( QWidget *parent, const IngredientList &ingredientList )
		: KDialog( parent, "shoppingviewdialog", true, QString::null,
		    KDialog::Close | KDialog::User1, KDialog::Close,
		    false, KStandardGuiItem::print() )
{
	// Design dialog
	KVBox *page = makeVBoxMainWidget();

	shoppingListView = new KHTMLPart( page );

	setInitialSize( QSize(350, 450) );

	connect ( this, SIGNAL( user1Clicked() ), this, SLOT( print() ) );
	connect ( this, SIGNAL( closeClicked() ), this, SLOT( accept() ) );

	//---------- Sort the list --------
	IngredientList list_copy = ingredientList;
	qSort( list_copy );

	//---------- Load  the list --------
	display( list_copy );
}


ShoppingListViewDialog::~ShoppingListViewDialog()
{}

void ShoppingListViewDialog::display( const IngredientList &ingredientList )
{
	QString recipeHTML;

	// Create HTML Code

	// Headers
	recipeHTML = QString( "<html><head><title>%1</title></head><body>" ).arg( i18n( "Shopping List" ) );
	recipeHTML += "<center><div STYLE=\"width: 95%\">";
	recipeHTML += QString( "<center><h1>%1</h1></center>" ).arg( i18n( "Shopping List" ) );

	// Ingredient List

	recipeHTML += "<div STYLE=\"border:medium solid blue; width:95%\"><table cellspacing=0px width=100%><tbody>";
	bool counter = true;

	KConfig *config = KGlobal::config();
	config->setGroup( "Formatting" );

	bool useAbbreviations = config->readEntry("AbbreviateUnits");
	bool useFraction = config->readEntry( "Fraction" );

	for ( IngredientList::const_iterator ing_it = ingredientList.begin(); ing_it != ingredientList.end(); ++ing_it ) {
		QString color = ( counter ) ? "#CBCEFF" : "#BFC2F0";
		counter = !counter;

		MixedNumber::Format number_format = ( useFraction ) ? MixedNumber::MixedNumberFormat : MixedNumber::DecimalFormat;
		QString amount_str = MixedNumber( ( *ing_it ).amount ).toString( number_format );

		QString unit = ( *ing_it ).units.determineName( ( *ing_it ).amount + ( *ing_it ).amount_offset, useAbbreviations );

		recipeHTML += QString( "<tr bgcolor=\"%1\"><td>- %2:</td><td>%3 %4</td></tr>" ).arg( color ).arg( ( *ing_it ).name ).arg( amount_str ).arg( unit );
	}
	recipeHTML += "</tbody></table></div>";
	// Close
	recipeHTML += "</div></center></body></html>";


	// Display
	shoppingListView->begin( KUrl( "file:/tmp/" ) ); // Initialize to /tmp, where photos and logos are stored
	shoppingListView->write( recipeHTML );
	shoppingListView->end();


}

void ShoppingListViewDialog::print()
{
	shoppingListView->view() ->print();
}

#include "shoppinglistviewdialog.moc"
