/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef SIMTAB
#define SIMTAB

#include "dmcompilersettings.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QGraphicsSimpleTextItem>
#include <QPainter>
#include <QWidget>

class GUISimulation;
class ModelNode;
namespace DM
{
class Group;
}

class DM_HELPER_DLL_EXPORT SimulationTab: public QGraphicsScene
{
	Q_OBJECT
private:
	// the drawing object
	QGraphicsView *viewer;

	// the scene, owning all elements to be rendered
	//QGraphicsScene *scene;
	QPointF cursorPos;
	
	// a pointer to the simulation object, which will be manipulated and drawn in this tab
	GUISimulation* sim;
	DM::Group* parentGroup;

	ModelNode* hoveredGroupNode;
public:
	SimulationTab(QWidget* parent, GUISimulation* sim, DM::Group* parentGroup);
	~SimulationTab();

	QGraphicsView* getQGViewer() {return viewer;};
	void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
	void dropEvent(QGraphicsSceneDragDropEvent *event);
	DM::Group* getParentGroup(){return parentGroup;};

	// for str+v & sim-drop
	void importSimulation(QIODevice* source, const QPointF& target);

	void wheelEvent(QGraphicsSceneWheelEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent * keyEvent );
	public slots:
	void enhanceSelection();
	void copySelection();
	void deleteSelection();
	void pasteSelection(const QPointF& pos);
};

#endif


#define ROOTGROUPNODE_H
#ifndef ROOTGROUPNODE_H
#define ROOTGROUPNODE_H

#include <modelnode.h>
#include <moduledescription.h>
#include "dmcompilersettings.h"
namespace DM {
    class Module;
//    class PortTuple;
}

class LinkNode;
//struct LinkNodeTuple;
//struct GUIPortTuple;

class DM_HELPER_DLL_EXPORT  RootGroupNode : public ModelNode
{
    Q_OBJECT

private:
    //QVector<GUIPortTuple * > OutputTuplePorts;
    //QVector<GUIPortTuple * > InPortTuplePorts;

    bool RePosFlag;
    std::string name;
    QVector<ModelNode * > childnodes;

protected:
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) ;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

public:
    RootGroupNode();
    virtual ~RootGroupNode();
    //void addTuplePort(DM::PortTuple * p);
    //void removeTuplePort(int Type, QString s);
    RootGroupNode( /*DM::Module *module, */GUISimulation * s);
    //virtual PortNode * getGUIPort(DM::Port * p);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QVector<ModelNode * > getChildNodes(){return this->childnodes;}
    //void RePosTuplePorts();
    void recalculateLandH() ;
    bool isGroup(){return true;}

    void addModelNode(ModelNode * m) ;

    void changeGroupID(QString Name);
    void setGroupZValue();
    void removeModelNode(ModelNode *m);
    void setSelected ( bool selected );
    //virtual void updatePorts();

};

#endif // ROOTGROUPNODE_H