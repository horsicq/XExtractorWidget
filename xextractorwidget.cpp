#include "xextractorwidget.h"
#include "ui_xextractorwidget.h"

XExtractorWidget::XExtractorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XExtractorWidget)
{
    ui->setupUi(this);
}

XExtractorWidget::~XExtractorWidget()
{
    delete ui;
}
