/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientparserdialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qheader.h>
#include <qapplication.h>
#include <qclipboard.h>

#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <kpushbutton.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kpopupmenu.h>

#include "datablocks/mixednumber.h"
#include "widgets/inglistviewitem.h"

IngredientParserDialog::IngredientParserDialog( const UnitList &units, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ), m_unitList(units)
{
	if ( !name )
		setName( "IngredientParserDialog" );
	IngredientParserDialogLayout = new QHBoxLayout( this, 11, 6, "IngredientParserDialogLayout"); 
	
	layout4 = new QVBoxLayout( 0, 0, 6, "layout4"); 
	
	textLabel1 = new QLabel( this, "textLabel1" );
	textLabel1->setTextFormat( QLabel::RichText );
	layout4->addWidget( textLabel1 );
	
	ingredientTextEdit = new QTextEdit( this, "ingredientTextEdit" );
	ingredientTextEdit->setTextFormat( QTextEdit::PlainText );
	layout4->addWidget( ingredientTextEdit );
	
	parseButton = new KPushButton( this, "parseButton" );
	layout4->addWidget( parseButton );

	previewLabel = new QLabel( this, "previewLabel" );
	previewLabel->setTextFormat( QLabel::RichText );
	layout4->addWidget( previewLabel );
	
	previewIngView = new KListView( this, "previewIngView" );
	previewIngView->addColumn( i18n( "Ingredient" ) );
	previewIngView->addColumn( i18n( "Amount" ) );
	previewIngView->addColumn( i18n( "Unit" ) );
	previewIngView->addColumn( i18n( "Preparation Method" ) );
	layout4->addWidget( previewIngView );
	IngredientParserDialogLayout->addLayout( layout4 );
	
	Layout5 = new QVBoxLayout( 0, 0, 6, "Layout5"); 
	
	buttonOk = new QPushButton( this, "buttonOk" );
	buttonOk->setAutoDefault( TRUE );
	buttonOk->setDefault( TRUE );
	Layout5->addWidget( buttonOk );
	
	buttonCancel = new QPushButton( this, "buttonCancel" );
	buttonCancel->setAutoDefault( TRUE );
	Layout5->addWidget( buttonCancel );
	
	//buttonHelp = new QPushButton( this, "buttonHelp" );
	//buttonHelp->setAutoDefault( TRUE );
	//Layout5->addWidget( buttonHelp );
	Spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	Layout5->addItem( Spacer1 );
	IngredientParserDialogLayout->addLayout( Layout5 );
	languageChange();
	resize( QSize(577, 371).expandedTo(minimumSizeHint()) );
	clearWState( WState_Polished );

	previewIngView->setItemsRenameable( true );
	previewIngView->setRenameable( 0, true );
	previewIngView->setRenameable( 1, true );
	previewIngView->setRenameable( 2, true );
	previewIngView->setRenameable( 3, true );

	ingredientTextEdit->setText( QApplication::clipboard()->text() );
	ingredientTextEdit->selectAll();

	KPopupMenu *kpop = new KPopupMenu( previewIngView );
	kpop->insertItem( i18n( "&Delete" ), this, SLOT( removeIngredient() ), Key_Delete );
 
	connect( parseButton, SIGNAL(clicked()), this, SLOT(parseText()) );

	connect( buttonOk, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( buttonCancel, SIGNAL(clicked()), this, SLOT(reject()) );
	//connect( buttonHelp, SIGNAL(clicked()), this, SLOT(dunno()) );
}

IngredientParserDialog::~IngredientParserDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

void IngredientParserDialog::languageChange()
{
	setCaption( i18n( "Ingredient Parser" ) );
	textLabel1->setText( i18n( "To use: Paste a list of ingredient below, click \"Parse Text\", and then you may correct any incorrectly parsed ingredients." ) );
	previewLabel->setText( i18n("Ingredients as understood by Krecipes:") );
	parseButton->setText( i18n( "Parse Text" ) );
	previewIngView->header()->setLabel( 0, i18n( "Ingredient" ) );
	previewIngView->header()->setLabel( 1, i18n( "Amount" ) );
	previewIngView->header()->setLabel( 2, i18n( "Unit" ) );
	previewIngView->header()->setLabel( 3, i18n( "Preparation Method" ) );
	buttonOk->setText( i18n( "&OK" ) );
	buttonOk->setAccel( QKeySequence( QString::null ) );
	buttonCancel->setText( i18n( "&Cancel" ) );
	buttonCancel->setAccel( QKeySequence( QString::null ) );
	//buttonHelp->setText( i18n( "&Help" ) );
	//buttonHelp->setAccel( QKeySequence( i18n( "F1" ) ) );
}

void IngredientParserDialog::accept()
{
	for ( IngListViewItem * it = (IngListViewItem*)previewIngView->firstChild(); it; it = (IngListViewItem*)it->nextSibling() ) {
		m_ingList.append(it->ingredient());
	}

	if ( m_ingList.count() == 0 ) {
		KMessageBox::questionYesNo(this,i18n("There are no parsed ingredients.\nContinue anyway?"));
	}

	QDialog::accept();
}

void IngredientParserDialog::removeIngredient()
{
	delete previewIngView->selectedItem();
}

void IngredientParserDialog::parseText()
{
	previewIngView->clear();

	int line = 0;
	QStringList ingredients = QStringList::split("\n",ingredientTextEdit->text());
	for ( QStringList::const_iterator it = ingredients.begin(); it != ingredients.end(); ++it ) {
		++line;
		int format_at = 0;
		Ingredient ing;


		//======amount======//
		int first_space = (*it).find( " ", format_at+1 );
		if ( first_space == -1 )
			first_space = (*it).length();

		int second_space = (*it).find( " ", first_space+1 );
		if ( second_space == -1 )
			second_space = (*it).length();

		Ingredient i;
		bool ok;
		i.setAmount((*it).mid(format_at,second_space-format_at),&ok);
		if ( !ok ) {
			i.setAmount((*it).mid(format_at,first_space-format_at),&ok);
			if ( ok ) format_at = first_space+1;
		}
		else
			format_at = second_space+1;

		if ( ok ) {
			ing.amount = i.amount;
			ing.amount_offset = i.amount_offset;
		}
		else
			kdDebug()<<"no amount on line "<<line<<endl;


		//======unit======//
		first_space = (*it).find( " ", format_at+1 );
		if ( first_space == -1 )
			first_space = (*it).length();

		bool isUnit = false;
		QString unitCheck = (*it).mid(format_at,first_space-format_at);

		for ( UnitList::const_iterator unit_it = m_unitList.begin(); unit_it != m_unitList.end(); ++unit_it ) {
			if ( (*unit_it).name == unitCheck || (*unit_it).plural == unitCheck ) {
				isUnit = true;
				format_at = first_space+1;
				break;
			}
		}

		if ( isUnit ) {
			ing.units.name = unitCheck;
			ing.units.plural = unitCheck;
		}
		else
			kdDebug()<<"no unit on line "<<line<<endl;


		//======ingredient======//
		int ing_end = (*it).find( QRegExp("(,|;)"), format_at+1 );
		if ( ing_end == -1 )
			ing_end = (*it).length();

		ing.name = (*it).mid(format_at,ing_end-format_at);
		format_at = ing_end+2;


		//======prep method======//
		int end = (*it).length();
		ing.prepMethodList = ElementList::split(",",(*it).mid(format_at,end-format_at));

		new IngListViewItem(previewIngView,ing);
	}
}

#include "ingredientparserdialog.moc"
