/*
Copyright © 2003 Richard Lärkäng
Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public 
License along with this library.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef MX2IMPORTER_H
#define MX2IMPORTER_H

#include "baseimporter.h"

#include <qdom.h>

class Recipe;

/** Class to import Mastercook's MX2 file format.  This is an XML-based file
  * format used since version 5 of Mastercook.
  * @author Jason Kivlighn
  */
class MX2Importer : public BaseImporter
{
public:
	MX2Importer();
	virtual ~MX2Importer();

protected:
	void parseFile( const QString& filename );

private:
	void readRecipe( const QDomNodeList& l, Recipe* );
} ;

#endif //MX2IMPORTER_H
