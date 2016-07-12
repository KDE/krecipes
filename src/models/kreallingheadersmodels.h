/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREALLINGHEADERSMODELS_H
#define KREALLINGHEADERSMODELS_H

#include <QObject>

class RecipeDB;
class KreSingleColumnProxyModel;
class KreIngHeader;
class KCompletion;
class QStandardItemModel;

class KreAllIngHeadersModels: public QObject {
Q_OBJECT

public:
	KreAllIngHeadersModels( RecipeDB * database );
	~KreAllIngHeadersModels();

	QStandardItemModel * sourceModel();
	KreSingleColumnProxyModel * ingHeaderNameModel();
	KCompletion * ingHeaderNameCompletion();

//private slots:
//	void ingHeaderCreatedDBSlot( const KreIngHeader & );
//	void ingHeaderModifiedDBSlot( const KreIngHeader & );
//	void ingHeaderRemovedDBSlot( const QVariant & );

private:
	RecipeDB * m_database;
	QStandardItemModel * m_sourceModel;
	KreSingleColumnProxyModel * m_ingHeaderNameModel;
	KCompletion * m_ingHeaderNameCompletion;
	
};

#endif
