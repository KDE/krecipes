/***************************************************************************
*   Copyright © 2012 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kreauthorlistwidget.h"

#include <klocalizedstring.h>
#include <KApplication>
#include <KConfigGroup>
#include <KGlobal>

#include <QStandardItemModel>
#include <QThread>

#include "datablocks/elementlist.h"
#include "backends/recipedb.h"

#include <kdebug.h>

#include "threadedqueries/loadingauthorsthread.h"


KreAuthorListWidget::KreAuthorListWidget( QWidget *parent, RecipeDB *db ):
	KreGenericListWidget( parent, db )
{
	//The thread to execute the SQL query.
	m_thread = new LoadingAuthorsThread;
	
	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << i18nc( "@title:column", "Id" ) << i18nc( "@title:column", "Author" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	setCurrentLimit( config.readEntry( "Limit", -1 ) );

	connect( m_database, SIGNAL( authorCreated( const Element & ) ), 
		SLOT( createAuthor( const Element & ) ) );
	connect( m_database, SIGNAL( authorRemoved( int ) ), 
		SLOT( removeAuthor( int ) ) );

}

KreAuthorListWidget::~KreAuthorListWidget()
{
	cancelLoad();
	delete m_thread;
}

void KreAuthorListWidget::createAuthor( const Element &author )
{
          Q_UNUSED(author);
	reload( ForceReload );
}

void KreAuthorListWidget::removeAuthor( int id )
{
          Q_UNUSED(id);
	reload( ForceReload );
}

int KreAuthorListWidget::elementCount()
{
	return m_database->authorCount();
}

void KreAuthorListWidget::load( int limit, int offset )
{
	m_thread->setLimit( limit );
	m_thread->setOffset( offset );

	connect( m_thread, SIGNAL(loadFinished(const ElementList&,int)), 
		this, SLOT(queryFinished(const ElementList&,int)), Qt::QueuedConnection );

	kDebug() << "Starting thread.";
	m_thread->start();
}

void KreAuthorListWidget::cancelLoad()
{
	m_thread->disconnect();
	m_thread->wait();
}

void KreAuthorListWidget::queryFinished(const ElementList & authorList, int authorsLoaded)
{
	kDebug() << "Thread done, I'm in" << (size_t)QThread::currentThreadId();
	m_sourceModel->setRowCount( authorsLoaded );
	ElementList::const_iterator author_it;
	int current_row = 0;
	QModelIndex index;
        for ( author_it = authorList.constBegin(); author_it != authorList.constEnd(); ++author_it ) {
		// Write the database id in the model.
		index = m_sourceModel->index( current_row, 0 );
                m_sourceModel->setData( index, QVariant(author_it->id), Qt::EditRole );
                m_sourceModel->itemFromIndex( index )->setEditable( false );
		// Write the name of the author in the model.
		index = m_sourceModel->index( current_row, 1 );
                m_sourceModel->setData( index, QVariant(author_it->name), Qt::EditRole );
                m_sourceModel->itemFromIndex( index )->setEditable( true );
		// Increment the row counter.
		++current_row;
	}
	
	m_thread->disconnect();
	emit loadFinishedPrivate();
}

int KreAuthorListWidget::idColumn()
{
	return 0;
}

#include "kreauthorlistwidget.moc"
