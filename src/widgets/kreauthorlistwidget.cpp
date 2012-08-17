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

#include "datablocks/elementlist.h"
#include "backends/recipedb.h"


KreAuthorListWidget::KreAuthorListWidget( QWidget *parent, RecipeDB *db ):
	KreGenericListWidget( parent, db )
{
	//The horizontal column labels.
	QStringList horizontalLabels;
	horizontalLabels << "Id" << i18nc( "@title:column", "Author" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );

	//The maximum number of elements to show in the author list.
	KConfigGroup config = KGlobal::config()->group( "Performance" );
	setCurrentLimit( config.readEntry( "Limit", -1 ) );

	connect( m_database, SIGNAL( authorCreated( const Element & ) ), 
		SLOT( createAuthor( const Element & ) ) );
	connect( m_database, SIGNAL( authorRemoved( int ) ), 
		SLOT( removeAuthor( int ) ) );

}

void KreAuthorListWidget::createAuthor( const Element &author )
{
	reload( ForceReload );
}

void KreAuthorListWidget::removeAuthor( int id )
{
	reload( ForceReload );
}

void KreAuthorListWidget::load( int limit, int offset )
{
	ElementList authorList;
	int loadedAuthorsNumber = m_database->loadAuthors( &authorList, limit, offset );
	m_sourceModel->setRowCount( loadedAuthorsNumber );

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
}

#include "kreauthorlistwidget.moc"
