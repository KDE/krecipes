/***************************************************************************
 *   Copyright (C) 2003 by krecipes.sourceforge.net authors                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "fractioninput.h"

FractionInput::FractionInput( QWidget *parent, const char *name ) : KLineEdit( parent, name ), m_fraction()
{
	connect( this, SIGNAL(lostFocus()), SLOT(parseText()) );
}

FractionInput::~FractionInput()
{
}

void FractionInput::parseText()
{
	const QString input = text();
	m_fraction = MixedNumber::fromString( input );
}
