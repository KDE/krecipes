/***************************************************************************
*   Copyright © 2003-2006 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef HTMLEXPORTER_H
#define HTMLEXPORTER_H

#include <qdom.h>

#include "baseexporter.h"
#include "klomanager.h"


/**
  * Exports a given recipe list as HTML
  * @author Jason Kivlighn
  */
class HTMLExporter : public BaseExporter, protected KLOManager
{
public:
	HTMLExporter( const QString&, const QString& );
	virtual ~HTMLExporter();

	virtual int supportedItems() const;

	static void removeHTMLFiles( const QString &filename, int recipe_id );
	static void removeHTMLFiles( const QString &filename, const QList<int> &recipe_ids );

protected:
	QString createContent( const Recipe& recipe );
	virtual QString createContent( const RecipeList & );
	virtual QString createHeader( const RecipeList & );
	virtual QString createFooter();

	virtual int progressInterval() const { return 1; }

	static QString escape( const QString & );

	QString m_templateContent;

private:
	void storePhoto( const Recipe &recipe );
	void populateTemplate( const Recipe &recipe, QString &content );

	bool m_error;
};

#endif //HTMLEXPORTER_H
