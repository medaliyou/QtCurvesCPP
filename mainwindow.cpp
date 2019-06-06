#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAstroid_clicked()
{
    // change background color for render area
    this->ui->renderArea->setShape(RenderArea::Astroid);
    this->ui->renderArea->repaint();
}

void MainWindow::on_btnCycloid_clicked()
{
    this->ui->renderArea->setShape(RenderArea::Cycloid);
    this->ui->renderArea->repaint();

}

void MainWindow::on_btnHuygensCicloid_clicked()
{
    this->ui->renderArea->setShape(RenderArea::HuygensCycloid);
    this->ui->renderArea->repaint();

}

void MainWindow::on_btnHypoCycloid_clicked()
{
    this->ui->renderArea->setShape(RenderArea::HypoCycloid);
    this->ui->renderArea->repaint();

}

void MainWindow::on_btnLine_clicked()
{
    this->ui->renderArea->setShape(RenderArea::Line);
    this->ui->renderArea->repaint();
}
