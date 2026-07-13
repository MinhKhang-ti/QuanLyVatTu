#include "nhanvienpage.h"
#include "nhanvienlogic.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QRegularExpressionValidator>

NhanVienPage::NhanVienPage(DS_NHANVIEN &dsRef, QWidget *parent) : QWidget(parent), dsnv(dsRef) {

    manvEdit = new QLineEdit;
    manvEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));
    manvEdit->setPlaceholderText("Nhập mã nhân viên");

    hoEdit = new QLineEdit;
    hoEdit->setPlaceholderText("Nhập họ nhân viên");

    tenEdit = new QLineEdit;
    tenEdit->setPlaceholderText("Nhập tên nhân viên");


    phaiCombo = new QComboBox;
    phaiCombo->addItems({"Nam", "Nữ"});

    errorLabel = new QLabel;
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    themButton = new QPushButton("Thêm nhân viên");
    themButton->setEnabled(false);
    xoaButton = new QPushButton("Xóa nhân viên đã chọn");

    QFormLayout *form = new QFormLayout;
    form->addRow("Ma NV:", manvEdit);
    form->addRow("Ho:", hoEdit);
    form->addRow("Ten:", tenEdit);
    form->addRow("Phai:", phaiCombo);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addWidget(themButton);
    btnRow->addWidget(xoaButton);
    btnRow->addStretch();

    table = new QTableWidget(0, 4);
    table->setHorizontalHeaderLabels({"MÃ NV", "HỌ", "TÊN", "PHÁI"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(btnRow);
    mainLayout->addWidget(table);

    connect(manvEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(hoEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(tenEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(themButton, &QPushButton::clicked, this, &NhanVienPage::onThemClicked);
    connect(xoaButton, &QPushButton::clicked, this, &NhanVienPage::onXoaClicked);
}

void NhanVienPage::validateForm() {
    QString loi;
    if (manvEdit->text().trimmed().isEmpty()) loi = "Mã nhân viên không được rỗng";
    else if (hoEdit->text().trimmed().isEmpty()) loi = "Họ không được rỗng";
    else if (tenEdit->text().trimmed().isEmpty()) loi = "Tên không được rỗng";

    if (!loi.isEmpty()) {
        errorLabel->setText(loi);
        errorLabel->setVisible(true);
        themButton->setEnabled(false);
    } else {
        errorLabel->setVisible(false);
        themButton->setEnabled(true);
    }
}

void NhanVienPage::onThemClicked() {
    std::string loi;
    bool ok = themNV(dsnv,
                     manvEdit->text().toUpper().toStdString().c_str(),
                     hoEdit->text().toStdString().c_str(),
                     tenEdit->text().toStdString().c_str(),
                     phaiCombo->currentText().toStdString().c_str(),
                     loi);
    if (!ok) {
        errorLabel->setText(QString::fromStdString(loi));
        errorLabel->setVisible(true);
        return;
    }
    manvEdit->clear();
    hoEdit->clear();
    tenEdit->clear();
    lamMoiBang();
}

void NhanVienPage::onXoaClicked() {
    int row = table->currentRow();
    if (row < 0) return;
    QString manv = table->item(row, 0)->text();
    std::string loi;
    xoaNV(dsnv, manv.toStdString().c_str(), loi);
    lamMoiBang();
}

void NhanVienPage::lamMoiBang() {
    table->setRowCount(dsnv.n);
    for (int i = 0; i < dsnv.n; i++) {
        table->setItem(i, 0, new QTableWidgetItem(dsnv.nodes[i]->MANV));
        table->setItem(i, 1, new QTableWidgetItem(dsnv.nodes[i]->HO));
        table->setItem(i, 2, new QTableWidgetItem(dsnv.nodes[i]->TEN));
        table->setItem(i, 3, new QTableWidgetItem(dsnv.nodes[i]->PHAI));
    }
}