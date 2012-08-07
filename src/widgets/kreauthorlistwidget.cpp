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
	m_database->loadAuthors( &authorList, limit, offset );

	ElementList::const_iterator author_it;
        for ( author_it = authorList.constBegin(); author_it != authorList.constEnd(); ++author_it ) {
                QStandardItem *itemId = new QStandardItem;
                itemId->setData( QVariant(author_it->id), Qt::EditRole );
                itemId->setEditable( false );
                QStandardItem *itemAuthor = new QStandardItem( author_it->name );
                itemAuthor->setEditable( true );
                QList<QStandardItem*> items;
                items << itemId << itemAuthor;
                m_sourceModel->appendRow( items );
	}
}

#include "kreauthorlistwidget.moc"
