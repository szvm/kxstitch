/***************************************************************************
 *   Copyright (C) 2009 by Stephen Allewell                                *
 *   stephen@mirramar.fsnet.co.uk                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


/*
struct LIBRARY (original)
{
  Q_UINT32 patterns;
  [
    Q_UINT16 checksum
    QByteArray compressedDataStream
    [
      QString scheme
      Q_INT32 width
      Q_INT32 height
      [
        Q_INT8 count
        Q_INT8 type
        Q_UINT32 color
      ] height x width
      Q_INT32 backstitches
      [
        QPoint start
        QPoint end
        Q_INT32 color
      ] backstitches
      Q_INT32 knots
      [
        QPoint start
        Q_INT32 color
      ] knots
    ]
  ] patterns
]

struct LIBRARY (new)
[
  "KXStitchLib"
  Q_INT16 version
  [
    Q_UINT16 key
    Q_UINT16 modifier
    Q_INT16 baseline
    Q_UINT16 checksum
    QByteArray compressedDataStream
    [
      QString scheme
      Q_INT32 width
      Q_INT32 height
      [
        Q_INT8 count
        Q_INT8 type
        Q_UINT32 color
      ] height x width
      Q_INT32 backstitches
      [
        QPoint start
        QPoint end
        Q_INT32 color
      ] backstitches
      Q_INT32 knots
      [
        QPoint start
        Q_INT32 color
      ] knots
    ]
  ]
]
*/

#include <QTreeWidget>

#include <KStandardDirs>

#include "LibraryPattern.h"
#include "LibraryTreeWidgetItem.h"


LibraryTreeWidgetItem::LibraryTreeWidgetItem(QTreeWidget *parent, const QString &name)
	:	QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
//	setDropEnabled(true);
	setText(0, name);
}


LibraryTreeWidgetItem::LibraryTreeWidgetItem(LibraryTreeWidgetItem *parent, const QString &name)
	:	QTreeWidgetItem(parent, QTreeWidgetItem::UserType)
{
//	setDropEnabled(true);
	setText(0, name);
}


int LibraryTreeWidgetItem::maxHeight()
{
	int max = 0;
	for (LibraryPattern *libraryPattern = first() ; libraryPattern ; libraryPattern = next())
	{
		max = std::max(max, libraryPattern->stitchData().height());
	}
	return max;
}


LibraryPattern *LibraryTreeWidgetItem::findCharacter(int key, Qt::KeyboardModifiers modifiers)
{
	LibraryPattern *libraryPattern;
	for (libraryPattern = first() ; libraryPattern ; libraryPattern = next())
	{
		if (libraryPattern->key() == key && libraryPattern->modifiers() == modifiers)
			break;
	}
	return libraryPattern;
}


LibraryPattern *LibraryTreeWidgetItem::first()
{
	LibraryPattern *libraryPattern = 0;
	m_libraryFilesIndex = 0;
	if (!m_libraryFiles.isEmpty())
	{
		libraryPattern = m_libraryFiles[0]->first();
		if (libraryPattern == 0) // the first file is empty
		{
			libraryPattern = next();
		}
	}
	return libraryPattern;
}


LibraryPattern *LibraryTreeWidgetItem::next()
{
	LibraryPattern *libraryPattern = 0;
	while (libraryPattern == 0 && !m_libraryFiles.isEmpty() && m_libraryFilesIndex != m_libraryFiles.count())
	{
		libraryPattern = m_libraryFiles[m_libraryFilesIndex]->next();
		if (libraryPattern == 0)    // reached the end of the current file
		{
			if (++m_libraryFilesIndex < m_libraryFiles.count())
			{
				libraryPattern = m_libraryFiles[m_libraryFilesIndex]->first();  // get the first pattern of the next file
			}
		}
	}
	return libraryPattern;
}


void LibraryTreeWidgetItem::addPath(const QString &path)
{
	m_libraryFiles.append(new LibraryFile(path));
}


QString LibraryTreeWidgetItem::path()
{
	return m_libraryFiles.first()->path();
}


QStringList LibraryTreeWidgetItem::paths()
{
	QStringList paths;
	QListIterator<LibraryFile *> libraryFilesIterator(m_libraryFiles);
	while (libraryFilesIterator.hasNext())
	{
		paths.append(libraryFilesIterator.next()->localFile());
	}
	return paths;
}


LibraryFile *LibraryTreeWidgetItem::writablePath()
{
	LibraryFile *libraryFile = 0;
	QListIterator<LibraryFile *> libraryFilesIterator(m_libraryFiles);
	while (libraryFilesIterator.hasNext())
	{
		libraryFile = libraryFilesIterator.next();
		if (!libraryFile->isWritable())
			return libraryFile;
	}
	// create a writable path
	QString path = m_libraryFiles[0]->path();
	path.replace(0, path.indexOf("library"), "");
	path = KGlobal::dirs()->saveLocation("appdata", path);
	addPath(path);
	libraryFile = m_libraryFiles.last();
	return libraryFile;
}


#if 0
void LibraryTreeWidgetItem::addPattern(KXStitchDragObject *libraryPattern)
{
	writablePath()->addPattern(libraryPattern);
}
#endif


void LibraryTreeWidgetItem::addPattern(LibraryPattern *libraryPattern)
{
	writablePath()->addPattern(libraryPattern);
}


#if 0
void LibraryTreeWidgetItem::deletePattern(PreviewIconViewItem *item)
{
	QListIterator<LibraryFile *> libraryFileIterator(m_libraryFiles);
	while (libraryFileIterator.hasNext())
	{
		LibraryFile *libraryFile = libraryFileIterator.next();
		for (LibraryPattern *libraryPattern = libraryFile->first() ; libraryPattern ; libraryPattern = libraryFile->next())
		{
			if (libraryPattern->iconViewItem() == item)
				libraryFile->deletePattern(pattern);
		}
	}
}
#endif