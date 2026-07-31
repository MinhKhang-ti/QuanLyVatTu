#include "nhanvienpage.h"
#include "ui_nhanvienpage.h"
#include "nhanvienlogic.h"
#include "fileio.h"

#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QMessageBox>
#include <cstring>

NhanVienPage::NhanVienPage(DS_NHANVIEN &dsRef, QWidget *parent) 
    : QWidget(parent), ui(new Ui::NhanVienPage), dsnv(dsRef) 
{
    ui->setupUi(this);

    // Cấu hình validator cho Mã nhân viên (chặn khoảng trắng, tối đa 10 ký tự chữ và số)
    ui->manvEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));

    // Cấu hình validator cho Tên nhân viên (chặn khoảng trắng, tối đa 20 ký tự)
    ui->tenEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[^\\s]{0,20}"), this));

    // Cấu hình bảng hiển thị
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
    ui->xoaButton->setEnabled(false);

    // Kết nối các tín hiệu nhập liệu
    connect(ui->manvEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(ui->hoEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    connect(ui->tenEdit, &QLineEdit::textChanged, this, &NhanVienPage::validateForm);
    
    // Kết nối tín hiệu tìm kiếm
    connect(ui->timKiemEdit, &QLineEdit::textChanged, this, &NhanVienPage::onTimKiemChanged);
    
    // Kết nối các tín hiệu nút bấm
    connect(ui->themButton, &QPushButton::clicked, this, &NhanVienPage::onThemClicked);
    connect(ui->suaButton, &QPushButton::clicked, this, &NhanVienPage::onSuaClicked);
    connect(ui->xoaButton, &QPushButton::clicked, this, &NhanVienPage::onXoaClicked);
    connect(ui->huyButton, &QPushButton::clicked, this, &NhanVienPage::onHuyClicked);
    
    // Kết nối sự kiện chọn dòng và đúp chuột vào bảng
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &NhanVienPage::onTableSelectionChanged);
    connect(ui->table, &QTableWidget::cellDoubleClicked, this, &NhanVienPage::onCellDoubleClicked);

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

    bool isValid = loi.isEmpty();
    if (!isValid) {
        ui->errorLabel->setText(loi);
        ui->errorLabel->setVisible(true);
        ui->themButton->setEnabled(false);
        ui->suaButton->setEnabled(false);
    } else {
        ui->errorLabel->setVisible(false);
        if (ui->manvEdit->isEnabled()) {
            ui->themButton->setEnabled(true);
            ui->suaButton->setEnabled(false);
        } else {
            ui->themButton->setEnabled(false);
            ui->suaButton->setEnabled(true);
        }
    }
}

void NhanVienPage::onThemClicked() {
    QString manv = ui->manvEdit->text().toUpper().simplified();
    QString ho = ui->hoEdit->text().simplified();
    QString ten = ui->tenEdit->text().simplified();

    std::string loi;
    bool ok = themNV(dsnv,
                     manv.toStdString().c_str(),
                     ho.toStdString().c_str(),
                     ten.toStdString().c_str(),
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
    luuNhanVien(dsnv, FILE_NHANVIEN);
}

void NhanVienPage::onSuaClicked() {
    QString manv = ui->manvEdit->text().toUpper().simplified();
    if (manv.isEmpty()) return;

    // Hiển thị hộp thoại hỏi xác nhận trước khi sửa thông tin
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận hiệu chỉnh", 
                                  QString("Bạn có chắc chắn muốn sửa thông tin nhân viên có mã %1?").arg(manv),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    QString ho = ui->hoEdit->text().simplified();
    QString ten = ui->tenEdit->text().simplified();

    std::string loi;
    bool ok = suaNV(dsnv,
                    manv.toStdString().c_str(),
                    ho.toStdString().c_str(),
                    ten.toStdString().c_str(),
                    ui->phaiCombo->currentText().toStdString().c_str(),
                    loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    onHuyClicked(); // Giải phóng form và bật lại ô nhập mã nhân viên
    lamMoiBang();
    luuNhanVien(dsnv, FILE_NHANVIEN);
}

void NhanVienPage::onXoaClicked() {
    int row = ui->table->currentRow();
    if (row < 0 || ui->table->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn nhân viên muốn xóa từ bảng danh sách!");
        return;
    }
    QString manv = ui->table->item(row, 0)->text();
    QString hoTen = ui->table->item(row, 1)->text() + " " + ui->table->item(row, 2)->text();
    
    // Hiển thị hộp thoại hỏi xác nhận trước khi xóa
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận xóa", 
                                  QString("Bạn có chắc chắn muốn xóa nhân viên %1 (Mã: %2)?").arg(hoTen).arg(manv),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    std::string loi;
    bool ok = xoaNV(dsnv, manv.toStdString().c_str(), loi);
    if (!ok) {
        QMessageBox::critical(this, "Lỗi xóa nhân viên", QString::fromStdString(loi));
        return;
    }
    
    onHuyClicked();
    lamMoiBang();
    luuNhanVien(dsnv, FILE_NHANVIEN);
}

void NhanVienPage::onHuyClicked() {
    // Bỏ chọn dòng hiện tại trên bảng
    ui->table->clearSelection();
    ui->table->setCurrentCell(-1, -1);

    // Kích hoạt lại ô nhập mã nhân viên và xóa trắng form nhập liệu
    ui->manvEdit->setEnabled(true);
    ui->manvEdit->clear();
    ui->hoEdit->clear();
    ui->tenEdit->clear();
    ui->phaiCombo->setCurrentIndex(0);
    ui->errorLabel->setVisible(false);

    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
    ui->xoaButton->setEnabled(false);
}

void NhanVienPage::onTableSelectionChanged() {
    int row = ui->table->currentRow();
    if (row < 0 || ui->table->selectedItems().isEmpty()) {
        ui->xoaButton->setEnabled(false);
        return;
    }

    QTableWidgetItem* itemMa = ui->table->item(row, 0);
    if (!itemMa) {
        ui->xoaButton->setEnabled(false);
        return;
    }

    QString manv = itemMa->text();
    int idx = timViTriNV(dsnv, manv.toStdString().c_str());
    if (idx != -1 && (dsnv.nodes[idx]->dshd != nullptr || dsnv.nodes[idx]->CoHD)) {
        // Nếu nhân viên đã có hóa đơn -> Vô hiệu hóa nút Xóa (không cho nhấn)
        ui->xoaButton->setEnabled(false);
    } else {
        // Nhân viên chưa có hóa đơn -> Cho phép nhấn nút Xóa
        ui->xoaButton->setEnabled(true);
    }
}

void NhanVienPage::onCellDoubleClicked(int row, int column) {
    Q_UNUSED(column);
    if (row < 0) return;

    QTableWidgetItem* itemMa = ui->table->item(row, 0);
    QTableWidgetItem* itemHo = ui->table->item(row, 1);
    QTableWidgetItem* itemTen = ui->table->item(row, 2);
    QTableWidgetItem* itemPhai = ui->table->item(row, 3);

    if (itemMa && itemHo && itemTen && itemPhai) {
        ui->manvEdit->setText(itemMa->text());
        ui->manvEdit->setEnabled(false); // Không cho phép sửa Mã nhân viên
        ui->hoEdit->setText(itemHo->text());
        ui->tenEdit->setText(itemTen->text());
        ui->phaiCombo->setCurrentText(itemPhai->text());

        ui->suaButton->setEnabled(true);
        ui->huyButton->setEnabled(true);
        ui->themButton->setEnabled(false); // Vô hiệu hóa nút thêm khi ở chế độ hiệu chỉnh
    }
}

void NhanVienPage::onTimKiemChanged() {
    lamMoiBang();
}

void NhanVienPage::lamMoiBang() {
    QString keyword = ui->timKiemEdit->text().trimmed().toUpper();

    int soKhop = 0;
    for (int i = 0; i < dsnv.n; i++) {
        QString manv = QString::fromUtf8(dsnv.nodes[i]->MANV).toUpper();
        QString ho = QString::fromUtf8(dsnv.nodes[i]->HO).toUpper();
        QString ten = QString::fromUtf8(dsnv.nodes[i]->TEN).toUpper();

        if (keyword.isEmpty() || 
            manv.contains(keyword) || 
            ho.contains(keyword) || 
            ten.contains(keyword)) 
        {
            soKhop++;
        }
    }

    ui->table->setRowCount(soKhop);
    int row = 0;
    for (int i = 0; i < dsnv.n; i++) {
        QString manv = QString::fromUtf8(dsnv.nodes[i]->MANV).toUpper();
        QString ho = QString::fromUtf8(dsnv.nodes[i]->HO).toUpper();
        QString ten = QString::fromUtf8(dsnv.nodes[i]->TEN).toUpper();

        if (keyword.isEmpty() || 
            manv.contains(keyword) || 
            ho.contains(keyword) || 
            ten.contains(keyword)) 
        {
            ui->table->setItem(row, 0, new QTableWidgetItem(dsnv.nodes[i]->MANV));
            ui->table->setItem(row, 1, new QTableWidgetItem(dsnv.nodes[i]->HO));
            ui->table->setItem(row, 2, new QTableWidgetItem(dsnv.nodes[i]->TEN));
            ui->table->setItem(row, 3, new QTableWidgetItem(dsnv.nodes[i]->PHAI));
            row++;
        }
    }
}
