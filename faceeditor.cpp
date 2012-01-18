
#include "global.h"
#include "faceeditor.h"

EyeGraphicsViewItem::EyeGraphicsViewItem(Eye eye)
    : m_eye(eye)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    setRect(0, 0,
            m_eye.size().width(), m_eye.size().height());
    //m_eye = eye;
}

QRectF EyeGraphicsViewItem::boundingRect() const
{
    return QRectF(0, 0,
                  m_eye.size().width(), m_eye.size().height());
}

void EyeGraphicsViewItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    m_eye.render(painter, QPoint(0, 0), QPoint(0, 0));
}

QPixmap EyeGraphicsViewItem::image()
{
    QPixmap pixmap(m_eye.size());
     //pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    m_eye.render(&painter, QPoint(0, 0), QPoint(0, 0));

    return pixmap;
}

FaceEditor::FaceEditor(Face face, QWidget *parent)
{

    m_face = face;
    scene = new QGraphicsScene(0, 0, m_face.facePixmap().size().width(),
                                     m_face.facePixmap().size().height());

    scene->setBackgroundBrush(QPixmap(":Images/transparency.png"));
    facePixmapItem = new QGraphicsPixmapItem();
    facePixmapItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    loadFaces();

    view = new QGraphicsView(scene);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(view);
    setLayout(layout);
}

void FaceEditor::loadFaces()
{
    scene->clear();

    facePixmapItem->setPixmap(m_face.facePixmap());
    scene->addItem(facePixmapItem);

    for(int i = 0; i < m_face.numberOfEyes(); i++)
    {
        EyeGraphicsViewItem *eyeItem = new EyeGraphicsViewItem(*m_face.eyesList[i]);
        if(m_face.eyesAbove() == FALSE)
            eyeItem->setZValue(-10);
        else
            eyeItem->setZValue(10);

        scene->addItem(eyeItem);
    }
}
