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

#include "baseexporter.h"

class CustomRectList;
class IngredientPropertyList;
class RecipeDB;
class KConfig;

class DivElement
{
public:
	DivElement( const QString &id, const QString &content );

	void addProperty( const QString &s ){ m_properties << s; }

	QString innerHTML() const{ return m_content; }
	QString id() const{return m_id;}
	QFont font();

	bool fixedHeight(){ return m_fixed_height; }
	void setFixedHeight( bool b ){ m_fixed_height = b; }

	QString generateHTML();
	QString generateCSS(bool noPositioning=false);

private:
	QString m_id;
	QString m_content;
	QStringList m_properties;

	bool m_fixed_height;
};

/**
  * Exports a given recipe list as HTML
  * @author Jason Kivlighn
  */
class HTMLExporter : public BaseExporter
{
public:
	HTMLExporter( RecipeDB *, const QString&, const QString, int width );
	virtual ~HTMLExporter();

	virtual QString createContent( const RecipeList & );

	static void removeHTMLFiles( const QString &filename, const QString &recipe_title );

protected:
	virtual QString extensions() const{ return ".html"; }
private:
	void storePhoto( const Recipe &recipe );
	int createBlocks( const Recipe &recipe, int offset = 0 );
	void pushItemsDownIfNecessary( QPtrList<QRect> &, QRect *top_geom );

	void readAlignmentProperties( DivElement *, KConfig * );
	void readBgColorProperties( DivElement *, KConfig * );
	void readFontProperties( DivElement *, KConfig * );
	void readTextColorProperties( DivElement *, KConfig * );
	void readVisibilityProperties( DivElement *, KConfig * );

	QPtrList<DivElement> div_elements;
	QPtrList<QRect> dimensions;
	IngredientPropertyList *properties;
	QRect temp_photo_geometry;

	RecipeDB *database;

	int m_width;
};

class CustomRectList : public QPtrList<QRect>
{
public:
	CustomRectList();

protected:
	int compareItems( QPtrCollection::Item, QPtrCollection::Item );
};

#endif //HTMLEXPORTER_H
