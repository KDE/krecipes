/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "inglistviewitem.h"

#include <klocale.h>
#include <kconfiggroup.h>
#include <kglobal.h>
#include <kdebug.h>

#include "datablocks/unit.h"
#include "datablocks/mixednumber.h"

IngSubListViewItem::IngSubListViewItem( Q3ListViewItem* qli, const Ingredient &i ) : IngListViewItem( qli, 0, i )
{
}

QString IngSubListViewItem::text( int column ) const
{
	if ( column == 0 ) {
		//kDebug()<<"displaying col 0 for "<<m_ing.name;
		return QString("%1 ").arg(i18n("OR"))+m_ing.name;
		//return m_ing.name;
	}
	else
		return IngListViewItem::text(column);

}

void IngSubListViewItem::setText( int column, const QString &text )
{
	switch ( column ) {
	case 0: {
		QString compare = QString("%1 ").arg(i18n("OR"));
		if ( text.left(compare.length()) == compare )
			m_ing.name = text.right(text.length()-compare.length());
		else
			m_ing.name = text;
		break;
	}
	default:
		IngListViewItem::setText(column,text);
		break;
	}
}

int IngSubListViewItem::rtti() const
{
	return INGSUBLISTVIEWITEM_RTTI;
}


IngListViewItem::IngListViewItem( Q3ListView* qlv, const Ingredient &i ) : Q3ListViewItem( qlv )
{
	init( i );
}

IngListViewItem::IngListViewItem( Q3ListView* qlv, Q3ListViewItem *after, const Ingredient &i ) : Q3ListViewItem( qlv, after )
{
	init( i );
}

IngListViewItem::IngListViewItem( Q3ListViewItem* qli, Q3ListViewItem *after, const Ingredient &i ) : Q3ListViewItem( qli, after )
{
	init( i );
}

int IngListViewItem::rtti() const
{
	return INGLISTVIEWITEM_RTTI;
}

Ingredient IngListViewItem::ingredient() const
{
	return m_ing;
}

void IngListViewItem::setGroup( int groupID )
{
	m_ing.groupID = groupID;
}

void IngListViewItem::setAmount( double amount, double amount_offset )
{
	amount_str.clear();

	if ( amount+amount_offset > 0 ) {
		KConfigGroup config = KGlobal::config()->group( "Formatting" );

		KLocale * locale = KGlobal::locale();
		amount_str = beautify( locale->formatNumber( amount, 6 ) );
		if ( amount_offset > 0 )
			amount_str += '-' + beautify( locale->formatNumber( amount+amount_offset, 6 ) );
	}

	m_ing.amount = amount;
	m_ing.amount_offset = amount_offset;

	//FIXME: make sure the right unit is showing after changing this (force a repaint... repaint() doesn't do the job right because it gets caught in a loop)
}

void IngListViewItem::setUnit( const Unit &unit )
{
	//### This shouldn't be necessary... the db backend should ensure this doesn't happen
	if ( !unit.name().isEmpty() )
		m_ing.units.setName(unit.name());
	if ( !unit.plural().isEmpty() )
		m_ing.units.setPlural(unit.plural());
}

void IngListViewItem::setPrepMethod( const QString &prepMethod )
{
	m_ing.prepMethodList = ElementList::split(",",prepMethod);
}

void IngListViewItem::setText( int column, const QString &text )
{
	switch ( column ) {
	case 0: {
		m_ing.name = text;
		break;
	}
	case 1: {
		bool ok;
		Ingredient i; i.setAmount(text, &ok);
		if (ok && !text.trimmed().isEmpty())
			setAmount( i.amount, i.amount_offset );
		break;
	}
	case 2:
		setUnit( Unit( text, m_ing.amount+m_ing.amount_offset ) );
		break;
	case 3:
		setPrepMethod( text );
		break;
	default:
		break;
	}
}

QString IngListViewItem::text( int column ) const
{
	switch ( column ) {
	case 0:
		return m_ing.name;
		break;
	case 1:
		return amount_str;
		break;
	case 2:
		return m_ing.units.determineName(m_ing.amount+m_ing.amount_offset, /*useAbbrev=*/ false );
		break;
	case 3:
		return m_ing.prepMethodList.join(",");
		break;
	default:
		return ( QString() );
	}
}

void IngListViewItem::init( const Ingredient &i )
{
	m_ing = i;

	setAmount( i.amount, i.amount_offset );
}


IngGrpListViewItem::IngGrpListViewItem( Q3ListView* qlv, Q3ListViewItem *after, const QString &group, int id ) : Q3ListViewItem( qlv, after )
{
	init( group, id );
}

int IngGrpListViewItem::rtti() const
{
	return INGGRPLISTVIEWITEM_RTTI;
}

QString IngGrpListViewItem::group() const
{
	return m_group;
}

int IngGrpListViewItem::id() const
{
	return m_id;
}

QString IngGrpListViewItem::text( int column ) const
{
	switch ( column ) {
	case 0:
		return m_group + ':';
		break;
	default:
		return ( QString() );
	}
}

void IngGrpListViewItem::setText( int column, const QString &text )
{
	switch ( column ) {
	case 0:
		if ( text.right(1) == ":" )
			m_group = text.left(text.length()-1);
		else
			m_group = text;
		break;
	default:
		break;
	}
}

void IngGrpListViewItem::init( const QString &group, int id )
{
	m_group = group;
	m_id = id;
}

