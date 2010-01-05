/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef HEADERCOMBOBOX_H
#define HEADERCOMBOBOX_H

#include <kcombobox.h>

class RecipeDB;
class Element;

class HeaderComboBox : public KComboBox
{
	Q_OBJECT

public:
	HeaderComboBox( bool, QWidget *parent, RecipeDB *db );

	void reload();

private slots:
	void createHeader( const Element & element);
	void removeHeader( int id );

private:	
	int findInsertionPoint( const QString &name );

	RecipeDB *database;
};

#endif //HEADERCOMBOBOX_H

