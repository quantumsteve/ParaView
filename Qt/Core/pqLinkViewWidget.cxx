/*=========================================================================

   Program: ParaView
   Module:    pqLinkViewWidget.cxx

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.1. 

   See License_v1.1.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/


#include "pqLinkViewWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QEvent>
#include <QApplication>
#include <QMouseEvent>

#include "QVTKWidget.h"
#include "pqRenderViewModule.h"
#include "pqApplicationCore.h"
#include "pqLinksModel.h"
#include "pqServerManagerModel.h"

pqLinkViewWidget::pqLinkViewWidget(pqRenderViewModule* firstLink)
  : QWidget(firstLink->getWidget(), 
            Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    RenderView(firstLink)
{
  QVBoxLayout* l = new QVBoxLayout(this);
  QLabel* label = new QLabel(this);
  l->addWidget(label);
  label->setText("Click on another view to link with.");
  label->setWordWrap(true);
  QHBoxLayout* hl = new QHBoxLayout;
  l->addLayout(hl);
  label = new QLabel("Name:", this);
  hl->addWidget(label);
  this->LineEdit = new QLineEdit(this);
  hl->addWidget(this->LineEdit);
  QPushButton* button = new QPushButton(this);
  l->addWidget(button);
  button->setText("Cancel");
  QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(close()));
  
  int index = 0;
  pqLinksModel* model = pqApplicationCore::instance()->getLinksModel();
  QString name = QString("CameraLink%1").arg(index);
  while(model->getLink(name))
    {
    name = QString("CameraLink%1").arg(++index);
    }
  this->LineEdit->setText(name);
  this->LineEdit->selectAll();
}

pqLinkViewWidget::~pqLinkViewWidget()
{
}
  
bool pqLinkViewWidget::event(QEvent* e)
{
  if(e->type() == QEvent::Show)
    {
    // window shown, watch events
    QApplication::instance()->installEventFilter(this);
    this->LineEdit->setFocus(Qt::OtherFocusReason);
    }
  else if(e->type() == QEvent::Hide)
    {
    // window hidden, done watching events
    QApplication::instance()->removeEventFilter(this);
    }
  return QWidget::event(e);
}

bool pqLinkViewWidget::eventFilter(QObject* watched, QEvent* e)
{
  if(e->type() == QEvent::MouseButtonPress ||
     e->type() == QEvent::MouseButtonDblClick )
    {
    pqServerManagerModel* smModel =
      pqApplicationCore::instance()->getServerManagerModel();
    QMouseEvent* me = static_cast<QMouseEvent*>(e);
    QPoint globalpos(me->globalX(), me->globalY());
    QWidget* wid = QApplication::widgetAt(globalpos);
    pqRenderViewModule* otherView;
    otherView = smModel->getRenderModule(qobject_cast<QVTKWidget*>(wid));
    
    // if the user clicked on another view
    if(otherView && otherView != this->RenderView)
      {
      QString name = this->LineEdit->text();
      pqLinksModel* model = pqApplicationCore::instance()->getLinksModel();
      vtkSMLink* link = model->getLink(name);
      if(link)
        {
        model->removeLink(name);
        }

      model->addCameraLink(name, 
        this->RenderView->getRenderModuleProxy(),
        otherView->getRenderModuleProxy());

      this->close();
      }
    // if the user didn't click in this window
    else if(!this->geometry().contains(globalpos))
      {
      // consume invalid mouse events
      return true;
      }
    }
  return QObject::eventFilter(watched, e);
}


