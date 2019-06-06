#include "renderarea.h"
#include <QPainter>
#include <QPaintEvent>
#include <math.h>
RenderArea::RenderArea(QWidget *parent) :
    QWidget(parent),
    mBackgroundColor (0, 0, 255),
    mPen(Qt::white),
    mShape (Circle)

{
    mPen.setWidth(2);
    on_shape_changed();
}
QSize RenderArea::minimumSizeHint() const
{
    return QSize(400,400);
}
QSize RenderArea::sizeHint() const
{
    return QSize(400,400);
}
void RenderArea::on_shape_changed(){
    switch (mShape){
        case Astroid:
            mScale = 40;
            mIntervalLength = 2 * M_PI;
            mStepCount = 256;
            break;
        case Cycloid:
            mScale = 4;
            mIntervalLength = 6 * M_PI;
            mStepCount = 128;
            break;
        case HuygensCycloid:
            mScale = 4;
            mIntervalLength = 4 * M_PI;
            mStepCount = 256;
            break;
        case HypoCycloid:
            mScale = 15;
            mIntervalLength = 2 * M_PI;
            mStepCount = 256;
            break;
         case Line:
            mIntervalLength = 1;
            mScale = 100;
            mStepCount = 128;
            break;
        case Circle:
           mIntervalLength = 2 * M_PI;
           mScale = 165;
           mStepCount = 128;
            break;
        case Ellipse:
            mIntervalLength = 2 * M_PI;
            mScale = 75;
            mStepCount = 256;
        break;
        case Fancy:
            mIntervalLength = 12 * M_PI;
            mScale = 10;
            mStepCount = 512;
        break;
        case Starfish:
            mIntervalLength = 6 * M_PI;
            mScale = 25;
            mStepCount = 256;
    break;
        default:
            break;
        }
}
QPointF RenderArea::compute(float t)
{
    switch (mShape){
        case Astroid:
            return compute_astroid(t);
            break;
        case Cycloid:
            return compute_cycloid(t);
            break;
        case HuygensCycloid:
            return compute_huygens(t);
            break;
        case HypoCycloid:
            return compute_hypo(t);
            break;
        case Line:
            return compute_line(t);
        case Circle:
            return compute_circle(t);
            break;
        case Ellipse:
            return compute_ellipse(t);
        case Fancy:
            return compute_fancy(t);
            break;
        case Starfish:
            return compute_starfish(t);
            break;
        default:
            break;
        }
    return QPointF(0, 0);
}
QPointF RenderArea::compute_astroid(float t){
    float cos_t = cos(t);
    float sin_t = sin(t);
    float x = 2 * cos_t * cos_t * cos_t;
    float y = 2 * sin_t * sin_t * sin_t;
    return QPointF(x, y);
}
QPointF RenderArea::compute_cycloid(float t){
    return QPointF(
           ( 1.5 * (1 - cos(t) )),
           ( 1.5 * (t - sin(t) ))
                );

}
QPointF RenderArea::compute_huygens(float t){

    return QPointF(
            4 * (3 * cos(t) - cos(3 * t) ),
            4 * (3 * sin(t) - sin(3 * t) )
                );
}
QPointF RenderArea::compute_hypo(float t){
    return QPointF(
               1.5 * ( 2 * cos(t) + cos(2 * t)),
               1.5 * ( 2 * sin(t) - sin(2 * t))
                );

}
QPointF RenderArea::compute_line(float t){
    return QPointF(1 - t, 1 - t);

}
QPointF RenderArea::compute_circle(float t){
    return QPointF(cos(t),sin(t));
}
QPointF RenderArea::compute_ellipse(float t){
    float a = 2;
    float b = 1.1;
    return QPointF(a * cos(t),b * sin(t));
}
QPointF RenderArea::compute_fancy(float t)
{
    float a = 11;
    float b = 6;
    float y = (a * sin(t) - b * sin((a/b)*t));
    float x = (a * cos(t) - b * cos((a/b)*t));
    return QPointF(x, y);

}
QPointF RenderArea::compute_starfish(float t)
{
    float R = 5,r = 3,d = 5;
    float c = R-r;float e = (R-r)/r;
    float x = c * cos(t) + d * cos(t * e);
    float y = c * sin(t) - d * sin(t * e);
    return QPointF(x, y);

}
void RenderArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true );

    painter.setBrush(mBackgroundColor);
    painter.setPen(mPen);

    //Drawing area
    painter.drawRect(this->rect());
    QPoint center = this->rect().center();
    QPoint prevPixel;
    QPointF prevPoint = compute(0);
    prevPixel.setX(prevPoint.x() * mScale + center.x());
    prevPixel.setY(prevPoint.y() * mScale + center.y());
    float step = mIntervalLength / mStepCount;

    for(float t = 0; t < mIntervalLength ; t+=step)
    {
        QPointF point = compute(t);
        QPoint pixel;
        pixel.setX(point.x() * mScale + center.x());
        pixel.setY(point.y() * mScale + center.y());

        painter.drawLine(pixel, prevPixel);
        prevPixel = pixel;
    }
    QPointF point = compute(mIntervalLength);
    QPoint pixel;
    pixel.setX(point.x() * mScale + center.x());
    pixel.setY(point.y() * mScale + center.y());

    painter.drawLine(pixel, prevPixel);

}
