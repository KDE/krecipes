/***************************************************************************
*   Copyright © 2003-2005 Jason Kivlighn <jkivlighn@gmail.com>            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "pagesetupdialog.h"

#include <QDir>

#include <QFileInfo>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <q3tl.h>

#include <khtmlview.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kconfig.h>
#include <kstandardaction.h>
#include <ktoolbar.h>
#include <ktoolbarpopupaction.h>
#include <kmenu.h>
#include <KActionCollection>

#include <widgets/thumbbar.h>
#include <kglobal.h>
#include <kvbox.h>
#include "setupdisplay.h"

PageSetupDialog::PageSetupDialog( QWidget *parent, const Recipe &sample, const QString &configEntry )
	: KDialog( parent ), m_configEntry(configEntry)
{
	QWidget *w = new QWidget;
	QVBoxLayout * layout = new QVBoxLayout( this );
	w->setLayout( layout );
	setButtons( KDialog::None );
	setMainWidget( w );
	KToolBar *toolbar = new KToolBar( this );
	KActionCollection *actionCollection = new KActionCollection( this );
	setCaption( i18n("Page Setup") );

	toolbar->addAction( KStandardAction::open( this, SLOT(loadFile()), actionCollection ) );
	toolbar->addAction( KStandardAction::save( this, SLOT( saveLayout() ), actionCollection ) );
	toolbar->addAction( KStandardAction::saveAs( this, SLOT( saveAsLayout() ), actionCollection ) );
	toolbar->addAction( KStandardAction::revert( this, SLOT( selectNoLayout() ), actionCollection ) );


	//KDE4 port do the last arguments of the KToolBarPopupAction and line 67 duplicate information ?
	KToolBarPopupAction *shown_items = new KToolBarPopupAction( KIcon( "configure" ), i18n( "Items Shown" ), toolbar);
	shown_items->setDelayed( false );
	shown_items_popup = shown_items->menu();
	shown_items_popup->setTitle( i18n( "Show Items" ) );
	toolbar->addAction( shown_items );

	layout->addWidget( toolbar );

	QLabel *help = new QLabel(i18n("<i>Usage: Select a template along the left, and right-click any element to edit the look of that element.</i>"),this);
	help->setWordWrap( true );
	layout->addWidget( help );

	KHBox *viewBox = new KHBox( this );
	ThumbBarView *thumbBar = new ThumbBarView(viewBox,Qt::Horizontal);
	connect(thumbBar,SIGNAL(signalURLSelected(const QString&)), this, SLOT(loadTemplate(const QString&)));
	QDir included_templates( getIncludedLayoutDir(), "*.xsl", QDir::Name | QDir::IgnoreCase, QDir::Files );
	for ( uint i = 0; i < included_templates.count(); i++ ) {
		new ThumbBarItem(thumbBar,included_templates.path() + '/' +included_templates[ i ]);
	}
	m_htmlPart = new SetupDisplay(sample, viewBox);
	//m_htmlPart->view()->setParent( viewBox, windowFlags() & ~Qt::WindowType_Mask);
	//m_htmlPart->view()->setGeometry( 0, 0 ,m_htmlPart->view()->width(),m_htmlPart->view()->height());


	layout->addWidget( viewBox );

	KHBox *buttonsBox = new KHBox( this );
	QPushButton *okButton = new QPushButton( i18n( "Save and Close" ), buttonsBox );
	okButton->setIcon( KIcon( "dialog-ok" ) );
	QPushButton *cancelButton = new QPushButton( i18n( "&Cancel" ), buttonsBox );
	cancelButton->setIcon( KIcon( "dialog-cancel") );
	layout->addWidget( buttonsBox );

	connect( m_htmlPart, SIGNAL(itemVisibilityChanged(KreDisplayItem*,bool)), this, SLOT(updateItemVisibility(KreDisplayItem*,bool)) );
	connect( okButton, SIGNAL( clicked() ), SLOT( accept() ) );
	connect( cancelButton, SIGNAL( clicked() ), SLOT( reject() ) );

	KConfigGroup config = KGlobal::config()->group( "Page Setup" );
	QSize defaultSize(722,502);
	resize(config.readEntry( "WindowSize", defaultSize ));

	//let's do everything we can to be sure at least some layout is loaded
	QString layoutFile = config.readEntry( m_configEntry+"Layout", KStandardDirs::locate( "appdata", "layouts/None.klo" ) );
	if ( layoutFile.isEmpty() || !QFile::exists( layoutFile ) )
		layoutFile = KStandardDirs::locate( "appdata", "layouts/None.klo" );

	QString tmpl = config.readEntry( m_configEntry+"Template", KStandardDirs::locate( "appdata", "layouts/Default.xsl" ) );
	if ( tmpl.isEmpty() || !QFile::exists( tmpl ) )
		tmpl = KStandardDirs::locate( "appdata", "layouts/Default.xsl" );
	kDebug()<<"tmpl: "<<tmpl;
	active_template = tmpl;
	loadLayout( layoutFile );

	thumbBar->setSelected(thumbBar->findItemByURL(active_template));

	initShownItems();
}

void PageSetupDialog::accept()
{
	if ( m_htmlPart->hasChanges() )
		saveLayout();

	KConfigGroup config( KGlobal::config(), "Page Setup" );
	config.writeEntry( m_configEntry+"Layout", active_filename );

	if ( !active_template.isEmpty() ) {
		config.writeEntry( m_configEntry+"Template", active_template );
	}

	config.writeEntry( "WindowSize", size() );

	KDialog::accept();
}

void PageSetupDialog::reject()
{
	if ( m_htmlPart->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "The recipe view layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout();
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	KDialog::reject();
}

void PageSetupDialog::updateItemVisibility( KreDisplayItem *item, bool visible )
{
	shown_items_popup->setItemChecked( widget_popup_map[ item ], visible );
}

void PageSetupDialog::initShownItems()
{
	shown_items_popup->clear();

	PropertiesMap properties = m_htmlPart->properties();

	QList<QString> nameList;
	QMap<QString,KreDisplayItem*> nameMap;

	for ( PropertiesMap::const_iterator it = properties.constBegin(); it != properties.constEnd(); ++it ) {
		nameList << it.key()->name;
		nameMap.insert( it.key()->name, it.key() );
	}
	qHeapSort( nameList );

	for ( QList<QString>::const_iterator it = nameList.constBegin(); it != nameList.constEnd(); ++it ) {
		KreDisplayItem *item = nameMap[*it];
		if ( properties[item] & SetupDisplay::Visibility ) {
			int new_id = shown_items_popup->insertItem ( *it );
			shown_items_popup->setItemChecked( new_id, item->show );
			shown_items_popup->connectItem( new_id, this, SLOT( setItemShown( int ) ) );

			popup_widget_map.insert( new_id, item );
			widget_popup_map.insert( item, new_id );
		}
	}
}

void PageSetupDialog::setItemShown( int id )
{
	m_htmlPart->setItemShown( popup_widget_map[ id ], shown_items_popup->isItemChecked( id ) );
}

void PageSetupDialog::loadFile()
{
	QString file = KFileDialog::getOpenFileName( KStandardDirs::locateLocal( "appdata", "layouts/" ), QString("*.klo *.xsl|%1").arg(i18n("Krecipes style or template file")), this, i18n( "Select Layout" ) );

	if ( file.endsWith(".klo") )
		loadLayout( file );
	else {
		active_template = file;
		m_htmlPart->loadTemplate( file );
	}
}

void PageSetupDialog::loadLayout( int popup_param )
{
	loadLayout( getIncludedLayoutDir() + '/' + included_layouts_map[ popup_param ] );
}

void PageSetupDialog::loadTemplate( const QString& filename )
{
	active_template = filename;
	m_htmlPart->loadTemplate( active_template );
}

void PageSetupDialog::loadLayout( const QString &filename )
{
	if ( m_htmlPart->hasChanges() ) {
		switch ( KMessageBox::questionYesNoCancel( this, i18n( "This layout has been modified.\nDo you want to save it?" ), i18n( "Save Layout?" ) ) ) {
		case KMessageBox::Yes:
			saveLayout();
			break;
		case KMessageBox::No:
			break;
		default:
			return ;
		}
	}

	m_htmlPart->loadLayout( filename );
	setActiveFile( filename );
}

void PageSetupDialog::reloadLayout()
{
	m_htmlPart->reload();
}

void PageSetupDialog::saveLayout()
{
	if ( m_htmlPart->hasChanges() ) {
		if ( have_write_perm )
			m_htmlPart->saveLayout( active_filename );
		else {
			switch ( KMessageBox::warningYesNo( this, i18n( "Unable to save the layout because you do not have sufficient permissions to modify this file.\nWould you like to instead save the current layout to a new file?" ) ) ) {
			case KMessageBox::Yes:
				saveAsLayout();
				break;
			default:
				break;
			}
		}
	}
}

void PageSetupDialog::saveAsLayout()
{
	QString filename = KFileDialog::getSaveFileName( KGlobal::mainComponent().dirs() ->saveLocation( "appdata", "layouts/" ), "*.klo|Krecipes Layout (*.klo)", this, QString() );

	if ( !filename.isEmpty() ) {
		if ( haveWritePerm( filename ) ) {
			m_htmlPart->saveLayout( filename );
			setActiveFile(filename);
		}
		else {
			switch ( KMessageBox::warningYesNo( this, i18n( "You have selected a file that you do not have the permissions to write to.\nWould you like to select another file?" ) ) ) {
			case KMessageBox::Yes:
				saveAsLayout();
				break;
			default:
				break;
			}
		}
	}
}

QString PageSetupDialog::getIncludedLayoutDir() const
{
	QFileInfo file_info( KStandardDirs::locate( "appdata", "layouts/None.klo" ) );
	return file_info.absolutePath();
}

void PageSetupDialog::setActiveFile( const QString &filename )
{
	active_filename = filename;
	have_write_perm = haveWritePerm( filename );
}

bool PageSetupDialog::haveWritePerm( const QString &filename )
{
	QFileInfo info( filename );

	if ( info.exists() )  //check that we can write to this particular file
	{
		QFileInfo info( filename );
		return info.isWritable();
	}
	else //check that we can write to the directory since the file doesn't exist
	{
		QFileInfo dir_info( info.absolutePath() );
		return dir_info.isWritable();
	}
}

void PageSetupDialog::selectNoLayout()
{
	m_htmlPart->loadLayout( QString() );
	setActiveFile( QString() );
//	QMap<int, KreDisplayItem*> popup_widget_map;
	QList<int> lst = popup_widget_map.uniqueKeys ();
	for (int i = 0; i < lst.size(); ++i)
	{
		shown_items_popup->setItemChecked(lst[i], true );
	}
}
#include "pagesetupdialog.moc"
