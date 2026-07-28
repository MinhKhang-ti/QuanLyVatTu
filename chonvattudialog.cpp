#include "chonvattudialog.h"
#include "ui_chonvattudialog.h"
#include "vattulogic.h"

#include <QMessageBox>
#include <QHeaderView>
#include <cstring>

ChonVatTuDialog::ChonVatTuDialog(TreeVT root, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChonVatTuDialog),
    rootTree(root)
{
    ui->setupUi(this);

    // Cấu hình bảng hiển thị
    ui->tableVatTu->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableVatTu->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableVatTu->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableVatTu->setSelectionMode(QAbstractItemView::SingleSelection);

    std::memset(&selectedVT, 0, sizeof(VATTU));

    // Kết nối tín hiệu
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &ChonVatTuDialog::onSearchTextChanged);
    connect(ui->tableVatTu, &QTableWidget::cellDoubleClicked, this, &ChonVatTuDialog::onTableDoubleClicked);
    connect(ui->chonButton, &QPushButton::clicked, this, &ChonVatTuDialog::onChonClicked);
    connect(ui->huyButton, &QPushButton::clicked, this, &ChonVatTuDialog::onHuyClicked);

    napDanhSachVatTu("");
}

ChonVatTuDialog::~ChonVatTuDialog()
{
    delete ui;
}

void ChonVatTuDialog::napDanhSachVatTu(const QString &filter)
{
    ui->tableVatTu->setRowCount(0);
    int total = 0;
    nodeVT** ds = duyetTheoTen(rootTree, total);
    if (!ds || total == 0) {
        if (ds) delete[] ds;
        return;
    }

    QString keyword = filter.trimmed();
    int rowCount = 0;

    for (int i = 0; i < total; i++) {
        QString mavt = QString::fromUtf8(ds[i]->vt.MAVT);
        QString tenvt = QString::fromUtf8(ds[i]->vt.TENVT);
        QString dvt = QString::fromUtf8(ds[i]->vt.DVT);

        bool match = keyword.isEmpty() ||
                     mavt.contains(keyword, Qt::CaseInsensitive) ||
                     tenvt.contains(keyword, Qt::CaseInsensitive);

        if (match) {
            ui->tableVatTu->insertRow(rowCount);
            ui->tableVatTu->setItem(rowCount, 0, new QTableWidgetItem(mavt));
            ui->tableVatTu->setItem(rowCount, 1, new QTableWidgetItem(tenvt));
            ui->tableVatTu->setItem(rowCount, 2, new QTableWidgetItem(dvt));
            ui->tableVatTu->setItem(rowCount, 3, new QTableWidgetItem(QString::number(ds[i]->vt.SoLuongTon)));
            rowCount++;
        }
    }

    delete[] ds;

    if (rowCount > 0) {
        ui->tableVatTu->selectRow(0);
    }
}

void ChonVatTuDialog::onSearchTextChanged(const QString &text)
{
    napDanhSachVatTu(text);
}

void ChonVatTuDialog::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row >= 0) {
        chonHangCurrent();
    }
}

void ChonVatTuDialog::onChonClicked()
{
    chonHangCurrent();
}

void ChonVatTuDialog::onHuyClicked()
{
    reject();
}

void ChonVatTuDialog::chonHangCurrent()
{
    int row = ui->tableVatTu->currentRow();
    if (row < 0 || row >= ui->tableVatTu->rowCount()) {
        QMessageBox::warning(this, "Thông báo", "Vui lòng chọn một vật tư từ danh sách!");
        return;
    }

    QString mavt = ui->tableVatTu->item(row, 0)->text();
    nodeVT* node = timVT(rootTree, mavt.toStdString().c_str());
    if (node) {
        selectedVT = node->vt;
        accept();
    } else {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy dữ liệu vật tư được chọn!");
    }
}

VATTU ChonVatTuDialog::getSelectedVatTu() const
{
    return selectedVT;
}

QString ChonVatTuDialog::getSelectedMaVT() const
{
    return QString::fromUtf8(selectedVT.MAVT);
}
