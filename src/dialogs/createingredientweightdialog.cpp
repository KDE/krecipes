/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createingredientweightdialog.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <kmessagebox.h>
#include <klocale.h>

#include "widgets/unitcombobox.h"
#include "widgets/fractioninput.h"
#include "backends/recipedb.h"

CreateIngredientWeightDialog::CreateIngredientWeightDialog( QWidget* parent, RecipeDB *db )
    : QDialog( parent )
{
	setName( "CreateIngredientWeightDialog" );
	CreateIngredientWeightDialogLayout = new QVBoxLayout( this, 11, 6, "CreateIngredientWeightDialogLayout"); 
	
	groupBox1 = new QGroupBox( this, "groupBox1" );
	groupBox1->setColumnLayout(0, Qt::Vertical );
	groupBox1->layout()->setSpacing( 6 );
	groupBox1->layout()->setMargin( 11 );
	groupBox1Layout = new QGridLayout( groupBox1->layout() );
	groupBox1Layout->setAlignment( Qt::AlignTop );
	
	perAmountEdit = new FractionInput( groupBox1 );
	
	groupBox1Layout->addWidget( perAmountEdit, 1, 1 );
	
	weightEdit = new FractionInput( groupBox1 );
	
	groupBox1Layout->addWidget( weightEdit, 0, 1 );
	
	weightUnitBox = new UnitComboBox( groupBox1, db, Unit::Mass );
	weightUnitBox->reload();
	
	groupBox1Layout->addWidget( weightUnitBox, 0, 2 );
	
	perAmountLabel = new QLabel( groupBox1, "perAmountLabel" );
	
	groupBox1Layout->addWidget( perAmountLabel, 1, 0 );
	
	weightLabel = new QLabel( groupBox1, "weightLabel" );
	
	groupBox1Layout->addWidget( weightLabel, 0, 0 );
	
	perAmountUnitBox = new UnitComboBox( groupBox1, db );
	perAmountUnitBox->reload();
	
	groupBox1Layout->addWidget( perAmountUnitBox, 1, 2 );
	CreateIngredientWeightDialogLayout->addWidget( groupBox1 );
	
	layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 
	spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout1->addItem( spacer1 );
	
	cancelButton = new QPushButton( this, "cancelButton" );
	layout1->addWidget( cancelButton );
	
	okButton = new QPushButton( this, "okButton" );
	layout1->addWidget( okButton );
	CreateIngredientWeightDialogLayout->addLayout( layout1 );
	languageChange();
	clearWState( WState_Polished );

	connect( okButton, SIGNAL(clicked()), SLOT(validateAndAccept()) );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
}

CreateIngredientWeightDialog::~CreateIngredientWeightDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void CreateIngredientWeightDialog::languageChange()
{
	setCaption( tr( "Form2" ) );
	groupBox1->setTitle( tr( "New Ingredient Weight" ) );
	perAmountLabel->setText( tr( "Per Amount:" ) );
	weightLabel->setText( tr( "Weight:" ) );
	cancelButton->setText( tr( "&Cancel" ) );
	cancelButton->setAccel( QKeySequence( tr( "Alt+C" ) ) );
	okButton->setText( tr( "&OK" ) );
	okButton->setAccel( QKeySequence( tr( "Alt+O" ) ) );
}

void CreateIngredientWeightDialog::validateAndAccept()
{
	if ( !perAmountEdit->isInputValid() ) {
		KMessageBox::error( this, i18n( "Amount field contains invalid input." ),
		                    i18n( "Invalid input" ) );
		perAmountEdit->setFocus();
		perAmountEdit->selectAll();
		return;
	}
	else if ( !weightEdit->isInputValid() ) {
		KMessageBox::error( this, i18n( "Amount field contains invalid input." ),
		                    i18n( "Invalid input" ) );
		weightEdit->setFocus();
		weightEdit->selectAll();
		return;
	}

	accept();
}

Weight CreateIngredientWeightDialog::weight() const
{
	Weight w;
	w.perAmount = perAmountEdit->value().toDouble();
	w.perAmountUnitID = perAmountUnitBox->id( perAmountUnitBox->currentItem() );
	w.weight = weightEdit->value().toDouble();
	w.weightUnitID = weightUnitBox->id( weightUnitBox->currentItem() );

	return w;
}

#include "createingredientweightdialog.moc"
