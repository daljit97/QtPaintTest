    #ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QOpenGLContext>

#include <memory>

struct Outline{
    QPolygonF points;

    void addPoint(QPointF p){
        points.append(p);
    }
    void clear(){
        points.clear();
    }
    // We need the operator== to compare 2 Outline types.
    bool operator==(const Outline &outline) const {
        return outline.points == points;
    }
};

class DrawingCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* window READ window WRITE setWindow NOTIFY windowChanged)
    Q_PROPERTY(bool drawing READ drawing WRITE setDrawing NOTIFY drawingChanged)
    Q_PROPERTY(int penWidth READ penWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(QString penColor READ penColor WRITE setPenColor NOTIFY penColorChanged)

public:
    explicit DrawingCanvas(QQuickItem *parent = nullptr);
    bool drawing() const;
    Q_INVOKABLE void initiateBuffer();

    Q_INVOKABLE void penPressed(QPointF pos);
    Q_INVOKABLE void penMoved(QPointF pos);
    Q_INVOKABLE void penReleased();
    int penWidth() const;

    void paint(QPainter *painter);

    QString penColor() const;


    QQuickWindow* window() const
    {
        return m_window;
    }

public slots:
    void setDrawing(bool drawing);

    void setPenWidth(int penWidth);

    void setPenColor(QString penColor);

    void setWindow(QQuickWindow* window)
    {
        if (m_window == window)
            return;

        m_window = window;
        emit windowChanged(m_window);
    }

signals:
    void drawingChanged(bool drawing);
    void penWidthChanged(int penWidth);
    void penColorChanged(QString penColor);

    void windowChanged(QQuickWindow* window);

private:
    void drawOnBuffer(QPointF pos);

    bool m_drawing;
    QImage m_buffer;
    QOpenGLContext context;
    int m_penWidth;
    QString m_penColor;

    QPointF m_lastPoint;
    Outline m_currentOutline;
    QRect m_updateRect;
    QVector<Outline> m_outlines;


    bool m_outlineEraser;
    QQuickWindow* m_window;
};

#endif // DRAWINGCANVAS_H
