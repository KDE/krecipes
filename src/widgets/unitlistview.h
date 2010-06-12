/***************************************************************************
*   Copyright © 2004 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2004 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2004 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNITLISTVIEW_H
#define UNITLISTVIEW_H

#include <KLocale>

#include "dblistviewbase.h"

#include "datablocks/unit.h"

class KComboBox;

class RecipeDB;
class KMenu;

class UnitListView : public DBListViewBase
{
	Q_OBJECT

public:
	UnitListView( QWidget *parent, RecipeDB *db );

public slots:
	virtual void load( int curr_limit, int curr_offset );

protected slots:
	virtual void createUnit( const Unit & ) = 0;
	virtual void removeUnit( int ) = 0;

	void checkCreateUnit( const Unit &el );
};

class StdUnitListView : public UnitListView
{
	Q_OBJECT

public:
	StdUnitListView( QWidget *parent, RecipeDB *db, bool editable = false );

	void insertTypeComboBox( Q3ListViewItem* );

protected:
	virtual void createUnit( const Unit & );
	virtual void removeUnit( int );

private slots:
	void hideTypeCombo();
	void updateType( int type );

private:
	KComboBox *typeComboBox;
};

class UnitListViewItem : public Q3ListViewItem
{
public:
	UnitListViewItem( Q3ListView* qlv, const Unit &u ) : Q3ListViewItem( qlv ), m_unit(u)
	{
		updateType(m_unit.type());
	}

	virtual QString text( int column ) const
	{
		switch ( column ) {
		case 0: return m_unit.name();
		case 1: return m_unit.nameAbbrev();
		case 2: return m_unit.plural();
		case 3: return m_unit.pluralAbbrev();
		case 4: return m_type;
		case 5: return QString::number(m_unit.id());
		default: return QString();
		}
	}

	void setType( Unit::Type type ){ m_unit.setType(type); updateType(type); }
	Unit::Type type(){ return m_unit.type(); }

	Unit unit() const { return m_unit; };
	void setUnit( const Unit &u ) { m_unit = u; }

protected:
	virtual void setText( int column, const QString &text ) {
		switch ( column ) {
		case 0: m_unit.setName(text); break;
		case 1: m_unit.setNameAbbrev(text); break;
		case 2: m_unit.setPlural(text); break;
		case 3: m_unit.setPluralAbbrev(text); break;
		}
	}

private:
	void updateType( Unit::Type t ) {
		switch ( t ) {
		case Unit::Other: m_type = i18nc("Unit type other", "Other"); break;
		case Unit::Mass: m_type = i18nc("Unit type mass", "Mass"); break;
		case Unit::Volume: m_type = i18nc("Unit type volume", "Volume"); break;
		default: break;
		}
	}

	Unit m_unit;
	QString m_type;
};

#endif //UNITLISTVIEW_H
