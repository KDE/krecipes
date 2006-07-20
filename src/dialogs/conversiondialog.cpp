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
#include <qwhatsthis.h>

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>

#include "backends/recipedb.h"
#include "widgets/unitcombobox.h"
#include "widgets/ingredientcombobox.h"
#include "widgets/fractioninput.h"

ConversionDialog::ConversionDialog( QWidget* parent, RecipeDB *db, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ), m_database(db)
{
	if ( !name )
		setName( "ConversionDialog" );
	setSizeGripEnabled( TRUE );
	ConversionDialogLayout = new QVBoxLayout( this, 11, 6, "ConversionDialogLayout");
	
	layout4 = new QGridLayout( 0, 1, 1, 0, 6, "layout4"); 
	
	ingredientBox = new IngredientComboBox( FALSE, this, db, i18n( "--Ingredient (optional)--" ) );
	ingredientBox->reload();
	
	layout4->addWidget( ingredientBox, 0, 3 );
	
	convertLabel = new QLabel( this, "convertLabel" );
	
	layout4->addWidget( convertLabel, 0, 0 );
	
	toUnitBox = new UnitComboBox( this, db );
	toUnitBox->reload();
	
	layout4->addWidget( toUnitBox, 1, 1 );
	
	fromUnitBox = new UnitComboBox( this, db );
	fromUnitBox->reload();
	
	layout4->addWidget( fromUnitBox, 0, 2 );
	
	amountEdit = new FractionInput( this );
	
	layout4->addWidget( amountEdit, 0, 1 );
	
	toLabel = new QLabel( this, "toLabel" );
	
	layout4->addWidget( toLabel, 1, 0 );
	ConversionDialogLayout->addLayout( layout4 );
	
	layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 
	Horizontal_Spacing2_2 = new QSpacerItem( 124, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout6->addItem( Horizontal_Spacing2_2 );
	
	convertButton = new QPushButton( this, "convertButton" );
	convertButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, convertButton->sizePolicy().hasHeightForWidth() ) );
	layout6->addWidget( convertButton );
	Horizontal_Spacing2_3 = new QSpacerItem( 124, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout6->addItem( Horizontal_Spacing2_3 );
	ConversionDialogLayout->addLayout( layout6 );
	
	layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 
	
	resultLabel = new QLabel( this, "resultLabel" );
	layout7->addWidget( resultLabel );
	
	resultText = new QLabel( this, "resultText" );
	resultText->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 1, 0, resultText->sizePolicy().hasHeightForWidth() ) );
	layout7->addWidget( resultText );
	ConversionDialogLayout->addLayout( layout7 );
	
	Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 
	
	buttonHelp = new QPushButton( this, "buttonHelp" );
	buttonHelp->setAutoDefault( TRUE );
	Layout1->addWidget( buttonHelp );
	Horizontal_Spacing2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	Layout1->addItem( Horizontal_Spacing2 );
	
	buttonClose = new QPushButton( this, "buttonClose" );
	buttonClose->setAutoDefault( TRUE );
	Layout1->addWidget( buttonClose );
	ConversionDialogLayout->addLayout( Layout1 );
	languageChange();
	resize( QSize(412, 163).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );
	
	// signals and slots connections
	connect( buttonClose, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( convertButton, SIGNAL( clicked() ), this, SLOT( convert() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ConversionDialog::~ConversionDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ConversionDialog::languageChange()
{
	setCaption( i18n( "Measurement Converter" ) );
	convertLabel->setText( i18n( "Convert" ) );
	toLabel->setText( i18n( "To" ) );
	convertButton->setText( i18n( "Convert" ) );
	resultLabel->setText( i18n( "<b>Result:</b>" ) );
	resultText->setText( QString::null );
	buttonHelp->setText( i18n( "&Help" ) );
	buttonHelp->setAccel( QKeySequence( i18n( "F1" ) ) );
	buttonClose->setText( i18n( "&Close" ) );
	buttonClose->setAccel( QKeySequence( i18n( "Alt+C" ) ) );
}

void ConversionDialog::convert()
{
	Ingredient result, ing;
	Unit unit = m_database->unitName(toUnitBox->id(toUnitBox->currentItem()));

	ing.amount = amountEdit->value().toDouble();
	ing.ingredientID = ingredientBox->id(ingredientBox->currentItem());
	ing.units = m_database->unitName(fromUnitBox->id(fromUnitBox->currentItem()));

	switch ( m_database->convertIngredientUnits( ing, unit, result ) ) {
	case RecipeDB::Success:
		resultLabel->setText( i18n( "<b>Result:</b>" ) );
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
