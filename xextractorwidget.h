/* Copyright (c) 2022 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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
    struct OPTIONS
    {
        bool bPE;
        bool bELF;
        bool bMACHO;
        bool bMACHOFAT;
        bool bZIP;
        bool bRAR;
    };

    explicit XExtractorWidget(QWidget *pParent=nullptr);
    ~XExtractorWidget();

    void setData(QIODevice *pDevice,OPTIONS options,bool bAuto);

private:
    Ui::XExtractorWidget *ui;
    QIODevice *g_pDevice;
    OPTIONS g_options;
};

#endif // XEXTRACTORWIDGET_H
