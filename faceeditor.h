#ifndef FACEEDITOR_H
#define FACEEDITOR_H

#include <QtGui>

class Eye;
class Face;

class EyeGraphicsViewItem : public QGraphicsEllipseItem
{
public:
    EyeGraphicsViewItem(Eye eye);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                                                        QWidget *widget);
    QPixmap image();
private:
    Eye m_eye;
    QPixmap pupilPixmap;
};

class FaceEditor : public QDialog
{
public:
    FaceEditor(Face face, QWidget *parent = 0);

    void loadFaces();

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QGraphicsPixmapItem *facePixmapItem;


    Face m_face;
};

#endif // FACEEDITOR_H
