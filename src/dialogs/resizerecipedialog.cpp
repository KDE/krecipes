/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "resizerecipedialog.h"

#include <cmath>

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qframe.h>
#include <qlabel.h>
#include <knuminput.h>
#include <klineedit.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include "recipe.h"
#include "fractioninput.h"

#define ROUND(a) (((a) - floor((a)) < ceil((a)) - (a)) ? floor((a)) : ceil((a)))
#define FACTOR_RADIO_BUTTON 0
#define SERVINGS_RADIO_BUTTON 1

ResizeRecipeDialog::ResizeRecipeDialog( QWidget *parent, Recipe *recipe ) : QDialog(parent,0,true),
  m_recipe(recipe)
{
    resizeRecipeDialogLayout = new QVBoxLayout( this, 11, 6 );

    buttonGroup = new QButtonGroup( this );
    buttonGroup->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)7, 0, 1, buttonGroup->sizePolicy().hasHeightForWidth() ) );
    buttonGroup->setLineWidth( 0 );
    buttonGroup->setColumnLayout(0, Qt::Vertical );
    buttonGroup->layout()->setSpacing( 6 );
    buttonGroup->layout()->setMargin( 11 );
    buttonGroupLayout = new QVBoxLayout( buttonGroup->layout() );
    buttonGroupLayout->setAlignment( Qt::AlignTop );

    servingsRadioButton = new QRadioButton( buttonGroup );
    buttonGroup->insert( servingsRadioButton, SERVINGS_RADIO_BUTTON );
    buttonGroupLayout->addWidget( servingsRadioButton );

    servingsFrame = new QFrame( buttonGroup );
    servingsFrame->setFrameShape( QFrame::Box );
    servingsFrame->setFrameShadow( QFrame::Sunken );
    servingsFrame->setLineWidth( 1 );
    servingsFrameLayout = new QGridLayout( servingsFrame, 1, 1, 11, 6 );

    currentServingsLabel = new QLabel( servingsFrame );

    servingsFrameLayout->addWidget( currentServingsLabel, 0, 0 );

    newServingsLabel = new QLabel( servingsFrame );

    servingsFrameLayout->addMultiCellWidget( newServingsLabel, 1, 1, 0, 1 );

    currentServingsInput = new KLineEdit( servingsFrame );
    currentServingsInput->setReadOnly( TRUE );
    currentServingsInput->setAlignment( Qt::AlignRight );
    servingsFrameLayout->addMultiCellWidget( currentServingsInput, 0, 0, 1, 2 );

    newServingsInput = new KIntNumInput( servingsFrame );
    newServingsInput->setMinValue( 1 );
    servingsFrameLayout->addWidget( newServingsInput, 1, 2 );

    buttonGroupLayout->addWidget( servingsFrame );

    factorRadioButton = new QRadioButton( buttonGroup );
    buttonGroup->insert( factorRadioButton, FACTOR_RADIO_BUTTON );
    buttonGroupLayout->addWidget( factorRadioButton );

    factorFrame = new QFrame( buttonGroup );
    factorFrame->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 1, 0, factorFrame->sizePolicy().hasHeightForWidth() ) );
    factorFrame->setFrameShape( QFrame::Box );
    factorFrame->setFrameShadow( QFrame::Sunken );
    factorFrame->setLineWidth( 1 );
    factorFrameLayout = new QHBoxLayout( factorFrame, 11, 6 );

    factorLabel = new QLabel( factorFrame );
    factorFrameLayout->addWidget( factorLabel );

    factorInput = new FractionInput( factorFrame );
    factorInput->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, factorInput->sizePolicy().hasHeightForWidth() ) );
    factorFrameLayout->addWidget( factorInput );
    buttonGroupLayout->addWidget( factorFrame );
    resizeRecipeDialogLayout->addWidget( buttonGroup );

    Layout1 = new QHBoxLayout( 0, 0, 6 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );
    resizeRecipeDialogLayout->addLayout( Layout1 );

    languageChange();

    adjustSize();
    clearWState( WState_Polished );

    newServingsInput->setValue( m_recipe->persons );
    currentServingsInput->setText( QString::number(m_recipe->persons) );

    buttonGroup->setButton( SERVINGS_RADIO_BUTTON );
    activateCurrentOption( SERVINGS_RADIO_BUTTON );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonGroup, SIGNAL( clicked(int) ), this, SLOT( activateCurrentOption(int) ) );
}

void ResizeRecipeDialog::languageChange()
{
    setCaption( i18n( "Resize Recipe" ) );
    buttonGroup->setTitle( QString::null );
    servingsRadioButton->setText( i18n( "Scale by servings" ) );
    newServingsLabel->setText( i18n( "New servings:" ) );
    currentServingsLabel->setText( i18n( "Current servings:" ) );
    factorRadioButton->setText( i18n( "Scale by factor" ) );
    factorLabel->setText( i18n( "Factor (i.e. 1/2 to half, 3 to triple):" ) );
    buttonOk->setText( i18n( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( i18n( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

void ResizeRecipeDialog::activateCurrentOption( int button_id )
{
	switch ( button_id )
	{
	case SERVINGS_RADIO_BUTTON:
		factorFrame->setEnabled(false);
		servingsFrame->setEnabled(true);
		break;
	case FACTOR_RADIO_BUTTON:
		factorFrame->setEnabled(true);
		servingsFrame->setEnabled(false);
		break;
	default: break;
	}
}

void ResizeRecipeDialog::accept()
{
	if ( currentServingsInput->text().toInt() == 0 )
		KMessageBox::error( this, i18n("Unable to scale a recipe with zero servings") );
	else if ( buttonGroup->selected() == servingsRadioButton )
	{
		int new_servings = newServingsInput->value();
		int current_servings = currentServingsInput->text().toInt();

		resizeRecipe( static_cast<double>(new_servings) / static_cast<double>(current_servings) );
	}
	else
	{
		if ( factorInput->isInputValid() && factorInput->value() > 0 )
			resizeRecipe( factorInput->value().toDouble() );
		else
		{
			KMessageBox::error( this, i18n("Invalid input") );
			factorInput->selectAll();
			return;
		}
	}

	QDialog::accept();
}

void ResizeRecipeDialog::resizeRecipe( double factor )
{
	int rounded_persons = static_cast<int>(ROUND(currentServingsInput->text().toInt() * factor));

	//adjust factor if when using this factor, we come out with a fraction of a person
	kdDebug()<<"factor given: "<<factor<<endl;
	factor = static_cast<double>(rounded_persons) / static_cast<double>(currentServingsInput->text().toInt());
	kdDebug()<<"modified factor: "<<factor<<endl;

	m_recipe->persons = rounded_persons;

	for ( Ingredient *ing = m_recipe->ingList.getFirst(); ing; ing = m_recipe->ingList.getNext() )
		ing->amount = ing->amount * factor;
}
