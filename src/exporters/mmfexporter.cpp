/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mmfexporter.h"

#include <kdebug.h>
#include <klocale.h>

MMFExporter::MMFExporter( RecipeDB *db, const QString& filename, const QString format ) :
  BaseExporter( db, filename, format )
{
	possible_formats << ".mmf";
}


MMFExporter::~MMFExporter()
{
}

QString MMFExporter::createContent( const QPtrList<Recipe>& recipes )
{
	QString content;



	return content;
}
