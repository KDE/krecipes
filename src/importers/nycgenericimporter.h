/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef NYCGENERICIMPORTER_H
#define NYCGENERICIMPORTER_H

#include <QString>
#include <QTextStream>

#include "baseimporter.h"
#include "datablocks/ingredientlist.h"
#include "datablocks/elementlist.h"

/** Class to import The NYC (Now You're Cooking) Generic Export file format.
  * @author Jason Kivlighn
  */
class NYCGenericImporter : public BaseImporter
{
public:
	NYCGenericImporter();
	~NYCGenericImporter();

protected:
	void parseFile( const QString& filename );

private:
	void importNYCGeneric( QTextStream &stream );
	void putDataInRecipe();
	void loadIngredientLine( const QString & );

	Recipe m_recipe;
	QString current_header;

	bool first;
};

#endif //NYCGENERICIMPORTER_H
