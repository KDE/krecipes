/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef KLOMANAGER_H
#define KLOMANAGER_H

#include <qdom.h>
#include <qfont.h>
#include <QSize>

#include "datablocks/kreborder.h"

class KLOManager
{
public:
	KLOManager();
	virtual ~KLOManager();

	static QStringList classes();

protected:
	void processDocument( const QDomDocument & );

	virtual void loadBackgroundColor( const QString &/*obj*/, const QColor& ){};
	virtual void loadFont( const QString &/*obj*/, const QFont& ){};
	virtual void loadTextColor( const QString &/*obj*/, const QColor& ){};
	virtual void loadVisibility( const QString &/*obj*/, bool ){};
	virtual void loadAlignment( const QString &/*obj*/, int ){};
	virtual void loadBorder( const QString &/*obj*/, const KreBorder& ){};
	virtual void loadColumns( const QString &/*obj*/, int ){};
	virtual void loadSize( const QString &/*obj*/, const QSize & ){};

	virtual void beginObject( const QString &/*obj*/ ){};
	virtual void endObject(){};

	bool getBoolAttribute( const QDomElement &obj, const QString &attr, bool defaultValue = true ) const;
	QColor getColorAttribute( const QDomElement &obj, const QString &attr, const QColor &defaultValue = Qt::white ) const;
	QString getTextAttribute( const QDomElement &obj, const QString &attr, const QString &defaultValue = QString() ) const;
	int getIntAttribute( const QDomElement &obj, const QString &attr, int defaultValue = 0 ) const;
	KreBorder getBorderAttribute( const QDomElement &obj, const QString &attr, const KreBorder &defaultValue = KreBorder() ) const;
	QFont getFontAttribute( const QDomElement &obj, const QString &attr, const QFont &defaultValue = QFont() ) const;
	QSize getSizeAttribute( const QDomElement &obj, const QString &attr, const QSize &defaultValue = QSize() ) const;

	QString alignmentAsCSS( int );
	QString borderAsCSS( const KreBorder & );
	QString bgColorAsCSS( const QColor & );
	QString fontAsCSS( const QFont & );
	QString sizeAsCSS( const QSize & );
	QString textColorAsCSS( const QColor & );
	QString visibilityAsCSS( bool );

private:
	QDomElement getLayoutAttribute( const QDomElement &obj, const QString &attr ) const;
};

#endif //KLOMANAGER_H
