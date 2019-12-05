#include "drawingcanvas.h"

#include <QPainter>

DrawingCanvas::DrawingCanvas(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    m_penWidth = 4;
}

bool DrawingCanvas::drawing() const
{
    return m_drawing;
}

void DrawingCanvas::penPressed(QPointF pos)
{
    setDrawing(true);
    m_currentOutline.addPoint(pos);
    m_lastPoint = pos;

}

void DrawingCanvas::penMoved(QPointF pos)
{
    if(drawing()){
        m_currentOutline.addPoint(pos);
        // draw the points on the buffer
        drawOnBuffer(pos);
    }
    m_lastPoint = pos;
}

void DrawingCanvas::penReleased()
{
    setDrawing(false);
    m_outlines.append(m_currentOutline);
    m_currentOutline.clear();
    m_lastPoint = QPointF();
}

void DrawingCanvas::paint(QPainter *painter)
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPen pen;
    pen.setWidth(penWidth());
    pen.setColor(penColor());

    painter->setPen(pen);
    painter->drawPixmap(m_updateRect, m_buffer, m_updateRect);

    m_updateRect = QRect();
}

QString DrawingCanvas::penColor() const
{
    return m_penColor;
}

int DrawingCanvas::penWidth() const
{
    return m_penWidth;
}

void DrawingCanvas::setDrawing(bool drawing)
{
    if (m_drawing == drawing)
        return;

    m_drawing = drawing;
    emit drawingChanged(m_drawing);
}

void DrawingCanvas::setPenWidth(int penWidth)
{
    if (m_penWidth == penWidth)
        return;

    m_penWidth = penWidth;
    emit penWidthChanged(m_penWidth);
}

void DrawingCanvas::setPenColor(QString penColor)
{
    if (m_penColor == penColor)
        return;

    m_penColor = penColor;
    emit penColorChanged(m_penColor);
}


void DrawingCanvas::initiateBuffer()
{
    qDebug() << this << "Initiating buffer" << width() << height();
    m_buffer = QPixmap(width(), height());
}

void DrawingCanvas::drawOnBuffer(QPointF pos)
{
    QPainter bufferPainter;
    if(bufferPainter.begin(&m_buffer)){
        QPen pen;
        pen.setWidth(penWidth());
        pen.setColor(penColor());

        bufferPainter.setPen(pen);
        bufferPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        // this will help smoothing the curves
        int pointsLength = m_currentOutline.points.length();

        QPainterPath path;
        if(pointsLength > 2){
            auto previousPoint = m_currentOutline.points.at(pointsLength - 3);

            auto mid1 = (m_lastPoint + previousPoint)/2;
            auto mid2 = (pos + m_lastPoint)/2;

            path.moveTo(mid1);
            path.quadTo(m_lastPoint, mid2);
            bufferPainter.drawPath(path);
        }
        // update the canvas
        int rad = (penWidth() / 2) + 2;

        auto dirtyRect = path.boundingRect().toRect().normalized()
                .adjusted(-rad, -rad, +rad, +rad);

        // change the canvas dirty region
        if(m_updateRect.isNull()){
            m_updateRect = dirtyRect;
        }
        else{
            m_updateRect = m_updateRect.united(dirtyRect);
        }
        update(dirtyRect);

        m_lastPoint = pos;
    }
    else{
        qDebug() << this << "Error initiating buffer painter";
    }
}
