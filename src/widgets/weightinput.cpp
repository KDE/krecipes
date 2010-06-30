/***************************************************************************
*   Copyright © 2006 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "weightinput.h"

#include <klocale.h>

#include "backends/recipedb.h"
#include "datablocks/element.h"
#include "datablocks/weight.h"
#include "prepmethodcombobox.h"

WeightInput::WeightInput( QWidget *parent, RecipeDB *database, Unit::Type type, MixedNumber::Format format ) : 
	AmountUnitInput(parent,database,type,format),
	m_database(database)
{
	prepMethodBox = new PrepMethodComboBox(false,this,database,i18n("-No Preparation-"));
	prepMethodBox->reload();

	connect( prepMethodBox, SIGNAL(activated(int)), SLOT(emitValueChanged()) );
}

void WeightInput::emitValueChanged()
{
	Weight w;
	w.setPerAmount(amount().toDouble());

	Unit u = unit();
	w.setPerAmountUnitId(u.id());
	w.setPerAmountUnit(u.determineName(w.perAmount(), /*useAbbrev=*/false));

	// FIXME: What happens here? Both name and id are not defined!
	Element prep;
	w.setPrepMethod(prep.name);
	w.setPrepMethodId(prep.id);
	emit valueChanged( w );
}

void WeightInput::setPrepMethod( const Element &prep )
{
	if ( prep.id == -1 )
		prepMethodBox->setSelected(0);
	else
		prepMethodBox->setSelected( prep.id );

}

Element WeightInput::prepMethod() const
{
	Element prep;
	prep.id = prepMethodBox->id( prepMethodBox->currentIndex() );
	if ( prep.id != -1 )
		prep.name = prepMethodBox->currentText();
	return prep;
}

#include "weightinput.moc"
