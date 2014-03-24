/**************************************************************************
*   Copyright © 2014 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KRECONVERSIONTABLE_H
#define KRECONVERSIONTABLE_H

#include "datablocks/unitratio.h"
#include "datablocks/elementlist.h"
#include "datablocks/unit.h"

#include <QObject>
#include <QTableWidget>


class QSignalMapper;


class KreConversionTable: public QTableWidget
{
	Q_OBJECT
public:

	KreConversionTable( QWidget* parent, int maxrows, int maxcols );
	~KreConversionTable();
	/*void createNewItem( int r, int c, double amount );*/
	void setUnitIDs( const IDList &idList );
	void setRatio( int ingID1, int ingID2, double ratio );
	void setRatio( const UnitRatio &r )
	{
		setRatio( r.unitId1(), r.unitId2(), r.ratio() );
	}
	void deleteRatio( int r, int c );
	int getUnitID( int rc );
	//QString text( int r, int c ) const; //Reimplement, otherwise it won't work this way*/
	void resize( int r, int c );
	//void clear( void );
private:

	//Internal Variables
	IDList unitIDs; // unit ID list to know the units by ID, not name
	QSignalMapper * m_signalMapper;
	QSignalMapper * m_signalMapperDeletion;
	bool m_textChanged;
	void initTable();

private slots:
	void onEditionFinished( const QString & rowcolString );
	void onCellEdited( const QString & rowcolString );

signals:
	void ratioChanged( int row, int col, double value );
	void ratioRemoved( int row, int col );
};

#endif
