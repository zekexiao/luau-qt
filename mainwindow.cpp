#include "mainwindow.h"

#include <QLayout>
#include <QPlainTextEdit>
#include <QPushButton>

#include "qluau.h"

#define createTestBtn(X) \
    { \
        auto btn = new QPushButton(#X); \
        connect(btn, &QPushButton::clicked, this, &MainWindow::on##X##BtnClicked); \
        btnLayout->addWidget(btn); \
    }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto centre = new QWidget(this);
    auto layout = new QHBoxLayout(centre);
    centre->setLayout(layout);

    m_codeEdit = new QPlainTextEdit();
    m_codeEdit->setPlainText(R"(function test_hello_world()
    print("hello world")
end

function test_bool()
    return true
end

function test_vector(input)
    return vector.create(1, 2, 3) + input
end

function test_number(input1, input2, input3)
    return input1 + input2 + input3
end

local a = {"red"};
function test_table()
    return { [3]=a, "blue", third="green", "yellow"}
end

)");
    layout->addWidget(m_codeEdit);

    auto btnLayout = new QVBoxLayout();

    createTestBtn(HelloWorld);
    createTestBtn(Bool);
    createTestBtn(Vector);
    createTestBtn(Number);
    createTestBtn(Table);
    layout->addLayout(btnLayout);

    this->setCentralWidget(centre);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onHelloWorldBtnClicked()
{
    auto code = m_codeEdit->toPlainText();
    QLuau qluau;
    qluau.load(code);
    auto result = qluau.call("test_hello_world", {});
    qDebug() << result;
}

void MainWindow::onBoolBtnClicked()
{
    auto code = m_codeEdit->toPlainText();
    QLuau qluau;
    qluau.load(code);
    auto result = qluau.call("test_bool", {});
    qDebug() << result;
}

void MainWindow::onVectorBtnClicked()
{
    auto code = m_codeEdit->toPlainText();
    QLuau qluau;
    qluau.load(code);
    auto result = qluau.call("test_vector", {QVector3D(1, 2, 3)});
    qDebug() << result;
}

void MainWindow::onNumberBtnClicked()
{
    auto code = m_codeEdit->toPlainText();
    QLuau qluau;
    qluau.load(code);
    auto result = qluau.call("test_number", {1, 2, 3});
    qDebug() << result;
}

void MainWindow::onTableBtnClicked()
{
    auto code = m_codeEdit->toPlainText();
    QLuau qluau;
    qluau.load(code);
    auto result = qluau.call("test_table", {});
    qDebug() << result;
}
