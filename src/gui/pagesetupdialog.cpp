/***************************************************************************
 *   Copyright (C) 2003 by Jason Kivlighn                                  *
 *   mizunoami44@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pagesetupdialog.h"

#include <qdir.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qtooltip.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kpopupmenu.h>

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample ) : KDialog(parent,0,true)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	KToolBar *toolbar = new KToolBar(this);
	setup_display = new SetupDisplay(sample,this);

	KActionCollection *actionCollection = new KActionCollection(this);
	
	KAction *std_open = KStdAction::open(0,0,0); //use this to create a custom open action
	KToolBarPopupAction *custom_open = new KToolBarPopupAction( std_open->text(), std_open->icon(), std_open->shortcut(), this, SLOT(loadLayout()), actionCollection, "open_popup" );
	KPopupMenu *open_popup = custom_open->popupMenu();
	open_popup->insertTitle( i18n("Included Layouts") );
	QDir included_layouts( getIncludedLayoutDir(), "*.klo", QDir::Name | QDir::IgnoreCase, QDir::Files );

	for ( unsigned int i = 0; i < included_layouts.count(); i++ )
	{
		int id = open_popup->insertItem(included_layouts[i],this,SLOT(loadLayout(int)));
		included_layouts_map.insert( id, included_layouts[i] );
	}
	
	custom_open->plug(toolbar);

	KStdAction::save(this, SLOT(saveLayout()), actionCollection)->plug(toolbar);
	KStdAction::saveAs(this, SLOT(saveAsLayout()), actionCollection)->plug(toolbar);
	KStdAction::redisplay(this, SLOT(reloadLayout()), actionCollection)->plug(toolbar);
	
	KToolBarPopupAction *shown_items = new KToolBarPopupAction( i18n("Items Shown"), "frame_edit" );
	shown_items->setDelayed(false);
	shown_items_popup = shown_items->popupMenu();
	shown_items_popup->insertTitle( i18n("Show Items") );
	shown_items->plug(toolbar);

	layout->addWidget(toolbar);

	KConfig *config=kapp->config();
	config->setGroup( "Page Setup" );
	
	//let's do everything we can to be sure at least some layout is loaded
	QString filename = config->readEntry("Layout",locate("appdata","layouts/default.klo"));
	if ( filename.isEmpty() || !QFile::exists(filename) )
		filename = locate("appdata","layouts/default.klo");
	loadLayout( filename );
	initShownItems();

	layout->addWidget(setup_display);

	QHBox *buttonsBox = new QHBox(this);
	KIconLoader *il = new KIconLoader;
	QPushButton *okButton = new QPushButton(il->loadIconSet("ok",KIcon::Small),i18n("Save and Close"),buttonsBox);
	QPushButton *cancelButton = new QPushButton(il->loadIconSet("cancel",KIcon::Small),i18n("&Cancel"),buttonsBox);
	layout->addWidget(buttonsBox);

	connect( okButton, SIGNAL(clicked()), SLOT(accept()) );
	connect( cancelButton, SIGNAL(clicked()), SLOT(reject()) );
	connect( setup_display, SIGNAL(itemVisibilityChanged(QWidget*,bool)), SLOT(updateItemVisibility(QWidget*,bool)) );
}

void PageSetupDialog::accept()
{
	if ( setup_display->hasChanges() )
		saveLayout();
	
	if ( !active_filename.isEmpty() )
	{
		KConfig *config=kapp->config();
		config->setGroup( "Page Setup" );
		config->writeEntry( "Layout", active_filename );
	}

	QDialog::accept();
}

void PageSetupDialog::reject()
{
	if ( setup_display->hasChanges() )
	{
		switch ( KMessageBox::questionYesNoCancel( this, i18n("This layout has been modified.\nDo you want to save it?"), i18n("Save Layout?") ) )
		{
		case KMessageBox::Yes: saveLayout(); break;
		case KMessageBox::No: break;
		default: return;
		}
	}

	QDialog::reject();
}

QSize PageSetupDialog::minimumSize() const
{
return(QSize(300,400));
}

QSize PageSetupDialog::sizeHint(void) const
{
return (minimumSize());
}

void PageSetupDialog::updateItemVisibility( QWidget *item, bool visible )
{
	shown_items_popup->setItemChecked( widget_popup_map[item], visible );
}

//TODO: Sort these by alphabetical order
void PageSetupDialog::initShownItems()
{
	for ( PropertiesMap::const_iterator it = setup_display->properties().begin(); it != setup_display->properties().end(); ++it )
	{
		if ( it.data() & SetupDisplay::Visibility )
		{
			int new_id = shown_items_popup->insertItem( QToolTip::textFor(it.key()->widget) );
			shown_items_popup->setItemChecked( new_id, it.key()->widget->isShown() );
			shown_items_popup->connectItem( new_id, this, SLOT(setItemShown(int)) );

			popup_widget_map.insert( new_id, it.key()->widget );
			widget_popup_map.insert( it.key()->widget, new_id );
		}
	}
}

void PageSetupDialog::setItemShown( int id )
{
	shown_items_popup->setItemChecked( id, !shown_items_popup->isItemChecked(id) );
	setup_display->setItemShown( popup_widget_map[id], shown_items_popup->isItemChecked(id) );
}

void PageSetupDialog::loadLayout()
{
	loadLayout( KFileDialog::getOpenFileName( locateLocal("appdata","layouts/"), "*.klo|Krecipes Layout (*.klo)", this, i18n("Select Layout") ) );
}

void PageSetupDialog::loadLayout( int popup_param )
{
	loadLayout( getIncludedLayoutDir() + "/" + included_layouts_map[popup_param] );
}

void PageSetupDialog::loadLayout( const QString &filename )
{
	if ( setup_display->hasChanges() )
	{
		switch ( KMessageBox::questionYesNoCancel( this, i18n("This layout has been modified.\nDo you want to save it?"), i18n("Save Layout?") ) )
		{
		case KMessageBox::Yes: saveLayout(); break;
		case KMessageBox::No: break;
		default: return;
		}
	}

	if ( !filename.isEmpty() )
	{
		setup_display->loadLayout( filename );
		setActiveFile(filename);
	}
}

void PageSetupDialog::reloadLayout()
{
	loadLayout( active_filename );
}

void PageSetupDialog::saveLayout()
{
	if ( setup_display->hasChanges() )
	{
		if ( have_write_perm )
			setup_display->saveLayout( active_filename );
		else
		{
			switch ( KMessageBox::warningYesNo( this, i18n("Unable to save the layout because you do not have sufficient permissions to modify this file.\nWould you like to instead save the current layout to a new file?") ) )
			{
			case KMessageBox::Yes: saveAsLayout(); break;
			default: break;
			}
		}
	}
}

void PageSetupDialog::saveAsLayout()
{
	QString filename = KFileDialog::getSaveFileName( KGlobal::instance()->dirs()->saveLocation("appdata","layouts/"), "*.klo|Krecipes Layout (*.klo)", this, QString::null );

	if ( !filename.isEmpty() )
	{
		if ( haveWritePerm(filename) )
		{
			setup_display->saveLayout(filename);
			setActiveFile(filename);
		}
		else
		{
			switch ( KMessageBox::warningYesNo( this, i18n("You have selected a file that you do not have the permissions to write to.\nWould you like to select another file?") ) )
			{
			case KMessageBox::Yes: saveAsLayout(); break;
			default: break;
			}
		}
	}
}

QString PageSetupDialog::getIncludedLayoutDir() const
{
	QFileInfo file_info( locate("appdata","layouts/default.klo") );
	return file_info.dirPath(true);
}

void PageSetupDialog::setActiveFile( const QString &filename )
{
	active_filename = filename;
	
	have_write_perm = haveWritePerm(filename);
}

bool PageSetupDialog::haveWritePerm( const QString &filename )
{
	QFileInfo info(filename);

	if ( info.exists() ) //check that we can write to this particular file
	{
		QFileInfo info(filename);
		return info.isWritable();
	}
	else //check that we can write to the directory since the file doesn't exist
	{
		QFileInfo dir_info( info.dirPath(true) );
		return dir_info.isWritable();
	}
}


#include "pagesetupdialog.moc"
