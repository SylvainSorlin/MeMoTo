#include <QPen>

#include <Engine/I_DiagramContainer.h>
#include <CommonGraphics/ConfigWidget.h>
#include <GraphicsItems/GeneralizationGraphicsItem.h>
#include <ConfigurationContexts/GeneralizationConfiguration.h>

const char* GeneralizationGraphicsItem::SERIALIZABLE_NAME="Generalizations";

static int generalizationStartReduction = 2;

GeneralizationGraphicsItem::GeneralizationGraphicsItem(
        const I_Connectable *p_ConnectFrom,
        const I_Connectable *p_ConnectTo,
        const QPoint &p_fromPoint,
        const QPoint &p_toPoint,
        I_ConnectableContainer* p_Container):
    GraphicConnector(p_ConnectFrom, p_ConnectTo, p_fromPoint, p_toPoint, p_Container)
  , m_Arrow()
{
    this->setStartReduction(generalizationStartReduction);
    GeneralizationGraphicsItem::refreshDisplay();
}

GeneralizationGraphicsItem::GeneralizationGraphicsItem(
        const I_Connectable *p_ConnectFrom,
        const I_Connectable *p_ConnectTo,
        const QPoint &p_fromPoint,
        const QPoint &p_toPoint,
        I_ConnectableContainer* p_Container,
        const QList<QPoint>& p_ForcedPath):
    GraphicConnector(p_ConnectFrom, p_ConnectTo, p_fromPoint, p_toPoint, p_Container, p_ForcedPath)
  , m_Arrow()
{
    this->setStartReduction(generalizationStartReduction);
    GeneralizationGraphicsItem::refreshDisplay();
}

GeneralizationGraphicsItem::GeneralizationGraphicsItem(const QJsonObject& p_JsonObject,
                                               I_ConnectableContainer* p_Container):
    GraphicConnector(p_JsonObject, p_Container)
  , m_Arrow()
{
    this->setStartReduction(generalizationStartReduction);
    GeneralizationGraphicsItem::refreshDisplay();
}

GeneralizationGraphicsItem::~GeneralizationGraphicsItem()
{

}

void GeneralizationGraphicsItem::reroute()
{
    GraphicConnector::reroute();

    this->refreshDisplay();
}

QString GeneralizationGraphicsItem::getSerializableName() const
{
    return SERIALIZABLE_NAME;
}

// I_Selectable overloads
void GeneralizationGraphicsItem::select()
{
    GraphicConnector::select();

    if( true == this->isFullySelected() )
    {
        this->openConfiguration();
    }
}
void GeneralizationGraphicsItem::select(QPoint p_Pos)
{
    GraphicConnector::select(p_Pos);

    if( true == this->isFullySelected() )
    {
        this->openConfiguration();
    }
}
void GeneralizationGraphicsItem::unselect()
{
    GraphicConnector::unselect();

    this->closeConfiguration();
}
QString GeneralizationGraphicsItem::getSelectableType() const
{
    return SERIALIZABLE_NAME;
}
QStringList GeneralizationGraphicsItem::getSearchFields() const
{
    return QStringList("ID");
}
QString GeneralizationGraphicsItem::getDataFromField(const QString& p_FieldName) const
{
    QString l_Ret;

    if( "ID" == p_FieldName )
    {
        l_Ret = this->getID().toString();
    }

    return l_Ret;
}

// I_Configurable
void GeneralizationGraphicsItem::openConfiguration()
{
    this->getConfig<GeneralizationConfiguration>().registerDiagram(this->getDiagramContainer());
    this->getConfig<GeneralizationConfiguration>().setListener(this->getSelectableType(), this->getID());

    this->getConfig<GeneralizationConfiguration>().setAutoRoute(!this->getPathIsForced());

    // Let's rock
    ConfigWidget::open(&this->getConfig<GeneralizationConfiguration>());
}
void GeneralizationGraphicsItem::applyConfiguration()
{
    this->setPathIsForced(!this->getConfig<GeneralizationConfiguration>().getAutoRoute());
    this->setCurved(this->getConfig<GeneralizationConfiguration>().getCurvedInfo());

    if( false == this->getPathIsForced() )
    {
        this->route(this->getPath().first(),
                    this->getPath().last());
    }

    this->getDiagramContainer()->changed(this);

    ConfigWidget::close();
}

void GeneralizationGraphicsItem::setupArrow()
{
    static const unsigned short ARROW_WIDTH = 10U;
    static const unsigned short ARROW_DEPTH = 10U;
    static const unsigned short GENERALIZATION_THICKNESS = 4U;
    static const Qt::GlobalColor GENERALIZATION_COLOR = Qt::black;

    // The middle point is the point in the middle of the tip, only for computation ease
    qreal l_lastLineAngle = this->getLastLineAngle();
    QPointF l_MiddlePoint;
    l_MiddlePoint.setX(this->getPath().last().x() - (qCos(l_lastLineAngle)*ARROW_DEPTH));
    l_MiddlePoint.setY(this->getPath().last().y() + (qSin(l_lastLineAngle)*ARROW_DEPTH));

    QList<QPointF> l_Points;
    // 3 points are needed to define the arrow
    l_Points.append(this->getPath().last()); // first point is end of line
    qreal l_currentPointAngle = l_lastLineAngle + (M_PI/2.0);
    l_Points.append(QPointF(l_MiddlePoint.x() + qCos(l_currentPointAngle)*ARROW_WIDTH,
                    l_MiddlePoint.y() - qSin(l_currentPointAngle)*ARROW_WIDTH));
    l_currentPointAngle += (M_PI);
    l_Points.append(QPointF(l_MiddlePoint.x() + qCos(l_currentPointAngle)*ARROW_WIDTH,
                    l_MiddlePoint.y() - qSin(l_currentPointAngle)*ARROW_WIDTH));

    for( unsigned short i_arrow=0U; i_arrow < m_Arrow.count(); i_arrow++ )
    {
        this->removeFromGroup(m_Arrow[i_arrow]);
        delete m_Arrow[i_arrow];
    }
    m_Arrow.clear();

    m_Arrow.append(new QGraphicsLineItem(QLineF(l_Points[0], l_Points[1])));
    m_Arrow.last()->setPen(QPen(GENERALIZATION_COLOR, GENERALIZATION_THICKNESS));
    this->addToGroup(m_Arrow.last());
    m_Arrow.append(new QGraphicsLineItem(QLineF(l_Points[0], l_Points[2])));
    m_Arrow.last()->setPen(QPen(GENERALIZATION_COLOR, GENERALIZATION_THICKNESS));
    this->addToGroup(m_Arrow.last());
}

void GeneralizationGraphicsItem::refreshDisplay()
{
    this->clear();

    GraphicConnector::refreshDisplay();

    this->setupArrow();
}

void GeneralizationGraphicsItem::clear()
{
    GraphicConnector::clear();

    for( unsigned short i_arrow=0U; i_arrow < m_Arrow.count(); i_arrow++ )
    {
        this->removeFromGroup(m_Arrow[i_arrow]);
        delete m_Arrow[i_arrow];
    }
    m_Arrow.clear();
}
