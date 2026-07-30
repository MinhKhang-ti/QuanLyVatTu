#include "vattupage.h"
#include "ui_vattupage.h"
#include "vattulogic.h"
#include "fileio.h"
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QSignalBlocker>
#include <QMessageBox>
#include <cstring>

VatTuPage::VatTuPage(TreeVT &rootRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::VatTuPage), root(rootRef)
{
    ui->setupUi(this);

    ui->maVTEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));
    ui->soLuongEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{0,6}"), this));

    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
    ui->xoaButton->setEnabled(false);

    connect(ui->maVTEdit, &QLineEdit::textChanged, this, &VatTuPage::capNhatTrangThaiNut);
    connect(ui->tenVTEdit, &QLineEdit::textChanged, this, &VatTuPage::capNhatTrangThaiNut);
    connect(ui->dvtEdit, &QLineEdit::textChanged, this, &VatTuPage::capNhatTrangThaiNut);
    connect(ui->soLuongEdit, &QLineEdit::textChanged, this, &VatTuPage::capNhatTrangThaiNut);
    connect(ui->themButton, &QPushButton::clicked, this, &VatTuPage::onThemClicked);
    connect(ui->xoaButton, &QPushButton::clicked, this, &VatTuPage::onXoaClicked);
    connect(ui->suaButton, &QPushButton::clicked, this, &VatTuPage::onSuaClicked);
    connect(ui->huyButton, &QPushButton::clicked, this, &VatTuPage::onHuyClicked);
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &VatTuPage::onTableSelectionChanged);
    connect(ui->timKiemEdit, &QLineEdit::textChanged, this, &VatTuPage::onTimKiemChanged);

    docVatTu(root, FILE_VATTU);
    lamMoiBang();
}

VatTuPage::~VatTuPage() {
    delete ui;
}

void VatTuPage::capNhatTrangThaiNut() {
    ui->errorLabel->setVisible(false);

    bool dangSua = !maVTDangSua.isEmpty();

    if (!dangSua) {
        bool duDuLieu = !ui->maVTEdit->text().trimmed().isEmpty()
        && !ui->tenVTEdit->text().trimmed().isEmpty()
            && !ui->dvtEdit->text().trimmed().isEmpty()
            && !ui->soLuongEdit->text().trimmed().isEmpty();

        bool coGoGiDo = !ui->maVTEdit->text().isEmpty()
                        || !ui->tenVTEdit->text().isEmpty()
                        || !ui->dvtEdit->text().isEmpty()
                        || !ui->soLuongEdit->text().isEmpty();

        ui->themButton->setEnabled(duDuLieu);
        ui->suaButton->setEnabled(false);
        ui->huyButton->setEnabled(coGoGiDo);
    } else {
        bool coThayDoi = (ui->tenVTEdit->text() != tenVTGoc) || (ui->dvtEdit->text() != dvtGoc);

        ui->themButton->setEnabled(false);
        ui->suaButton->setEnabled(coThayDoi);
        ui->huyButton->setEnabled(true);
    }
}

void VatTuPage::onThemClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận thêm",
        QString("Bạn có chắc chắn muốn thêm vật tư mới (Mã: %1)?").arg(ui->maVTEdit->text().toUpper()),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    QString mavt = ui->maVTEdit->text().toUpper().simplified();
    QString tenvt = ui->tenVTEdit->text().simplified();
    QString dvt = ui->dvtEdit->text().simplified();
    int soLuong = ui->soLuongEdit->text().toInt();

    std::string loi;
    bool ok = themVT(root, mavt.toStdString().c_str(), tenvt.toStdString().c_str(),
                     dvt.toStdString().c_str(), soLuong, loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onXoaClicked() {
    int row = ui->table->currentRow();
    QString mavt = ui->table->item(row, 0)->text();
    QString tenvt = ui->table->item(row, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận xóa",
        QString("Bạn có chắc chắn muốn xóa vật tư %1 (Mã: %2)?").arg(tenvt).arg(mavt),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    std::string loi;
    bool ok = xoaVT(root, mavt.toStdString().c_str(), loi);
    if (!ok) {
        QMessageBox::critical(this, "Lỗi xóa vật tư", QString::fromStdString(loi));
        return;
    }

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onSuaClicked() {
    if (maVTDangSua.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận hiệu chỉnh",
        QString("Bạn có chắc chắn muốn sửa thông tin vật tư có mã %1?").arg(maVTDangSua),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    QString tenMoi = ui->tenVTEdit->text().simplified();
    QString dvtMoi = ui->dvtEdit->text().simplified();

    std::string loi;
    bool ok = suaVT(root, maVTDangSua.toStdString().c_str(),
                    tenMoi.toStdString().c_str(), dvtMoi.toStdString().c_str(), loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onHuyClicked() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận hủy",
        "Bạn có chắc chắn muốn hủy thao tác hiện tại? Dữ liệu đang nhập sẽ mất.",
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    ui->table->clearSelection();
    ui->table->setCurrentCell(-1, -1);
    resetForm();
}

void VatTuPage::onTableSelectionChanged() {
    int row = ui->table->currentRow();
    if (row < 0) {
        ui->suaButton->setEnabled(false);
        ui->huyButton->setEnabled(false);
        ui->xoaButton->setEnabled(false);
        return;
    }
    maVTDangSua = ui->table->item(row, 0)->text();
    ui->maVTEdit->setText(maVTDangSua);
    ui->tenVTEdit->setText(ui->table->item(row, 1)->text());
    ui->dvtEdit->setText(ui->table->item(row, 2)->text());
    ui->soLuongEdit->setText(ui->table->item(row, 3)->text());

    tenVTGoc = ui->table->item(row, 1)->text();
    dvtGoc = ui->table->item(row, 2)->text();
    soLuongGoc = ui->table->item(row, 3)->text().toInt();

    ui->maVTEdit->setReadOnly(true);
    ui->soLuongEdit->setReadOnly(true);
    ui->xoaButton->setEnabled(true);
    capNhatTrangThaiNut();
}

void VatTuPage::onTimKiemChanged() {
    lamMoiBang();
}

void VatTuPage::lamMoiBang() {
    int soLuong = 0;
    nodeVT** ds = duyetTheoTen(root, soLuong);

    QString keyword = ui->timKiemEdit->text().trimmed().toUpper();

    int soKhop = 0;
    for (int i = 0; i < soLuong; i++) {
        QString mavt = QString::fromUtf8(ds[i]->vt.MAVT).toUpper();
        QString tenvt = QString::fromUtf8(ds[i]->vt.TENVT).toUpper();
        QString dvt = QString::fromUtf8(ds[i]->vt.DVT).toUpper();
        if (keyword.isEmpty() || mavt.contains(keyword) || tenvt.contains(keyword) || dvt.contains(keyword)) {
            soKhop++;
        }
    }

    ui->table->setRowCount(soKhop);
    int row = 0;
    for (int i = 0; i < soLuong; i++) {
        QString mavt = QString::fromUtf8(ds[i]->vt.MAVT).toUpper();
        QString tenvt = QString::fromUtf8(ds[i]->vt.TENVT).toUpper();
        QString dvt = QString::fromUtf8(ds[i]->vt.DVT).toUpper();
        if (keyword.isEmpty() || mavt.contains(keyword) || tenvt.contains(keyword) || dvt.contains(keyword)) {
            ui->table->setItem(row, 0, new QTableWidgetItem(ds[i]->vt.MAVT));
            ui->table->setItem(row, 1, new QTableWidgetItem(ds[i]->vt.TENVT));
            ui->table->setItem(row, 2, new QTableWidgetItem(ds[i]->vt.DVT));
            ui->table->setItem(row, 3, new QTableWidgetItem(QString::number(ds[i]->vt.SoLuongTon)));
            row++;
        }
    }

    delete[] ds;
}

void VatTuPage::resetForm() {
    QSignalBlocker b1(ui->maVTEdit);
    QSignalBlocker b2(ui->tenVTEdit);
    QSignalBlocker b3(ui->dvtEdit);
    QSignalBlocker b4(ui->soLuongEdit);

    ui->maVTEdit->clear();
    ui->tenVTEdit->clear();
    ui->dvtEdit->clear();
    ui->soLuongEdit->clear();
    ui->maVTEdit->setReadOnly(false);
    ui->soLuongEdit->setReadOnly(false);
    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
    ui->xoaButton->setEnabled(false);
    maVTDangSua.clear();

    capNhatTrangThaiNut();
}