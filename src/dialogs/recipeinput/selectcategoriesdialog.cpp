/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "selectcategoriesdialog.h"

#include "actionshandlers/krecategoryactionshandler.h"
#include "widgets/krecategorieslistwidget.h"
#include "backends/recipedb.h"
#include "datablocks/categorytree.h"

#include <KVBox>
#include <KLocale>
#include <KPushButton>


SelectCategoriesDialog::SelectCategoriesDialog( QWidget *parent, const ElementList &items_on, RecipeDB *db )
		: KDialog( parent ),
		database(db)
{
	setCaption(i18nc("@title:window", "Categories" ));
	setButtons(KDialog::Ok | KDialog::Cancel);
	setDefaultButton(KDialog::Ok);
	setModal( true );
	KVBox *page = new KVBox( this );
	setMainWidget( page );
	//Design UI

	//Category List
	m_categoriesListWidget = new KreCategoriesListWidget( page, db, true );
	m_categoriesListWidget->reload();
	m_categoriesListWidget->checkCategories( items_on );
	m_categoriesListWidget->expandAll();

	//New category button
	KPushButton *newCatButton = new KPushButton( page );
	newCatButton->setText( i18nc( "@action:button", "&New Category..." ) );
	newCatButton->setIcon( KIcon( "list-add" ) );

	// Connect signals & Slots
	connect ( newCatButton, SIGNAL( clicked() ), SLOT( createNewCategory() ) );
}

SelectCategoriesDialog::~SelectCategoriesDialog()
{}

void SelectCategoriesDialog::getSelectedCategories( ElementList *newSelected )
{
	*newSelected = m_categoriesListWidget->checkedCategories();
}

void SelectCategoriesDialog::createNewCategory( void )
{
	KreCategoryActionsHandler actionsHandler( m_categoriesListWidget, database );
	actionsHandler.createNew();
}


#include "selectcategoriesdialog.moc"
