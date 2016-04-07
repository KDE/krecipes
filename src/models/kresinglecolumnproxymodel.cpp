/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kresinglecolumnproxymodel.h"

KreSingleColumnProxyModel::KreSingleColumnProxyModel( int column ):
	QSortFilterProxyModel( 0 ),
	m_acceptedColumn( column )
{
}

bool KreSingleColumnProxyModel::filterAcceptsColumn( int source_column, 
	const QModelIndex& source_parent ) const
{
	if ( source_column == m_acceptedColumn ) {
		return QSortFilterProxyModel::filterAcceptsColumn(
			source_column, source_parent );
	} else {
		return false;
	}
}
