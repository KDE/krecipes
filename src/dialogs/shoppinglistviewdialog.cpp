/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "shoppinglistviewdialog.h"
#include "datablocks/ingredientlist.h"
#include "datablocks/mixednumber.h"

#include <QPushButton>

#include <klocale.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kvbox.h>
#include <KStandardGuiItem>
#include <q3tl.h>

ShoppingListViewDialog::ShoppingListViewDialog( QWidget *parent, const IngredientList &ingredientList )
		: KDialog( parent )
{
	this->setObjectName( "shoppingviewdialog" );
	this->setCaption( i18nc( "@title:window", "Shopping List" ) );
	this->setModal( true );
	this->setButtons( KDialog::Close | KDialog::User1 );
	this->setButtonGuiItem( KDialog::User1 , KStandardGuiItem::print() );
	this->setDefaultButton( KDialog::Close );
	this->showButtonSeparator( false );
	
	// Design dialog
	KVBox *page = new KVBox ( this );
	setMainWidget( page );

	shoppingListView = new KHTMLPart( page );

	setInitialSize( QSize(350, 450) );

	connect ( this, SIGNAL( user1Clicked() ), this, SLOT( print() ) );
	connect ( this, SIGNAL( closeClicked() ), this, SLOT( accept() ) );

	//---------- Sort the list --------
	IngredientList list_copy = ingredientList;
	qHeapSort( list_copy );

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

	KConfigGroup config = KGlobal::config()->group( "Formatting" );

	bool useAbbreviations = config.readEntry("AbbreviateUnits", false );
	bool useFraction = config.readEntry( "Fraction", false );

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
