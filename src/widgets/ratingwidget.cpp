/***************************************************************************
    copyright            : (C) 2003-2005 by Robby Stephenson
    email                : robby@periapsis.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of version 2 of the GNU General Public License as  *
 *   published by the Free Software Foundation;                            *
 *                                                                         *
 ***************************************************************************/

#include "ratingwidget.h"

#include <kglobal.h> // needed for KMAX
#include <kiconloader.h>
#include <kdebug.h>

#include <q3intdict.h>
#include <qlayout.h>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <QBoxLayout>
#include <kvbox.h>

namespace {
  static const int RATING_WIDGET_MAX_STAR_SIZE = 24;
}

const QPixmap& RatingWidget::pixmap(const QString& value_) {
  static Q3IntDict<QPixmap> pixmaps;
  if(pixmaps.isEmpty()) {
    pixmaps.insert(-1, new QPixmap());
  }
  bool ok;
  int n = value_.toInt(&ok);
  if(!ok || n < 1 || n > 10) {
    return *pixmaps[-1];
  }
  if(pixmaps[n]) {
    return *pixmaps[n];
  }

  QString picName = QString::fromLatin1("stars%1").arg(n);
  QPixmap* pix = new QPixmap(UserIcon(picName));
  pixmaps.insert(n, pix);
  return *pix;
}

RatingWidget::RatingWidget(int stars, QWidget* parent_, const char* name_/*=0*/)
    : KHBox(parent_), m_currIndex(-1), m_min(0), m_max(stars*2) {
  m_pixOn = UserIcon(QString::fromLatin1("star_on"));
  m_pixOff = UserIcon(QString::fromLatin1("star_off"));
  m_pixHalf = UserIcon(QString::fromLatin1("star_half"));
  setSpacing(0);

  // find maximum width and height
  int w = qMax(RATING_WIDGET_MAX_STAR_SIZE, qMax(m_pixOn.width(), m_pixOff.width()));
  int h = qMax(RATING_WIDGET_MAX_STAR_SIZE, qMax(m_pixOn.height(), m_pixOff.height()));
  for(int i = 0; i < stars; ++i) {
    QLabel* l = new QLabel(this);
    l->setFixedSize(w, h);
    m_widgets.append(l);
  }
  init();

  QBoxLayout* l = dynamic_cast<QBoxLayout*>(layout());
  if(l) {
    l->addStretch(1);
  }
}

void RatingWidget::init() {
  m_total = qMin(m_max/2, static_cast<int>(m_widgets.count()));
  uint i = 0;
  for( ; static_cast<int>(i) < m_total; ++i) {
    m_widgets.at(i)->setPixmap(m_pixOff);
  }
  for( ; i < m_widgets.count(); ++i) {
    m_widgets.at(i)->setPixmap(QPixmap());
  }
  update();
}

void RatingWidget::update() {
  int i = 0;
  for( ; i <= (m_currIndex-1)/2; ++i) {
    m_widgets.at(i)->setPixmap(m_pixOn);
  }
  for( ; i < m_total; ++i) {
    m_widgets.at(i)->setPixmap(m_pixOff);
  }

  if ( m_currIndex % 2 == 0 ) {
    m_widgets.at(m_currIndex/2)->setPixmap(m_pixHalf);
  }

  KHBox::update();
}

void RatingWidget::mousePressEvent(QMouseEvent* event_) {
  // only react to left button
  if(event_->button() != Qt::LeftButton) {
    return;
  }

  int idx;
  QWidget* child = childAt(event_->pos());
  bool left = false;
  if(child) {
    QRect child_geom_left_half = child->geometry();
    child_geom_left_half.setWidth(child_geom_left_half.width()/2);
    if ( child_geom_left_half.contains(event_->pos()) )
      left = true;

    idx = m_widgets.indexOf(static_cast<QLabel*>(child));
    // if the widget is clicked beyond the maximum value, clear it
    // remember total and min are values, but index is zero-based!
    if(idx > m_total-1) {
      idx = -1;
    } else if(idx < m_min-1) {
      idx = m_min-1; // limit to minimum, remember index is zero-based
    }
  } else {
    idx = -1;
  }

    int oldCurrent = m_currIndex;

    m_currIndex = idx*2+1;

    if ( left )
      m_currIndex--;

    if ( oldCurrent != m_currIndex ) {
      update();
      emit modified();
    }
}

void RatingWidget::clear() {
  m_currIndex = -1;
  update();
}

QString RatingWidget::text() const {
  // index is index of the list, which is zero-based. Add 1!
  return m_currIndex == -1 ? QString::null : QString::number(double(m_currIndex+1)/2);
}

void RatingWidget::setText(const QString& text_) {
  bool ok;
  // text is value, subtract one to get index
  m_currIndex =text_.toInt(&ok)-1;
  if(ok) {
    if(m_currIndex > m_total-1) {
      m_currIndex = -1;
    } else if(m_currIndex < m_min-1) {
      m_currIndex = m_min-1; // limit to minimum, remember index is zero-based
    }
  } else {
    m_currIndex = -1;
  }
  update();
}

#include "ratingwidget.moc"
