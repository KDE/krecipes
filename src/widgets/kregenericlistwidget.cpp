/***************************************************************************
*   Copyright © 2012-2015 José Manuel Santamaría Lema <panfaust@gmail.com> *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kregenericlistwidget.h"

#include "ui_kregenericlistwidget.h"

//#include <kdebug.h>

#include <KApplication>
#include <KIcon>
#include <KPixmapSequenceWidget>
#include <KRecursiveFilterProxyModel>
#include <KLocalizedString>

#include <QStandardItemModel>
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
	m_proxyModel = new KRecursiveFilterProxyModel( this );
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
	connect( ui->m_treeView, SIGNAL(customContextMenuRequested(QPoint)),
		this, SLOT(contextMenuSlot(QPoint)) );
	m_anim = new KPixmapSequenceWidget;
	m_anim->setVisible( false );
	QGridLayout * layout = new QGridLayout;
	layout->addWidget( m_anim );
	ui->m_treeView->setLayout( layout );

	//The title label is hidden by default, if you change it with setListTitle(string)
	//it will be automatically displayed.
	ui->m_titleLabel->setVisible( false );

	//The filter text box.
	KConfigGroup configPerformance( KGlobal::config(), "Performance" );
	setSearchAsYouType( configPerformance.readEntry( "SearchAsYouType", true ) );

	//Navigation buttons
	ui->m_previousButton->setIcon( KIcon( "arrow-left" ) );
	ui->m_nextButton->setIcon( KIcon( "arrow-right" ) );
	connect( ui->m_previousButton, SIGNAL(clicked()), this, SLOT(activatePreviousPage()) );
	connect( ui->m_nextButton, SIGNAL(clicked()), this, SLOT(activateNextPage()) );

	//Connect signals/slots
	connect( this, SIGNAL(loadFinishedPrivate()), this, SLOT(loadFinishedPrivateSlot()) );
}

KreGenericListWidget::~KreGenericListWidget()
{
	delete ui;
}

void KreGenericListWidget::setListTitle( const QString & text )
{
	ui->m_titleLabel->setText( text );
	ui->m_titleLabel->setVisible( true );
}

void KreGenericListWidget::clear()
{
	m_sourceModel->setRowCount( 0 );
}

void KreGenericListWidget::setSearchAsYouType( bool value )
{
	disconnect( ui->m_searchBox );
	if (value) {
		connect( ui->m_searchBox, SIGNAL(textChanged(QString)),
			this, SLOT(setFilter(QString)) );
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
	disconnect( m_proxyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		this, SIGNAL(itemsChanged(QModelIndex,QModelIndex)) );

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
	connect( m_proxyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
		this, SIGNAL(itemsChanged(QModelIndex,QModelIndex)) );
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

QList<int> KreGenericListWidget::currentRows()
{
	QList<int> result;
	QModelIndexList index_list = ui->m_treeView->selectionModel()->selectedRows();

	QModelIndexList::iterator it;
	for ( it = index_list.begin(); it != index_list.end(); it++)
	{
		result << it->row();
	}

	return result;
}

QModelIndex KreGenericListWidget::currentParent() const
{
	return ui->m_treeView->currentIndex().parent();
}

void KreGenericListWidget::edit( int row, const QModelIndex & parent )
{
	QModelIndex index = m_proxyModel->index( row, 1, parent );
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

QVariant KreGenericListWidget::selectedRowData( int column, int role )
{
	int row = currentRow();
	QModelIndex index = m_proxyModel->index( row, column, currentParent() );
	if ( index.isValid() )
		return index.data( role );
	else
		return QVariant();

}

QVariant KreGenericListWidget::getData( int row, int column, int role )
{
	QModelIndex index = m_proxyModel->index( row, column, currentParent() );
	if ( index.isValid() ) {
		return index.data( role );
	} else {
		return QVariant();
	}
}

QStandardItem * KreGenericListWidget::getItem( int row, int column )
{
	QModelIndex proxy_index = m_proxyModel->index( row, column, currentParent() );
	QModelIndex source_index = m_proxyModel->mapToSource( proxy_index );
	if ( source_index.isValid() ) {
		return m_sourceModel->itemFromIndex( source_index );
	} else {
		return 0;
	}
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

QAbstractItemModel * KreGenericListWidget::model()
{
	return m_proxyModel;
}

#include "kregenericlistwidget.moc"
