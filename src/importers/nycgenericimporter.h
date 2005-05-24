/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef NYCGENERICIMPORTER_H
#define NYCGENERICIMPORTER_H

#include <qstring.h>
#include <qdatetime.h>
//Added by qt3to4:
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

	void resetVars();
	void putDataInRecipe();

	void loadIngredientLine( const QString & );

	int m_servings;

	QString m_instructions;
	QString m_title;
	QString current_header;

	ElementList m_authors;
	ElementList m_categories;
	IngredientList m_ingredients;

	QTime m_preptime;

	bool first;
};

#endif //NYCGENERICIMPORTER_H
