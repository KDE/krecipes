/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
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

class PrepMethodComboBox : public KComboBox
{
	Q_OBJECT

public:
	PrepMethodComboBox( bool, QWidget *parent, RecipeDB *db, const QString &specialItem = QString() );

	void reload();
	int id( int row );
	int id( const QString &ing );
	void setSelected( int prepID );

private slots:
	void createPrepMethod( const Element &element );
	void removePrepMethod( int id );

	int findInsertionPoint( const QString &name );

private:
	RecipeDB *database;
	QMap<int, int> prepMethodComboRows; // Contains the prep method id for every given row in the combobox
	QString m_specialItem;
};

#endif //PREPMETHODCOMBOBOX_H

