/***************************************************************************
*   Copyright (C) 2003 by                                                 *
*   Jason Kivlighn (mizunoami44@users.sourceforge.net)                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "fractioninput.h"

FractionInput::FractionInput( QWidget *parent, const char *name ) : KLineEdit( parent, name )
{}

FractionInput::~FractionInput()
{}

void FractionInput::setValue( double d )
{
	MixedNumber m( d );
	setText( m.toString( MixedNumber::MixedNumberFormat ) );
}

void FractionInput::setValue( const MixedNumber &m )
{
	setText( m.toString( MixedNumber::MixedNumberFormat ) );
}

MixedNumber FractionInput::value() const
{
	return MixedNumber::fromString( text() );
}

bool FractionInput::isInputValid() const
{
	bool ok;
	MixedNumber::fromString( text(), &ok );
	return ok;
}
