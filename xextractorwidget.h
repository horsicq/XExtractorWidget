#ifndef XEXTRACTORWIDGET_H
#define XEXTRACTORWIDGET_H

#include <QWidget>

namespace Ui {
class XExtractorWidget;
}

class XExtractorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit XExtractorWidget(QWidget *parent = nullptr);
    ~XExtractorWidget();

private:
    Ui::XExtractorWidget *ui;
};

#endif // XEXTRACTORWIDGET_H
