// Copyright (c) 2012  Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Author(s) : Saar Katz <kats.saar@gmail.com>


#include <fstream>

#include <QFileDialog>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Typedefs.h"
#include "ChoiseDialogPolygon.h"
#include "MinkowskiSumCalculator.h"

#define CGAL_POLYGON_EDGE_DEFAULT_COLOR "blue"
#define CGAL_POLYGON_FILL_DEFAULT_COLOR "blue"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setAcceptDrops(true);
  
  //m_polygonList = new PolygonTableModel(this);
  //ui->PolygonsTableView->setModel(m_polygonList);

  setScene();
  setEventFilterManagerGroup();
  setViewControl();
  //setPolygonInputDevice();
  setDrawNavigateActionGroup();
  connectActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAction_ImportPolygon() {
  QFileInfo file = QFileInfo(QFileDialog::getOpenFileName(
    this,
    tr("Import polygon"),
    "./",
    tr("All types (*.*)")));
  if (file.exists())
  {
    QList<PolygonWithHoles*>* pwhList = readPolygonFile(file.filePath());
    if (pwhList != NULL)
    {
      int numPolygons = pwhList->count();
      int i = 1;
      for (QList<PolygonWithHoles*>::iterator pwh = pwhList->begin(); pwh != pwhList->end(); pwh++) {
        PolygonItem* pi = new PolygonItem(*pwh);
        PolygonListItem* pli = new PolygonListItem(
          numPolygons > 1 ? file.baseName() + "_" + QString::number(i) : file.baseName(),
          *new QColor(CGAL_POLYGON_EDGE_DEFAULT_COLOR), true, pi);
        m_polygonList->appendItem(pli);
        pi->graphics()->setBrush(*new QBrush(
          *new QColor(CGAL_POLYGON_FILL_DEFAULT_COLOR)));
        pi->graphics()->setEdgesPen(*new QPen(*new QBrush(pli->getColor()), 1));
        pi->graphics()->setVerticesPen(*new QPen(Qt::transparent));
        ui->SceneGraphicsView->scene()->addItem(pi->graphics());
        i++;
      }
    }
  }
}
void MainWindow::onAction_DrawNewPolygon(bool checked)
{
  if (checked)
    m_manager->setCurrentFilterName("draw");
}
void MainWindow::onAction_NavigateScene(bool checked)
{
  if (checked)
    m_manager->setCurrentFilterName("navigate");
}
void MainWindow::onAction_SelfMinkowskiSum() {
  ChoiseDialogPolygon* dialog = new ChoiseDialogPolygon(m_polygonList, this);
  if (!dialog->exec()) { 
    return;
  }
  PolygonWithHoles* p = dialog->getChoise();
  if (p == NULL) {
    return;
  }
  qDebug() << Q_FUNC_INFO << "Starting minkowski sum calculation...";
  MinkowskiSumCalculator* m = new MinkowskiSumCalculator();
  std::list<PolygonWithHoles>* polygonList = new std::list<PolygonWithHoles>();
  polygonList->push_back(*p);
  m->setInput(*polygonList);
  std::shared_ptr<const MinkowskiSumResult> sharetPtrResult = m->getSum(2);
  qDebug() << Q_FUNC_INFO << "Minkowski sum calculation is complete!";
  const MinkowskiSumResult* r = sharetPtrResult.get();
  for (std::list<PolygonWithHoles>::const_iterator it = r->getSum().begin(); it != r->getSum().end(); ++it) {
    PolygonWithHoles* pwh = new PolygonWithHoles(*it);
    PolygonItem* pi = new PolygonItem(pwh);
    PolygonListItem* pli = new PolygonListItem(tr("MinkSum"), *new QColor(0, 255, 191), true, pi);
    m_polygonList->appendItem(pli);
    pi->graphics()->setBrush(*new QBrush(*new QColor(0, 255, 191, 127)));
    pi->graphics()->setEdgesPen(*new QPen(*new QBrush(pli->getColor()), 1));
    pi->graphics()->setVerticesPen(*new QPen(*new QColor(0, 255, 191, 0)));
    ui->SceneGraphicsView->scene()->addItem(pi->graphics());
  }
}

void MainWindow::setScene()
{
  QGraphicsScene* scene = new QGraphicsScene();
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  scene->setSceneRect(-100, -100, 100, 100);
  ui->SceneGraphicsView->setScene(scene);
  ui->SceneGraphicsView->setMouseTracking(true);
  ui->SceneGraphicsView->scale(1, -1);
}

void MainWindow::setEventFilterManagerGroup()
{
  m_manager = new EventFilterManagerGroup();
  m_manager->addObjectToWatch("viewport", ui->SceneGraphicsView->viewport());
  m_manager->addObjectToWatch("scene", ui->SceneGraphicsView->scene());
}

void MainWindow::setViewControl()
{
  m_manager->addFilterWidget("viewport", "navigate", new CGAL::Qt::GraphicsViewNavigation());
}

void MainWindow::setPolygonInputDevice()
{
  //manager->addFilterWidget("scene", "draw",
  //	new PolylineInput(ui->SceneGraphicsView, ui->SceneGraphicsView->scene, 0, false));
}

void MainWindow::setDrawNavigateActionGroup()
{
return;  /*
m_drawNavigateActionGroup = new QActionGroup(this);
  m_drawNavigateActionGroup->addAction(ui->actionNavigateScene);
  m_drawNavigateActionGroup->addAction(ui->actionDrawNewPolygon);*/
}

void MainWindow::connectActions()
{
  //connect(ui->actionNavigateScene, SIGNAL(toggled(bool)), 
    //      this, SLOT(onAction_NavigateScene(bool)));
 // connect(ui->actionDrawNewPolygon, SIGNAL(toggled(bool)), 
   //       this, SLOT(onAction_DrawNewPolygon(bool)));
  //ui->actionNavigateScene->setChecked(true);

  connect(ui->actionLoad, SIGNAL(triggered()),
          this, SLOT(onAction_ImportPolygon()));

  //connect(ui->actionSelfMinkowskiSum, SIGNAL(triggered()),
    //      this, SLOT(onAction_SelfMinkowskiSum()));
}

QList<PolygonWithHoles*>* MainWindow::readPolygonFile(QString aFilename) 
{
  std::ifstream in_file(qPrintable(aFilename));

  if (in_file) {
    unsigned int n_regions;
    in_file >> n_regions;
return NULL;    
auto polygonList = new QList<PolygonWithHoles*>();
    for (unsigned int r = 0; r < n_regions; ++r) {
      unsigned int n_boundaries;
      in_file >> n_boundaries;
      Polygon outer;
      std::vector<Polygon> holes;
      for (unsigned int r = 0; r < n_boundaries; ++r) {
        Polygon p;
        in_file >> p;
        if (r == 0) {
          outer = p;
        }
        else {
          holes.push_back(p);
        }
      }
      polygonList->append(new PolygonWithHoles(outer, holes.begin(), holes.end()));
    }
    return polygonList;
  }
  return NULL;
}
