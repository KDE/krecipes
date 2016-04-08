/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreelement.h"

KreElement::KreElement()
{
}

KreElement::KreElement( const QVariant & id, const QString & name ) :
	m_id( id ), m_name( name )
{
}

QVariant KreElement::id() const
{
	return m_id;
}

void KreElement::setId( const QVariant & id )
{
	m_id = id;
}

QString KreElement::name() const
{
	return m_name;
}

void KreElement::setName( const QString & name )
{
	m_name = name;
}
