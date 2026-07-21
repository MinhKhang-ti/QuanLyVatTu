#include "vattupage.h"
#include "fileio.h"
#include "ui_vattupage.h"
#include "vattulogic.h"
#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QSignalBlocker>

VatTuPage::VatTuPage(TreeVT &rootRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::VatTuPage), root(rootRef)
{
    ui->setupUi(this);
    // Cấu hình validator
    ui->maVTEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));
    ui->soLuongEdit->setValidator(new QIntValidator(0, 999999, this));
    // Cấu hình bảng hiển thị
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(false);
    // Kết nối các tín hiệu
    connect(ui->maVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->tenVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->dvtEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->soLuongEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->themButton, &QPushButton::clicked, this, &VatTuPage::onThemClicked);
    connect(ui->xoaButton, &QPushButton::clicked, this, &VatTuPage::onXoaClicked);
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &VatTuPage::onTableSelectionChanged);
    connect(ui->suaButton, &QPushButton::clicked, this, &VatTuPage::onSuaClicked);

     docVatTu(root, FILE_VATTU);
    lamMoiBang();
}

VatTuPage::~VatTuPage() {
    delete ui;
}

void VatTuPage::validateForm() {
    QString loi;
    if (ui->maVTEdit->text().trimmed().isEmpty()) loi = "Mã vật tư không được để trống";
    else if (ui->tenVTEdit->text().trimmed().isEmpty()) loi = "Tên vật tư không được để trống";
    else if (ui->dvtEdit->text().trimmed().isEmpty()) loi = "Đơn vị tính không được để trống";
    else if (ui->soLuongEdit->text().trimmed().isEmpty()) loi = "Số lượng tồn không được để trống";

    if (!loi.isEmpty()) {
        ui->errorLabel->setText(loi);
        ui->errorLabel->setVisible(true);
        ui->themButton->setEnabled(false);
    } else {
        ui->errorLabel->setVisible(false);
        ui->themButton->setEnabled(true);
    }
}

void VatTuPage::onThemClicked() {
    std::string loi;
    bool ok = themVT(root,
                     ui->maVTEdit->text().toUpper().toStdString().c_str(),
                     ui->tenVTEdit->text().toStdString().c_str(),
                     ui->dvtEdit->text().toStdString().c_str(),
                     ui->soLuongEdit->text().toInt(),
                     loi);
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
    if (row < 0) return;
    QString mavt = ui->table->item(row, 0)->text();
    std::string loi;
    xoaVT(root, mavt.toStdString().c_str(), loi);
    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onTableSelectionChanged() {
    int row = ui->table->currentRow();
    if (row < 0) {
        ui->suaButton->setEnabled(false);
        return;
    }
    maVTDangSua = ui->table->item(row, 0)->text();
    ui->maVTEdit->setText(maVTDangSua);
    ui->tenVTEdit->setText(ui->table->item(row, 1)->text());
    ui->dvtEdit->setText(ui->table->item(row, 2)->text());
    ui->soLuongEdit->setText(ui->table->item(row, 3)->text());

    ui->maVTEdit->setReadOnly(true);
    ui->soLuongEdit->setReadOnly(true);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(true);
}

void VatTuPage::onSuaClicked() {
    if (maVTDangSua.isEmpty()) return;

    std::string loi;
    bool ok = suaVT(root,
                    maVTDangSua.toStdString().c_str(),
                    ui->tenVTEdit->text().toStdString().c_str(),
                    ui->dvtEdit->text().toStdString().c_str(),
                    loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::lamMoiBang() {
    int soLuong = 0;
    VATTU* ds = duyetTheoTen(root, soLuong);
    ui->table->setRowCount(soLuong);
    for (int i = 0; i < soLuong; i++) {
        ui->table->setItem(i, 0, new QTableWidgetItem(ds[i].MAVT));
        ui->table->setItem(i, 1, new QTableWidgetItem(ds[i].TENVT));
        ui->table->setItem(i, 2, new QTableWidgetItem(ds[i].DVT));
        ui->table->setItem(i, 3, new QTableWidgetItem(QString::number(ds[i].SoLuongTon)));
    }
    delete[] ds; // giai phong mang dong sau khi da dung xong
}

void VatTuPage::resetForm() {
    // Chan tin hieu textChanged trong luc reset, tranh goi validateForm() oan
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
    maVTDangSua.clear();

    // QSignalBlocker tu dong "mo lai" khi ra khoi ham (het scope)
    // nen goi validateForm() 1 lan duy nhat, thu cong, de cap nhat dung trang thai nut
    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
}