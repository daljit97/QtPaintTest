#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QQmlApplicationEngine>
#include "drawingcanvas.h"

class MyWidget : public QWidget{
public:
    MyWidget(){

    }

protected:
    void paintEvent(QPaintEvent* event) override{
        QPainter painter(this);
//        auto l1 = QPointF(20,20);
//        auto l2 = QPointF(100, 100);

//        auto w = 15; // rectangle width
//        QLineF line = QLineF(l1, l2);
//        QRectF rect = QRectF(0, -w/2, line.length(), w); // the rectangle will have the same width as the line's length
//        painter.translate(line.p1());
//        painter.rotate(-line.angle()); // rotate the rectangle

//        rect.setWidth(line.length());
//        rect.setHeight(w);

//        painter.setPen(Qt::NoPen);
//        painter.setBrush(Qt::blue);
//        painter.drawRoundedRect(rect, 20,20);

//        painter.resetTransform();
//        painter.setPen(Qt::red);
//        painter.drawLine(line); // draw the line
//        painter.drawEllipse(l1, 10,10); // draw the origin
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        for (qreal i = 5; i <= 200; i += 0.5){
            QPointF centerPoint(i, i*i/10);
            qreal centerRadius = 5;

            QRadialGradient radialGrad(centerPoint, centerRadius);
            radialGrad.setColorAt(0.000, QColor(0, 0, 0, 255));
            radialGrad.setColorAt(0.5, QColor(0, 0, 0, 0.8 * 255));
            radialGrad.setColorAt(1.000, QColor(0, 0, 0, 0.000));

            QPen pen;
            pen.setWidth(400);
            pen.setColor("black");
            pen.setBrush(radialGrad);

            painter.setPen(pen);
            painter.drawPoint(centerPoint);

        }
    }
};

//int main(int argc, char *argv[])
//{
//    QApplication app(argc, argv);
//    MyWidget widget;
//    widget.resize(QSize(1000,1000));
//    widget.show();
//    return app.exec();
//}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qmlRegisterType<DrawingCanvas>("Drawing", 1, 0, "DrawingCanvas");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
