/***************************************************************************
*   Copyright © 2003 Unai Garro <ugarro@gmail.com>                        *
*   Copyright © 2003 Cyril Bosselut <bosselut@b1project.com>              *
*   Copyright © 2003 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2010 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef RESIZERECIPEDIALOG_H
#define RESIZERECIPEDIALOG_H

#include <kdialog.h>

class Recipe;

namespace Ui {
	class ResizeRecipeDialog;
}

/**
  *@author Jason Kivlighn
  */
class ResizeRecipeDialog : public KDialog
{
	Q_OBJECT

public:
	ResizeRecipeDialog( QWidget *parent, Recipe* );

protected slots:
	void accept();
	void activateCurrentOption( int );

private:
	void resizeRecipe( double factor );

	Recipe *m_recipe;
	Ui::ResizeRecipeDialog * ui;
};

#endif //RESIZERECIPEDIALOG_H
