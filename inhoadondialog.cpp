#include "inhoadondialog.h"
#include "hoadonlogic.h"
#include "vattulogic.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QRegularExpressionValidator>

InHoaDonDialog::InHoaDonDialog(TreeVT& rootRef, DS_NHANVIEN& dsRef, QWidget* parent)
    : QDialog(parent), root(rootRef), dsnv(dsRef)
{
    setWindowTitle("In hóa đơn");
    resize(700, 650);

    // ==== Bảng danh sách toàn bộ hóa đơn — MỚI ====
    QLabel* danhSachTitle = new QLabel("Chọn 1 hóa đơn trong danh sách, hoặc nhập số HĐ bên dưới:", this);
    danhSachTable = new QTableWidget(this);
    danhSachTable->setColumnCount(4);
    danhSachTable->setHorizontalHeaderLabels({"Số HĐ", "Ngày lập", "Loại", "Người lập"});
    danhSachTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    danhSachTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    danhSachTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    danhSachTable->setMaximumHeight(180);

    // ==== Ô nhập nhanh ====
    soHDEdit = new QLineEdit(this);
    soHDEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[A-Za-z0-9]{0,20}"), this));
    soHDEdit->setPlaceholderText("Hoặc gõ trực tiếp số hóa đơn...");
    timButton = new QPushButton("Tìm", this);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: red; font-weight: bold;");
    errorLabel->setVisible(false);
    errorLabel->setWordWrap(true);

    thongTinLabel = new QLabel(this);
    thongTinLabel->setWordWrap(true);

    table = new QTableWidget(this);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Tên vật tư", "Số lượng", "Đơn giá", "VAT (%)", "Thành tiền"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    tongTienLabel = new QLabel(this);
    tongTienLabel->setAlignment(Qt::AlignRight);
    QFont f = tongTienLabel->font();
    f.setPointSize(13);
    f.setBold(true);
    tongTienLabel->setFont(f);
    tongTienLabel->setStyleSheet("color: #1976d2;");

    tienChuLabel = new QLabel(this);
    tienChuLabel->setAlignment(Qt::AlignRight);
    tienChuLabel->setStyleSheet("color: black; font-size: 16px;");

    QHBoxLayout* timLayout = new QHBoxLayout();
    timLayout->addWidget(new QLabel("Số hóa đơn:", this));
    timLayout->addWidget(soHDEdit);
    timLayout->addWidget(timButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(danhSachTitle);
    mainLayout->addWidget(danhSachTable);
    mainLayout->addLayout(timLayout);
    mainLayout->addWidget(errorLabel);
    mainLayout->addWidget(thongTinLabel);
    mainLayout->addWidget(table);
    mainLayout->addWidget(tongTienLabel);
    mainLayout->addWidget(tienChuLabel);

    connect(timButton, &QPushButton::clicked, this, &InHoaDonDialog::onTimClicked);
    connect(soHDEdit, &QLineEdit::returnPressed, this, &InHoaDonDialog::onTimClicked);
    connect(danhSachTable, &QTableWidget::cellClicked, this, &InHoaDonDialog::onChonHDTrongDanhSach);

    napDanhSachHD();   // đổ danh sách ngay khi mở dialog
}


void InHoaDonDialog::napDanhSachHD() {
    danhSachTable->setRowCount(0);
    int row = 0;
    for (int i = 0; i < dsnv.n; i++) {
        NHANVIEN* nv = dsnv.nodes[i];
        nodeHD* p = nv->dshd;
        while (p) {
            danhSachTable->insertRow(row);
            QString ngay = QString("%1/%2/%3")
                               .arg(p->hd.NgayLap.ngay, 2, 10, QChar('0'))
                               .arg(p->hd.NgayLap.thang, 2, 10, QChar('0'))
                               .arg(p->hd.NgayLap.nam);
            QString loai = (p->hd.Loai == 'N') ? "Nhập" : "Xuất";
            QString hoTen = QString("%1 %2").arg(nv->HO, nv->TEN);

            danhSachTable->setItem(row, 0, new QTableWidgetItem(p->hd.SoHD));
            danhSachTable->setItem(row, 1, new QTableWidgetItem(ngay));
            danhSachTable->setItem(row, 2, new QTableWidgetItem(loai));
            danhSachTable->setItem(row, 3, new QTableWidgetItem(hoTen));

            p = p->next;
            row++;
        }
    }
}

void InHoaDonDialog::onChonHDTrongDanhSach(int row, int /*column*/) {
    QTableWidgetItem* item = danhSachTable->item(row, 0);
    if (!item) return;
    soHDEdit->setText(item->text());
    onTimClicked();   // tái sử dụng logic tìm + hiển thị đã có
}

void InHoaDonDialog::xoaBang() {
    table->setRowCount(0);
    thongTinLabel->clear();
    tongTienLabel->clear();
    tienChuLabel->clear();
}

void InHoaDonDialog::onTimClicked() {
    errorLabel->setVisible(false);
    xoaBang();

    QString soHD = soHDEdit->text().trimmed().toUpper();
    if (soHD.isEmpty()) {
        errorLabel->setText("Vui lòng nhập hoặc chọn số hóa đơn!");
        errorLabel->setVisible(true);
        return;
    }

    int idxNV = -1;
    nodeHD* hd = timHoaDonTrongHeThong(dsnv, soHD.toStdString().c_str(), idxNV);
    if (!hd) {
        errorLabel->setText("Không tìm thấy hóa đơn có số: " + soHD);
        errorLabel->setVisible(true);
        return;
    }

    hienThiHoaDon(hd, idxNV);
}

void InHoaDonDialog::hienThiHoaDon(nodeHD* hd, int idxNV) {
    NHANVIEN* nv = dsnv.nodes[idxNV];
    QString hoTen = QString("%1 %2").arg(nv->HO, nv->TEN);
    QString ngay = QString("%1/%2/%3")
                       .arg(hd->hd.NgayLap.ngay, 2, 10, QChar('0'))
                       .arg(hd->hd.NgayLap.thang, 2, 10, QChar('0'))
                       .arg(hd->hd.NgayLap.nam);
    QString loai = (hd->hd.Loai == 'N') ? "Phiếu nhập" : "Phiếu xuất";

    thongTinLabel->setText(QString("<b>Ngày lập:</b> %1 &nbsp;&nbsp; <b>Người lập:</b> %2 &nbsp;&nbsp; <b>Loại:</b> %3")
                               .arg(ngay, hoTen, loai));

    const DS_CTHD& ds = hd->hd.dscthd;
    table->setRowCount(ds.n);
    for (int i = 0; i < ds.n; i++) {
        const CT_HOADON& ct = ds.nodes[i];
        nodeVT* vtNode = timVT(root, ct.MAVT);
        QString tenVT = vtNode ? QString::fromUtf8(vtNode->vt.TENVT) : "(vật tư đã bị xóa khỏi danh mục)";

        double thanhTien = tinhTriGiaDong(ct);
        table->setItem(i, 0, new QTableWidgetItem(tenVT));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(ct.SoLuong)));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(ct.DonGia, 'f', 0)));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(ct.VAT, 'f', 1)));
        table->setItem(i, 4, new QTableWidgetItem(QString::number(thanhTien, 'f', 0)));
    }

    double tongTien = tinhTongTriGiaHD(ds);
    tongTienLabel->setText(QString("TỔNG TRỊ GIÁ HÓA ĐƠN: %1 VNĐ").arg(QString::number(tongTien, 'f', 0)));

    QString tienChu = QString::fromStdString(docSoThanhChu(static_cast<long long>(tongTien)));
    tienChuLabel->setText("(Bằng chữ: " + tienChu + ")");
}