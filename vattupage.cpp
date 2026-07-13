#include "vattupage.h"
#include "vattulogic.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QIntValidator>
#include <QRegularExpressionValidator>

VatTuPage::VatTuPage(TreeVT &rootRef, QWidget *parent) : QWidget(parent), root(rootRef) {
    maVTEdit = new QLineEdit;
    maVTEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));
    maVTEdit->setPlaceholderText("nhập mã VT");

    tenVTEdit = new QLineEdit;
    tenVTEdit->setPlaceholderText("nhập tên VT");
    dvtEdit = new QLineEdit;
    dvtEdit->setPlaceholderText("nhập đv tính");

    soLuongEdit = new QLineEdit;
    soLuongEdit->setValidator(new QIntValidator(0, 999999, this));
    soLuongEdit->setPlaceholderText("chỉ nhập số nguyên không âm");

    errorLabel = new QLabel;
    errorLabel->setStyleSheet("color: red;");
    errorLabel->setVisible(false);

    themButton = new QPushButton("Thêm vật tư");
    themButton->setEnabled(false);
    xoaButton = new QPushButton("Xóa vật tư");

    QFormLayout *form = new QFormLayout;
    form->addRow("MÃ VẬT TƯ:", maVTEdit);
    form->addRow("TÊN VẬT TƯ:", tenVTEdit);
    form->addRow("ĐƠN VỊ TÍNH:", dvtEdit);
    form->addRow("SỐ LƯỢNG TỒN:", soLuongEdit);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->addWidget(themButton);
    btnRow->addWidget(xoaButton);
    btnRow->addStretch();

    table = new QTableWidget(0, 4);
    table->setHorizontalHeaderLabels({"Mã VT", "Tên vật tư", "Đơn vị tính", "SL tồn"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(form);
    mainLayout->addWidget(errorLabel);
    mainLayout->addLayout(btnRow);
    mainLayout->addWidget(table);

    connect(maVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(tenVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(dvtEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(soLuongEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(themButton, &QPushButton::clicked, this, &VatTuPage::onThemClicked);
    connect(xoaButton, &QPushButton::clicked, this, &VatTuPage::onXoaClicked);
}

void VatTuPage::validateForm() {
    QString loi;
    if (maVTEdit->text().trimmed().isEmpty()) loi = "Mã vật tư không được để trống";
    else if (tenVTEdit->text().trimmed().isEmpty()) loi = "Tên vật tư không được để trống";
    else if (dvtEdit->text().trimmed().isEmpty()) loi = "Đơn vị tính không được để trống";
    else if (soLuongEdit->text().trimmed().isEmpty()) loi = "Số lượng tồn không được để trống";

    if (!loi.isEmpty()) {
        errorLabel->setText(loi);
        errorLabel->setVisible(true);
        themButton->setEnabled(false);
    } else {
        errorLabel->setVisible(false);
        themButton->setEnabled(true);
    }
}

void VatTuPage::onThemClicked() {
    std::string loi;
    bool ok = themVT(root,
                     maVTEdit->text().toUpper().toStdString().c_str(),
                     tenVTEdit->text().toStdString().c_str(),
                     dvtEdit->text().toStdString().c_str(),
                     soLuongEdit->text().toInt(),
                     loi);
    if (!ok) {
        errorLabel->setText(QString::fromStdString(loi));
        errorLabel->setVisible(true);
        return;
    }
    maVTEdit->clear();
    tenVTEdit->clear();
    dvtEdit->clear();
    soLuongEdit->clear();
    lamMoiBang();
}

void VatTuPage::onXoaClicked() {
    int row = table->currentRow();
    if (row < 0) return;
    QString mavt = table->item(row, 0)->text();
    std::string loi;
    xoaVT(root, mavt.toStdString().c_str(), loi);
    lamMoiBang();
}

void VatTuPage::lamMoiBang() {
    int soLuong = 0;
    VATTU* ds = duyetTheoTen(root, soLuong);

    table->setRowCount(soLuong);
    for (int i = 0; i < soLuong; i++) {
        table->setItem(i, 0, new QTableWidgetItem(ds[i].MAVT));
        table->setItem(i, 1, new QTableWidgetItem(ds[i].TENVT));
        table->setItem(i, 2, new QTableWidgetItem(ds[i].DVT));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(ds[i].SoLuongTon)));
    }

    delete[] ds; // giai phong mang dong sau khi da dung xong
}