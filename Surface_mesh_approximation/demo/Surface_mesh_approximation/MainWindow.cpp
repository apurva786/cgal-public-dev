#include "MainWindow.h"
#include "Scene.h"
#include <CGAL/Qt/debug.h>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTextStream>
#include <QUrl>
#include <QFileDialog>
#include <QSettings>
#include <QHeaderView>
#include <QClipboard>

#include "ui_MainWindow.h"

#include "dialSettings.h"

#include <QMimeData> 


MainWindow::MainWindow(QWidget* parent)
  : CGAL::Qt::DemosMainWindow(parent)
{
  ui = new Ui::MainWindow;
  ui->setupUi(this);

  // saves some pointers from ui, for latter use.
  m_pViewer = ui->viewer;

  // does not save the state of the viewer 
  m_pViewer->setStateFileName(QString::null);

  // accepts drop events
  setAcceptDrops(true);

  // setups scene
  m_pScene = new Scene;
  m_pViewer->setScene(m_pScene);

  // connects actionQuit (Ctrl+Q) and qApp->quit()
  connect(ui->actionQuit, SIGNAL(triggered()),
    this, SLOT(quit()));

  this->addRecentFiles(ui->menuFile, ui->actionQuit);
  connect(this, SIGNAL(openRecentFile(QString)),
    this, SLOT(open(QString)));

  this->addAboutDemo(":/cgal/VSA_demo/about.html");
  this->addAboutCGAL();

  readSettings();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::updateViewerBBox()
{
  m_pScene->update_bbox();
  const Bbox_3 bbox = m_pScene->bbox();
  const double xmin = bbox.xmin();
  const double ymin = bbox.ymin();
  const double zmin = bbox.zmin();
  const double xmax = bbox.xmax();
  const double ymax = bbox.ymax();
  const double zmax = bbox.zmax();
  qglviewer::Vec 
    vec_min(xmin, ymin, zmin),
    vec_max(xmax, ymax, zmax);
  m_pViewer->setSceneBoundingBox(vec_min,vec_max);
  m_pViewer->camera()->showEntireScene();
}

void MainWindow::open(QString filename)
{
  QFileInfo fileinfo(filename);
  if (fileinfo.isFile() && fileinfo.isReadable()) {
    int index = m_pScene->open(filename);
    if (index >= 0) {
      QSettings settings;
      settings.setValue("OFF open directory",
        fileinfo.absoluteDir().absolutePath());
      this->addToRecentFiles(filename);

      // update bbox
      updateViewerBBox();
      m_pViewer->update();

      ui->actionViewPolyhedron->setChecked(true);
      ui->actionViewWireframe->setChecked(false);
      ui->actionViewBoundary->setChecked(false);
      ui->actionViewAnchors->setChecked(false);
      ui->actionViewApproximation->setChecked(false);

      ui->actionL21->setChecked(true);
      ui->actionL2->setChecked(false);
      ui->actionCompact->setChecked(false);
    }
  }
}

void MainWindow::quit()
{
  writeSettings();
  close();
}

void MainWindow::readSettings()
{
  this->readState("MainWindow", Size|State);
}

void MainWindow::writeSettings()
{
  this->writeState("MainWindow");
  std::cerr << "Write setting... done.\n";
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->mimeData()->hasFormat("text/uri-list"))
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
  Q_FOREACH(QUrl url, event->mimeData()->urls()) {
    QString filename = url.toLocalFile();
    if (!filename.isEmpty()) {
      QTextStream(stderr) << QString("dropEvent(\"%1\")\n").arg(filename);
      open(filename);
    }
  }
  event->acceptProposedAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}

void MainWindow::on_actionLoadPolyhedron_triggered()
{
  QSettings settings;
  QString directory = settings.value("OFF open directory",
    QDir::current().dirName()).toString();
  QStringList filenames = 
    QFileDialog::getOpenFileNames(this,
    tr("Load polyhedron..."),
    directory,
    tr("OFF files (*.off)\n"
    "All files (*)"));
  if (!filenames.isEmpty()) {
    Q_FOREACH(QString filename, filenames) {
      open(filename);
    }
  }
}

void MainWindow::on_actionSaveApproximation_triggered()
{
  QString file_name = QFileDialog::getSaveFileName(this,
    tr("Save Approximation Mesh"),
    ".",
    tr("OFF (*.off)"));
  if (file_name.isEmpty())
    return;
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pScene->save_approximation(file_name.toStdString());
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionSaveSnapshot_triggered()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pViewer->saveSnapshot(QString("snapshot.png"));
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionCopySnapshot_triggered()
{
  // copy snapshot to clipboard
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QClipboard *qb = QApplication::clipboard();
  m_pViewer->makeCurrent();
  m_pViewer->raise();
  QImage snapshot = m_pViewer->grabFrameBuffer(true);
  qb->setImage(snapshot);
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionL21_triggered()
{
  ui->actionL2->setChecked(false);
  ui->actionCompact->setChecked(false);

  ui->actionViewPolyhedron->setChecked(true);
  ui->actionViewWireframe->setChecked(false);
  ui->actionViewBoundary->setChecked(false);
  ui->actionViewAnchors->setChecked(false);
  ui->actionViewApproximation->setChecked(false);

  m_pScene->set_metric(0);
  m_pViewer->update();
}

void MainWindow::on_actionL2_triggered()
{
  ui->actionL21->setChecked(false);
  ui->actionCompact->setChecked(false);

  ui->actionViewPolyhedron->setChecked(true);
  ui->actionViewWireframe->setChecked(false);
  ui->actionViewBoundary->setChecked(false);
  ui->actionViewAnchors->setChecked(false);
  ui->actionViewApproximation->setChecked(false);

  m_pScene->set_metric(1);
  m_pViewer->update();
}

void MainWindow::on_actionCompact_triggered()
{
  ui->actionL21->setChecked(false);
  ui->actionL2->setChecked(false);

  ui->actionViewPolyhedron->setChecked(true);
  ui->actionViewWireframe->setChecked(false);
  ui->actionViewBoundary->setChecked(false);
  ui->actionViewAnchors->setChecked(false);
  ui->actionViewApproximation->setChecked(false);
  
  m_pScene->set_metric(2);
  m_pViewer->update();
}

void MainWindow::on_actionSeeding_triggered()
{
  SettingsDialog dial;
  if (dial.exec() == QDialog::Accepted) {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int init = dial.InitRandom->isChecked() ? 0 : (dial.InitIncremental->isChecked() ? 1 : 2);
    int inner_iterations = dial.NumInnerIterations->value();
    int iterations = dial.NumIterations->value();
    if (dial.checkBox_number->isChecked())
      // seeding by number
      m_pScene->seeding_by_number(init, dial.NumSegments->value(), inner_iterations, iterations);
    else
      // seeding by error
      m_pScene->seeding_by_error(init, dial.ErrorDrop->value(), inner_iterations, iterations);
    
    m_pViewer->update();

    QApplication::restoreOverrideCursor();
    ui->actionViewBoundary->setChecked(true);
  }
}

void MainWindow::on_actionFit_triggered()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pScene->run_one_step();
  m_pViewer->update();
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionAdd_triggered()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pScene->add_one_proxy();
  m_pViewer->update();
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionTeleport_triggered()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pScene->teleport_one_proxy();
  m_pViewer->update();
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionMeshing_triggered()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  m_pScene->extract_mesh();
  m_pViewer->update();
  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionViewPolyhedron_triggered()
{
  m_pScene->toggle_view_polyhedron();
  m_pViewer->update();
}

void MainWindow::on_actionViewWireframe_triggered()
{
  m_pScene->toggle_view_wireframe();
  m_pViewer->update();
}

void MainWindow::on_actionViewBoundary_triggered()
{
  m_pScene->toggle_view_boundary();
  m_pViewer->update();
}

void MainWindow::on_actionViewAnchors_triggered()
{
  m_pScene->toggle_view_anchors();
  m_pViewer->update();
}

void MainWindow::on_actionViewApproximation_triggered()
{
  m_pScene->toggle_view_approximation();
  m_pViewer->update();
}
