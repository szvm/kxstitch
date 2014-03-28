/*
 * Copyright (C) 2010 by Stephen Allewell
 * sallewell@users.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "Preview.h"

#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QStyleOptionRubberBand>

#include "configuration.h"
#include "Document.h"
#include "Renderer.h"


Preview::Preview(QWidget *parent)
    :   QWidget(parent),
        m_document(0),
        m_renderer(new Renderer()),
        m_zoomFactor(1.0)
{
    setObjectName("Preview#");
    m_renderer->setRenderStitchesAs(Configuration::EnumRenderer_RenderStitchesAs::ColorBlocks);
    m_renderer->setRenderBackstitchesAs(Configuration::EnumRenderer_RenderBackstitchesAs::ColorLines);
    m_renderer->setRenderKnotsAs(Configuration::EnumRenderer_RenderKnotsAs::ColorBlocks);
}


Preview::~Preview()
{
    delete m_renderer;
}


void Preview::setDocument(Document *document)
{
    m_document = document;
    readDocumentSettings();
}


Document *Preview::document()
{
    return m_document;
}


void Preview::readDocumentSettings()
{
    int width = m_document->pattern()->stitches().width();
    int height = m_document->pattern()->stitches().height();
    m_cellWidth = 4;
    m_cellHeight = 4 * m_document->property("horizontalClothCount").toDouble() / m_document->property("verticalClothCount").toDouble();
    m_previewWidth = m_cellWidth * width * m_zoomFactor;
    m_previewHeight = m_cellHeight * height * m_zoomFactor;
    resize(m_previewWidth, m_previewHeight);
}


void Preview::setVisibleCells(const QRect &cells)
{
    m_visible = cells;
    update();
}


void Preview::loadSettings()
{
    update();
}


void Preview::mousePressEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_start = m_tracking = m_end = QPoint(e->pos().x() / m_cellWidth, e->pos().y() / m_cellHeight);
    }
}


void Preview::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        m_tracking = QPoint(e->pos().x() / m_cellWidth, e->pos().y() / m_cellHeight);

        if (m_tracking != m_start) {
            QRect updateArea = QRect(m_start * m_cellWidth, m_end * m_cellWidth).normalized();
            m_end = m_tracking;
            m_rubberBand = QRect(m_start, m_end).normalized();
            update(updateArea.united(QRect(m_rubberBand.left() * m_cellWidth, m_rubberBand.top() * m_cellHeight, m_rubberBand.width() * m_cellWidth, m_rubberBand.height() * m_cellHeight)));
        }
    }
}


void Preview::mouseReleaseEvent(QMouseEvent *)
{
    if (m_start == m_end) {
        emit clicked(m_start);
    } else {
        emit clicked(QRect(m_start, m_end).normalized());
    }

    QRect updateArea = QRect(m_rubberBand.left() * m_cellWidth, m_rubberBand.height() * m_cellHeight, m_rubberBand.width() * m_cellWidth, m_rubberBand.height() * m_cellHeight);
    m_rubberBand = QRect();
    update(updateArea);
}


void Preview::paintEvent(QPaintEvent *e)
{
    if (m_document == 0) {
        return;
    }

    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(e->rect(), m_document->property("fabricColor").value<QColor>());
    painter.setWindow(0, 0, m_document->pattern()->stitches().width(), m_document->pattern()->stitches().height());

    m_renderer->render(&painter, m_document->pattern(), e->rect(), false, true, true, true, -1);

    if (m_visible.width() * m_cellWidth < m_previewWidth || m_visible.height() * m_cellHeight < m_previewHeight) {
        painter.setPen(Qt::white);
        painter.setBrush(Qt::NoBrush);
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.drawRect(m_visible);
    }

    if (m_rubberBand.isValid()) {
        QStyleOptionRubberBand opt;
        opt.initFrom(this);
        opt.shape = QRubberBand::Rectangle;
        opt.opaque = false;
        opt.rect = m_rubberBand;
        style()->drawControl(QStyle::CE_RubberBand, &opt, &painter);
    }

    painter.end();
}
