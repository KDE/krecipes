/***************************************************************************
*   Copyright (C) 2006 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "recipeprintpreview.h"

#include <kvbox.h>

#include <kdebug.h>
#include <klocale.h>

#include "recipeviewdialog.h"
#include "pagesetupdialog.h"

RecipePrintPreview::RecipePrintPreview( QWidget *parent, RecipeDB *db, const QList<int> &ids )
    : KDialog( parent )
{
    setCaption( i18n("Print Preview") );
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::User1 | KDialog::Help);
    setDefaultButton(KDialog::Ok);
    setModal( true );
    KVBox *page = new KVBox( this );
    setMainWidget( page );
    setHelp("print-recipe");
    setButtonText( KDialog::User1, i18n("&Edit") );
    setButtonText( KDialog::Ok, i18n("&Print") );

    setSizeGripEnabled( true );


    recipeView = new RecipeViewDialog( page, db );
    recipeView->loadRecipes( ids, "Print" );

    //setInitialSize( QSize(450,500), false );
    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT( slotUser1() ) );
}

RecipePrintPreview::~RecipePrintPreview()
{
}

void RecipePrintPreview::slotOk()
{
	recipeView->printNoPreview();
	accept();
}

void RecipePrintPreview::slotUser1( void )
{
	PageSetupDialog pageSetup( this, Recipe(), "Print" );
	if ( pageSetup.exec() == QDialog::Accepted )
		reload();
}

void RecipePrintPreview::reload()
{
	recipeView->reload( "Print" );
}

#include "recipeprintpreview.moc"
