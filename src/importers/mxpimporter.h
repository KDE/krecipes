/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MXPIMPORTER_H
#define MXPIMPORTER_H

#include <qstring.h>

#include "baseimporter.h"
#include "ingredientlist.h"
#include "elementlist.h"

/** Class to import MasterCook's MXP (MasterCook Export) file format.
  * @author Jason Kivlighn
  */
class MXPImporter : public BaseImporter
{
public:
	MXPImporter( const QString &file );
	~MXPImporter();

private:
	void importMXP( QTextStream &stream );
	void importMac( QTextStream &stream );
	void importGeneric( QTextStream &stream );

	QString getNextQuotedString( QTextStream &stream );

	void putDataInRecipe();

	int m_servings;

	QString m_description;
	QString m_instructions;
	QString m_internet;
	QString m_notes;
	QString m_prep_time;
	QString m_serving_ideas;
	QString m_source;
	QString m_title;
	QString m_wine;

	ElementList m_authors;
	ElementList m_categories;
	IngredientList m_ingredients;
};

#endif //MXPIMPORTER_H
