#include "nhanvienpage.h"
#include "ui_nhanvienpage.h"
#include "nhanvienlogic.h"

#include <QRegularExpressionValidator>
#include <QHeaderView>

NhanVienPage::NhanVienPage(DS_NHANVIEN &dsRef, QWidget *parent) 
    : QWidget(parent), ui(new Ui::NhanVienPage), dsnv(dsRef) 
{
    ui->setupUi(this);

    // Cấu hình validator cho Mã nhân viên
    ui->manvEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));

    // Cấu hình bảng hiển thị
    ui->table->horizontalHeader()->setStretchLastSection(true);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);

    // Kết nối các tín hiệu
    connect(ui->manvEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(ui->hoEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(ui->tenEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(ui->themButton, &QPushButton::clicked, this, &NhanVienPage::onThemClicked);
    connect(ui->xoaButton, &QPushButton::clicked, this, &NhanVienPage::onXoaClicked);

    lamMoiBang();
}

NhanVienPage::~NhanVienPage() {
    delete ui;
}

void NhanVienPage::validateForm() {
    QString loi;
    if (ui->manvEdit->text().trimmed().isEmpty()) loi = "Mã nhân viên không được rỗng";
    else if (ui->hoEdit->text().trimmed().isEmpty()) loi = "Họ không được rỗng";
    else if (ui->tenEdit->text().trimmed().isEmpty()) loi = "Tên không được rỗng";

    if (!loi.isEmpty()) {
        ui->errorLabel->setText(loi);
        ui->errorLabel->setVisible(true);
        ui->themButton->setEnabled(false);
    } else {
        ui->errorLabel->setVisible(false);
        ui->themButton->setEnabled(true);
    }
}

void NhanVienPage::onThemClicked() {
    std::string loi;
    bool ok = themNV(dsnv,
                     ui->manvEdit->text().toUpper().toStdString().c_str(),
                     ui->hoEdit->text().toStdString().c_str(),
                     ui->tenEdit->text().toStdString().c_str(),
                     ui->phaiCombo->currentText().toStdString().c_str(),
                     loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }
    ui->manvEdit->clear();
    ui->hoEdit->clear();
    ui->tenEdit->clear();
    lamMoiBang();
}

void NhanVienPage::onXoaClicked() {
    int row = ui->table->currentRow();
    if (row < 0) return;
    QString manv = ui->table->item(row, 0)->text();
    std::string loi;
    xoaNV(dsnv, manv.toStdString().c_str(), loi);
    lamMoiBang();
}

void NhanVienPage::lamMoiBang() {
    ui->table->setRowCount(dsnv.n);
    for (int i = 0; i < dsnv.n; i++) {
        ui->table->setItem(i, 0, new QTableWidgetItem(dsnv.nodes[i]->MANV));
        ui->table->setItem(i, 1, new QTableWidgetItem(dsnv.nodes[i]->HO));
        ui->table->setItem(i, 2, new QTableWidgetItem(dsnv.nodes[i]->TEN));
        ui->table->setItem(i, 3, new QTableWidgetItem(dsnv.nodes[i]->PHAI));
    }
}