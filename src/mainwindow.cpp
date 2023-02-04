#include "mainwindow.h"
#include "scene.h"
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    view = new View();
    setCentralWidget(view);
    QMenu *menu = new QMenu("File", this);
    menuBar()->addMenu(menu);
    QAction *loadObjAction = new QAction("Load OBJ file");
    menu->addAction(loadObjAction);
    connect(loadObjAction, &QAction::triggered, this, &MainWindow::loadObjFile);
}

void MainWindow::loadObjFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Choose obj file", ".", "obj files (*.obj)");
    if(!fileName.isEmpty())
    {
        Scene* scene = qobject_cast<Scene*>(view->scene());
        if(scene)
        {
            //
            scene->preapareScene(fileName);
        }
    }
}
