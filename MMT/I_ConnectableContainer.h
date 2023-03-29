#ifndef I_CONNECTABLECONTAINER_H
#define I_CONNECTABLECONTAINER_H

#include "I_Connectable.h"
#include "I_Connector.h"

#include "I_Serializable.h"

class I_ConnectableContainer:
        public I_Serializable
{
public:
    virtual QList<I_Connector*> getAllConnectors() const = 0;

    virtual QList<I_Selectable*> getAllSelectables(bool p_recursive=false) const;
    virtual const QList<I_Selectable*>& getPureSelectables() const;
    virtual I_Selectable* getSelectableFromTypeAndID(const QString& p_ConnectableType,
                                                     const QUuid& p_SelectableID) const;
    virtual QList<I_Connectable*> getAllConnectables(bool p_recursive=false) const;
    virtual const QList<I_Connectable*>& getPureConnectables() const;
    virtual const I_Connectable* getConnectableFromTypeAndID(const QString& p_ConnectableType,
                                                             const QUuid& p_ConnectableID) const;

    virtual I_Selectable* getSelectable(QPoint p_Pos) const;
    virtual I_Selectable* getSelectableAt(unsigned int p_index) const;
    virtual I_Connectable* getConnectable(QPoint p_Pos) const;
    virtual I_Connectable* getConnectableAt(unsigned int p_index) const;
    virtual void addSelectable(I_Selectable* p_Selectable);
    virtual void addConnectable(I_Connectable* p_Connectable);
    virtual void deleteSelectableAt(unsigned int p_index);
    virtual void deleteConnectableAt(unsigned int p_index);
    virtual bool isItYourChild(const I_Connectable* p_Connectable) const;

    virtual void clearAll()
    {
        while(0U != m_Selectables.count())
        {
            delete m_Selectables.takeAt(0U);
        }
        while(0U != m_Connectables.count())
        {
            delete m_Connectables.takeAt(0U);
        }
    }

    I_Connectable* selectConnectable(QPoint p_Pos, eConnectionPoint p_ConnectPoint) const
    {
        // Iterate through item groups to find out witch one has been selected
        I_Connectable* l_Connectable = nullptr;

        unsigned short l_Plan = MAX_PLAN + 10U; // More than the max

        QList<I_Connectable*> l_LocalConnectables = this->getAllConnectables();

        for( unsigned int i_selectable=0U; i_selectable < l_LocalConnectables.count(); i_selectable++ )
        {
            if( (l_Plan > l_LocalConnectables[i_selectable]->getPlan()) &&
                    (true == l_LocalConnectables[i_selectable]->isItYou(p_Pos)) )
            {
                l_Plan = l_LocalConnectables[i_selectable]->getPlan();
                l_Connectable = l_LocalConnectables[i_selectable];
            }
        }

        if( nullptr != l_Connectable )
        {
            if( false == l_Connectable->selectForConnection(p_Pos, p_ConnectPoint) )
            {
                l_Connectable = nullptr;
            }
        }
        else
        {
            // Not found, do nothing
        }

        return l_Connectable;
    }

    void rerouteConnectableConnections(I_Connectable* p_Connectable)
    {
        QList<I_Connector*> l_Connectors = this->getAllConnectors();

        for(unsigned int i_connectors = 0U;
            i_connectors < l_Connectors.count();
            i_connectors++ )
        {
            if( l_Connectors[i_connectors]->needReroute() )
            {
                l_Connectors[i_connectors]->reroute();
            }
        }
    }

    QList<I_Selectable*> getCurrentSelected() const
    {
        QList<I_Selectable*> l_Ret;

        for( unsigned int i_itemsSelected = 0U; i_itemsSelected < m_CurrentSelectedType.count(); i_itemsSelected++ )
        {
                l_Ret.append(this->getSelectableFromTypeAndID(
                                 m_CurrentSelectedType[i_itemsSelected],
                                 m_CurrentSelectedID[i_itemsSelected]));
        }

        return l_Ret;
    }
    void setCurrentSelected(const I_Selectable* p_NewSelection)
    {
        if( nullptr != p_NewSelection )
        {
            m_CurrentSelectedType.append(p_NewSelection->getSelectableType());
            m_CurrentSelectedID.append(p_NewSelection->getID());
        }
        else
        {
            // Nullptr, do nothing
        }
    }
    const I_Connectable* getFromConnectable()
    {
        return this->getConnectableFromTypeAndID(m_FromConnectableType, m_FromConnectableID);
    }
    void setFromConnectable(I_Connectable* p_FromConnectable)
    {
        if( nullptr != p_FromConnectable )
        {
            m_FromConnectableType = p_FromConnectable->getConnectableType();
            m_FromConnectableID = p_FromConnectable->getID();
        }
        else
        {
            m_FromConnectableType = "";
        }
    }

    QPoint getSelectionCoord();
    void unselectAll();
    virtual QJsonObject toJson() const;
private:
    QList<QString> m_CurrentSelectedType;
    QList<QUuid> m_CurrentSelectedID;

    QString m_FromConnectableType;
    QUuid m_FromConnectableID;

    QList<I_Selectable*> m_Selectables;
    QList<I_Connectable*> m_Connectables;
};

#endif // I_CONNECTABLECONTAINER_H
