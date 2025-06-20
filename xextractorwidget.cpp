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
#include "xextractorwidget.h"

#include "ui_xextractorwidget.h"

XExtractorWidget::XExtractorWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XExtractorWidget)
{
    ui->setupUi(this);

    XOptions::adjustToolButton(ui->toolButtonScan, XOptions::ICONTYPE_SCAN);
    XOptions::adjustToolButton(ui->toolButtonSave, XOptions::ICONTYPE_SAVE);
    XOptions::adjustToolButton(ui->toolButtonDumpAll, XOptions::ICONTYPE_DUMPTOFILE);

    ui->comboBoxType->setToolTip(tr("Type"));
    ui->comboBoxMapMode->setToolTip(tr("Mode"));
    ui->comboBoxOptions->setToolTip(tr("Options"));
    ui->checkBoxDeepScan->setToolTip(tr("Deep scan"));
    ui->checkBoxHeuristicScan->setToolTip(tr("Heuristic scan"));
    ui->toolButtonScan->setToolTip(tr("Scan"));
    ui->toolButtonSave->setToolTip(tr("Save"));
    ui->toolButtonDumpAll->setToolTip(tr("Dump all"));
    ui->labelSize->setToolTip(tr("Size"));

    g_pDevice = nullptr;
    g_options = {};

    QList<XComboBoxEx::CUSTOM_FLAG> listCustomFlags;

    QList<XBinary::FT> listFileTypes = XExtractor::getAvailableFileTypes();

    qint32 nNumberOfRecords = listFileTypes.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XComboBoxEx::_addCustomFlag(&listCustomFlags, listFileTypes.at(i), XFormats::getFileFormatExtsString(listFileTypes.at(i)), false);
    }

    ui->comboBoxOptions->addCustomFlags(tr("Options"), listCustomFlags);
}

XExtractorWidget::~XExtractorWidget()
{
    delete ui;
}

void XExtractorWidget::setData(QIODevice *pDevice, const XExtractor::OPTIONS &options, bool bAuto)
{
    g_pDevice = pDevice;
    g_options = options;

    qint32 nNumberOfRecords = options.listFileTypes.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        ui->comboBoxOptions->setCustomFlag(options.listFileTypes.at(i));
    }

    ui->checkBoxDeepScan->setChecked(options.bDeepScan);
    ui->checkBoxHeuristicScan->setChecked(options.bHeuristicScan);

    XFormats::setFileTypeComboBox(options.fileType, g_pDevice, ui->comboBoxType);
    XFormats::getMapModesList(options.fileType, ui->comboBoxMapMode);

    if (bAuto) {
        reload();
    }
}

void XExtractorWidget::reload()
{
    ui->labelSize->setText(XBinary::valueToHexEx(g_pDevice->size()));

    XExtractor::DATA extractor_data = {};

    QList<quint64> listFlags = ui->comboBoxOptions->getCustomFlags();

    qint32 nNumberOfRecords = listFlags.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        extractor_data.options.listFileTypes.append((XBinary::FT)listFlags.at(i));
    }

    extractor_data.options.bDeepScan = ui->checkBoxDeepScan->isChecked();
    extractor_data.options.bHeuristicScan = ui->checkBoxHeuristicScan->isChecked();

    XBinary::_MEMORY_MAP memoryMap = XFormats::getMemoryMap((XBinary::FT)(ui->comboBoxType->currentData().toULongLong()),
                                                            (XBinary::MAPMODE)(ui->comboBoxMapMode->currentData().toULongLong()), g_pDevice);

    XExtractor xextractor;
    XDialogProcess dep(XOptions::getMainWidget(this), &xextractor);
    dep.setGlobal(getShortcuts(), getGlobalOptions());
    xextractor.setData(g_pDevice, &extractor_data, dep.getPdStruct());
    dep.start();
    dep.showDialogDelay();

    if (dep.isSuccess()) {
        qint32 nNumberOfRecords = extractor_data.listRecords.count();

        QStandardItemModel *pModel = new QStandardItemModel(nNumberOfRecords, 6);

        pModel->setHeaderData(0, Qt::Horizontal, tr("Offset"));
        pModel->setHeaderData(1, Qt::Horizontal, tr("Address"));
        pModel->setHeaderData(2, Qt::Horizontal, "");
        pModel->setHeaderData(3, Qt::Horizontal, tr("Size"));
        pModel->setHeaderData(4, Qt::Horizontal, tr("Type"));
        pModel->setHeaderData(5, Qt::Horizontal, "");

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(XBinary::valueToHexEx(extractor_data.listRecords.at(i).nOffset));

                pItem->setData(extractor_data.listRecords.at(i).nOffset, Qt::UserRole + 0);
                pItem->setData(extractor_data.listRecords.at(i).nSize, Qt::UserRole + 1);
                pItem->setData(extractor_data.listRecords.at(i).sExt, Qt::UserRole + 2);

                pModel->setItem(i, 0, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                XADDR nAddress = XBinary::offsetToAddress(&memoryMap, extractor_data.listRecords.at(i).nOffset);

                if (nAddress != (XADDR)-1) {
                    pItem->setText(XBinary::valueToHexEx(nAddress));
                }

                pModel->setItem(i, 1, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(XBinary::getMemoryRecordByOffset(&memoryMap, extractor_data.listRecords.at(i).nOffset).sName);

                pModel->setItem(i, 2, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(XBinary::valueToHexEx(extractor_data.listRecords.at(i).nSize));

                pModel->setItem(i, 3, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(XBinary::fileTypeIdToString(extractor_data.listRecords.at(i).fileType));

                pModel->setItem(i, 4, pItem);
            }
            {
                QStandardItem *pItem = new QStandardItem;

                pItem->setText(extractor_data.listRecords.at(i).sString);

                pModel->setItem(i, 5, pItem);
            }
            //            {
            //                QPushButton *result=new QPushButton(this); // TODO
            //                remove

            //                result->setText(tr("Dump"));
            //                result->setProperty("OFFSET",extractor_data.listRecords.at(i).nOffset);
            //                result->setProperty("SIZE",extractor_data.listRecords.at(i).nSize);

            //                connect(result,SIGNAL(clicked()),this,SLOT(pushButtonAction()));

            //                ui->tableWidgetResult->setCellWidget(i,4,result);
            //            }
        }

        XOptions::setModelTextAlignment(pModel, 0, Qt::AlignRight | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, 1, Qt::AlignRight | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, 2, Qt::AlignLeft | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, 3, Qt::AlignRight | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, 4, Qt::AlignLeft | Qt::AlignVCenter);
        XOptions::setModelTextAlignment(pModel, 5, Qt::AlignLeft | Qt::AlignVCenter);

        ui->tableViewResult->setCustomModel(pModel, true);

        //        ui->tableViewResult->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        //        ui->tableViewResult->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        //        ui->tableViewResult->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
        ui->tableViewResult->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

        connect(ui->tableViewResult->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
                SLOT(on_tableViewSelection(QItemSelection, QItemSelection)));
    }
}

DumpProcess::RECORD XExtractorWidget::getDumpProcessRecord(QModelIndex index)
{
    DumpProcess::RECORD result = {};

    result.nOffset = ui->tableViewResult->model()->data(index, Qt::UserRole + 0).toLongLong();
    result.nSize = ui->tableViewResult->model()->data(index, Qt::UserRole + 1).toLongLong();
    QString sExt = ui->tableViewResult->model()->data(index, Qt::UserRole + 2).toString();

    QString sName = QString("%1_%2").arg(XBinary::valueToHexEx(result.nOffset), XBinary::valueToHexEx(result.nSize));

    result.sFileName = XBinary::getResultFileName(g_pDevice, QString("%1.%2").arg(sName, sExt));

    return result;
}

void XExtractorWidget::adjustView()
{
    getGlobalOptions()->adjustWidget(this, XOptions::ID_VIEW_FONT_CONTROLS);
    getGlobalOptions()->adjustTableView(ui->tableViewResult, XOptions::ID_VIEW_FONT_TABLEVIEWS);
}

void XExtractorWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)

    reload();
}

void XExtractorWidget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
    // TODO
}

void XExtractorWidget::on_toolButtonScan_clicked()
{
    reload();
}

void XExtractorWidget::on_toolButtonSave_clicked()
{
    XShortcutsWidget::saveTableModel(ui->tableViewResult->getProxyModel(), XBinary::getResultFileName(g_pDevice, QString("%1.txt").arg(tr("Extract"))));
}

void XExtractorWidget::on_toolButtonDumpAll_clicked()
{
    QString sDirectory = QFileDialog::getExistingDirectory(this, tr("Dump all"), XBinary::getDeviceDirectory(g_pDevice));

    if (!sDirectory.isEmpty()) {
        qint32 nNumberOfRecords = ui->tableViewResult->model()->rowCount();

        if (nNumberOfRecords) {
            QList<DumpProcess::RECORD> listRecords;

            for (qint32 i = 0; i < nNumberOfRecords; i++) {
                QModelIndex index = ui->tableViewResult->model()->index(i, 0);

                DumpProcess::RECORD record = getDumpProcessRecord(index);

                record.sFileName = sDirectory + QDir::separator() + QFileInfo(record.sFileName).fileName();

                listRecords.append(record);
            }

            QString sJsonFileName = sDirectory + QDir::separator() + XBinary::getDeviceFileBaseName(g_pDevice) + ".patch.json";

            DumpProcess dumpProcess;
            XDialogProcess dd(this, &dumpProcess);
            dd.setGlobal(getShortcuts(), getGlobalOptions());
            dumpProcess.setData(g_pDevice, listRecords, DumpProcess::DT_DUMP_DEVICE_OFFSET, sJsonFileName, dd.getPdStruct());
            dd.start();
            dd.showDialogDelay();
        }
    }
}

void XExtractorWidget::on_tableViewResult_customContextMenuRequested(const QPoint &pos)
{
    qint32 nRow = ui->tableViewResult->currentIndex().row();

    if (nRow != -1) {
        QMenu contextMenu(this);

        QList<XShortcuts::MENUITEM> listMenuItems;

        getShortcuts()->_addMenuItem(&listMenuItems, X_ID_TABLE_SELECTION_DUMPTOFILE, this, SLOT(dumpToFile()), XShortcuts::GROUPID_NONE);
        getShortcuts()->_addMenuItem_CopyRow(&listMenuItems, ui->tableViewResult);

        if (g_options.bMenu_Hex) {
            getShortcuts()->_addMenuItem(&listMenuItems, X_ID_TABLE_FOLLOWIN_HEX, this, SLOT(_hexSlot()), XShortcuts::GROUPID_FOLLOWIN);
        }

        QList<QObject *> listObjects = getShortcuts()->adjustContextMenu(&contextMenu, &listMenuItems);

        contextMenu.exec(ui->tableViewResult->viewport()->mapToGlobal(pos));

        XOptions::deleteQObjectList(&listObjects);
    }
}

void XExtractorWidget::dumpToFile()
{
    qint32 nRow = ui->tableViewResult->currentIndex().row();

    if (nRow != -1) {
        QModelIndex index = ui->tableViewResult->selectionModel()->selectedIndexes().at(0);

        DumpProcess::RECORD record = getDumpProcessRecord(index);

        record.sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), record.sFileName);

        if (!record.sFileName.isEmpty()) {
            DumpProcess dumpProcess;
            XDialogProcess dd(this, &dumpProcess);
            dd.setGlobal(getShortcuts(), getGlobalOptions());
            dumpProcess.setData(g_pDevice, record, DumpProcess::DT_DUMP_DEVICE_OFFSET, dd.getPdStruct());
            dd.start();
            dd.showDialogDelay();
        }
    }
}

void XExtractorWidget::_hexSlot()
{
    if (g_options.bMenu_Hex) {
        qint32 nRow = ui->tableViewResult->currentIndex().row();

        if (nRow != -1) {
            QModelIndex index = ui->tableViewResult->selectionModel()->selectedIndexes().at(0);

            DumpProcess::RECORD record = getDumpProcessRecord(index);

            emit followLocation(record.nOffset, XBinary::LT_OFFSET, record.nSize, XOptions::WIDGETTYPE_HEX);
        }
    }
}

void XExtractorWidget::on_comboBoxType_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    XBinary::FT fileType = (XBinary::FT)(ui->comboBoxType->currentData().toInt());
    XFormats::getMapModesList(fileType, ui->comboBoxMapMode);

    reload();
}

void XExtractorWidget::on_comboBoxMapMode_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

void XExtractorWidget::viewSelection()
{
    QItemSelectionModel *pSelectionModel = ui->tableViewResult->selectionModel();

    if (pSelectionModel) {
        QModelIndexList listIndexes = pSelectionModel->selectedIndexes();

        if (listIndexes.count()) {
            QModelIndex indexNumber = listIndexes.at(0);

            DumpProcess::RECORD record = getDumpProcessRecord(indexNumber);

            if (record.nOffset != -1) {
                emit currentLocationChanged(record.nOffset, XBinary::LT_OFFSET, record.nSize);
            }
        }
    }
}

void XExtractorWidget::on_tableViewResult_clicked(const QModelIndex &index)
{
    Q_UNUSED(index)

    viewSelection();
}

void XExtractorWidget::on_tableViewSelection(const QItemSelection &itemSelected, const QItemSelection &itemDeselected)
{
    Q_UNUSED(itemSelected)
    Q_UNUSED(itemDeselected)

    viewSelection();
}
