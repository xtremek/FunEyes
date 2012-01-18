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

#ifndef FACESDIALOG_H
#define FACESDIALOG_H

#include "ui_facemanager.h"
#include "global.h"

#include <QLineEdit>

class RenameDialog : public QDialog
{
public:
    RenameDialog(QWidget *parent, QString currentText)
    {
        setWindowTitle("Rename face:");

        captionLabel = new QLabel("Type a new name:");

        renameEdit = new QLineEdit();
        renameEdit->setText(currentText);

        renameButton = new QPushButton("Rename");
        renameButton->setIcon(QIcon(":/Icons/rename.png"));

        cancelButton = new QPushButton("Cancel");
        cancelButton->setIcon(QIcon(":/Icons/cancel.png"));

        QGridLayout *mainLayout = new QGridLayout(this);
        mainLayout->addWidget(captionLabel, 0, 0, 1, 2);
        mainLayout->addWidget(renameEdit, 1, 0, 2, 3);
        mainLayout->addWidget(renameButton, 3, 1);
        mainLayout->addWidget(cancelButton, 3, 2);

        connect(renameButton, SIGNAL(clicked()),
                this, SLOT(accept()));
        connect(cancelButton, SIGNAL(clicked()),
                this, SLOT(reject()));
    }

    QString name()
    {
        return renameEdit->text();
    }
private:
    QLabel *captionLabel;
    QLineEdit *renameEdit;
    QPushButton *renameButton, *cancelButton;
};

class FaceManager : public QDialog
{
    Q_OBJECT

public:
    FaceManager(QWidget *parent, FacesList facesList);

    int selectedFace() const;

    void selectFaceIndex(int face);

    FacesList getFacesList() const { return m_facesList; }

private slots:
    void acceptedDialog();
    void rejectedDialog();

    void on_switchFaceCheckBox_stateChanged(int state);
    void on_randomizeCheckBox_stateChanged(int state);
    void on_facesListWidget_currentRowChanged(int row);

    void on_addFacePushButton_clicked();
    void on_removeFacePushButton_clicked();

    void on_eyeColorComboBox_activated(const QString &text);

    void contextMenuEvent(QContextMenuEvent *event);

    void renameFace();

    void browseForFacesDirectory();
private:
    void loadAndPopulateListWidget(bool reload);

private:
    Ui::faceManager ui;
    QAction *renameAction, *removeAction, *editAction;

    FacesList m_facesList;
    int previousRow, currentFace;
    QString m_facesDir;

    QStringList eyeColors;
};

#endif
