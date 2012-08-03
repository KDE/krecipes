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

#include <QStandardItemModel>

#include <kdebug.h>


KreAuthorListWidget::KreAuthorListWidget( QWidget *parent, RecipeDB *db ):
	KreGenericListWidget( parent, db )
{
	QStringList horizontalLabels;
	horizontalLabels << "Id" << i18nc( "@title:column", "Author" );
	m_sourceModel->setHorizontalHeaderLabels( horizontalLabels );
}

void KreAuthorListWidget::createAuthor( const Element &author )
{
	//TODO
}

void KreAuthorListWidget::removeAuthor( int id )
{
	//TODO
}


#include "kreauthorlistwidget.moc"
