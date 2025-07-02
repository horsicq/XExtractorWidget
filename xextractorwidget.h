/* Copyright (c) 2022-2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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

#include "dumpprocess.h"
#include "xdialogprocess.h"
#include "xextractor.h"
#include "xshortcutswidget.h"

namespace Ui {
class XExtractorWidget;
}

class XExtractorWidget : public XShortcutsWidget {
    Q_OBJECT

public:
    explicit XExtractorWidget(QWidget *pParent = nullptr);
    ~XExtractorWidget();

    void setData(QIODevice *pDevice, const XExtractor::OPTIONS &options, bool bAuto);
    void reload();
    DumpProcess::RECORD getDumpProcessRecord(QModelIndex index);
    virtual void adjustView();
    virtual void reloadData(bool bSaveSelection);
    void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);

private slots:
    void on_toolButtonScan_clicked();
    void on_toolButtonSave_clicked();
    void on_toolButtonDumpAll_clicked();
    void on_tableViewResult_customContextMenuRequested(const QPoint &pos);
    void dumpToFile();
    void _hexSlot();
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void on_comboBoxMapMode_currentIndexChanged(int nIndex);
    void viewSelection();
    void on_tableViewResult_clicked(const QModelIndex &index);
    void on_tableViewSelection(const QItemSelection &itemSelected, const QItemSelection &itemDeselected);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XExtractorWidget *ui;
    QIODevice *g_pDevice;
    XExtractor::OPTIONS g_options;
};

#endif  // XEXTRACTORWIDGET_H
