#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QtGui>
#include "milp.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    MixedIntegerLinearProgram *program;

private slots:
    void on_btnOpenProblemFile_clicked();

    void on_btnOpenMatrix_clicked();

    void on_btnOpenConstraints_clicked();

    void on_btnOpenObjective_clicked();

    void on_btnOpenInteger_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_btnOpen_clicked();

    void on_btnCreate_clicked();

    void on_btnExportLP_clicked();

    void on_btnSaveProblemFile_clicked();

    void on_btnSolve_clicked();

    void on_btnSend_clicked();

    void on_btnSave_clicked();

private:
    Ui::MainWindow *ui;

    void AddSolvers(bool);
    bool AskAboutUnsavedChanges();
    void LoadEnvironment();
    void ShowMessageAboutCorrectOpen();
    void ShowMessageAboutCorrectSave();
    void ShowMessageAboutEmptyInput();
    void ShowMessageAboutInvalid();
};

#endif // MAINWINDOW_H
