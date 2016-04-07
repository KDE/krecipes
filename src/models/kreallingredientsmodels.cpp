/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gail.com>       *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
****************************************************************************/

#include "kreallingredientsmodels.h"

#include <QStandardItemModel>


KreAllIngredientsModels::KreAllIngredientsModels( RecipeDB * database ):
	m_database( database )
{
	m_sourceModel = new QStandardItemModel( this );
}

QStandardItemModel * KreAllIngredientsModels::sourceModel()
{
	return m_sourceModel;
}
