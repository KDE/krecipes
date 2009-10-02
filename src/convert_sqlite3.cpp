/***************************************************************************
*   Copyright © 2005 Jason Kivlighn <jkivlighn@gmail.com>                 *
*   Copyright © 2009 José Manuel Santamaría Lema <panfaust@gmail.com>     *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "convert_sqlite3.h"

#include <QApplication>
#include <QFile>

#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kmessagebox.h>
#include <KProcess>
#include <QEventLoop>
#include <KLocale>

#include <kglobalsettings.h>


ConvertSQLite3::ConvertSQLite3( const QString &db_file ) : QObject(), error(false)
{
	this->db_file = db_file;
	m_localEventLoop = new QEventLoop;
}

void ConvertSQLite3::convert()
{
	kDebug() << "converting";
	QString file = db_file;
	if ( file.isEmpty() ) {
		KConfigGroup config = KGlobal::config()->group("Server");
		file = config.readEntry("DBFile");
	}

	KProcess *p1 = new KProcess;
	KProcess *p2 = new KProcess;

	//sqlite OLD.DB .dump | sqlite3 NEW.DB
	QStringList cmd1, cmd2;
	cmd1 << "sqlite" << file << ".dump";
	cmd2 << "sqlite3" << (file + ".new");

	*p1 << cmd1;
	*p2 << cmd2;

	p1->setOutputChannelMode( KProcess::MergedChannels );
	p1->setStandardOutputProcess( p2 );

	QApplication::connect( p1, SIGNAL(finished(int,QProcess::ExitStatus)), this,
		SLOT(process1Finished(int,QProcess::ExitStatus)) );
	QApplication::connect( p2, SIGNAL(finished(int,QProcess::ExitStatus)), this,
		SLOT(process2Finished(int,QProcess::ExitStatus)) );

	p2->start();
	p1->start();
	m_localEventLoop->exec();

	if ((m_exitCode1 != 0) || (m_exitStatus1 != QProcess::NormalExit) ||
		(m_exitCode2 != 0) || (m_exitStatus2 != QProcess::NormalExit)) {
		KMessageBox::error( 0, i18n("Conversion process failed. "
			"Probably the file %1 is not an SQLite 2 database.").arg(file) );
		return;
	}

	QString backup_file = file+".sqlite2";
	int i = 1;
	while ( QFile::exists(backup_file) ) {
		backup_file = backup_file.left(file.length()+8)+'.'+QString::number(i);
		++i;
	}

	if ( !copyFile( file, backup_file ) ) {
		kDebug()<<"Unable to backup SQLite 2 database... aborting"<<endl
		  <<"A successfully converted SQLite 3 file is available at \""<<file<<".new\".";
	}
	else {
		kDebug()<<"SQLite 2 database backed up to "<<backup_file;
		if ( !copyFile( file+".new", file ) ) {
			kDebug()<<"Unable to copy the new SQLite 3 database to: "<<file<<"."<<endl
			  <<"You may manually move \""<<file<<".new\" to \""<<file<<"\"";
		}
		else {
			KMessageBox::information( 0, i18n("Conversion successful! "
				"SQLite 2 database backed up to %1").arg(backup_file) );
			QFile::remove(file+".new");
		}
	}
}

ConvertSQLite3::~ConvertSQLite3()
{
}

bool ConvertSQLite3::copyFile( const QString &oldFilePath, const QString &newFilePath )
{
	//load both files
	QFile oldFile(oldFilePath);
	QFile newFile(newFilePath);
	bool openOld = oldFile.open( QIODevice::ReadOnly );
	bool openNew = newFile.open( QIODevice::WriteOnly );

	//if either file fails to open bail
	if(!openOld || !openNew) { return false; }

	//copy contents
	uint BUFFER_SIZE = 16000;
	char* buffer = new char[BUFFER_SIZE];
	while(!oldFile.atEnd())
	{
		Q_ULONG len = oldFile.read( buffer, BUFFER_SIZE );
		newFile.write( buffer, len );
	}

	//deallocate buffer
	delete[] buffer;
	buffer = NULL;
	return true;
}

void ConvertSQLite3::process1Finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	m_exitCode1 = exitCode;
	m_exitStatus1 = exitStatus;
}

void ConvertSQLite3::process2Finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	m_exitCode2 = exitCode;
	m_exitStatus2 = exitStatus;
	m_localEventLoop->exit();
}

#include "convert_sqlite3.moc"

