/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef NYCGENERICIMPORTER_H
#define NYCGENERICIMPORTER_H

#include <qstring.h>

#include "baseimporter.h"
#include "ingredientlist.h"
#include "elementlist.h"

/** Class to import The NYC (Now You're Cooking) Generic Export file format.
  * @author Jason Kivlighn
  */
class NYCGenericImporter : public BaseImporter
{
public:
	NYCGenericImporter( const QString &file );
	~NYCGenericImporter();

private:
	void importNYCGeneric( QTextStream &stream );

	void resetVars();
	void putDataInRecipe();

	void loadIngredientLine( const QString & );

	int m_servings;

	QString m_instructions;
	QString m_title;

	ElementList m_authors;
	ElementList m_categories;
	IngredientList m_ingredients;

	bool first;
};

#endif //NYCGENERICIMPORTER_H
