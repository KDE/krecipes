/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef UNITCOMBOBOX_H
#define UNITCOMBOBOX_H

#include <kcombobox.h>

#include <qmap.h>

#include "datablocks/unit.h"

class RecipeDB;

class UnitComboBox : public KComboBox
{
	Q_OBJECT

public:
	UnitComboBox( QWidget *parent, RecipeDB *db, Unit::Type type = Unit::All );

	void reload();
	int id( int row ) const;
	void setSelected( int unitID );
	Unit unit() const;

protected:
	virtual void popup();

private slots:
	void createUnit( const Unit & );
	void removeUnit( int id );

	int findInsertionPoint( const QString &name );

private:
	void loadUnits( const UnitList &unitList );

	RecipeDB *database;
	QMap<int, int> unitComboRows; // Contains the unit id for every given row in the unit combobox
	Unit::Type m_type;
};

#endif //UNITCOMBOBOX_H

