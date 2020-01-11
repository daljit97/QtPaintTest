#include "drawingcanvas.h"

#include <QPainter>
#include <QtSvg/QSvgGenerator>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QSGGeometryNode>
#include <QSGTexture>
#include <QSGSimpleTextureNode>
#include <math.h>

DrawingCanvas::DrawingCanvas(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    m_penWidth = 4;
    m_glBuffer = nullptr;
    //    setRenderTarget(FramebufferObject);
}

bool DrawingCanvas::drawing() const
{
    return m_drawing;
}

void DrawingCanvas::penPressed(QPointF pos)
{
    if(!context.isValid()){
        context.create();
    }
    context.makeCurrent(window());
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
    context.doneCurrent();
}

void DrawingCanvas::paint(QPainter *painter)
{
    //    auto l1 = QPointF(100,100);
    //    auto l2 = QPointF(500, 800);

    //    auto w = 30; // rectangle width
    //    QLineF line = QLineF(l1, l2);
    //    QRectF rect = QRectF(0, -w/2, line.length(), w); // the rectangle will have the same width as the line's length
    //    painter->translate(line.p1());
    //    painter->rotate(-line.angle()); // rotate the rectangle

    //    rect.setWidth(line.length());
    //    rect.setHeight(w);

    //    painter->setPen(Qt::NoPen);
    //    painter->setBrush(Qt::blue);
    //    painter->drawRect(rect);

    //    painter->resetTransform();
    //    painter->setPen(Qt::red);
    //    painter->drawLine(line); // draw the line
    //    painter->drawEllipse(l1, 10,10); // draw the origin

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

void DrawingCanvas::saveSvg()
{
    QString path = "/home/casa78/Pictures/image.svg";

    QSvgGenerator generator;
    generator.setFileName("/home/casa78/Pictures/image.svg");
    generator.setSize(QSize(width(), height()));
    generator.setViewBox(QRect(0, 0, width(), height()));
    generator.setTitle(tr("SVG Generator Example Drawing"));
    generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                                "Example provided with Qt."));
    QPainter painter(&generator);
    QPen pen;
    pen.setWidth(penWidth());
    pen.setColor(penColor());
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);
    for (auto o : m_outlines){
        auto points = o.points;

        for(int j = 0; j < points.size(); j++){
            if(j > 2){
                QPainterPath path;
                auto previousPoint = points.at(j - 2);
                auto lastPoint = points.at(j-1);
                auto mid1 = (lastPoint + previousPoint)/2;
                auto mid2 = (points.at(j) + lastPoint)/2;

                path.moveTo(mid1);
                path.quadTo(lastPoint, mid2);


                painter.drawPath(path);
            }
        }
    }

    painter.end();
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
    int rad = (penWidth()) + 500;
    //    QElapsedTimer timer;
    //    timer.start();
    QPainter bufferPainter;
    if(bufferPainter.begin(&m_buffer)){
        //        bufferPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        bufferPainter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        // this will help smoothing the curves
        int pointsLength = m_currentOutline.points.length();
        QPixmap image= QPixmap("/home/casa78/QtPaintTest/ball.png").scaledToWidth(penWidth(), Qt::SmoothTransformation);
        QPainterPath path;
        if(pointsLength > 2){
            auto previousPoint = m_currentOutline.points.at(pointsLength - 3);

            auto mid1 = (m_lastPoint + previousPoint)/2;
            auto mid2 = (pos + m_lastPoint)/2;

            path.moveTo(m_lastPoint);
            path.lineTo(pos);

            qreal centerRadius = 5;
            auto pathLength = path.length();

            // 2px long
            // 4px radius
            //
            qreal step = centerRadius*2/pathLength/3;
            qDebug() << pathLength << centerRadius << step;
            qDebug() << step;
            for (qreal i = 0.0; i <= 1; i+=step){
                QPointF centerPoint = path.pointAtPercent(i);

                QRadialGradient radialGrad(centerPoint, centerRadius);
                radialGrad.setColorAt(0.000, QColor(0, 0, 0, 255));
                radialGrad.setColorAt(0.5, QColor(0, 0, 0, 0.8 * 255));
                radialGrad.setColorAt(1.000, QColor(0, 0, 0, 0.000));

                QPen pen;
                pen.setWidth(400);
                pen.setColor("black");
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
