/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "usdadatadialog.h"

#include <kdebug.h>
#include <kstandarddirs.h>
#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtextstream.h>
#include <qvbox.h>

#include "backends/recipedb.h"
#include "backends/usda_property_data.h"
#include "backends/usda_unit_data.h"
#include "widgets/krelistview.h"
#include "datablocks/weight.h"

USDADataDialog::USDADataDialog( const Element &ing, RecipeDB *db, QWidget *parent )
		: KDialogBase( parent, "usdaDataDialog", true, QString::null,
		    KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok ),
		ingredient( ing ),
		database( db )
{
	setCaption( QString( i18n( "Load ingredient properties for: \"%1\"" ) ).arg( ingredient.name ) );

	QVBox *page = makeVBoxMainWidget();

	setButtonText( KDialogBase::Ok, i18n( "&Load" ) );

	KreListView *krelistview = new KreListView( page, QString::null, true, 0 );

	listView = krelistview->listView();
	listView->addColumn( i18n( "USDA Ingredient" ) );
	listView->addColumn( i18n( "Id" ) );
	listView->setAllColumnsShowFocus( true );

	loadDataFromFile();

	connect( listView, SIGNAL( doubleClicked( QListViewItem*, const QPoint &, int ) ), this, SLOT( slotOk() ) );
}

USDADataDialog::~USDADataDialog()
{}

void USDADataDialog::loadDataFromFile()
{
	QString abbrev_file = locate( "appdata", "data/abbrev.txt" );
	if ( abbrev_file.isEmpty() ) {
		kdDebug() << "Unable to find abbrev.txt data file." << endl;
		return ;
	}

	QFile file( abbrev_file );
	if ( !file.open( IO_ReadOnly ) ) {
		kdDebug() << "Unable to open data file: " << abbrev_file << endl;
		return ;
	}

	int index = 0;
	QTextStream stream( &file );
	while ( !stream.atEnd() ) {
		QString line = stream.readLine();
		if ( line.isEmpty() ) {
			continue;
		}

		QStringList fields = QStringList::split( "^", line, true );
		loaded_data << fields;

		QString ing_id = fields[ 0 ].mid( 1, fields[ 1 ].length() - 2 );
		QString ing_name = fields[ 1 ].mid( 1, fields[ 1 ].length() - 2 );
		( void ) new QListViewItem( listView, ing_name, QString::number( index ) ); //using an index instead of the actual id will help find the data later

		index++;
	}
}

void USDADataDialog::slotOk()
{
	QListViewItem * item = listView->selectedItem();
	if ( item ) {
		int index = item->text( 1 ).toInt();
		QStringList data = loaded_data[ index ];

		int grams_id = database->findExistingUnitByName( "g" ); //get this id because all data is given per gram
		if ( grams_id == -1 ) {
			//FIXME: take advantage of abbreviations
			Unit unit("g","g");
			unit.type = Unit::Mass;
			database->createNewUnit( unit );
			grams_id = database->lastInsertID();
		}
		else {
			Unit unit = database->unitName(grams_id);
			if ( unit.type != Unit::Mass ) {
				unit.type = Unit::Mass;
				database->modUnit( unit );
			}
		}

		IngredientPropertyList property_list;
		database->loadProperties( &property_list );
		IngredientPropertyList existing_ing_props;
		database->loadProperties( &existing_ing_props, ingredient.id );

		int i = 0;
		for ( QStringList::const_iterator it = data.at( 2 ); !property_data_list[ i ].name.isEmpty(); ++it, ++i ) {
			int property_id = property_list.findByName( property_data_list[ i ].name );
			if ( property_id == -1 ) {
				database->addProperty( property_data_list[ i ].name, property_data_list[ i ].unit );
				property_id = database->lastInsertID();
			}

			double amount = ( *it ).toDouble() / 100.0; //data givin per 100g so divide by 100 to get the amount in 1 gram

			if ( existing_ing_props.find( property_id ) != existing_ing_props.end() )  //property already added to ingredient, so just update
				database->changePropertyAmountToIngredient( ingredient.id, property_id, amount, grams_id );
			else
				database->addPropertyToIngredient( ingredient.id, property_id, amount, grams_id );
		}

		i+=2;

		int i_initial = i;
		WeightList weights = database->ingredientWeightUnits( ingredient.id );
		for ( ; i < i_initial+3; ++i ) {
			Weight w;
			w.weight = data[i].toDouble();

			i++;

			QString amountAndWeight = data[i].mid( 1, data[i].length() - 2 );
			if ( !amountAndWeight.isEmpty() ) {
				int spaceIndex = amountAndWeight.find(" ");
				w.perAmount = amountAndWeight.left(spaceIndex).toDouble();

				QString perAmountUnit = amountAndWeight.right(amountAndWeight.length()-spaceIndex-1);
				if ( !parseUSDAUnitAndPrep( perAmountUnit, w.perAmountUnit, w.prepMethod ) )
					continue;

				int unitID = database->findExistingUnitByName( w.perAmountUnit );
				if ( unitID == -1 ) {
					for ( int i = 0; unit_data_list[ i ].name; ++i ) {
						if ( w.perAmountUnit == unit_data_list[ i ].name || w.perAmountUnit == unit_data_list[ i ].plural ) {
							database->createNewUnit( Unit(unit_data_list[ i ].name,unit_data_list[ i ].plural) );
						}
					}

					unitID = database->lastInsertID();
				}
				w.perAmountUnitID = unitID;

				if ( !w.prepMethod.isEmpty() ) {
					int prepID = database->findExistingPrepByName( w.prepMethod );
					if ( prepID == -1 ) {
						database->createNewPrepMethod( w.prepMethod );
						prepID = database->lastInsertID();
					}
					w.prepMethodID = prepID;
				}

				bool exists = false;
				for ( WeightList::const_iterator it = weights.begin(); it != weights.end(); ++it ) {
					if ( (*it).perAmountUnitID == w.perAmountUnitID && (*it).prepMethodID == w.prepMethodID ) {
						exists = true;
						break;
					}
				}
				if ( exists )
					continue;

				w.weightUnitID = grams_id;
				w.ingredientID = ingredient.id;
				database->addIngredientWeight( w );
			}
		}

		accept();
	}
	else
		reject();
}


#include "usdadatadialog.moc"
