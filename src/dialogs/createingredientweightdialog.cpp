/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "createingredientweightdialog.h"

#include <q3groupbox.h>
#include <QLabel>
#include <QPushButton>
#include <q3whatsthis.h>

#include <KVBox>
#include <QGridLayout>

#include <kmessagebox.h>
#include <klocale.h>
#include <kvbox.h>

#include "widgets/unitcombobox.h"
#include "widgets/prepmethodcombobox.h"
#include "widgets/fractioninput.h"
#include "datablocks/weight.h"
#include "backends/recipedb.h"

CreateIngredientWeightDialog::CreateIngredientWeightDialog( QWidget* parent, RecipeDB *db )
	: KDialog( parent )
{
	setCaption( i18n( "Add Ingredient Weight" ) );
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );
	KVBox *page = new KVBox( this );
	setMainWidget( page );

	groupBox1 = new Q3GroupBox( page );
	groupBox1->setColumnLayout(0, Qt::Vertical );
	groupBox1->layout()->setSpacing( 6 );
	groupBox1->layout()->setMargin( 11 );
	groupBox1Layout = new QGridLayout( groupBox1->layout() );
	groupBox1Layout->setAlignment( Qt::AlignTop );

	weightEdit = new FractionInput( groupBox1 );

	groupBox1Layout->addWidget( weightEdit, 0, 1 );

	weightUnitBox = new UnitComboBox( groupBox1, db, Unit::Mass );
	weightUnitBox->reload();

	groupBox1Layout->addWidget( weightUnitBox, 0, 2, 1, 2, 0 );

	perAmountLabel = new QLabel( groupBox1 );
	perAmountLabel->setObjectName( "perAmountLabel" );

	groupBox1Layout->addWidget( perAmountLabel, 1, 0 );

	weightLabel = new QLabel( groupBox1 );
	weightLabel->setObjectName( "weightLabel" );

	groupBox1Layout->addWidget( weightLabel, 0, 0 );

	perAmountEdit = new FractionInput( groupBox1 );

	groupBox1Layout->addWidget( perAmountEdit, 1, 1 );

	perAmountUnitBox = new UnitComboBox( groupBox1, db );
	perAmountUnitBox->reload();

	groupBox1Layout->addWidget( perAmountUnitBox, 1, 2 );

	prepMethodBox = new PrepMethodComboBox( false, groupBox1, db, i18n("-No Preparation-") );
	prepMethodBox->reload();
	groupBox1Layout->addWidget( prepMethodBox, 1, 3 );

	languageChange();
	//clearWState( WState_Polished );

	weightEdit->setFocus();
	connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
}

CreateIngredientWeightDialog::~CreateIngredientWeightDialog()
{
	// no need to delete child widgets, Qt does it all for us
}

void CreateIngredientWeightDialog::languageChange()
{
	groupBox1->setTitle( i18n( "New Ingredient Weight" ) );
	perAmountLabel->setText( i18n( "Per Amount:" ) );
	weightLabel->setText( i18n( "Weight:" ) );
}

void CreateIngredientWeightDialog::slotOk()
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
	w.perAmountUnitID = perAmountUnitBox->id( perAmountUnitBox->currentIndex() );
	w.weight = weightEdit->value().toDouble();
	w.weightUnitID = weightUnitBox->id( weightUnitBox->currentIndex() );
	w.prepMethodID = prepMethodBox->id( prepMethodBox->currentIndex() );
	w.prepMethod = prepMethodBox->currentText();

	return w;
}

#include "createingredientweightdialog.moc"
