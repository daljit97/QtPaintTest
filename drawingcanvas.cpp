#include "drawingcanvas.h"

#include <QPainter>
#include <QPainterPath>
#include <QtSvg/QSvgGenerator>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QOpenGLFramebufferObject>
#include <math.h>

DrawingCanvas::DrawingCanvas(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    m_penWidth = 4;
        setRenderTarget(FramebufferObject);
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
    painter->drawImage(m_updateRect, m_buffer, m_updateRect);
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
    m_buffer = QImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    m_buffer.fill(Qt::transparent);
}



void DrawingCanvas::drawOnBuffer(QPointF pos)
{
    int rad = (penWidth()) + 50;

    QPainter bufferPainter;
    if(bufferPainter.begin(&m_buffer)){
        bufferPainter.setRenderHints(QPainter::SmoothPixmapTransform);
        bufferPainter.setRenderHint(QPainter::Antialiasing);
        bufferPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        int pointsLength = m_currentOutline.points.length();

        QPainterPath path;
        if(pointsLength > 2){
            auto previousPoint = m_currentOutline.points.at(pointsLength - 3);

            auto mid1 = (m_lastPoint + previousPoint)/2;
            auto mid2 = (pos + m_lastPoint)/2;

            path.moveTo(m_lastPoint);
            path.lineTo(pos);

            qreal centerRadius = 1.5;

            auto pathLength = path.length();
            qreal step = std::min(centerRadius*2/(pathLength)/10, 1.0);
            for (qreal i = 0.0; i <= 1; i+=step){
                QPointF centerPoint = path.pointAtPercent(i);

                QRadialGradient radialGrad(centerPoint, centerRadius);
                radialGrad.setColorAt(0.000, QColor(0, 0, 0, 255));
                radialGrad.setColorAt(0.2, QColor(0, 0, 0, 0.2 * 255));
                radialGrad.setColorAt(1.000, QColor(0, 0, 0, 0));

                QPen pen;
                pen.setCapStyle(Qt::RoundCap);
                pen.setJoinStyle(Qt::RoundJoin);
                pen.setWidth(centerRadius * 2);
                pen.setBrush(radialGrad);

                bufferPainter.setPen(pen);
                bufferPainter.drawPoint(centerPoint);
            }
        }
        // update the canvas

        auto dirtyRect = path.boundingRect().toRect().normalized()
                .adjusted(-rad, -rad, +rad, +rad);

        // change the canvas dirty region
        if(m_updateRect.isNull()){
            m_updateRect = dirtyRect;
        }
        else{
            m_updateRect = m_updateRect.united(dirtyRect);
        }
        update(m_updateRect);

        m_lastPoint = pos;
    }
    else{
        qDebug() << this << "Error initiating buffer painter";
    }
    //    qDebug() << "DRAWING IN " << timer.elapsed() << "ms";
}
