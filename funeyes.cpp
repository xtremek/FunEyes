/***************************************************************************
   This file is part of FunEyes
   
   Created by Jesse Zamora <xtremek2008@aim.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include <math.h>

#include <QApplication>
#include <QWidget>
#include <QActionGroup>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QBitmap>
#include <QPainter>
#include <QMenu>
#include <QTimer>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QSettings>

#include "funeyes.h"
#include "facemanager.h"

// defines the interval to poll the mouse position
#define UPDATE_INTERVAL 50

FunEyesWidget::FunEyesWidget(QWidget *parent)
    : QWidget(parent)
{
    int ret;
    // basically set the window to have no background or frame
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);

    // let it poll the mouse position
    setMouseTracking(TRUE);

    // load the global program settings
    FunEyes::loadSettings();

    if(FunEyes::facesDir().isNull())
    {
        FunEyes::setFacesDir(QApplication::applicationDirPath()
				+ "/Faces");
    }

    // load the faces
    ret = loadFaces();
    // exit if there was a problem loading faces
    if(ret != 0)
    {
	qApp->quit();
	return;
    }

    // create context menu actions
    manageFacesAction = new QAction("Manage Faces", this);
    manageFacesAction->setIcon(QIcon(":/Icons/configure.png"));
    quitAction = new QAction("Quit", this);
    quitAction->setIcon(QIcon(":/Icons/exit.png"));

    // REVIEW THESE GUYS!
    dragPosition = QPoint(0, 0);
    mousePosition = QCursor::pos();

    // connect the actions
    connect(actionGroup, SIGNAL(triggered(QAction*)),
	    this, SLOT(actionUpdateFace(QAction*)));
    connect(manageFacesAction, SIGNAL(triggered()),
	    this, SLOT(manageFaces()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

    // create and connect the timers
    timer = new QTimer();
    faceTimer = new QTimer();

    connect(faceTimer, SIGNAL(timeout()), SLOT(chooseFace()));
    connect(timer, SIGNAL(timeout()), SLOT(updateFromMousePosition()));
    timer->start(UPDATE_INTERVAL);

    // no dialogs are open right now...
    isDialogOpen = FALSE;
}

// Load the faces into the facesList from the faces directory
int FunEyesWidget::loadFaces()
{
    QDir facesDirectory(FunEyes::facesDir());
    QFileInfoList list;
    int i;

    // basically search for any .face files in the 
    // faces directory...
    facesDirectory.setNameFilters(QStringList("*.face"));
    list = facesDirectory.entryInfoList();

    // in the event that there are none...
    if(list.size() == 0)
    {
	// let the user add some!
	FaceManager faceManager(this, facesList);
	
	// if the user presses the "Ok" button..
	if(faceManager.exec() == QDialog::Accepted)
	{
	    // get the list, and
	    facesList = faceManager.getFacesList();
	    // check to see if they did add any faces.
	    if(facesList.size() == 0)
	    {
		// if not, quit.
		return -1;
	    }
	}
	else
	{
	    // otherwise, quit
	    return -1;
	}
    }

    // load the faces into facesList
    for(i = 0; i < list.size(); i++)
    {
	//qDebug(qPrintable(funEyesSettings.facesDir + "/" + fileInfo.fileName()))
	// the Face::Face(QString &) constructor automatically
	// loads the face into the Face object
	Face face(list[i].filePath());
	// once we've loaded it, add it to facesList
	facesList.append(face);
    }

    //qDebug("Loading...");
    // now load the context menu items which allow the
    // user to quickly switch between faces.
    actionGroup = new QActionGroup(this);
    for(i = 0; i < facesList.size(); i++)
    {
	// the context items bear the same name as the in the .face
	// file...
	QAction *action = new QAction(facesList[i].name(), actionGroup);
	action->setIcon(facesList[i].facePixmap());
	action->setCheckable(TRUE);
    }

    // connect the actionGroup to the actionUpdateFace slot so
    // when the user selects an item, it can be displayed.
    connect(actionGroup, SIGNAL(triggered(QAction*)),
	    this, SLOT(actionUpdateFace(QAction*)));

    // select the last faced used
    actionGroup->actions()[FunEyes::currentFace()]->setChecked(TRUE);
    setFace(FunEyes::currentFace());
    // set the comment for the face as well
    setToolTip(facesList[FunEyes::currentFace()].comment());

    return 0;

}

// Reloads the faces from the faces directory
void FunEyesWidget::reloadFaces()
{
    // Cleanup actionGroup & facesList
    delete actionGroup;
    facesList.clear();

    // now just load em from the faces directory!
    loadFaces();
}

// Sets the face to facesList[faceIndex]
void FunEyesWidget::setFace(int faceIndex)
{
    // copy the pixmap from facesList[faceIndex] into
    // currentFacePixmap
    currentFacePixmap = facesList[faceIndex].facePixmap();

    // this sets the current face to faceIndex
    FunEyes::setCurrentFace(faceIndex);
  
    // makes the background transparent
    setMask(currentFacePixmap.createHeuristicMask());

    QRect widgetGeometry = geometry();
    setGeometry(widgetGeometry.x(), widgetGeometry.y(), 
				    currentFacePixmap.width(),
				    currentFacePixmap.height());
    setWindowTitle("FunEyes - Beta 2: " + facesList[faceIndex].name());
    setWindowIcon(QIcon(facesList[faceIndex].getDefaultFacePixmap()));
    setToolTip(facesList[faceIndex].comment());
    actionGroup->actions()[faceIndex]->setChecked(TRUE);

    // if the window is visible, update it!
    if(isVisible())
	update();
}


// when the user clicks a face name in the context menu,
// this slot is called
void FunEyesWidget::actionUpdateFace(QAction * action)
{
    // search for the face in facesList
    for(int i = 0; i < facesList.size(); i++)
    {
	if(facesList[i].name() == action->text())
	{
	    // then set the face to that one
	    setFace(i);
	    break;
	}
    }
}

void FunEyesWidget::updateFromMousePosition()
{
    QPoint newPosition;

    newPosition = QCursor::pos();

    if(newPosition.x() == mousePosition.x() &&
       newPosition.y() == mousePosition.y())
    {
	return;
    }

    mousePosition = newPosition;

    if(isVisible())
	update();
}

// this is called when the user clicks the "Manage Faces"
// context menu item. It displays the Face Manager and
// saves any settings changed in the dialog.
void FunEyesWidget::manageFaces()
{
    FaceManager faceManager(this, facesList);

    // stop painting the face
    isDialogOpen = TRUE;
    // set the face in the face manager to the current one
    faceManager.selectFaceIndex(FunEyes::currentFace());
    // run it
    if(faceManager.exec() == QDialog::Accepted)
    {
	// if the user presses "Ok", get the facesList from it
	//facesList = faceManager.getFacesList();
	// if the user wants to have FunEyes switch faces at
	// timed intervals, or if they want to stop it,
	// this code does it
	if(FunEyes::switchFaces())
	{
	    if(faceTimer->isActive())
		faceTimer->stop();

	    // start the timer
	    faceTimer->start(FunEyes::updateFreqMin() * 1000 * 60);

	}
	else
	{
	    // stop the timer
	    if(faceTimer->isActive())
		faceTimer->stop();
	}
	
	// be sure to reload the faces
	reloadFaces();
	// get the selected face and set the current one to that
	setFace(faceManager.selectedFace());
    }
  
    // we have now closed the dialog; it's ok to start painting again
    isDialogOpen = FALSE;
}


void FunEyesWidget::chooseFace()
{
    int currentFace;
    if(FunEyes::randomize() == FALSE)
    {
	currentFace = FunEyes::currentFace();
	if(currentFace == facesList.size())
	    FunEyes::setCurrentFace(0);
	else
	    FunEyes::setCurrentFace(++currentFace);

	setFace(FunEyes::currentFace());
    }
    else
    {
	int randNum;

	randNum = (int) (qrand() % (facesList.size() - 0));
	if(randNum == FunEyes::currentFace())
	{
	    return;
	}
	FunEyes::setCurrentFace(randNum);
	setFace(randNum);
    }
}

// saves the settings and quits
void FunEyesWidget::quit()
{
    FunEyes::saveSettings();

    qApp->quit();
}



void FunEyesWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
	dragPosition = event->globalPos() - frameGeometry().topLeft();
	event->accept();
    }
}

void FunEyesWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
	move(event->globalPos() - dragPosition);
	event->accept();
    }
}

void FunEyesWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    menu.addActions(actionGroup->actions());
    menu.addSeparator();
    menu.addAction(manageFacesAction);
    menu.addSeparator();
    menu.addAction(quitAction);
    menu.exec(event->globalPos());
}

void FunEyesWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if(facesList.size() == 0)
    {
	return;
    }

    if(isDialogOpen)
	return;

    QPainter painter(this);
    facesList[FunEyes::currentFace()].render(&painter,
					  frameGeometry().topLeft(),
					   mousePosition);
}


QSize FunEyesWidget::sizeHint()
{
    return currentFacePixmap.size();
}


