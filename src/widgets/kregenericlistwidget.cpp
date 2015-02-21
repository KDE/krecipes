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
#include <KPixmapSequenceWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QGridLayout>
#include <QTimer>

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
	m_proxyModel->setFilterKeyColumn( 1 );
	m_proxyModel->setSourceModel( m_sourceModel );
	ui->m_treeView->setModel( m_proxyModel );

	//The QTreeView
	KConfigGroup configAdvanced( KGlobal::config(), "Advanced" );
	if ( !configAdvanced.readEntry( "ShowID", false ) ) {
		ui->m_treeView->hideColumn( 0 );
	}
	ui->m_treeView->setRootIsDecorated( false );
	ui->m_treeView->setUniformRowHeights( true );
	ui->m_treeView->setContextMenuPolicy( Qt::CustomContextMenu );
	connect( ui->m_treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
		this, SLOT(contextMenuSlot(const QPoint &)) );
	m_anim = new KPixmapSequenceWidget;
	m_anim->setVisible( false );
	QGridLayout * layout = new QGridLayout;
	layout->addWidget( m_anim );
	ui->m_treeView->setLayout( layout );

	//The filter text box.
	KConfigGroup configPerformance( KGlobal::config(), "Performance" );
	setSearchAsYouType( configPerformance.readEntry( "SearchAsYouType", true ) );

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

void KreGenericListWidget::setSearchAsYouType( bool value )
{
	disconnect( ui->m_searchBox );
	if (value) {
		connect( ui->m_searchBox, SIGNAL(textChanged(const QString &)),
			this, SLOT(setFilter(const QString &)) );
	} else {
		connect( ui->m_searchBox, SIGNAL(returnPressed()),
			this, SLOT(setFilter()) );	
		connect( ui->m_searchBox, SIGNAL(clearButtonClicked()),
			this, SLOT(setFilter()) );
	}
}

void KreGenericListWidget::setCurrentLimit( int value )
{
	if (value == -1) { 
		ui->m_pageNavigationWidget->setVisible( false );
		ui->m_searchBox->setClickMessage( i18nc( "@label:textbox",
			"Type here the text to search" ) );
	} else {
		ui->m_pageNavigationWidget->setVisible( true );
		ui->m_searchBox->setClickMessage( i18nc( "@label:textbox",
			"Type here the text to search in the current page" ) );
	}
	m_currentLimit = value;
}

void KreGenericListWidget::reload( ReloadFlags flags )
{
          Q_UNUSED(flags);
	this->setEnabled( false );

	m_loadFinished = false;
	QTimer::singleShot( busyAnimationThreshold, this, SLOT(startAnimation()) );

	if (m_currentLimit != -1) { //-1 means unlimited. 
		//If we are at the first page, the previous button must be disabled.
		if (m_currentOffset == 0)
			ui->m_previousButton->setEnabled( false );
		else
			ui->m_previousButton->setEnabled( true );

		//If we are at the last page, the next button must be disabled.
		if ( m_currentOffset + m_currentLimit >= elementCount() )
			ui->m_nextButton->setEnabled( false );
		else
			ui->m_nextButton->setEnabled( true );
	}

	//Cancel/wait for any possible loading thread in progress.
	KApplication::setOverrideCursor( Qt::WaitCursor );
	cancelLoad();
	KApplication::restoreOverrideCursor();

	//Disconnect this signal, because calling load(...) may alter the model
	//data triggering this signal, we must restore this signal/slot connection
	//as soon as the load is finished.
	disconnect( m_proxyModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
		this, SIGNAL(itemsChanged(const QModelIndex &, const QModelIndex &)) );

	connect( this, SIGNAL(loadFinishedPrivate()), this, SLOT(loadFinishedPrivateSlot()) );

	//Reload the current page.
	load( m_currentLimit, m_currentOffset );

}


void KreGenericListWidget::startAnimation()
{
	if (!m_loadFinished) {
		m_anim->setVisible( true );
	}
}

void KreGenericListWidget::loadFinishedPrivateSlot()
{
	connect( m_proxyModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
		this, SIGNAL(itemsChanged(const QModelIndex &, const QModelIndex &)) );
	m_loadFinished = true;
	m_anim->setVisible( false );
	this->setEnabled( true );
}


void KreGenericListWidget::setFilter()
{
	setFilter( ui->m_searchBox->text() );
}

void KreGenericListWidget::setFilter( const QString & filter )
{
	QRegExp filterRegExp( filter, Qt::CaseInsensitive, QRegExp::FixedString );
	m_proxyModel->setFilterRegExp( filterRegExp );
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

int KreGenericListWidget::currentRow ()
{
	QModelIndex index = ui->m_treeView->currentIndex();
	if ( index.isValid() )
		return index.row();
	else
		return -1;
}

QModelIndex KreGenericListWidget::currentParent() const
{
	return ui->m_treeView->currentIndex().parent();
}

void KreGenericListWidget::edit( int row )
{
	QModelIndex index = m_proxyModel->index( row, 1 );
	ui->m_treeView->edit( index );
}

int KreGenericListWidget::selectedRowId()
{
	int row = currentRow();
	QModelIndex index = m_proxyModel->index( row, idColumn(), currentParent() );
	if ( index.isValid() )
		return index.data().toInt();
	else
		return -1;
}

void KreGenericListWidget::contextMenuSlot( const QPoint & point)
{
	QPoint globalPoint( ui->m_treeView->mapToGlobal(point) );
	QHeaderView * header = ui->m_treeView->header();
	if ( !header->isHidden() ) {
		globalPoint.setY( globalPoint.y() + header->height() );
	}
	emit contextMenuRequested( ui->m_treeView->indexAt(point),
		globalPoint );
}


#include "kregenericlistwidget.moc"
