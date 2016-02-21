/***************************************************************************
*   Copyright © 2016 José Manuel Santamaría Lema <panfaust@gmail.com>      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ingredientseditor.h"
#include "ui_ingredientseditor.h"

IngredientsEditor::IngredientsEditor( QWidget * parent)
		: QWidget( parent )
{
	ui = new Ui::IngredientsEditor;
	ui->setupUi( this );
}


