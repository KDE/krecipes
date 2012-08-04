/***************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kregenericlistwidget.h"

#include "ui_kregenericlistwidget.h"

#include <KApplication>
#include <KIcon>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "backends/recipedb.h"


KreGenericListWidget::KreGenericListWidget( QWidget *parent, RecipeDB *db ):
	QWidget(parent),
	m_currentOffset(0),
	ui(new Ui::KreGenericListWidget),
	m_database(db)
{

	ui->setupUi(this);
	setCurrentLimit(-1); //-1 means no limit

	//The data models
	m_sourceModel = new QStandardItemModel( 0, 2, this ); 
	m_proxyModel = new QSortFilterProxyModel( this );
	m_proxyModel->setDynamicSortFilter( true );
	m_proxyModel->setSourceModel( m_sourceModel );
	ui->m_treeView->setModel( m_proxyModel );

	//The QTreeView
	KConfigGroup config( KGlobal::config(), "Advanced" );
	if ( !config.readEntry( "ShowID", false ) ) {
		ui->m_treeView->hideColumn( 0 );
	}
	ui->m_treeView->setRootIsDecorated( false );

	//Navigation buttons
	ui->m_previousButton->setIcon( KIcon( "arrow-left" ) );
	ui->m_nextButton->setIcon( KIcon( "arrow-right" ) );
	connect( ui->m_previousButton, SIGNAL(clicked()), this, SLOT(activatePreviousPage()) );
	connect( ui->m_nextButton, SIGNAL(clicked()), this, SLOT(activateNextPage()) );

}

KreGenericListWidget::~KreGenericListWidget()
{
	delete ui;
}

void KreGenericListWidget::setCurrentLimit( int value )
{
	if (value == -1)
		ui->m_pageNavigationWidget->setVisible( false );
	else
		ui->m_pageNavigationWidget->setVisible( true );
	m_currentLimit = value;
}

void KreGenericListWidget::reload( ReloadFlags flags )
{
	if (m_currentLimit != -1) { //-1 means unlimited. 
		//If we are at the first page, the previous button must be disabled.
		if (m_currentOffset == 0)
			ui->m_previousButton->setEnabled( false );
		else
			ui->m_previousButton->setEnabled( true );

		//If we are at the last page, the next button must be disabled.
		int authorCount = m_database->authorCount();
		if ( m_currentOffset + m_currentLimit >= authorCount )
			ui->m_nextButton->setEnabled( false );
		else
			ui->m_nextButton->setEnabled( true );
	}

	//Reload the current page.
	KApplication::setOverrideCursor( Qt::WaitCursor );
	m_sourceModel->setRowCount( 0 );
	load( m_currentLimit, m_currentOffset );
	KApplication::restoreOverrideCursor();
}

void KreGenericListWidget::activatePreviousPage()
{
	if ( m_currentOffset != 0 ) {
		m_currentOffset -= m_currentLimit;
		if ( m_currentOffset < 0 )
			m_currentOffset = 0;
		reload(ForceReload);
	}
}

void KreGenericListWidget::activateNextPage()
{
	m_currentOffset += m_currentLimit;
        reload(ForceReload);
}


#include "kregenericlistwidget.moc"
