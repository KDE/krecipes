/***************************************************************************
*   Copyright © 2003-2004 Unai Garro <ugarro@gmail.com>                   *
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "conversiontable.h"
#include "datablocks/mixednumber.h"
#include "widgets/fractioninput.h"

#include <QToolTip>

#include <kglobal.h>
#include <klocale.h>

class ConversionTableToolTip 
{
public:
	ConversionTableToolTip( ConversionTable *t ) : table( t )
	{}

	void maybeTip( const QPoint &pos )
	{
		if ( !table )
			return ;

		QPoint cp = table->viewportToContents( pos );

		int row = table->rowAt( cp.y() );
		int col = table->columnAt( cp.x() );

		if ( row == col )
			return ;

		QString row_unit = table->verticalHeader() ->label( row );
		QString col_unit = table->horizontalHeader() ->label( col );
		QString text = table->text( row, col );
		if ( text.isEmpty() )
			text = 'X'; //### Is this i18n friendly???

		QRect cr = table->cellGeometry( row, col );
		cr.moveTopLeft( table->contentsToViewport( cr.topLeft() ) );
		QToolTip::showText( pos, QString( "1 %1 = %2 %3" ).arg( row_unit ).arg( text ).arg( col_unit ), table->viewport(), cr );		
	}

private:
	ConversionTable *table;
};

ConversionTable::ConversionTable( QWidget* parent, int maxrows, int maxcols ) : Q3Table( maxrows, maxcols, parent, "table" )
{
	editBoxValue = -1;
	items.setAutoDelete( true );
	widgets.setAutoDelete( true );

	( void ) new ConversionTableToolTip( this );
}

ConversionTable::~ConversionTable()
{}
#include <kdebug.h>
void ConversionTable::unitRemoved( int id )
{
	int index = unitIDs.indexOf( id );
	kDebug() << "index:" << index ;
	removeRow( index );
	removeColumn( index );
	kDebug() << "done" ;
}

void ConversionTable::unitCreated( const Unit &unit )
{
	insertColumns( numCols() );
	insertRows( numRows() );
	unitIDs.append( unit.id() );
	horizontalHeader() ->setLabel( numRows() - 1, unit.name() );
	verticalHeader() ->setLabel( numCols() - 1, unit.name() );
}

Q3TableItem* ConversionTable::item( int r, int c ) const
{
	return items.find( indexOf( r, c ) );
}

void ConversionTable::setItem( int r, int c, Q3TableItem *i )
{
	items.replace( indexOf( r, c ), i );
	i->setRow( r ); // Otherwise the item
	i->setCol( c ); //doesn't know where it is!
	updateCell( r, c );
}

void ConversionTable::clearCell( int r, int c )
{
	items.remove( indexOf( r, c ) );
}

void ConversionTable::takeItem( Q3TableItem *item )
{
	items.setAutoDelete( false );
	items.remove( indexOf( item->row(), item->col() ) );
	items.setAutoDelete( true );
}

void ConversionTable::insertWidget( int r, int c, QWidget *w )
{
	widgets.replace( indexOf( r, c ), w );
}

QWidget* ConversionTable::cellWidget( int r, int c ) const
{
	return widgets.find( indexOf( r, c ) );
}

void ConversionTable::clearCellWidget( int r, int c )
{
	QWidget * w = widgets.take( indexOf( r, c ) );
	if ( w )
		w->deleteLater();
}


ConversionTableItem::ConversionTableItem( Q3Table *t, EditType et ) : Q3TableItem( t, et, QString() )
{
	// we do not want this item to be replaced
	setReplaceable( false );
}

void ConversionTableItem::paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected )
{
	QColorGroup g( cg );

	// Draw in gray all those cells which are not editable

	if ( row() == col() )
		g.setColor( QColorGroup::Base, Qt::gray );
	Q3TableItem::paint( p, g, cr, selected );
}

QWidget* ConversionTableItem::createEditor() const
{
	FractionInput *editor = new FractionInput( table()->viewport(), MixedNumber::DecimalFormat );

	MixedNumber current;
	MixedNumber::fromString( text(), current, true );
	if ( current.toDouble() > 1e-8 )
		editor->setValue( current, 0 );

	return editor;
}

void ConversionTableItem::setContentFromEditor( QWidget *w )
{
	// the user changed the value of the combobox, so synchronize the
	// value of the item (its text), with the value of the combobox
	if ( w->inherits( "FractionInput" ) ) {
		FractionInput* editor = ( FractionInput* ) w;
		if ( editor->isInputValid() && !editor->isEmpty() && editor->value().toDouble() > 1e-6 ) {
			setText( editor->value().toString(MixedNumber::DecimalFormat) );
			emit ratioChanged( row(), col(), editor->value().toDouble() ); // Signal to store
		}
		else {
			setText( QString() );
			emit ratioRemoved( row(), col() );
		}
	}
	else
		Q3TableItem::setContentFromEditor( w );
}

void ConversionTableItem::setText( const QString &s )
{
	Q3TableItem::setText( s );
}
QString ConversionTable::text( int r, int c ) const			 // without this function, the usual (text(r,c)) won't work
{
	if ( item( r, c ) )
		return item( r, c ) ->text();  //Note that item(r,c) was reimplemented here for large sparse tables...
	else
		return QString();
}

void ConversionTable::initTable()
{

	for ( int r = 0;r < numRows();r++ ) {
		this->createNewItem( r, r, 1.0 );
		item( r, r ) ->setEnabled( false ); // Diagonal is not editable
	}
}

void ConversionTable::createNewItem( int r, int c, double amount )
{

	ConversionTableItem * ci = new ConversionTableItem( this, Q3TableItem::WhenCurrent );
	ci->setText( beautify( KGlobal::locale() ->formatNumber( amount, 5 ) ) );
	setItem( r, c, ci );
	// connect signal (forward) to know when it's actually changed
	connect( ci, SIGNAL( ratioChanged( int, int, double ) ), this, SIGNAL( ratioChanged( int, int, double ) ) );
	connect( ci, SIGNAL( ratioRemoved( int, int ) ), this, SIGNAL( ratioRemoved( int, int ) ) );
	connect( ci, SIGNAL( signalRepaintCell( int, int ) ), this, SLOT( repaintCell( int, int ) ) );
}

void ConversionTable::setUnitIDs( const IDList &idList )
{
	unitIDs = idList;
}

void ConversionTable::setRatio( int ingID1, int ingID2, double ratio )
{
	int indexID1 = unitIDs.indexOf( ingID1 );
	int indexID2 = unitIDs.indexOf( ingID2 );

	createNewItem( indexID1, indexID2, ratio );
}


int ConversionTable::getUnitID( int rc )
{
	return unitIDs.at( rc );
}

QWidget * ConversionTable::beginEdit ( int row, int col, bool replace )
{
	// If there's no item, create it first.
	if ( !item( row, col ) ) {
		createNewItem( row, col, 0 );
	}

	// Then call normal beginEdit
	return Q3Table::beginEdit( row, col, replace );
}

void ConversionTableItem::setTextAndSave( const QString &s )
{
	setText( s ); // Change text
	emit signalRepaintCell( row(), col() ); // Indicate to update the cell to the table. Otherwise it's not repainted
	emit ratioChanged( row(), col(), s.toDouble() ); // Signal to store
}

void ConversionTable::repaintCell( int r, int c )
{
	Q3Table::updateCell( r, c );
}

void ConversionTable::resize( int r, int c )
{
	setNumRows( r );
	setNumCols( c );
	initTable();
}

void ConversionTable::clear( void )
{
	items.clear();
	widgets.clear();
	unitIDs.clear();
	resize( 0, 0 );

}

//TODO this is incomplete/wrong
void ConversionTable::swapRows( int row1, int row2, bool /*swapHeader*/ )
{
	//if ( swapHeader )
	//((QTableHeader*)verticalHeader())->swapSections( row1, row2, false );

	QVector<Q3TableItem*> tmpContents;
	tmpContents.resize( numCols() );
	QVector<QWidget*> tmpWidgets;
	tmpWidgets.resize( numCols() );
	int i;

	items.setAutoDelete( false );
	widgets.setAutoDelete( false );
	for ( i = 0; i < numCols(); ++i ) {
		Q3TableItem *i1, *i2;
		i1 = item( row1, i );
		i2 = item( row2, i );
		if ( i1 || i2 ) {
			tmpContents.insert( i, i1 );
			items.remove( indexOf( row1, i ) );
			items.insert( indexOf( row1, i ), i2 );
			items.remove( indexOf( row2, i ) );
			items.insert( indexOf( row2, i ), tmpContents[ i ] );
			if ( items[ indexOf( row1, i ) ] )
				items[ indexOf( row1, i ) ] ->setRow( row1 );
			if ( items[ indexOf( row2, i ) ] )
				items[ indexOf( row2, i ) ] ->setRow( row2 );
		}

		QWidget *w1, *w2;
		w1 = cellWidget( row1, i );
		w2 = cellWidget( row2, i );
		if ( w1 || w2 ) {
			tmpWidgets.insert( i, w1 );
			widgets.remove( indexOf( row1, i ) );
			widgets.insert( indexOf( row1, i ), w2 );
			widgets.remove( indexOf( row2, i ) );
			widgets.insert( indexOf( row2, i ), tmpWidgets[ i ] );
		}
	}
	items.setAutoDelete( false );
	widgets.setAutoDelete( true );

	//updateRowWidgets( row1 );
	//updateRowWidgets( row2 );
	/*
	if ( curRow == row1 )
	curRow = row2;
	else if ( curRow == row2 )
	curRow = row1;
	if ( editRow == row1 )
	editRow = row2;
	else if ( editRow == row2 )
	editRow = row1;*/
}

//TODO this is incomplete/wrong
void ConversionTable::swapColumns( int col1, int col2, bool /*swapHeader*/ )
{
	//if ( swapHeader )
	//((QTableHeader*)horizontalHeader())->swapSections( col1, col2, false );

	QVector<Q3TableItem*> tmpContents;
	tmpContents.resize( numRows() );
	QVector<QWidget*> tmpWidgets;
	tmpWidgets.resize( numRows() );
	int i;

	items.setAutoDelete( false );
	widgets.setAutoDelete( false );
	for ( i = 0; i < numRows(); ++i ) {
		Q3TableItem *i1, *i2;
		i1 = item( i, col1 );
		i2 = item( i, col2 );
		if ( i1 || i2 ) {
			tmpContents.insert( i, i1 );
			items.remove( indexOf( i, col1 ) );
			items.insert( indexOf( i, col1 ), i2 );
			items.remove( indexOf( i, col2 ) );
			items.insert( indexOf( i, col2 ), tmpContents[ i ] );
			if ( items[ indexOf( i, col1 ) ] )
				items[ indexOf( i, col1 ) ] ->setCol( col1 );
			if ( items[ indexOf( i, col2 ) ] )
				items[ indexOf( i, col2 ) ] ->setCol( col2 );
		}

		QWidget *w1, *w2;
		w1 = cellWidget( i, col1 );
		w2 = cellWidget( i, col2 );
		if ( w1 || w2 ) {
			tmpWidgets.insert( i, w1 );
			widgets.remove( indexOf( i, col1 ) );
			widgets.insert( indexOf( i, col1 ), w2 );
			widgets.remove( indexOf( i, col2 ) );
			widgets.insert( indexOf( i, col2 ), tmpWidgets[ i ] );
		}
	}
	items.setAutoDelete( false );
	widgets.setAutoDelete( true );

	columnWidthChanged( col1 );
	columnWidthChanged( col2 );
	/*
	if ( curCol == col1 )
	curCol = col2;
	else if ( curCol == col2 )
	curCol = col1;
	if ( editCol == col1 )
	editCol = col2;
	else if ( editCol == col2 )
	editCol = col1;*/
}

//TODO this is incomplete/wrong
void ConversionTable::swapCells( int row1, int col1, int row2, int col2 )
{
	items.setAutoDelete( false );
	widgets.setAutoDelete( false );
	Q3TableItem *i1, *i2;
	i1 = item( row1, col1 );
	i2 = item( row2, col2 );
	if ( i1 || i2 ) {
		Q3TableItem * tmp = i1;
		items.remove( indexOf( row1, col1 ) );
		items.insert( indexOf( row1, col1 ), i2 );
		items.remove( indexOf( row2, col2 ) );
		items.insert( indexOf( row2, col2 ), tmp );
		if ( items[ indexOf( row1, col1 ) ] ) {
			items[ indexOf( row1, col1 ) ] ->setRow( row1 );
			items[ indexOf( row1, col1 ) ] ->setCol( col1 );
		}
		if ( items[ indexOf( row2, col2 ) ] ) {
			items[ indexOf( row2, col2 ) ] ->setRow( row2 );
			items[ indexOf( row2, col2 ) ] ->setCol( col2 );
		}
	}

	QWidget *w1, *w2;
	w1 = cellWidget( row1, col1 );
	w2 = cellWidget( row2, col2 );
	if ( w1 || w2 ) {
		QWidget * tmp = w1;
		widgets.remove( indexOf( row1, col1 ) );
		widgets.insert( indexOf( row1, col1 ), w2 );
		widgets.remove( indexOf( row2, col2 ) );
		widgets.insert( indexOf( row2, col2 ), tmp );
	}

	//updateRowWidgets( row1 );
	//updateRowWidgets( row2 );
	//updateColWidgets( col1 );
	//updateColWidgets( col2 );
	items.setAutoDelete( false );
	widgets.setAutoDelete( true );
}

#include "conversiontable.moc"
