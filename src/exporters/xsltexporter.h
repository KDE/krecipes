/***************************************************************************
*   Copyright © 2007 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef XSLTEXPORTER_H
#define XSLTEXPORTER_H

#include <qdom.h>
#include <qmap.h>

#include <limits.h>

#include "baseexporter.h"
#include "klomanager.h"


/**
  * Exports a given recipe list as HTML using a given XSL
  * @author Jason Kivlighn
  */
class XSLTExporter : public BaseExporter, protected KLOManager
{
public:
	XSLTExporter( const QString&, const QString& );
	virtual ~XSLTExporter();

	virtual int supportedItems() const;

	void setTemplate( const QString &filename );
	void setStyle( const QString &filename );

protected:
	virtual QString createContent( const RecipeList & );
	virtual QString createHeader( const RecipeList & );
	virtual QString createFooter();

	virtual int progressInterval() const { return INT_MAX; }

	virtual void loadBackgroundColor( const QString &obj, const QColor& );
	virtual void loadFont( const QString &obj, const QFont& );
	virtual void loadTextColor( const QString &obj, const QColor& );
	virtual void loadVisibility( const QString &obj, bool );
	virtual void loadAlignment( const QString &obj, int );
	virtual void loadBorder( const QString &obj, const KreBorder& );
	virtual void loadColumns( const QString & obj, int cols );

	virtual void beginObject( const QString &obj );
	virtual void endObject();

	static QString escape( const QString & );

private:
	void storePhoto( const Recipe &recipe );
	void populateTemplate( const Recipe &recipe, QString &content );

	QString m_layoutFilename;
	QString m_templateFilename;
	QString m_cachedCSS;
	QMap<QString,bool> m_visibilityMap;
	QMap<QString,int> m_columnsMap;
	bool m_error;
};

#endif //XSLTEXPORTER_H
