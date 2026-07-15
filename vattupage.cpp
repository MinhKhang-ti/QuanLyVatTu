#include "vattupage.h"
#include "ui_vattupage.h"
#include "vattulogic.h"

#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QHeaderView>

VatTuPage::VatTuPage(TreeVT &rootRef, QWidget *parent) 
    : QWidget(parent), ui(new Ui::VatTuPage), root(rootRef) 
{
    ui->setupUi(this);

    // Cấu hình validator
    ui->maVTEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));
    ui->soLuongEdit->setValidator(new QIntValidator(0, 999999, this));

    // Cấu hình bảng hiển thị
    ui->table->horizontalHeader()->setStretchLastSection(true);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);

    // Kết nối các tín hiệu
    connect(ui->maVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->tenVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->dvtEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->soLuongEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->themButton, &QPushButton::clicked, this, &VatTuPage::onThemClicked);
    connect(ui->xoaButton, &QPushButton::clicked, this, &VatTuPage::onXoaClicked);

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
    ui->maVTEdit->clear();
    ui->tenVTEdit->clear();
    ui->dvtEdit->clear();
    ui->soLuongEdit->clear();
    lamMoiBang();
}

void VatTuPage::onXoaClicked() {
    int row = ui->table->currentRow();
    if (row < 0) return;
    QString mavt = ui->table->item(row, 0)->text();
    std::string loi;
    xoaVT(root, mavt.toStdString().c_str(), loi);
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