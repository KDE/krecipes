/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PREPMETHODCOMBOBOX_H
#define PREPMETHODCOMBOBOX_H

#include <kcombobox.h>

#include <qmap.h>

#include "datablocks/element.h"

class RecipeDB;
class ElementList;

class PrepMethodComboBox : public KComboBox
{
	Q_OBJECT

public:
	PrepMethodComboBox( bool, QWidget *parent, RecipeDB *db );

	void reload();
	int id( int row );
	int id( const QString &ing );

private slots:
	void createPrepMethod( const Element &element );
	void removePrepMethod( int id );

	int findInsertionPoint( const QString &name );

private:
	RecipeDB *database;
	QMap<int, int> prepMethodComboRows; // Contains the prep method id for every given row in the combobox
};

#endif //PREPMETHODCOMBOBOX_H

