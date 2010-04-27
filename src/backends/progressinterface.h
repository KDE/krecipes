/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#ifndef PROGRESSINTERFACE_H
#define PROGRESSINTERFACE_H

#include <QObject>
#include <qwidget.h>

class KProgressDialog;

class RecipeDB;
class ProgressSlotObject;

/**  This class is used to monitor events from the database that may take
  *  awhile.  Before a potentially long operation, the database will
  *  call progressBegin(), progress() a number of times, and then progressDone().
  *
  *  This class may be subclassed to perform certain operations during long 
  *  operations by implementing the three virtual functions.  The default
  *  implementation displays a progress bar dialog.
  */
class ProgressInterface
{
public:
	ProgressInterface( QWidget *parent );
	virtual ~ProgressInterface();

	void listenOn( RecipeDB* );

protected:
	friend class ProgressSlotObject;

	virtual void progressBegin(int,const QString &caption,const QString &text,int rate);
	virtual void progressDone();
	virtual void progress();

private:
	ProgressSlotObject *slot_obj;
	KProgressDialog *progress_dlg;
	RecipeDB *database;

	int m_rate;
	int m_rate_at;
};

class ProgressSlotObject : public QObject
{
Q_OBJECT

public:
	ProgressSlotObject( QWidget*parent, ProgressInterface *p ) : QObject(parent), pInterface(p){}

public slots:
	void progressBegin(int i,const QString &caption=QString(),const QString &text=QString(),int rate=1){ pInterface->progressBegin(i,caption,text,rate); }
	void progressDone(){ pInterface->progressDone(); }
	void progress(){ pInterface->progress(); }

private:
	ProgressInterface *pInterface;
};

#endif
