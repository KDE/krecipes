/***************************************************************************
*   Copyright (C) 2005 by                                                 *
*   Jason Kivlighn (jkivlighn@gmail.com                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "convert_sqlite3.h"

#include <qapplication.h>
#include <qfile.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kprocio.h>

//FIXME: Some messages should be given to the user about success/failure, but that can't be done in the 0.8.x branch due to i18n.

ConvertSQLite3::ConvertSQLite3( const QString &db_file ) : QObject(), error(false)
{
	QString file = db_file;
	if ( file.isEmpty() ) {
		KConfig *config = KGlobal::config();
		config->setGroup("Server");
		file = config->readEntry("DBFile");
	}

	KProcIO *p = new KProcIO;
	p->setUseShell(true);

	//sqlite OLD.DB .dump | sqlite3 NEW.DB
	*p << "sqlite" << file << ".dump" <<
	  "|" << "sqlite3" << file+".new";

	QApplication::connect( p, SIGNAL(readReady(KProcIO*)), this, SLOT(processOutput(KProcIO*)) );

	bool success = p->start( KProcess::Block, true );
	if ( !success ) {
		kdDebug()<<"Conversion failed... unable to start KProcess"<<endl;
		return;
	}

	if ( error )
		return;


	QString backup_file = file+".sqlite2";
	int i = 1;
	while ( QFile::exists(backup_file) ) {
		backup_file = backup_file.left(file.length()+8)+"."+QString::number(i);
		++i;
	}

	if ( !copyFile( file, backup_file ) ) {
		kdDebug()<<"Unable to backup SQLite 2 database... aborting"<<endl
		  <<"A successfully converted SQLite 3 file is available at \""<<file<<".new\"."<<endl;
	}
	else {
		kdDebug()<<"SQLite 2 database backed up to "<<backup_file<<endl;
		if ( !copyFile( file+".new", file ) ) {
			kdDebug()<<"Unable to copy the new SQLite 3 database to: "<<file<<"."<<endl
			  <<"You may manually move \""<<file<<".new\" to \""<<file<<"\""<<endl;
		}
		else {
			kdDebug()<<"Conversion successful!"<<endl;
			QFile::remove(file+".new");
		}
	}
}

ConvertSQLite3::~ConvertSQLite3()
{
}

void ConvertSQLite3::processOutput( KProcIO* p )
{
	QString error_str, buffer;
	while ( p->readln(buffer) != -1 ) {
		error_str += buffer;
	}

	KMessageBox::error( 0, error_str );

	error = true;

	p->ackRead();
}

bool ConvertSQLite3::copyFile( const QString &oldFilePath, const QString &newFilePath )
{
	//load both files
	QFile oldFile(oldFilePath);
	QFile newFile(newFilePath);
	bool openOld = oldFile.open( IO_ReadOnly );
	bool openNew = newFile.open( IO_WriteOnly );
	
	//if either file fails to open bail
	if(!openOld || !openNew) { return false; }
	
	//copy contents
	uint BUFFER_SIZE = 16000;
	char* buffer = new char[BUFFER_SIZE];
	while(!oldFile.atEnd())
	{
		Q_ULONG len = oldFile.readBlock( buffer, BUFFER_SIZE );
		newFile.writeBlock( buffer, len );
	}
	
	//deallocate buffer
	delete[] buffer;
	buffer = NULL;
	return true;
}

#include "convert_sqlite3.moc"

