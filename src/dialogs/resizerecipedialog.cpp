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

#include "resizerecipedialog.h"

#include <cmath>

#include <kvbox.h>
#include <qvariant.h>
#include <q3buttongroup.h>
#include <q3frame.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <knuminput.h>
#include <klineedit.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

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
    setCaption(i18n("Resize Recipe" ));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setDefaultButton(KDialog::Ok);
    setModal( true );
    KVBox *page = new KVBox( this );

	buttonGroup = new Q3ButtonGroup( page );
	buttonGroup->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 5, ( QSizePolicy::SizeType ) 7, 0, 1, buttonGroup->sizePolicy().hasHeightForWidth() ) );
	buttonGroup->setLineWidth( 0 );
	buttonGroup->setColumnLayout( 0, Qt::Vertical );
	buttonGroup->layout() ->setSpacing( 6 );
	buttonGroup->layout() ->setMargin( 11 );
	buttonGroupLayout = new Q3VBoxLayout( buttonGroup->layout() );
	buttonGroupLayout->setAlignment( Qt::AlignTop );

	yieldRadioButton = new QRadioButton( buttonGroup );
	buttonGroup->insert( yieldRadioButton, SERVINGS_RADIO_BUTTON );
	buttonGroupLayout->addWidget( yieldRadioButton );

	yieldFrame = new Q3Frame( buttonGroup );
	yieldFrame->setFrameShape( Q3Frame::Box );
	yieldFrame->setFrameShadow( Q3Frame::Sunken );
	yieldFrame->setLineWidth( 1 );
	yieldFrameLayout = new Q3GridLayout( yieldFrame, 1, 1, 11, 6 );

	currentYieldLabel = new QLabel( yieldFrame );

	yieldFrameLayout->addWidget( currentYieldLabel, 0, 0 );

	newYieldLabel = new QLabel( yieldFrame );

	yieldFrameLayout->addMultiCellWidget( newYieldLabel, 1, 1, 0, 1 );

	currentYieldInput = new KLineEdit( yieldFrame );
	currentYieldInput->setReadOnly( TRUE );
	currentYieldInput->setAlignment( Qt::AlignRight );
	yieldFrameLayout->addMultiCellWidget( currentYieldInput, 0, 0, 1, 2 );

	newYieldInput = new FractionInput( yieldFrame );
	yieldFrameLayout->addWidget( newYieldInput, 1, 2 );

	buttonGroupLayout->addWidget( yieldFrame );

	factorRadioButton = new QRadioButton( buttonGroup );
	buttonGroup->insert( factorRadioButton, FACTOR_RADIO_BUTTON );
	buttonGroupLayout->addWidget( factorRadioButton );

	factorFrame = new Q3Frame( buttonGroup );
	factorFrame->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 5, 1, 0, factorFrame->sizePolicy().hasHeightForWidth() ) );
	factorFrame->setFrameShape( Q3Frame::Box );
	factorFrame->setFrameShadow( Q3Frame::Sunken );
	factorFrame->setLineWidth( 1 );
	factorFrameLayout = new Q3HBoxLayout( factorFrame, 11, 6 );

	factorLabel = new QLabel( factorFrame );
	factorFrameLayout->addWidget( factorLabel );

	factorInput = new FractionInput( factorFrame );
	factorInput->setSizePolicy( QSizePolicy( ( QSizePolicy::SizeType ) 7, ( QSizePolicy::SizeType ) 5, 0, 0, factorInput->sizePolicy().hasHeightForWidth() ) );
	factorFrameLayout->addWidget( factorInput );
	buttonGroupLayout->addWidget( factorFrame );

	languageChange();


	newYieldInput->setValue( m_recipe->yield.amount, 0 ); //Ignore the range info, it doesn't work in this context
	currentYieldInput->setText( m_recipe->yield.toString() );

	if ( recipe->yield.amount_offset > 0 ) {
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
	buttonGroup->setTitle( QString::null );
	yieldRadioButton->setText( i18n( "Scale by yield" ) );
	newYieldLabel->setText( i18n( "New yield:" ) );
	currentYieldLabel->setText( i18n( "Current yield:" ) );
	factorRadioButton->setText( i18n( "Scale by factor" ) );
	factorLabel->setText( i18n( "Factor (i.e. 1/2 to half, 3 to triple):" ) );
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
	if ( currentYieldInput->text().toInt() == 0 )
		KMessageBox::error( this, i18n( "Unable to scale a recipe with zero yield" ) );
	else if ( buttonGroup->selected() == yieldRadioButton ) {
		if ( newYieldInput->isInputValid() ) {
			double new_yield = newYieldInput->value().toDouble();
			double current_yield = MixedNumber::fromString(currentYieldInput->text()).toDouble();

			resizeRecipe( new_yield / current_yield );
		}
		else {
			KMessageBox::error( this, i18n( "Invalid input" ) );
			newYieldInput->selectAll();
			return;
		}
	}
	else {
		if ( factorInput->isInputValid() && factorInput->value() > 0 )
			resizeRecipe( factorInput->value().toDouble() );
		else {
			KMessageBox::error( this, i18n( "Invalid input" ) );
			factorInput->selectAll();
			return ;
		}
	}

	QDialog::accept();
}

//TODO YIELD: handle ranges
void ResizeRecipeDialog::resizeRecipe( double factor )
{
	m_recipe->yield.amount = MixedNumber::fromString(currentYieldInput->text()).toDouble() * factor;

	for ( IngredientList::iterator ing_it = m_recipe->ingList.begin(); ing_it != m_recipe->ingList.end(); ++ing_it ) {
		( *ing_it ).amount = ( *ing_it ).amount * factor;
		( *ing_it ).amount_offset = ( *ing_it ).amount_offset * factor;
	}
}

#include "resizerecipedialog.moc"
