/**************************************************************************
*   Copyright © 2014 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "kreconversiontable.h"
#include "datablocks/mixednumber.h"
#include "widgets/fractioninput.h"

#include <KLineEdit>

#include <QSignalMapper>


KreConversionTable::KreConversionTable( QWidget* parent, int maxrows, int maxcols ) : QTableWidget( maxrows, maxcols, parent )
{
	m_textChanged = false;
	m_signalMapper = new QSignalMapper;
	m_signalMapperDeletion = new QSignalMapper;
	connect( m_signalMapper, SIGNAL(mapped(const QString&)),
		this, SLOT(onEditionFinished(const QString&)) );
	connect( m_signalMapperDeletion, SIGNAL(mapped(const QString&)),
		this, SLOT(onCellEdited(const QString&)) );
}

KreConversionTable::~KreConversionTable()
{
	delete m_signalMapper;
	delete m_signalMapperDeletion;
}

void KreConversionTable::onEditionFinished( const QString & rowcolString )
{
	QStringList splitString = rowcolString.split(" ");
	int row = splitString.first().toInt();
	int col = splitString.last().toInt();
	KLineEdit * editor = (KLineEdit*)this->cellWidget( row, col );
	if ( m_textChanged ) {
		double ratioValue = editor->text().toDouble();
		editor->setToolTip( QString("1 %1 = %2 %3")
					.arg( this->verticalHeaderItem(row)->text() )
					.arg( editor->text() )
					.arg( this->horizontalHeaderItem(col)->text() )
		);
		m_textChanged = false;
		emit ratioChanged( row, col, ratioValue );
	}

}

void KreConversionTable::onCellEdited( const QString & rowcolString )
{
	QStringList splitString = rowcolString.split(" ");
	int row = splitString.first().toInt();
	int col = splitString.last().toInt();
	KLineEdit * editor = (KLineEdit*)this->cellWidget( row, col );
	if ( editor->text().isEmpty() ) {
		editor->setToolTip("");
		emit ratioRemoved( row, col );
	} else {
		editor->setToolTip( QString("1 %1 = %2 %3")
					.arg( this->verticalHeaderItem(row)->text() )
					.arg( editor->text() )
					.arg( this->horizontalHeaderItem(col)->text() )
		);
		m_textChanged = true;
	}
}


void KreConversionTable::initTable()
{
	int r,c;
	int rowCount = this->rowCount();
	int colCount = this->columnCount();
	for ( r = 0; r < rowCount; r++ ) {
		for ( c = 0; c < colCount; c++ ) {
			if (r == c ) {
				// Diagonal items should not be editable; disable them and grey them out
				QTableWidgetItem * diagonalItem = new QTableWidgetItem;
				diagonalItem->setFlags( Qt::NoItemFlags ); 
				QPalette palette;
				diagonalItem->setBackground( palette.brush( QPalette::Disabled, QPalette::Button ) );
				this->setItem( r, c, diagonalItem );
			} else {
				// Non-diagonal items will be KLineEdit's only accepting doubles.
				KLineEdit * editor = new KLineEdit;
				editor->setValidator( new QDoubleValidator );
				this->setCellWidget( r, c, editor );
				m_signalMapper->setMapping( editor,
					QString::number(r) + " " + QString::number(c) );
				m_signalMapperDeletion->setMapping( editor,
					QString::number(r) + " " + QString::number(c) );
				connect( editor, SIGNAL(editingFinished()), m_signalMapper, SLOT(map()) );
				connect( editor, SIGNAL(textEdited(const QString&)), m_signalMapperDeletion, SLOT(map()) );
			}
		}
	}
}


void KreConversionTable::setUnitIDs( const IDList &idList )
{
	unitIDs = idList;
}


void KreConversionTable::setRatio( int ingID1, int ingID2, double ratio )
{
	int indexID1 = unitIDs.indexOf( ingID1 );
	int indexID2 = unitIDs.indexOf( ingID2 );

	KLineEdit * editor = (KLineEdit*)this->cellWidget( indexID1, indexID2 );

	QString text = QString::number(ratio, 'g', 6);
	editor->setText( QString::number(ratio, 'g', 6) );
	editor->setToolTip( QString("1 %1 = %2 %3")
				.arg( this->verticalHeaderItem(indexID1)->text() )
				.arg( text )
				.arg( this->horizontalHeaderItem(indexID2)->text() )
	);

}


void KreConversionTable::deleteRatio( int r, int c )
{
	KLineEdit * editor = (KLineEdit*)this->cellWidget( r, c );
	editor->setText("");
	editor->setToolTip("");
}


int KreConversionTable::getUnitID( int rc )
{
	return unitIDs.at( rc );
}


void KreConversionTable::resize( int r, int c )
{
	setRowCount( r );
	setColumnCount( c );
	initTable();
}

