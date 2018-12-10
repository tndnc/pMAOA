#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../gnuplot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    menuBar()->setNativeMenuBar(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    this->statusBar()->showMessage("alsdjalsjd");
}

void MainWindow::on_pushButton_clicked()
{
    gnuplot plot;
    plot("plot ../../points.dat");
}
