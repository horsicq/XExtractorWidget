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
    // ui->checkBoxHeuristicScan->setToolTip(tr("Heuristic scan"));
    ui->toolButtonScan->setToolTip(tr("Scan"));
    ui->toolButtonSave->setToolTip(tr("Save"));
    ui->toolButtonDumpAll->setToolTip(tr("Dump all"));
    ui->labelSize->setToolTip(tr("Size"));
    ui->comboBoxExtractorMode->setToolTip(tr("Mode"));

    g_pDevice = nullptr;
    g_pXInfoDB = nullptr;
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

void XExtractorWidget::setData(QIODevice *pDevice, XInfoDB *pXInfoDB, const XExtractor::OPTIONS &options, bool bAuto)
{
    g_pDevice = pDevice;
    g_pXInfoDB = pXInfoDB;
    g_options = options;

    qint32 nNumberOfRecords = options.listFileTypes.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        ui->comboBoxOptions->setCustomFlag(options.listFileTypes.at(i));
    }

    ui->checkBoxDeepScan->setChecked(options.bDeepScan);
    // ui->checkBoxHeuristicScan->setChecked(options.bHeuristicScan);

    XBinary::FT fileType = XFormats::setFileTypeComboBox(options.fileType, g_pDevice, ui->comboBoxType);
    XFormats::getMapModesList(fileType, ui->comboBoxMapMode);

    {
        bool bBlock = ui->comboBoxExtractorMode->blockSignals(true);

        ui->comboBoxExtractorMode->clear();
        ui->comboBoxExtractorMode->addItem(XExtractor::extractorModeToString(XExtractor::EMODE_HEURISTIC), XExtractor::EMODE_HEURISTIC);

        if (XExtractor::isFormatModeAvailable(fileType)) {
            ui->comboBoxExtractorMode->addItem(XExtractor::extractorModeToString(XExtractor::EMODE_FORMAT), XExtractor::EMODE_FORMAT);
        }

        ui->comboBoxExtractorMode->addItem(XExtractor::extractorModeToString(XExtractor::EMODE_RAW), XExtractor::EMODE_RAW);

        ui->comboBoxExtractorMode->blockSignals(bBlock);
    }

    XHexView::OPTIONS hex_options = {};

    ui->widgetHex->setData(pDevice, hex_options, true);
    ui->widgetHex->setXInfoDB(g_pXInfoDB);

    if (bAuto) {
        reload();
    }
}

void XExtractorWidget::reload()
{
    ui->labelSize->setText(XBinary::valueToHexEx(g_pDevice->size()));

    g_extractor_data = {};

    QList<quint64> listFlags = ui->comboBoxOptions->getCustomFlags();

    qint32 nNumberOfRecords = listFlags.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        g_extractor_data.options.listFileTypes.append((XBinary::FT)listFlags.at(i));
    }

    g_extractor_data.options.bDeepScan = ui->checkBoxDeepScan->isChecked();
    g_extractor_data.options.bAnalyze = true;
    g_extractor_data.options.emode = (XExtractor::EMODE)(ui->comboBoxExtractorMode->currentData().toInt());
    // extractor_data.options.bHeuristicScan = ui->checkBoxHeuristicScan->isChecked();

    g_extractor_data.memoryMap = XFormats::getMemoryMap((XBinary::FT)(ui->comboBoxType->currentData().toULongLong()),
                                                            (XBinary::MAPMODE)(ui->comboBoxMapMode->currentData().toULongLong()), g_pDevice);

    XExtractor xextractor;
    XDialogProcess dep(XOptions::getMainWidget(this), &xextractor);
    dep.setGlobal(getShortcuts(), getGlobalOptions());
    xextractor.setData(g_pDevice, &g_extractor_data, dep.getPdStruct());
    dep.start();
    dep.showDialogDelay();

    if (dep.isSuccess()) {
        XModel_Extractor *pModel = new XModel_Extractor(&g_extractor_data, this);

        ui->tableViewResult->setCustomModel(pModel, true);

        connect(ui->tableViewResult->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
                SLOT(on_tableViewSelection(QItemSelection, QItemSelection)));

        if (g_pXInfoDB) {
            qint32 nNumberOfRecords = g_extractor_data.listRecords.count();

            for (qint32 i = 0; i < nNumberOfRecords; i++) {

                QString sComment = g_extractor_data.listRecords.at(i).sString;
                XInfoDB::BOOKMARKRECORD record = {};
                record.sUUID = XBinary::generateUUID();
                record.sColorBackground = QColor(Qt::yellow).name();
                record.nLocation = g_extractor_data.listRecords.at(i).nOffset;
                record.locationType = XBinary::LT_OFFSET;
                record.nSize = g_extractor_data.listRecords.at(i).nSize;
                record.sComment = sComment;

                g_pXInfoDB->_addBookmarkRecord(record);
            }

            if (nNumberOfRecords) {
                g_pXInfoDB->reloadView();
            }
        }
    }
}

DumpProcess::RECORD XExtractorWidget::getDumpProcessRecord(QModelIndex index)
{
    DumpProcess::RECORD result = {};

    result.nOffset = ui->tableViewResult->model()->data(index, Qt::UserRole + XModel::USERROLE_OFFSET).toLongLong();
    result.nSize = ui->tableViewResult->model()->data(index, Qt::UserRole + XModel::USERROLE_SIZE).toLongLong();
    QString sExt = ui->tableViewResult->model()->data(index, Qt::UserRole + XModel::USERROLE_STRING2).toString();

    QString sName = QString("%1_%2").arg(XBinary::valueToHexEx(result.nOffset), XBinary::valueToHexEx(result.nSize));

    result.sFileName = XBinary::getResultFileName(g_pDevice, QString("%1.%2").arg(sName, sExt));

    return result;
}

void XExtractorWidget::adjustView()
{
    getGlobalOptions()->adjustWidget(this, XOptions::ID_VIEW_FONT_CONTROLS);
    getGlobalOptions()->adjustTableView(ui->tableViewResult, XOptions::ID_VIEW_FONT_TABLEVIEWS);

    ui->widgetHex->adjustView();
}

void XExtractorWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)

    reload();
}

void XExtractorWidget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);

    ui->widgetHex->setGlobal(pShortcuts, pXOptions);
}

void XExtractorWidget::registerShortcuts(bool bState)
{
    XShortcutsWidget::registerShortcuts(bState);
    ui->widgetHex->registerShortcuts(bState);
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
                emit currentLocationChanged(record.nOffset, XBinary::LT_OFFSET, record.nSize); // TODO mb remove, use signals from widgetHex

                ui->widgetHex->goToLocation(record.nOffset, XBinary::LT_OFFSET);
                ui->widgetHex->setLocationOffset(record.nOffset, XBinary::LT_OFFSET, record.nSize);
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
