/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MMFEXPORTER_H
#define MMFEXPORTER_H

#include "baseexporter.h"

class RecipeDB;

/**
  * Export class for the Meal-Master file format
  * @author Jason Kivlighn
  */
class MMFExporter : public BaseExporter
{
public:
	MMFExporter( RecipeDB *, const QString&, const QString );
	virtual ~MMFExporter();

	virtual QString createContent( const QPtrList<Recipe> & );
};

#endif //MMFEXPORTER_H
