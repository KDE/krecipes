/***************************************************************************
    Copyright (C) 2003-2009 Robby Stephenson <robby@periapsis.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

/* File obtained from tellico, developed in KDE svn:
http://websvn.kde.org/trunk/extragear/office/tellico/src/gui/ratingwidget.cpp?view=log
This file was developed by Robby Stephenson and slightly modified by in order to
including it in krecipes and to allow ratings having n stars and a half. */

#include "ratingwidget.h"

#include <kiconloader.h>

#include <QHash>
#include <QPixmap>
#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>

#include <kdebug.h>

namespace {
  static const int RATING_WIDGET_MAX_ICONS = 10; // same as in Field::ratingValues()
  static const int RATING_WIDGET_MAX_STAR_SIZE = 24;
}

const QPixmap& RatingWidget::pixmap(const QString& value_) {
  static QHash<int, QPixmap*> pixmaps;
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

RatingWidget::RatingWidget(int stars, QWidget* parent_)
    : KHBox(parent_), m_currIndex(-1), m_min(0), m_max(stars) {
  m_pixOn = UserIcon(QLatin1String("star_on"));
  m_pixOff = UserIcon(QLatin1String("star_off"));
  m_pixHalf = UserIcon(QLatin1String("star_half"));
  setSpacing(0);

  // find maximum width and height
  int w = qMax(RATING_WIDGET_MAX_STAR_SIZE, qMax(m_pixOn.width(), m_pixOff.width()));
  int h = qMax(RATING_WIDGET_MAX_STAR_SIZE, qMax(m_pixOn.height(), m_pixOff.height()));
  for(int i = 0; i < RATING_WIDGET_MAX_ICONS; ++i) {
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
  updateBounds();
  m_total = qMin(m_max, static_cast<int>(m_widgets.count()));
  int i = 0;
  for( ; i < m_total; ++i) {
    m_widgets.at(i)->setPixmap(m_pixOff);
  }
  for( ; i < m_widgets.count(); ++i) {
    m_widgets.at(i)->setPixmap(QPixmap());
  }
  update();
}

void RatingWidget::updateBounds() {
  if(m_max > RATING_WIDGET_MAX_ICONS) {
    kDebug() << "max is too high: " << m_max;
    m_max = RATING_WIDGET_MAX_ICONS;
  }
  if(m_min < 1) {
    m_min = 1;
  }
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
  idx = idx*2+1;
  if ( left )
    idx--;
  if(m_currIndex != idx) {
    m_currIndex = idx;
    update();
    emit signalModified();
  }
}

void RatingWidget::clear() {
  m_currIndex = -1;
  update();
}

QString RatingWidget::text() const {
  // index is index of the list, which is zero-based. Add 1!
  return m_currIndex == -1 ? QString() : QString::number(double(m_currIndex+1)/2);
}

void RatingWidget::setText(const QString& text_) {
  bool ok;
  // text is value, subtract one to get index
  m_currIndex = (text_.toDouble(&ok)-1)/2;
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
