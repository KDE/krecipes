/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientsdialog.h"

#include "backends/recipedb.h"
#include "createelementdialog.h"
#include "dialogs/dependanciesdialog.h"
#include "widgets/ingredientlistview.h"
#include "dialogs/ingredientgroupsdialog.h"
#include "dialogs/editpropertiesdialog.h"
#include "actionshandlers/ingredientactionshandler.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kconfig.h>

#include <q3header.h>
#include <KTabWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <KPushButton>
#include <QPointer>

IngredientsDialog::IngredientsDialog( QWidget* parent, RecipeDB *db ) : QWidget( parent )
{

	// Store pointer to database
	database = db;

	// Design dialog

	QHBoxLayout* page_layout = new QHBoxLayout( this );

	tabWidget = new KTabWidget( this );

	ingredientTab = new QWidget( tabWidget );

	layout = new QHBoxLayout( ingredientTab );

	listLayout = new QVBoxLayout;

	ingredientListView = new KreListView ( ingredientTab, QString(), true, 0 );
	StdIngredientListView *list_view = new StdIngredientListView( ingredientListView, database, true );
	ingredientActionsHandler = new IngredientActionsHandler( list_view, database );
	ingredientListView->setListView( list_view );
	listLayout->addWidget( ingredientListView );
	ingredientListView->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

	QVBoxLayout *buttonLayout = new QVBoxLayout();

	addIngredientButton = new KPushButton( ingredientTab );
	addIngredientButton->setText( i18nc( "@action:button", "Create..." ) );
	addIngredientButton->setIcon( KIcon( "list-add") );
	buttonLayout->addWidget( addIngredientButton );

	removeIngredientButton = new KPushButton( ingredientTab );
	removeIngredientButton->setText( i18nc( "@action:button", "Delete" ) );
	removeIngredientButton->setIcon( KIcon( "list-remove" ) );
	buttonLayout->addWidget( removeIngredientButton );

	buttonLayout->addStretch();
	
	QPushButton *propertyButton = new KPushButton( i18nc("@action:button", "Property Information"), ingredientTab );
	propertyButton->setIcon( KIcon( "document-properties") );
	listLayout->addWidget( propertyButton );

	layout->addLayout( listLayout );
	layout->addLayout( buttonLayout );

	tabWidget->insertTab( -1, ingredientTab, i18nc( "@title:tab", "Ingredients" ) );

	groupsDialog = new IngredientGroupsDialog(database,tabWidget,"groupsDialog");
	tabWidget->insertTab( -1, groupsDialog, i18nc( "@title:tab", "Headers" ) );

	page_layout->addWidget( tabWidget );

	// Signals & Slots
	connect( addIngredientButton, SIGNAL( clicked() ), ingredientActionsHandler, SLOT( createNew() ) );
	connect( removeIngredientButton, SIGNAL( clicked() ), ingredientActionsHandler, SLOT( remove() ) );
	connect( propertyButton, SIGNAL( clicked() ), this, SLOT( showPropertyEdit() ) );

	DependanciesDialog d( this, ListInfo() );
}


IngredientsDialog::~IngredientsDialog()
{
}

void IngredientsDialog::reloadIngredientList( ReloadFlags flag )
{
	( ( StdIngredientListView* ) ingredientListView->listView() ) ->reload(flag);
}

void IngredientsDialog::reload( ReloadFlags flag )
{
	reloadIngredientList( flag );
	groupsDialog->reload( flag );
}

ActionsHandlerBase * IngredientsDialog::getActionsHandler() const
{
	if ( tabWidget->currentWidget() == ingredientTab )
		return ingredientActionsHandler;
	else //if ( tabWidget->currentWidget() == groupsDialog )
		return groupsDialog->getActionsHandler();
}

void IngredientsDialog::addAction( KAction * action )
{
	ingredientActionsHandler->addAction( action );
	groupsDialog->addAction( action );
}

void IngredientsDialog::showPropertyEdit()
{
	Q3ListViewItem * ing_it = ingredientListView->listView() ->selectedItem(); // Find selected ingredient
	if ( ing_it ) {
		QPointer<EditPropertiesDialog> d = new EditPropertiesDialog( ing_it->text(1).toInt(),ing_it->text(0),database,this );
		d->exec();
		delete d;
	}
	else
		KMessageBox::information( this, i18nc( "@info", "No ingredient selected." ), QString() );
}

#include "ingredientsdialog.moc"
