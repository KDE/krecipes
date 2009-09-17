/***************************************************************************
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef REZKONVIMPORTER_H
#define REZKONVIMPORTER_H

#include <QStringList>

#include "baseimporter.h"
#include "datablocks/recipe.h"

class RezkonvImporter : public BaseImporter
{
public:
	RezkonvImporter();
	~RezkonvImporter();

protected:
	void parseFile( const QString &filename );

private:
	void loadIngredient( const QString &line, Recipe & recipe, bool &is_sub );
	void loadIngredientHeader( const QString &line, Recipe & recipe );
	void loadInstructions( QStringList::const_iterator &raw_text, Recipe & recipe );
	void loadReferences( QStringList::const_iterator &raw_text, Recipe & recipe );

	void readRange( const QString &, double &amount, double &amount_offset );
	void readRecipe( const QStringList &raw_text );

	QStringList::const_iterator m_end_it;

	QString current_header;
};

#endif //REZKONVIMPORTER_H
