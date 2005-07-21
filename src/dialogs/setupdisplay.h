/***************************************************************************
*   Copyright (C) 2003 by Jason Kivlighn                                  *
*   mizunoami44@users.sourceforge.net                                     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef SETUPDISPLAY_H
#define SETUPDISPLAY_H

#include <qdom.h>
#include <qwidget.h>
#include <qmap.h>

#include "datablocks/recipe.h"
#include "widgets/dragarea.h"
#include "datablocks/kreborder.h"
#include "krepagelayout.h"
#include "kreunit.h"

#include <math.h>

class KPopupMenu;

class QAction;
class QLabel;
class QWidget;

class DragArea;
class KoRuler;

class KreDisplayItem
{
public:
	enum OverflowType { ShrinkToFit = 0, Grow };

	KreDisplayItem( QWidget *w = 0 ) : widget( w ), overflow( Grow )
	{}

	//bool operator<(const KreDisplayItem & ) const { return true; } //required to be a map key

	QWidget *widget;
	KreBorder border;
	OverflowType overflow;
};

typedef QMap< KreDisplayItem*, unsigned int > PropertiesMap;

// ### maybe we should move koffice's KoRect/KoPoint/KoSize to kdelibs...
class PreciseRect
{
public:
	PreciseRect( double top = 0, double left = 0, double width = 0, double height = 0 ) :
			m_top( top ), m_left( left ), m_width( width ), m_height( height )
	{}

	PreciseRect( const QRect &r )
	{
		m_top = r.top();
		m_left = r.left();
		m_width = r.width();
		m_height = r.height();
	}

	QRect toQRect() const
	{
		return QRect( qRound( m_top ), qRound( m_left ), qRound( m_width ), qRound( m_height ) );
	}

	double top()
	{
		return m_top;
	}
	double left()
	{
		return m_left;
	}
	double width()
	{
		return m_width;
	}
	double height()
	{
		return m_height;
	}

	void setTop( double d )
	{
		m_top = d;
	}
	void setLeft( double d )
	{
		m_left = d;
	}
	void setWidth( double d )
	{
		m_width = d;
	}
	void setHeight( double d )
	{
		m_height = d;
	}

private:
	double m_top, m_left, m_width, m_height;
};

/** @brief A very specialized @ref DragArea for editing the recipe setup
  *
  * Set up the items of a recipe for display.
  *
  * @author Jason Kivlighn
  */
class SetupDisplay : public QWidget
{
	Q_OBJECT

public:
	SetupDisplay( const Recipe &, bool show_ruler, QWidget *parent );
	~SetupDisplay();

	enum Properties { None = 0, BackgroundColor = 1, TextColor = 2, Font = 4, Visibility = 8, Geometry = 16, Alignment = 32, StaticHeight = 64, Border = 128, Overflow = 256 };

	void saveLayout( const QString & );
	void loadLayout( const QString & );
	virtual QSize sizeHint( void ) const;
	QSize minimumSize() const;
	bool hasChanges() const
	{
		return has_changes;
	}

	void setItemShown( QWidget *item, bool visible );

	const PropertiesMap properties() const
	{
		return * box_properties;
	}

signals:
	void itemVisibilityChanged( QWidget *, bool );

protected slots:
	void widgetClicked( QMouseEvent *, QWidget * );
	void changeMade();

	//slots to set properties of item boxes
	void setBackgroundColor();
	void setBorder();
	void setTextColor();
	void setFont();
	void setOverflow( QAction * );
	void setShown( int id );
	void setAlignment( QAction * );

	void openPageLayoutDia();
	void updatePageLayout( const KoPageLayout &page_layout, bool set_change=true );

private:
	DragArea *drag_area;

	KoRuler *hruler;
	KoRuler *vruler;

	QLabel *title_box;
	QLabel *instr_box;
	QLabel *photo_box;
	QLabel *servings_box;
	QLabel *categories_box;
	QLabel *authors_box;
	QLabel *id_box;
	QLabel *ingredients_box;
	QLabel *properties_box;
	QLabel *preptime_box;

	PropertiesMap *box_properties;
	QMap<QWidget*, KreDisplayItem*> *widget_item_map;

	bool has_changes;
	KPopupMenu *popup;

	KoPageLayout page_layout;
	KoUnit::Unit unit;

	// Methods
	void createWidgets( const Recipe &sample, DragArea *canvas );
	void toAbsolute( PreciseRect * );
	void toPercentage( PreciseRect * );

	void loadPageLayout( const QDomElement &tag );

	void loadFont( KreDisplayItem *, const QDomElement &tag );
	void loadOverflow( KreDisplayItem *item, const QDomElement &tag );
	void loadGeometry( KreDisplayItem *, const QDomElement &tag );
	void loadBackgroundColor( KreDisplayItem *, const QDomElement &tag );
	void loadTextColor( KreDisplayItem *, const QDomElement &tag );
	void loadVisibility( KreDisplayItem *, const QDomElement &tag );
	void loadAlignment( KreDisplayItem *, const QDomElement &tag );
	void loadBorder( KreDisplayItem *, const QDomElement &tag );

	void createItem( QWidget *w, unsigned int properties );
};

#endif //SETUPDISPLAY_H

