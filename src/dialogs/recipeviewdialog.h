/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *   Cyril Bosselut (bosselut@b1project.com)                               *
 *   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RECIPEVIEWDIALOG_H
#define RECIPEVIEWDIALOG_H

#include <qvbox.h>
#include <qrect.h>
#include <khtmlview.h>
#include <khtml_part.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qfont.h>

class RecipeDB;
class Recipe;
class IngredientPropertyList;
class CustomRectList;
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
@author Unai Garro
*/

class RecipeViewDialog : public QVBox
{
Q_OBJECT

public:
    RecipeViewDialog(QWidget *parent, RecipeDB *db, int recipeID=-1);

    ~RecipeViewDialog();
    void loadRecipe(int recipeID);

private:

  // Internal Variables
  KHTMLPart *recipeView;
  QPtrList<DivElement> div_elements;
  QPtrList<QRect> dimensions;
  RecipeDB  *database;
  Recipe *loadedRecipe;
  IngredientPropertyList *properties;

  QRect temp_photo_geometry;

  // Internal Methods
  void showRecipe(void);
  void createBlocks();
  void pushItemsDownIfNecessary( QPtrList<QRect> &, QRect *top_geom );

  void readAlignmentProperties( DivElement *, KConfig * );
  void readBgColorProperties( DivElement *, KConfig * );
  void readFontProperties( DivElement *, KConfig * );
  void readTextColorProperties( DivElement *, KConfig * );
  void readVisibilityProperties( DivElement *, KConfig * );

public slots:
	void print(void);

};

class CustomRectList : public QPtrList<QRect>
{
public:
	CustomRectList();

protected:
	int compareItems( QPtrCollection::Item, QPtrCollection::Item );
};

#endif
