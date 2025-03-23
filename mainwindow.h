#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPlainTextEdit;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onHelloWorldBtnClicked();
    void onBoolBtnClicked();
    void onVectorBtnClicked();
    void onNumberBtnClicked();
    void onTableBtnClicked();

private:
    QPlainTextEdit *m_codeEdit;
};
#endif // MAINWINDOW_H
