/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "resizerecipedialog.h"

#include <cmath>

#include <kvbox.h>
#include <q3buttongroup.h>
#include <qframe.h>
#include <QFormLayout>
#include <QVBoxLayout>
#include <knuminput.h>
#include <klineedit.h>
#include <QRadioButton>
#include <q3whatsthis.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "datablocks/recipe.h"
#include "widgets/fractioninput.h"

#define FACTOR_RADIO_BUTTON 0
#define SERVINGS_RADIO_BUTTON 1

ResizeRecipeDialog::ResizeRecipeDialog( QWidget *parent, Recipe *recipe )
		: KDialog( parent ),
		m_recipe( recipe )
{
	setCaption(i18nc("@title:window", "Resize Recipe" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );
	KVBox *page = new KVBox( this );
	setMainWidget( page );
	buttonGroup = new Q3ButtonGroup( page );
	buttonGroup->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 5, ( QSizePolicy::SizeType ) 7, 0, 1, buttonGroup->sizePolicy().hasHeightForWidth() ) );
	buttonGroup->setLineWidth( 0 );
	buttonGroup->setColumnLayout( 0, Qt::Vertical );
	buttonGroup->layout() ->setSpacing( 6 );
	buttonGroup->layout() ->setMargin( 11 );
	buttonGroupLayout = new QVBoxLayout( buttonGroup->layout() );
	buttonGroupLayout->setAlignment( Qt::AlignTop );

	yieldRadioButton = new QRadioButton( buttonGroup );
	buttonGroup->insert( yieldRadioButton, SERVINGS_RADIO_BUTTON );
	buttonGroupLayout->addWidget( yieldRadioButton );

	yieldFrame = new QFrame( buttonGroup );
	yieldFrame->setFrameStyle( QFrame::StyledPanel|QFrame::Raised );
	yieldFrameLayout = new QFormLayout( yieldFrame );

	currentYieldInput = new KLineEdit( yieldFrame );
	currentYieldInput->setReadOnly( true );
	currentYieldInput->setAlignment( Qt::AlignRight );
	yieldFrameLayout->addRow( i18nc( "@label:textbox",
		"Current yield:" ), currentYieldInput );

	newYieldInput = new FractionInput( yieldFrame );
	yieldFrameLayout->addRow( i18nc( "@label:textbox",
		"New yield:" ), newYieldInput );

	buttonGroupLayout->addWidget( yieldFrame );

	factorRadioButton = new QRadioButton( buttonGroup );
	buttonGroup->insert( factorRadioButton, FACTOR_RADIO_BUTTON );
	buttonGroupLayout->addWidget( factorRadioButton );

	factorFrame = new QFrame( buttonGroup );
	factorFrame->setFrameStyle( QFrame::StyledPanel|QFrame::Raised );
	factorFrameLayout = new QFormLayout( factorFrame );

	factorInput = new FractionInput( factorFrame );
	factorFrameLayout->addRow( i18nc( "@label:textbox",
		"Factor (e.g. 1/2 to half, 3 to triple):" ), factorInput );

	buttonGroupLayout->addWidget( factorFrame );

	languageChange();


	newYieldInput->setValue( m_recipe->yield.amount(), 0 ); //Ignore the range info, it doesn't work in this context
	currentYieldInput->setText( m_recipe->yield.toString() );

	if ( recipe->yield.amountOffset() > 0 ) {
		yieldRadioButton->setEnabled(false);
		buttonGroup->setButton( FACTOR_RADIO_BUTTON );
		activateCurrentOption( FACTOR_RADIO_BUTTON );
	}
	else {
		buttonGroup->setButton( SERVINGS_RADIO_BUTTON );
		activateCurrentOption( SERVINGS_RADIO_BUTTON );
	}

	// signals and slots connections
	connect( buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( activateCurrentOption( int ) ) );
}

void ResizeRecipeDialog::languageChange()
{
	// Warning: This is not useful at all since were putting strings in QFormLayout's
	// in the constructor.
	buttonGroup->setTitle( QString() );
	yieldRadioButton->setText( i18nc( "@option:radio", "Scale by yield" ) );
	factorRadioButton->setText( i18nc( "@option:radio", "Scale by factor" ) );
}

void ResizeRecipeDialog::activateCurrentOption( int button_id )
{
	switch ( button_id ) {
	case SERVINGS_RADIO_BUTTON:
		factorFrame->setEnabled( false );
		yieldFrame->setEnabled( true );
		break;
	case FACTOR_RADIO_BUTTON:
		factorFrame->setEnabled( true );
		yieldFrame->setEnabled( false );
		break;
	default:
		break;
	}
}

void ResizeRecipeDialog::accept()
{
	if ( m_recipe->yield.amount() == 0 )
		KMessageBox::error( this, i18nc( "@info", "Unable to scale a recipe with zero yield" ) );
	else if ( buttonGroup->selected() == yieldRadioButton ) {
		if ( newYieldInput->isInputValid() ) {
			double new_yield = newYieldInput->value().toDouble();
			double current_yield = MixedNumber::fromString(currentYieldInput->text()).toDouble();

			resizeRecipe( new_yield / current_yield );
		}
		else {
			KMessageBox::error( this, i18nc( "@info", "Invalid input" ) );
			newYieldInput->selectAll();
			return;
		}
	}
	else {
		if ( factorInput->isInputValid() && factorInput->value() > 0 )
			resizeRecipe( factorInput->value().toDouble() );
		else {
			KMessageBox::error( this, i18nc( "@info", "Invalid input" ) );
			factorInput->selectAll();
			return ;
		}
	}

	QDialog::accept();
}

void ResizeRecipeDialog::resizeRecipe( double factor )
{
	m_recipe->yield.setAmount(MixedNumber::fromString(currentYieldInput->text()).toDouble() * factor);

	for ( IngredientList::iterator ing_it = m_recipe->ingList.begin(); ing_it != m_recipe->ingList.end(); ++ing_it ) {
		( *ing_it ).amount = ( *ing_it ).amount * factor;
		( *ing_it ).amount_offset = ( *ing_it ).amount_offset * factor;
	}
}

#include "resizerecipedialog.moc"
