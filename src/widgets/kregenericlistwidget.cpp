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


KreGenericListWidget::KreGenericListWidget( QWidget *parent, RecipeDB *db ):
	QWidget(parent),
	ui(new Ui::KreGenericListWidget),
	m_database(db)
{

	ui->setupUi(this);

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
}

KreGenericListWidget::~KreGenericListWidget()
{
	delete ui;
}

void KreGenericListWidget::reload( ReloadFlags flags )
{
	KApplication::setOverrideCursor( Qt::WaitCursor );
	load(-1,-1);
	KApplication::restoreOverrideCursor();
}


#include "kregenericlistwidget.moc"
