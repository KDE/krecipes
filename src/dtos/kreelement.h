/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#ifndef KREELEMENT_H
#define KREELEMENT_H

#include <QString>
#include <QVariant>

class KreElement {

public:
	KreElement();
	KreElement( const QVariant & id, const QString & name );

	QVariant id() const;
	void setId( const QVariant & id );

	QString name() const;
	void setName( const QString & name );

protected:
	QVariant m_id;
	QString m_name;

};

#endif
