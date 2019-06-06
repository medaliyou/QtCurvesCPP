#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAstroid_clicked();

    void on_btnCycloid_clicked();

    void on_btnHuygensCicloid_clicked();

    void on_btnHypoCycloid_clicked();

    void on_btnLine_clicked();

    void on_spinScale_valueChanged(double arg1);
    void on_spinIntervalLength_valueChanged(double arg1);

    void on_spinCount_valueChanged(int arg1);

private:
    void update_ui();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
