/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KRESINGLECOLUMNMODEL_H
#define KRESINGLECOLUMNMODEL_H

#include <QSortFilterProxyModel>

class KreSingleColumnProxyModel: public QSortFilterProxyModel {
Q_OBJECT

public:
	KreSingleColumnProxyModel( int column, QObject * parent = 0 );

protected:
	bool filterAcceptsColumn( int source_column, 
		const QModelIndex& source_parent ) const;

	int m_acceptedColumn;

};

#endif
