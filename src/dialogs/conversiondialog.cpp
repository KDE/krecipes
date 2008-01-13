/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "conversiondialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <kcombobox.h>
#include <qlabel.h>
#include <klineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <q3vbox.h>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>

#include "backends/recipedb.h"
#include "widgets/unitcombobox.h"
#include "widgets/ingredientcombobox.h"
#include "widgets/prepmethodcombobox.h"
#include "widgets/fractioninput.h"

ConversionDialog::ConversionDialog( QWidget* parent, RecipeDB *db, const char* name )
		: KDialog( parent, name, false, i18n( "Measurement Converter" ),
		    KDialog::Close | KDialogBase::User1 | KDialogBase::Help, KDialogBase::Close ),
		m_database(db)
{
	setHelp("measure-converter");
	setButtonText( KDialog::User1, i18n("Convert") );

	setSizeGripEnabled( TRUE );

	KVBox *page = makeVBoxMainWidget();
	
	Q3HBox *vbox = new Q3VBox(page);

	Q3HBox *fromTopBox = new Q3HBox(vbox);
	convertLabel = new QLabel( fromTopBox, "convertLabel" );

	amountEdit = new FractionInput( fromTopBox );

	fromUnitBox = new UnitComboBox( fromTopBox, db );
	fromUnitBox->reload();
	fromTopBox->setStretchFactor( fromUnitBox, 2 );
	fromTopBox->setSpacing(3);

	Q3HBox *fromBottomBox = new Q3HBox(vbox);
	
	ingredientBox = new IngredientComboBox( FALSE, fromBottomBox, db, i18n( "--Ingredient (optional)--" ) );
	ingredientBox->reload();

	prepMethodBox = new PrepMethodComboBox( false, fromBottomBox, db, i18n( "-No Preparation-" ) );
	prepMethodBox->reload();
	fromBottomBox->setSpacing(3);
	
	Q3HBox *toBox = new Q3HBox(vbox);

	toLabel = new QLabel( toBox, "toLabel" );

	toUnitBox = new UnitComboBox( toBox, db );
	toUnitBox->reload();
	toBox->setStretchFactor( toUnitBox, 2 );
	toBox->setSpacing(8);

	Q3HBox *resultBox = new Q3HBox(vbox);
	resultLabel = new QLabel( resultBox, "resultLabel" );
	resultText = new QLabel( resultBox, "resultText" );
	resultBox->setStretchFactor( resultText, 2 );

	languageChange();

	setInitialSize( QSize(300, 200).expandedTo(minimumSizeHint()) );
	
	// signals and slots connections
	connect ( this, SIGNAL( closeClicked() ), this, SLOT( accept() ) );
}

ConversionDialog::~ConversionDialog()
{
}

void ConversionDialog::languageChange()
{
	convertLabel->setText( i18n( "Convert" ) );
	toLabel->setText( i18n( "To" ) );
	resultLabel->setText( i18n( "<b>Result:</b>" ) );
	resultText->setText( QString::null );
}

void ConversionDialog::show()
{
	reset();
	KDialog::show();
}

void ConversionDialog::reset()
{
	resultText->setText( QString::null );
	ingredientBox->setCurrentItem( 0 );
	prepMethodBox->setCurrentItem( 0 );
	toUnitBox->setCurrentItem( 0 );
	fromUnitBox->setCurrentItem( 0 );
	amountEdit->clear();
}

void ConversionDialog::slotUser1()
{
	convert();
}

void ConversionDialog::convert()
{
	Ingredient result, ing;
	Unit unit = m_database->unitName(toUnitBox->id(toUnitBox->currentItem()));

	ing.amount = amountEdit->value().toDouble();
	ing.ingredientID = ingredientBox->id(ingredientBox->currentItem());
	ing.units = m_database->unitName(fromUnitBox->id(fromUnitBox->currentItem()));

	int prepID = prepMethodBox->id(prepMethodBox->currentItem());
	if ( prepID != -1 )
		ing.prepMethodList.append(Element(QString::null,prepID));

	switch ( m_database->convertIngredientUnits( ing, unit, result ) ) {
	case RecipeDB::Success:
		resultLabel->setText( i18n( "<b>Result:</b>" ) );
		resultText->setText(QString::number(result.amount)+" "+((result.amount>1)?result.units.plural:result.units.name));
		break;
	case RecipeDB::MismatchedPrepMethodUsingApprox:
		resultLabel->setText( i18n( "<b>Approximated result:</b>" ) );
		resultText->setText(QString::number(result.amount)+" "+((result.amount>1)?result.units.plural:result.units.name));
		break;
	case RecipeDB::MissingUnitConversion:
		resultLabel->setText( i18n( "<b>Error:</b>" ) );
		resultText->setText( i18n("Missing unit conversion") );
		break;
	case RecipeDB::MissingIngredientWeight:
		resultLabel->setText( i18n( "<b>Error:</b>" ) );
		resultText->setText( i18n("No ingredient weight available") );
		break;
	case RecipeDB::MismatchedPrepMethod:
		resultLabel->setText( i18n( "<b>Error:</b>" ) );
		resultText->setText( i18n("No ingredient weight available for this method of preparation") );
		break;
	case RecipeDB::MissingIngredient:
		resultLabel->setText( i18n( "<b>Error:</b>" ) );
		resultText->setText( i18n("Ingredient required for conversion") );
		break;
	case RecipeDB::InvalidTypes:
		resultLabel->setText( i18n( "<b>Error:</b>" ) );
		resultText->setText( i18n("Impossible unit conversion based on unit types") );
		break;
	}
}

#include "conversiondialog.moc"
