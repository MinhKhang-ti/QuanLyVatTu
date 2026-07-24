#include "hoadonpage.h"
#include "ui_hoadonpage.h"
#include "hoadonlogic.h"
#include "vattulogic.h"
#include "nhanvienlogic.h"
#include "fileio.h"

#include <QDate>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <cstring>
#include <algorithm>

HoaDonPage::HoaDonPage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::HoaDonPage), root(rootRef), dsnv(dsRef)
{
    ui->setupUi(this);

    // Cấu hình validator cho Số hóa đơn (tối đa 20 ký tự chữ và số)
    ui->soHDEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,20}"), this));

    // Cấu hình validator số lượng, đơn giá, VAT
    ui->soLuongEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[1-9][0-9]{0,8}"), this));
    ui->donGiaEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{1,12}(\\.[0-9]{1,2})?"), this));
    ui->vatEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{1,2}(\\.[0-9]{1,2})?"), this));

    // Cấu hình ngày lập mặc định là ngày hiện tại
    ui->ngayLapEdit->setDate(QDate::currentDate());

    // Cấu hình bảng hiển thị chi tiết hóa đơn
    ui->tableCTHD->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCTHD->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCTHD->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->errorLabel->setVisible(false);
    cthdTam.n = 0;

    // Kết nối tín hiệu nút bấm
    connect(ui->themCTButton, &QPushButton::clicked, this, &HoaDonPage::onThemCTClicked);
    connect(ui->xoaCTButton, &QPushButton::clicked, this, &HoaDonPage::onXoaCTClicked);
    connect(ui->ghiButton, &QPushButton::clicked, this, &HoaDonPage::onGhiClicked);
    connect(ui->huyButton, &QPushButton::clicked, this, &HoaDonPage::onHuyClicked);

    napDuLieuCombo();
    capNhatBangCTHD();
}

HoaDonPage::~HoaDonPage()
{
    delete ui;
}

void HoaDonPage::napDuLieuCombo() {
    napNVCombo();
    napVTCombo();
}

void HoaDonPage::napNVCombo() {
    ui->nvCombo->clear();
    for (int i = 0; i < dsnv.n; i++) {
        QString itemText = QString("%1 - %2 %3")
                               .arg(dsnv.nodes[i]->MANV)
                               .arg(dsnv.nodes[i]->HO)
                               .arg(dsnv.nodes[i]->TEN);
        ui->nvCombo->addItem(itemText, QString(dsnv.nodes[i]->MANV));
    }
}

void HoaDonPage::napVTCombo() {
    ui->vtCombo->clear();
    int soLuong = 0;
    VATTU* ds = duyetTheoTen(root, soLuong);
    for (int i = 0; i < soLuong; i++) {
        QString itemText = QString("%1 - %2 (Tồn: %3)")
                               .arg(ds[i].MAVT)
                               .arg(ds[i].TENVT)
                               .arg(ds[i].SoLuongTon);
        ui->vtCombo->addItem(itemText, QString(ds[i].MAVT));
    }
    delete[] ds;
}

void HoaDonPage::onThemCTClicked() {
    ui->errorLabel->setVisible(false);

    if (ui->vtCombo->currentIndex() < 0) {
        ui->errorLabel->setText("Vui lòng chọn vật tư!");
        ui->errorLabel->setVisible(true);
        return;
    }

    QString mavt = ui->vtCombo->currentData().toString();
    int soLuong = ui->soLuongEdit->text().toInt();
    float donGia = ui->donGiaEdit->text().toFloat();
    float vat = ui->vatEdit->text().toFloat();

    if (soLuong <= 0) {
        ui->errorLabel->setText("Số lượng phải lớn hơn 0!");
        ui->errorLabel->setVisible(true);
        return;
    }
    if (donGia <= 0) {
        ui->errorLabel->setText("Đơn giá phải lớn hơn 0!");
        ui->errorLabel->setVisible(true);
        return;
    }

    // Kiểm tra tồn kho nếu là Hóa đơn Xuất ('X')
    char loaiHD = (ui->loaiCombo->currentIndex() == 0) ? 'N' : 'X';
    nodeVT* node = timVT(root, mavt.toStdString().c_str());

    if (!node) {
        ui->errorLabel->setText("Vật tư không tồn tại trong hệ thống!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (loaiHD == 'X') {
        // Tính tổng số lượng của vật tư này (nếu đã có trong cthdTam)
        int soLuongDaChon = 0;
        for (int i = 0; i < cthdTam.n; i++) {
            if (strcmp(cthdTam.nodes[i].MAVT, mavt.toStdString().c_str()) == 0) {
                soLuongDaChon += cthdTam.nodes[i].SoLuong;
            }
        }
        int tonKho = node->vt.SoLuongTon;
        if (soLuongDaChon + soLuong > tonKho) {
            ui->errorLabel->setText(
                QString("LỖI XUẤT HÀNG: Số lượng xuất vượt quá tồn kho!\n"
                        "Vật tư [%1 - %2] hiện chỉ còn tồn: %3 trong kho.")
                    .arg(node->vt.MAVT)
                    .arg(node->vt.TENVT)
                    .arg(tonKho));
            ui->errorLabel->setVisible(true);
            return;
        }
    }

    std::string loi;
    bool ok = themCTHD(cthdTam, mavt.toStdString().c_str(), soLuong, donGia, vat, loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    ui->soLuongEdit->clear();
    ui->donGiaEdit->clear();
    capNhatBangCTHD();
}

void HoaDonPage::onXoaCTClicked() {
    int row = ui->tableCTHD->currentRow();
    if (row < 0 || ui->tableCTHD->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn vật tư muốn xóa khỏi hóa đơn đang lập!");
        return;
    }

    QString mavt = ui->tableCTHD->item(row, 0)->text();
    std::string loi;
    xoaCTHD(cthdTam, mavt.toStdString().c_str(), loi);

    capNhatBangCTHD();
}

void HoaDonPage::onGhiClicked() {
    ui->errorLabel->setVisible(false);

    QString soHD = ui->soHDEdit->text().trimmed().toUpper();
    if (soHD.isEmpty()) {
        ui->errorLabel->setText("Vui lòng nhập Số hóa đơn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (ui->nvCombo->currentIndex() < 0) {
        ui->errorLabel->setText("Vui lòng chọn Nhân viên lập hóa đơn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    if (cthdTam.n == 0) {
        ui->errorLabel->setText("Hóa đơn phải có ít nhất 1 vật tư!");
        ui->errorLabel->setVisible(true);
        return;
    }

    // Kiểm tra Số HĐ đã tồn tại trong toàn bộ hệ thống chưa
    for (int i = 0; i < dsnv.n; i++) {
        if (timHoaDon(dsnv.nodes[i]->dshd, soHD.toStdString().c_str()) != nullptr) {
            ui->errorLabel->setText("Số hóa đơn này đã tồn tại trong hệ thống! Vui lòng nhập số HĐ khác.");
            ui->errorLabel->setVisible(true);
            return;
        }
    }

    QString manv = ui->nvCombo->currentData().toString();
    int idxNV = timViTriNV(dsnv, manv.toStdString().c_str());
    if (idxNV == -1) {
        ui->errorLabel->setText("Không tìm thấy nhân viên chọn!");
        ui->errorLabel->setVisible(true);
        return;
    }

    QDate qDate = ui->ngayLapEdit->date();
    Date ngayLap;
    ngayLap.ngay = qDate.day();
    ngayLap.thang = qDate.month();
    ngayLap.nam = qDate.year();

    char loaiHD = (ui->loaiCombo->currentIndex() == 0) ? 'N' : 'X';

    // Xác nhận từ người dùng trước khi ghi
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận Ghi Hóa Đơn",
        QString("Bạn có chắc chắn muốn GHI hóa đơn %1 (%2) với %3 vật tư?")
            .arg(soHD)
            .arg(loaiHD == 'N' ? "Nhập" : "Xuất")
            .arg(cthdTam.n),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    // 1. Tạo Node Hóa đơn mới
    nodeHD* hdMoi = taoHoaDon(soHD.toStdString().c_str(), ngayLap, loaiHD);
    hdMoi->hd.dscthd = cthdTam;

    // 2. Thêm vào DSLK của Nhân viên
    themHoaDonVaoDS(dsnv.nodes[idxNV]->dshd, hdMoi);
    dsnv.nodes[idxNV]->CoHD = true;

    // 3. Tự động cập nhật số lượng tồn trên Cây BST Vật tư
    for (int i = 0; i < cthdTam.n; i++) {
        nodeVT* node = timVT(root, cthdTam.nodes[i].MAVT);
        if (node) {
            if (loaiHD == 'N') {
                node->vt.SoLuongTon += cthdTam.nodes[i].SoLuong;
            } else if (loaiHD == 'X') {
                node->vt.SoLuongTon -= cthdTam.nodes[i].SoLuong;
            }
        }
    }

    // 4. Lưu lại dữ liệu ra đĩa
    luuNhanVien(dsnv, FILE_NHANVIEN);
    luuVatTu(root, FILE_VATTU);

    QMessageBox::information(this, "Thành công", 
        QString("Đã ghi hóa đơn %1 thành công! Số lượng tồn kho đã được cập nhật.").arg(soHD));

    // Reset Form
    resetForm();
    napVTCombo(); // Cập nhật lại tồn kho mới trên combobox
}

void HoaDonPage::onHuyClicked() {
    resetForm();
}

void HoaDonPage::resetForm() {
    ui->soHDEdit->clear();
    ui->soLuongEdit->clear();
    ui->donGiaEdit->clear();
    ui->vatEdit->setText("5");
    ui->ngayLapEdit->setDate(QDate::currentDate());
    ui->loaiCombo->setCurrentIndex(0);
    ui->errorLabel->setVisible(false);

    cthdTam.n = 0;
    capNhatBangCTHD();
}

void HoaDonPage::capNhatBangCTHD() {
    ui->tableCTHD->setRowCount(cthdTam.n);
    double tongTienHD = 0;

    for (int i = 0; i < cthdTam.n; i++) {
        const CT_HOADON& ct = cthdTam.nodes[i];
        nodeVT* node = timVT(root, ct.MAVT);
        QString tenVT = node ? QString::fromUtf8(node->vt.TENVT) : "---";

        double thanhTien = tinhTriGiaDong(ct);
        tongTienHD += thanhTien;

        ui->tableCTHD->setItem(i, 0, new QTableWidgetItem(ct.MAVT));
        ui->tableCTHD->setItem(i, 1, new QTableWidgetItem(tenVT));
        ui->tableCTHD->setItem(i, 2, new QTableWidgetItem(QString::number(ct.SoLuong)));
        ui->tableCTHD->setItem(i, 3, new QTableWidgetItem(QString::number(ct.DonGia, 'f', 0)));
        ui->tableCTHD->setItem(i, 4, new QTableWidgetItem(QString::number(ct.VAT, 'f', 1)));
        ui->tableCTHD->setItem(i, 5, new QTableWidgetItem(QString::number(thanhTien, 'f', 0)));
    }

    ui->tongTienLabel->setText(QString("%1 VNĐ").arg(QString::number(tongTienHD, 'f', 0)));
}