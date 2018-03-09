#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    program = new MixedIntegerLinearProgram();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete program;
}

void MainWindow::on_btnOpenProblemFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        QString::fromUtf8("Открыть файл задачи"));
    ui->txtOpenProblemFile->setText(fileName);
}

void MainWindow::on_btnOpenMatrix_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        QString::fromUtf8("Открыть файл матрицы"));
    ui->txtOpenMatrix->setText(fileName);
}

void MainWindow::on_btnOpenConstraints_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        QString::fromUtf8("Открыть файл с вектором ограничений"));
    ui->txtOpenConstraints->setText(fileName);
}

void MainWindow::on_btnOpenObjective_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        QString::fromUtf8("Открыть файл с вектором целевой функции"));
    ui->txtOpenObjective->setText(fileName);
}

void MainWindow::on_btnOpenInteger_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        QString::fromUtf8("Открыть файл с вектором целочисленности"));
    ui->txtOpenInteger->setText(fileName);
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->txtOpenInteger->setEnabled(true);
        ui->btnOpenInteger->setEnabled(true);
    }
    else
    {
        ui->txtOpenInteger->setText("");
        ui->txtOpenInteger->setEnabled(false);
        ui->btnOpenInteger->setEnabled(false);
    }
}

bool MainWindow::AskAboutUnsavedChanges() {
    QMessageBox msgbox;
    msgbox.setText(QString::fromUtf8("Несохранённые вычисления будут потеряны"));
    msgbox.setInformativeText(QString::fromUtf8("Продолжить?"));
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::Yes);
    int ret = msgbox.exec();
    return ret == QMessageBox::Yes;
}

void MainWindow::AddSolvers(bool isInteger)
{
    if (isInteger) {
        ui->comboSolver->addItem("Google");
        ui->comboSolver->addItem("CBC");
    } else {
        ui->comboSolver->addItem("Google");
        ui->comboSolver->addItem("BPMPD");
        ui->comboSolver->addItem("CLP");
    }
    ui->comboNEOS->addItem("CPLEX");
    ui->comboNEOS->addItem("Gurobi");
}

void MainWindow::LoadEnvironment() {
    ui->comboSolver->clear();
    ui->comboNEOS->clear();
    ui->txtValue->clear();
    ui->txtExportLP->clear();
    ui->txtNumber->clear();
    ui->txtPassword->clear();
    ui->txtSaveProblemFile->clear();
    if (program->isValid()) {
        AddSolvers(program->isInteger());
        ShowMessageAboutCorrectOpen();
    } else {
        ShowMessageAboutInvalid();
    }
}

void MainWindow::ShowMessageAboutEmptyInput() {
    QMessageBox msgbox;
    msgbox.setWindowTitle(QString::fromUtf8("Ошибка"));
    msgbox.setText(QString::fromUtf8("Не выбран(ы) файл(ы)"));
    msgbox.exec();
}

void MainWindow::ShowMessageAboutCorrectOpen() {
    QMessageBox msgbox;
    msgbox.setWindowTitle(QString::fromUtf8("Задача"));
    msgbox.setText(QString::fromUtf8("Файл задачи успешно открыт"));
    msgbox.exec();
}

void MainWindow::ShowMessageAboutCorrectSave() {
    QMessageBox msgbox;
    msgbox.setWindowTitle(QString::fromUtf8("Задача"));
    msgbox.setText(QString::fromUtf8("Файл задачи успешно сохранён"));
    msgbox.exec();
}

void MainWindow::ShowMessageAboutInvalid() {
    QMessageBox msgbox;
    msgbox.setWindowTitle(QString::fromUtf8("Ошибка"));
    msgbox.setText(QString::fromUtf8("Некорректные данные в задаче"));
    msgbox.exec();
}

void MainWindow::on_btnOpen_clicked()
{
    if (ui->txtOpenProblemFile->text() == QString()) {
        ShowMessageAboutEmptyInput();
        return;
    }
    if (program->unsavedChanges()) {
        if (!AskAboutUnsavedChanges()) {
            return;
        }
    }
    program->open(ui->txtOpenProblemFile->text());
    LoadEnvironment();
}

void MainWindow::on_btnCreate_clicked()
{
    if (ui->txtOpenMatrix->text() == QString() ||
        ui->txtOpenConstraints->text() == QString() ||
        ui->txtOpenObjective->text() == QString() ||
        (ui->checkBox->isChecked() && ui->txtOpenInteger->text() == QString())) {
        ShowMessageAboutEmptyInput();
        return;
    }
    if (program->unsavedChanges()) {
        if (!AskAboutUnsavedChanges()) {
            return;
        }
    }
    program->create(ui->txtOpenMatrix->text(),
                    ui->txtOpenConstraints->text(),
                    ui->txtOpenObjective->text(),
                    ui->txtOpenInteger->text());
    LoadEnvironment();
}

void MainWindow::on_btnExportLP_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                        QString::fromUtf8("Выбрать файл для экспорта в код на LP"));
    ui->txtExportLP->setText(fileName);
}

void MainWindow::on_btnSaveProblemFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                        QString::fromUtf8("Выбрать файл для сохранения задачи"));
    ui->txtSaveProblemFile->setText(fileName);
}

void MainWindow::on_btnSolve_clicked()
{
    if (!program->isValid()) {
        ShowMessageAboutInvalid();
        return;
    }
    ui->txtValue->setText(QString::number(program->solve(ui->comboSolver->currentText(), Parameters())));
}

void MainWindow::on_btnSend_clicked()
{
    if (!program->isValid()) {
        ShowMessageAboutInvalid();
        return;
    }
    NEOS_Query query = program->send(ui->comboNEOS->currentText(),
                                     Parameters(),
                                     ui->txtEmail->text(),
                                     ui->txtExportLP->text());
    ui->txtNumber->setText(query.number);
    ui->txtPassword->setText(query.password);
}

void MainWindow::on_btnSave_clicked()
{
    if (ui->txtSaveProblemFile->text() == QString()) {
        ShowMessageAboutEmptyInput();
        return;
    }
    if (!program->isValid()) {
        ShowMessageAboutInvalid();
    } else {
        program->save(ui->txtSaveProblemFile->text());
        ShowMessageAboutCorrectSave();
    }
}
