/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
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

class IngredientPropertyList;
class RecipeDB;
class KProgress;

/**
  * Exports a given recipe list as HTML
  * @author Jason Kivlighn
  */
class HTMLExporter : public BaseExporter
{
public:
	HTMLExporter( RecipeDB *, const QString&, const QString& );
	virtual ~HTMLExporter();

	virtual int supportedItems() const;

	static void removeHTMLFiles( const QString &filename, const QString &recipe_title );
	static void removeHTMLFiles( const QString &filename, const QStringList &recipe_titles );

protected:
	QString createContent( const Recipe& recipe );
	virtual QString createContent( const RecipeList & );
	virtual QString createHeader( const RecipeList & );
	virtual QString createFooter();

	virtual int progressInterval() const { return 1; }

	static QString escape( const QString & );

	QString m_templateContent;

private:
	void storePhoto( const Recipe &recipe, const QDomDocument &doc );
	void populateTemplate( const Recipe &recipe, const QDomDocument &doc, QString &content );
	void replaceIfVisible( QString &content, const QString &name, const QString &html );

	QString HTMLIfVisible( const QString &name, const QString &html );

	QString readAlignmentProperties( const QDomDocument &doc, const QString &object );
	QString readBorderProperties( const QDomDocument &doc, const QString &object );
	QString readBgColorProperties( const QDomDocument &doc, const QString &object );
	QString readFontProperties( const QDomDocument &doc, const QString &object );
	QString readTextColorProperties( const QDomDocument &doc, const QString &object );
	QString readVisibilityProperties( const QDomDocument &doc, const QString &object );

	QString generateCSSClasses( const QDomDocument &layout );
	QDomElement getLayoutAttribute( const QDomDocument &, const QString &object, const QString &attribute );

	IngredientPropertyList *properties;

	RecipeDB *database;

	QString layout_filename;

	bool m_error;
	QDomDocument doc;
};

#endif //HTMLEXPORTER_H
