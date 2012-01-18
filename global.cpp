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

#include <QMessageBox>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "global.h"
 
bool FunEyes::m_switchFaces = 0;
bool FunEyes::m_randomize = 0;
int FunEyes::m_updateFreqMin = 1;
QString FunEyes::m_facesDir;
int FunEyes::m_currentFace = 0;

void FunEyes::loadSettings()
{
    QVariant ret;
    QSettings settings("Zamora Studios", "FunEyes");

    if((ret = settings.value("lastFace")) != NULL)
	m_currentFace = ret.toInt();
    else
	m_currentFace = 0;

    if((ret = settings.value("switchFaces")) != NULL)
	m_switchFaces = ret.toBool();
    else
	m_switchFaces = FALSE;

    if((ret = settings.value("randomizeFaces")) != NULL)
	m_randomize = ret.toBool();
    else
	m_randomize = FALSE;

    if((ret = settings.value("updateFrequency")) != NULL)
	m_updateFreqMin = ret.toInt();
    else
	m_updateFreqMin = 1;

    if((ret = settings.value("facesDirectory")) != NULL)
	m_facesDir = ret.toString();
    else
	m_facesDir = QString();

}

void FunEyes::saveSettings()
{
    QSettings settings("Zamora Studios", "FunEyes");

    qDebug("Saving...");

    settings.setValue("lastFace", 
		      m_currentFace);
    settings.setValue("switchFaces",
		      m_switchFaces);
    settings.setValue("randomizeFaces",
		      m_randomize);
    settings.setValue("updateFrequency",
		      m_updateFreqMin);
    settings.setValue("facesDirectory",
		      m_facesDir);
    settings.sync();
}

Face::Face()
{
    m_numberOfEyes = 0;
    m_eyesAbove = FALSE;
}

Face::Face(QString faceFilename)
{
    m_numberOfEyes = 0;
    m_eyesAbove = FALSE;
    load(faceFilename);
}

int Face::load(QString faceFilename)
{
    QMessageBox msgBox;

    if(!QFile::exists(faceFilename))
    {
	msgBox.setText(QString("Face file %1 does not exist!")
				.arg(faceFilename));
	msgBox.exec();
	return -1;
    }

    m_faceFileName = faceFilename;
    readFaceFile();


    //qDebug(qPrintable(m_imageFileName));

    QFileInfo fileInfo(m_faceFileName);
    QString imageFilename = fileInfo.path() + "/" +
						  m_imageFileName;
    // Now load the pixmap
    if(!m_facePixmap.load(imageFilename))
    {
	qDebug("Error loading %s!\n", qPrintable(imageFilename));
	return -1;
    }

    return 0;
}

int Face::save()
{
    writeFaceFile();

    return 0;
}

int Face::readFaceFile()
{
    QXmlStreamReader reader;
    QString element, elementText;
    QFile faceFile;

    faceFile.setFileName(m_faceFileName);

    if(!faceFile.open(QIODevice::ReadOnly))
    {
	//display an error
	qDebug("Could not open %s\n", qPrintable(m_faceFileName));
	return -1;
    }

    reader.setDevice(&faceFile);

    while(!reader.atEnd())
    {
	reader.readNext();
	element = reader.name().toString();

	if(reader.isStartElement())
	{
	    if(element == "name")
		m_name = reader.readElementText();
	    else if(element == "image")
	    {
		QFileInfo fileInfo(faceFile);
		m_imageFileName = reader.readElementText();
            }
            else if(element == "eyes_above")
            {
                if(reader.readElementText().toInt() == 1)
                {
                    m_eyesAbove = TRUE;
                }
            }
	    else if(element == "eye")
	    {
		QPoint pos;
                QSize size;
		while(!reader.atEnd())
		{
		    reader.readNext();
		    element = reader.name().toString();
		    if(reader.isStartElement())
		    {
                        elementText = reader.readElementText();
                        if(element == "x")
			    pos.setX(elementText.toInt());
			else if(element == "y")
                            pos.setY(elementText.toInt());
			else if(element == "width")
			    size.setWidth(elementText.toInt());
			else if(element == "height")
			    size.setHeight(elementText.toInt());
			else if(element == "color")
			{
			    QString color = elementText;
                            addEye(pos, size, color);
			    break;
			}

		    }
		}
	    }
	    else if(element == "comment")
		m_comment = reader.readElementText();
	}

	if(reader.error())
	{
	    qDebug(qPrintable(reader.errorString()));
	}
    }

    faceFile.close();

    return 0;
}

int Face::writeFaceFile()
{
    QXmlStreamWriter writer;
    QFile faceFile;

    faceFile.setFileName(m_faceFileName);
    if(!faceFile.open(QIODevice::WriteOnly))
    {
	qDebug("Error writing file!");
	return -1;
    }

    writer.setDevice(&faceFile);

    writer.writeStartElement("face");
    writer.writeCharacters("\n\t");
    writer.writeTextElement("name", m_name);
    writer.writeCharacters("\n\t");
    writer.writeTextElement("image", m_imageFileName);
    writer.writeCharacters("\n\t");
    if(m_eyesAbove == TRUE)
        writer.writeTextElement("eyes_above", QString("1"));
    else
        writer.writeTextElement("eyes_above", QString("0"));
    writer.writeCharacters("\n\t");
    for(int i = 0; i < m_numberOfEyes; i++)
    {
	writer.writeStartElement("eye");
	writer.writeCharacters("\n\t\t");
	writer.writeTextElement("x", QString("%1")
		.arg(eyesList[i]->pos().x()));
	writer.writeCharacters("\n\t\t");
	writer.writeTextElement("y", QString("%1")
		.arg(eyesList[i]->pos().y()));
	writer.writeCharacters("\n\t\t");
	writer.writeTextElement("width", QString("%1")
		.arg(eyesList[i]->size().width()));
	writer.writeCharacters("\n\t\t");
	writer.writeTextElement("height", QString("%1")
		.arg(eyesList[i]->size().height()));
	writer.writeCharacters("\n\t\t");
	writer.writeTextElement("color", eyesList[i]->eyeColor());
	writer.writeCharacters("\n\t");
	writer.writeEndElement();
    }
    writer.writeCharacters("\n\t");
    writer.writeTextElement("comment", m_comment);
    writer.writeCharacters("\n");
    writer.writeEndElement();

    faceFile.close();

    return 0;
}
