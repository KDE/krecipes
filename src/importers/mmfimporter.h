/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MMFIMPORTER_H
#define MMFIMPORTER_H

#include <qstring.h>

#include "baseimporter.h"
#include "ingredientlist.h"
#include "elementlist.h"

struct expand_unit_info
{
   const char *short_form;
   const char *expanded_form;
};

/** Class to import Meal-Master's MMF (Meal-Master Format) file format.
  * @author Jason Kivlighn
  */
class MMFImporter : public BaseImporter
{
public:
	MMFImporter( const QString &file );
	~MMFImporter();

private:
	enum FormatVersion { FromDatabase, VersionMMMMM, VersionBB, VersionNormal };

	void importMMF( QTextStream &stream );

	/** Parses the line and save it if the line is a valid ingredient and return true.
	  * Returns false if not an ingredient.
	  */
	bool loadIngredientLine( const QString &, IngredientList & );

	/** Parses the line and save it if the line is a valid ingredient header and return true.
	  * Returns false if not an ingredient header.
	  */
	bool loadIngredientHeader( const QString & );

	void resetVars();
	void putDataInRecipe();

	int m_servings;

	QString m_instructions;
	QString m_title;

	ElementList m_authors;
	ElementList m_categories;
	IngredientList m_left_col_ing;
	IngredientList m_right_col_ing;
	IngredientList m_all_ing;

	FormatVersion version;
};

#endif //MMFIMPORTER_H
