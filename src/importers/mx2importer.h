/*
Copyright (C) 2003 Richard Lärkäng and Jason Kivlighn

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MX2IMPORTER_H
#define MX2IMPORTER_H

#include "baseimporter.h"

#include <qdom.h>

class Recipe;

class MX2Importer : public BaseImporter
{
public:
	MX2Importer();
	virtual ~MX2Importer();

protected:
	void parseFile(const QString& filename);

private:
	void readRecipe(const QDomNodeList& l, Recipe*);
} ;

#endif //MX2IMPORTER_H
