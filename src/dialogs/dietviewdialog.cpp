/***************************************************************************
 *   Copyright (C) 2003 by                                                 *
 *   Unai Garro (ugarro@users.sourceforge.net)                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "dietviewdialog.h"
#include <kiconloader.h>
#include <klocale.h>

DietViewDialog::DietViewDialog(QWidget *parent, const RecipeList &recipeList, int dayNumber, int mealNumber, const QValueList <int> &dishNumbers):QWidget(parent)
{

 // Design the dialog

 	// Border spacers
 layout = new QGridLayout( this, 1, 1, 0, 0);
 QSpacerItem* spacer_left = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addItem( spacer_left, 1,0 );
 QSpacerItem* spacer_top = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed );
 layout->addItem(spacer_top,0,1);
 QSpacerItem* spacer_right = new QSpacerItem( 10,10, QSizePolicy::Fixed, QSizePolicy::Minimum );
 layout->addItem(spacer_right,1,2);
 QSpacerItem* spacer_bottom = new QSpacerItem( 10,10, QSizePolicy::Minimum, QSizePolicy::Fixed);
 layout->addItem(spacer_bottom,2,1);

 	// The html part
 htmlBox=new QVBox (this); layout->addWidget(htmlBox,1,1);
 dietView=new KHTMLPart(htmlBox);
 	// Buttons
 KIconLoader il;
 buttonBox=new QHBox(htmlBox);
 okButton=new QPushButton(il.loadIconSet("ok",KIcon::Small),i18n("Create &Shopping List"),buttonBox);
 printButton=new QPushButton(il.loadIconSet("fileprint",KIcon::Small),i18n("&Print"),buttonBox);
 cancelButton=new QPushButton(il.loadIconSet("cancel",KIcon::Small),i18n("&Cancel"),buttonBox);


 connect(okButton, SIGNAL(clicked()), this, SLOT(slotOk()) );
 connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()) );
 connect(printButton, SIGNAL(clicked()), this, SLOT(print()) );

 // Show the diet
 showDiet(recipeList,dayNumber,mealNumber,dishNumbers);
}

DietViewDialog::~DietViewDialog()
{
}

void DietViewDialog::showDiet(const RecipeList &recipeList, int dayNumber, int mealNumber, const QValueList <int> &dishNumbers)
{


// Header
QString htmlCode=QString("<html><head><title>%1</title>").arg(i18n("Diet"));

// CSS
htmlCode+="<STYLE type=\"text/css\">\n";
htmlCode+="#calendar{border: thin solid black}";
htmlCode+="#dayheader{ background-color: #D6D6D6; color: black; border:none;}";
htmlCode+="#day{ background-color: #E5E5E5; color: black; border:medium solid #D6D6D6;}";
htmlCode+="#meal{ background-color: #CDD4FF; color: black; border:thin solid #B4BEFF;align:center;}";
htmlCode+="#dish{overflow:hidden; height:1.5em;}";
htmlCode +="</STYLE>";


htmlCode+="</head><body>"; //  /Header

// Calendar border
htmlCode+=QString("<div id=\"calendar\">");

// Title
htmlCode+=QString("<center><div STYLE=\"width: 80%\">");
htmlCode+=QString("<h1>%1</h1></div></center>").arg(i18n("Diet"));

// Diet table
htmlCode+=QString("<center><div STYLE=\"width: 90%\">");
htmlCode+=QString("<table><tbody>");


QValueList <int>::ConstIterator it; it=dishNumbers.begin();
RecipeList::ConstIterator rit; rit=recipeList.begin();

for (int row=0,day=0; row<=((dayNumber-1)/7); row++) // New row (week)
	{
	htmlCode+=QString("<tr>");

	for (int col=0; (col<7) && (day<dayNumber); col++,day++) // New column (day)
		{
		htmlCode+=QString("<td><div id=\"day\"");
		htmlCode+=QString("<div id=\"dayheader\"><center>");
		htmlCode+=QString(i18n("Day %1")).arg(day+1);
		htmlCode+=QString("</center></div>");
		for (int meal=0;meal<mealNumber;meal++) // Meals in each cell
			{
			int dishNumber=*it;
			htmlCode+=QString("<div id=\"meal\">");
			for (int dish=0; dish<dishNumber;dish++) // Dishes in each Meal
				{
				htmlCode+=QString("<div id=\"dish\">");
				htmlCode+=(*rit).title; htmlCode+="<br>";
				htmlCode+=QString("</div>");
				rit++;
				}
			it++;
			htmlCode+=QString("</div>");
			}
		it=dishNumbers.begin(); // meals have same dish number everyday
		htmlCode+=QString("</div></td>");
		}

	htmlCode+=QString("</tr>");
}

htmlCode+=QString("</tbody></table>");
htmlCode+=QString("</div></center>");
htmlCode+=QString("</div></body></html>");

resize(QSize(600,400));
// Display it
dietView->begin(KURL("file:/tmp/" )); // Initialize to /tmp, where photos and logos can be stored
dietView->write(htmlCode);
dietView->end();
}

void DietViewDialog::print(void)
{
	dietView->view()->print();
}

void DietViewDialog::slotOk(void)
{
emit signalOk();
close();
}

#include "dietviewdialog.moc"
