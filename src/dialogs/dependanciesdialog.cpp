/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Unai Garro (ugarro@users.sourceforge.net)                             *
*   Cyril Bosselut (bosselut@b1project.com)                               *
*                                                                         *
*   Copyright (C) 2003-2005 by                                            *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "dependanciesdialog.h"
#include "datablocks/elementlist.h"

#include <qvbox.h>

#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>

DependanciesDialog::DependanciesDialog( QWidget *parent, const ElementList* recipeList, const ElementList* propertiesList, bool deps_are_deleted )
		: KDialogBase( parent, "DependanciesDialog", true, QString::null,
		    KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Cancel ),
		recipeListView(0), propertiesListView(0), m_depsAreDeleted(deps_are_deleted)
{
	QVBox *page = makeVBoxMainWidget();

	// Design the dialog

	instructionsLabel = new QLabel( page );
	instructionsLabel->setMinimumSize( QSize( 100, 30 ) );
	instructionsLabel->setMaximumSize( QSize( 10000, 10000 ) );
	instructionsLabel->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
	
	if ( m_depsAreDeleted ) {
		instructionsLabel->setText( i18n( "<b>WARNING:</b> The following will have to be removed also, since currently they use the element you have chosen to be removed." ) );
	}
	else {
		instructionsLabel->setText( i18n( "<b>WARNING:</b> The following currently use the element you have chosen to be removed." ) );
	}

	if ( recipeList ) {
		if ( !( recipeList->isEmpty() ) ) {
			recipeBox = new QGroupBox( 1, Qt::Vertical, i18n( "Recipes" ), page );
			recipeListView = new KListView( recipeBox );

			KConfig * config = KGlobal::config();
			config->setGroup( "Advanced" );
			bool show_id = config->readBoolEntry( "ShowID", false );
			recipeListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );

			recipeListView->addColumn( i18n( "Recipe Title" ) );
			recipeListView->setAllColumnsShowFocus( true );
			loadList( recipeListView, recipeList );
		}
	}

	if ( propertiesList ) {
		if ( !( propertiesList->isEmpty() ) ) {
			propertiesBox = new QGroupBox( 1, Qt::Vertical, i18n( "Properties" ), page );
			propertiesListView = new KListView( propertiesBox );

			KConfig * config = KGlobal::config();
			config->setGroup( "Advanced" );
			bool show_id = config->readBoolEntry( "ShowID", false );
			propertiesListView->addColumn( i18n( "Id" ), show_id ? -1 : 0 );
			
			propertiesListView->addColumn( i18n( "Property" ) );
			loadList( propertiesListView, propertiesList );
		}
	}

	setSizeGripEnabled( true );
}

DependanciesDialog::~DependanciesDialog()
{}


void DependanciesDialog::loadList( KListView* listView, const ElementList *list )
{
	for ( ElementList::const_iterator el_it = list->begin(); el_it != list->end(); ++el_it ) {
		QString id;
		int idnum = ( *el_it ).id;
		if ( idnum < 0 )
			id = "-";
		else
			id = QString::number( idnum );
		QListViewItem* it = new QListViewItem( listView, id, ( *el_it ).name );
		listView->insertItem( it );
	}
}

void DependanciesDialog::accept()
{
	if ( recipeListView && m_depsAreDeleted ) {
		switch ( KMessageBox::warningYesNo(this,
			i18n("<b>You are about to permanantly delete recipes from your database.</b><br><br>Are you sure you wish to proceed?"),
			QString::null,KStdGuiItem::yes(),KStdGuiItem::no(),"DeleteRecipe") )
		{
		case KMessageBox::Yes: QDialog::accept(); break;
		case KMessageBox::No: QDialog::reject(); break;
		}
	}

	QDialog::accept();
}
