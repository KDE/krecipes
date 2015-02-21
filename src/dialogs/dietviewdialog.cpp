/***************************************************************************
*   Copyright © 2003-2005 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dietviewdialog.h"

#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <KStandardGuiItem>
//Added by qt3to4:
#include <Q3ValueList>
#include <KVBox>

DietViewDialog::DietViewDialog( QWidget *parent, const RecipeList &recipeList, int dayNumber, int mealNumber, const QList <int> &dishNumbers )
		: KDialog( parent )
{
	setCaption( i18nc( "@title:window", "View Diet" ) );
	setButtons(KDialog::User2 | KDialog::Close | KDialog::User1);
	setDefaultButton(KDialog::User2);
	setModal( false );
	KVBox *page = new KVBox( this );
	setMainWidget(page);
	setButtonText( KDialog::User1, KStandardGuiItem::print().text() );
	setButtonIcon( KDialog::User1, KIcon( "document-print" ) );

	setButtonText( KDialog::User2, i18nc( "@action:button", "Create &Shopping List" ) );
	setButtonIcon( KDialog::User2, KIcon( "view-pim-tasks" ) );

	// The html part
	dietView = new KHTMLPart( page );

	setInitialSize( QSize(350, 450) );

	setSizeGripEnabled( true );

	connect ( this, SIGNAL( user2Clicked() ), this, SLOT( slotOk() ) );
	connect ( this, SIGNAL( closeClicked() ), this, SLOT( close() ) );
	connect ( this, SIGNAL( user1Clicked() ), this, SLOT( print() ) );

	// Show the diet
	showDiet( recipeList, dayNumber, mealNumber, dishNumbers );
}

DietViewDialog::~DietViewDialog()
{}

void DietViewDialog::showDiet( const RecipeList &recipeList, int dayNumber, int mealNumber, const QList <int> &dishNumbers )
{


	// Header
	QString htmlCode = QString( "<html><head><title>%1</title>" ).arg( i18n( "Diet" ) );

	// CSS
	htmlCode += "<STYLE type=\"text/css\">\n";
	htmlCode += "#calendar{border: thin solid black}";
	htmlCode += ".dayheader{ background-color: #D6D6D6; color: black; border:none;}";
	htmlCode += ".day{ background-color: #E5E5E5; color: black; border:medium solid #D6D6D6;}";
	htmlCode += ".meal{ background-color: #CDD4FF; color: black; border:thin solid #B4BEFF; text-align:center;}";
	htmlCode += ".dish{font-size: smaller; overflow: hidden; height:2.5em;}";
	htmlCode += "</STYLE>";


	htmlCode += "</head><body>"; //  /Header

	// Calendar border
	htmlCode += QString( "<div id=\"calendar\">" );

	// Title
	htmlCode += QString( "<center><div STYLE=\"width: 100%\">" );
	htmlCode += QString( "<h1>%1</h1></div></center>" ).arg( i18n( "Diet" ) );

	// Diet table
	htmlCode += QString( "<center><div STYLE=\"width: 98%\">" );
	htmlCode += QString( "<table><tbody>" );


	QList <int>::ConstIterator it = dishNumbers.begin();
	RecipeList::ConstIterator rit;
	rit = recipeList.begin();

	for ( int row = 0, day = 0; row <= ( ( dayNumber - 1 ) / 7 ); row++ )  // New row (week)
	{
		htmlCode += QString( "<tr>" );

		for ( int col = 0; ( col < 7 ) && ( day < dayNumber ); col++, day++ )  // New column (day)
		{
			htmlCode += QString( "<td><div class=\"day\">" );
			htmlCode += QString( "<div class=\"dayheader\"><center>" );
			htmlCode += i18n( "Day %1", day + 1 );
			htmlCode += QString( "</center></div>" );
			for ( int meal = 0;meal < mealNumber;meal++ )  // Meals in each cell
			{
				int dishNumber = *it;
				htmlCode += QString( "<div class=\"meal\">" );
				for ( int dish = 0; dish < dishNumber;dish++ )  // Dishes in each Meal
				{
					htmlCode += QString( "<div class=\"dish\">" );
					htmlCode += ( *rit ).title;
					htmlCode += "<br>";
					htmlCode += QString( "</div>" );
					rit++;
				}
				it++;
				htmlCode += QString( "</div>" );
			}
			it = dishNumbers.begin(); // meals have same dish number everyday
			htmlCode += QString( "</div></td>" );
		}

		htmlCode += QString( "</tr>" );
	}

	htmlCode += QString( "</tbody></table>" );
	htmlCode += QString( "</div></center>" );
	htmlCode += QString( "</div></body></html>" );

	resize( QSize( 600, 400 ) );

	// Display it
	dietView->begin( KUrl( KStandardDirs::locateLocal( "tmp", "/" ) ) ); // Initialize to tmp dir, where photos and logos can be stored
	dietView->write( htmlCode );
	dietView->end();
}

void DietViewDialog::print( void )
{
	dietView->view()->print();
}

void DietViewDialog::slotOk( void )
{
	emit signalOk();
	close();
}

#include "dietviewdialog.moc"
