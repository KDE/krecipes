/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com)                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "progressinterface.h"

#include <qapplication.h>

#include <kprogress.h>

#include "recipedb.h"

ProgressInterface::ProgressInterface( QWidget *parent ) : progress_dlg(0)
{
	slot_obj = new ProgressSlotObject(parent,this);
}

ProgressInterface::~ProgressInterface()
{
	listenOn(0);
	delete slot_obj;
}

void ProgressInterface::progressBegin( int steps, const QString &caption, const QString &text )
{
	progress_dlg = new KProgressDialog((QWidget*)slot_obj->parent(),0,caption,text,true);

	if ( steps == 0 )
		progress_dlg->progressBar()->setPercentageVisible(false);
	progress_dlg->progressBar()->setTotalSteps( steps );

	//progress_dlg->setAllowCancel(false);
	progress_dlg->show();
}

void ProgressInterface::progressDone()
{
	delete progress_dlg;
	progress_dlg = 0;
}

void ProgressInterface::progress()
{
	if ( progress_dlg->wasCancelled() ) {
		database->cancelOperation();
	}
	else {
		progress_dlg->progressBar()->advance(1);
		qApp->processEvents();
	}
}

void ProgressInterface::listenOn( RecipeDB *db )
{
	slot_obj->disconnect();

	if ( db ) {
		slot_obj->connect( db, SIGNAL(progressBegin(int,const QString&,const QString&)), slot_obj, SLOT(progressBegin(int,const QString&,const QString&)) );
		slot_obj->connect( db, SIGNAL(progressDone()), slot_obj, SLOT(progressDone()) );
		slot_obj->connect( db, SIGNAL(progress()), slot_obj, SLOT(progress()) );
	}

	database = db;
}
