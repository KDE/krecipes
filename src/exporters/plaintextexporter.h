/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PLAINTEXTEXPORTER_H
#define PLAINTEXTEXPORTER_H

#include "baseexporter.h"

/**
  * Export class to export recipes into text format.
  * Recipes exported with this class are not meant to be imported back
  * into Krecipes or any other program.
  *
  * @author Jason Kivlighn
  */
class PlainTextExporter : public BaseExporter
{
public:
	PlainTextExporter( const QString&, const QString& );
	virtual ~PlainTextExporter();

	virtual int supportedItems() const;

protected:
	virtual QString createContent( const RecipeList & );
};

#endif //PLAINTEXTEXPORTER_H
