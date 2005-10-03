/***************************************************************************
*   Copyright (C) 2005 by Jason Kivlighn                                  *
*   jkivlighn@gmail.com                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "editratingdialog.h"

#include <klocale.h>

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <knuminput.h>
#include <qheader.h>
#include <klistview.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include "datablocks/rating.h"

EditRatingDialog::EditRatingDialog( const Rating &rating, QWidget* parent, const char* name ) : QDialog(parent,name)
{
	if ( !name )
		setName( "EditRatingDialog" );

	init();
	loadRating(rating);
}

/*
 *  Constructs a EditRatingDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
EditRatingDialog::EditRatingDialog( QWidget* parent, const char* name )
    : QDialog( parent, name )
{
	if ( !name )
		setName( "EditRatingDialog" );

	init();
}

void EditRatingDialog::init()
{
	EditRatingDialogLayout = new QVBoxLayout( this, 11, 6, "EditRatingDialogLayout"); 
	
	layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 
	
	criteriaLabel = new QLabel( this, "criteriaLabel" );
	layout8->addWidget( criteriaLabel );
	
	criteriaComboBox = new QComboBox( FALSE, this, "criteriaComboBox" );
	criteriaComboBox->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, criteriaComboBox->sizePolicy().hasHeightForWidth() ) );
	criteriaComboBox->setEditable( TRUE );
	layout8->addWidget( criteriaComboBox );
	
	starsLabel = new QLabel( this, "starsLabel" );
	layout8->addWidget( starsLabel );
	
	starsSpinBox = new KDoubleSpinBox( this, "starsSpinBox" );
	starsSpinBox->setMaxValue( 5 );
	starsSpinBox->setLineStep( 0.5 );
	layout8->addWidget( starsSpinBox );
	
	addButton = new QPushButton( this, "addButton" );
	layout8->addWidget( addButton );
	EditRatingDialogLayout->addLayout( layout8 );
	
	criteriaListView = new KListView( this, "criteriaListView" );
	criteriaListView->addColumn( i18n( "Criteria" ) );
	criteriaListView->addColumn( i18n( "Stars" ) );
	criteriaListView->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, criteriaListView->sizePolicy().hasHeightForWidth() ) );
	EditRatingDialogLayout->addWidget( criteriaListView );
	
	commentsLabel = new QLabel( this, "commentsLabel" );
	EditRatingDialogLayout->addWidget( commentsLabel );
	
	commentsEdit = new QTextEdit( this, "commentsEdit" );
	EditRatingDialogLayout->addWidget( commentsEdit );
	
	layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 
	
	raterLabel = new QLabel( this, "raterLabel" );
	layout2->addWidget( raterLabel );
	
	lineEdit1 = new QLineEdit( this, "lineEdit1" );
	layout2->addWidget( lineEdit1 );
	EditRatingDialogLayout->addLayout( layout2 );
	
	layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 
	
	okButton = new QPushButton( this, "okButton" );
	layout7->addWidget( okButton );
	
	cancelButton = new QPushButton( this, "cancelButton" );
	layout7->addWidget( cancelButton );
	buttonsSpacer = new QSpacerItem( 240, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
	layout7->addItem( buttonsSpacer );
	EditRatingDialogLayout->addLayout( layout7 );
	languageChange();
	resize( QSize(358, 331).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );

	connect( addButton, SIGNAL(clicked()), this, SLOT(slotAddRatingCriteria()) );
	connect( okButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
EditRatingDialog::~EditRatingDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void EditRatingDialog::languageChange()
{
	setCaption( i18n( "EditRatingDialog" ) );
	criteriaLabel->setText( i18n( "Criteria:" ) );
	starsLabel->setText( i18n( "Stars:" ) );
	addButton->setText( i18n( "Add" ) );
	criteriaListView->header()->setLabel( 0, i18n( "Criteria" ) );
	criteriaListView->header()->setLabel( 1, i18n( "Stars" ) );
	commentsLabel->setText( i18n( "Comments:" ) );
	raterLabel->setText( i18n( "Rater:" ) );
	okButton->setText( i18n( "&OK" ) );
	okButton->setAccel( QKeySequence( i18n( "Alt+O" ) ) );
	cancelButton->setText( i18n( "&Cancel" ) );
	cancelButton->setAccel( QKeySequence( i18n( "Alt+C" ) ) );
}

Rating EditRatingDialog::rating() const
{
	Rating r;

	for ( QListViewItem *it = criteriaListView->firstChild(); it; it = it->nextSibling() ) {
		RatingCriteria rc;
		rc.name = it->text(0);
		rc.stars = it->text(1).toDouble();
		r.append( rc );
	}

	r.comment = commentsEdit->text();

	return r;
}

void EditRatingDialog::loadRating( const Rating &rating )
{
	
}

void EditRatingDialog::slotAddRatingCriteria()
{
	(void)new QListViewItem(criteriaListView,criteriaComboBox->lineEdit()->text(),QString::number(starsSpinBox->value()));
}

#include "editratingdialog.moc"
