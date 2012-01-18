/***************************************************************************
   This file is part of FunEyes
   
   Created by Jesse Zamora <xtremek2008@aim.com>
   based on KEyes by <FIND AUTHOR NAME!!!>

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

#ifndef KEYES_H
#define KEYES_H

#include <QWidget>

#include "global.h"

class QTimer;
class QActionGroup;
class QPainter;
class QSettings;

/* Name: 	class FunEyesWidget 
   Purpose: 	Serves as the widget which contains the
		face & eyes.
   Description: This class contains the calling code in
		FunEyes.
*/      
class FunEyesWidget : public QWidget
{
    Q_OBJECT

public:
    FunEyesWidget(QWidget *parent = 0);

private slots:
    // changes the face when clicked from the context menu
    void actionUpdateFace(QAction*);

    // Updates the mousePosition variable
    void updateFromMousePosition();

    // connected to the Manage Faces context menu item
    void manageFaces();

    // changes the face based on specific settings
    void chooseFace();

    // handles the quit event
    void quit();

protected:
    // Reimplemented from QWidget
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual QSize sizeHint();

    // load the faces from the directory
    int loadFaces();
    // reload the faces
    void reloadFaces();
    // change the curent face
    void setFace(int faceIndex);


private:
    // pixmap for the current visible face
    QPixmap currentFacePixmap;

    // THESE GUYS NEED TO BE REVIEWED
    QPoint mousePosition, dragPosition;

    // the action group containing checkable items
    QActionGroup *actionGroup;
    // "Manage Faces" context menu item action
    QAction *manageFacesAction;
    // "Quit" context menu item action
    QAction *quitAction;

    // face redraw timer and face selector timer
    QTimer *timer, *faceTimer;

    // global class faces list
    FacesList facesList;

    // used to prevent paint event from drawing
    // while dialog is open
    bool isDialogOpen;
};

#endif
