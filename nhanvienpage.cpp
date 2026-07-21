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

    // Khởi tạo các đỉnh ngăn xếp bằng con trỏ null
    undoStackTop = nullptr;
    redoStackTop = nullptr;

    // Cấu hình validator cho Mã nhân viên
    ui->manvEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));

    // Cấu hình bảng hiển thị
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
    
    // Khởi tạo trạng thái nút Undo/Redo ban đầu
    updateUndoRedoButtons();

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
    connect(ui->undoButton, &QPushButton::clicked, this, &NhanVienPage::onUndoClicked);
    connect(ui->redoButton, &QPushButton::clicked, this, &NhanVienPage::onRedoClicked);
    
    // Kết nối sự kiện đúp chuột vào bảng để sửa thông tin nhân viên
    connect(ui->table, &QTableWidget::cellDoubleClicked, this, &NhanVienPage::onCellDoubleClicked);

    lamMoiBang();
}

NhanVienPage::~NhanVienPage() {
    // Giải phóng bộ nhớ của các ngăn xếp lưu lịch sử
    clearStack(undoStackTop);
    clearStack(redoStackTop);
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
    saveState(); // Lưu trạng thái trước khi thực hiện thêm mới
    
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
        
        // Hủy lưu trạng thái nếu thêm thất bại
        DS_NHANVIEN* failedState = popState(undoStackTop);
        freeState(failedState);
        updateUndoRedoButtons();
        return;
    }
    
    ui->manvEdit->clear();
    ui->hoEdit->clear();
    ui->tenEdit->clear();
    lamMoiBang();
    luuNhanVien(dsnv, FILE_NHANVIEN);
}

void NhanVienPage::onSuaClicked() {
    QString manv = ui->manvEdit->text().trimmed();
    if (manv.isEmpty()) return;

    // Hiển thị hộp thoại hỏi xác nhận trước khi sửa thông tin
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Xác nhận hiệu chỉnh", 
                                  QString("Bạn có chắc chắn muốn sửa thông tin nhân viên có mã %1?").arg(manv),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    saveState(); // Lưu trạng thái trước khi chỉnh sửa

    std::string loi;
    bool ok = suaNV(dsnv,
                    manv.toUpper().toStdString().c_str(),
                    ui->hoEdit->text().toStdString().c_str(),
                    ui->tenEdit->text().toStdString().c_str(),
                    ui->phaiCombo->currentText().toStdString().c_str(),
                    loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);

        // Hủy lưu trạng thái nếu sửa thất bại
        DS_NHANVIEN* failedState = popState(undoStackTop);
        freeState(failedState);
        updateUndoRedoButtons();
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

    saveState(); // Lưu trạng thái trước khi xóa
    
    std::string loi;
    bool ok = xoaNV(dsnv, manv.toStdString().c_str(), loi);
    if (!ok) {
        QMessageBox::critical(this, "Lỗi xóa nhân viên", QString::fromStdString(loi));
        
        // Hủy lưu trạng thái nếu xóa thất bại
        DS_NHANVIEN* failedState = popState(undoStackTop);
        freeState(failedState);
        updateUndoRedoButtons();
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
}

void NhanVienPage::onCellDoubleClicked(int row, int column) {
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

void NhanVienPage::onUndoClicked() {
    if (undoStackTop == nullptr) return;

    // Lấy trạng thái trước đó từ ngăn xếp (nhưng chưa pop vội để kiểm tra an toàn)
    DS_NHANVIEN* previousState = undoStackTop->state;

    // Kiểm tra an toàn: Không cho phép Undo nếu làm biến mất nhân viên đã lập hóa đơn
    for (int i = 0; i < dsnv.n; i++) {
        int idx = timViTriNV(*previousState, dsnv.nodes[i]->MANV);
        if (idx == -1) { // Nhân viên này sẽ bị xóa nếu ta thực hiện Undo
            if (dsnv.nodes[i]->dshd != nullptr) {
                QMessageBox::warning(this, "Không thể Undo", 
                    QString("Không thể quay lại thao tác vì nhân viên %1 (Mã: %2) đã lập hóa đơn trong hệ thống!")
                    .arg(dsnv.nodes[i]->TEN).arg(dsnv.nodes[i]->MANV));
                return; 
            }
        }
    }

    // Tiến hành lấy trạng thái ra khỏi ngăn xếp
    previousState = popState(undoStackTop);

    // Lưu trạng thái hiện tại vào redoStack
    DS_NHANVIEN* currentState = cloneState(dsnv);
    pushState(redoStackTop, currentState);

    // Giải phóng bộ nhớ động của danh sách dsnv hiện tại (giữ lại danh sách hóa đơn)
    for (int i = 0; i < dsnv.n; i++) {
        // Chuyển giao con trỏ hóa đơn mới nhất sang trạng thái khôi phục
        int idxBackup = timViTriNV(*previousState, dsnv.nodes[i]->MANV);
        if (idxBackup != -1) {
            previousState->nodes[idxBackup]->dshd = dsnv.nodes[i]->dshd;
        }
        delete dsnv.nodes[i];
    }

    // Phục hồi dữ liệu
    dsnv.n = previousState->n;
    for (int i = 0; i < previousState->n; i++) {
        dsnv.nodes[i] = previousState->nodes[i];
    }

    previousState->n = 0;
    delete previousState;

    lamMoiBang();
    onHuyClicked();
    updateUndoRedoButtons();
    luuNhanVien(dsnv, FILE_NHANVIEN);
}

void NhanVienPage::onRedoClicked() {
    if (redoStackTop == nullptr) return;

    // Lấy trạng thái tiếp theo từ ngăn xếp (nhưng chưa pop vội để kiểm tra an toàn)
    DS_NHANVIEN* nextState = redoStackTop->state;

    // Kiểm tra an toàn: Không cho phép Redo nếu làm biến mất nhân viên đã lập hóa đơn
    for (int i = 0; i < dsnv.n; i++) {
        int idx = timViTriNV(*nextState, dsnv.nodes[i]->MANV);
        if (idx == -1) { // Nhân viên này sẽ bị xóa nếu ta thực hiện Redo
            if (dsnv.nodes[i]->dshd != nullptr) {
                QMessageBox::warning(this, "Không thể Redo", 
                    QString("Không thể làm lại thao tác vì nhân viên %1 (Mã: %2) đã lập hóa đơn trong hệ thống!")
                    .arg(dsnv.nodes[i]->TEN).arg(dsnv.nodes[i]->MANV));
                return;
            }
        }
    }
    // Tiến hành lấy trạng thái ra khỏi ngăn xếp
    nextState = popState(redoStackTop);

    // Lưu trạng thái hiện tại vào undoStack
    DS_NHANVIEN* currentState = cloneState(dsnv);
    pushState(undoStackTop, currentState);

    // Giải phóng bộ nhớ động của danh sách dsnv hiện tại (giữ lại danh sách hóa đơn)
    for (int i = 0; i < dsnv.n; i++) {
        // Chuyển giao con trỏ hóa đơn mới nhất sang trạng thái khôi phục
        int idxBackup = timViTriNV(*nextState, dsnv.nodes[i]->MANV);
        if (idxBackup != -1) {
            nextState->nodes[idxBackup]->dshd = dsnv.nodes[i]->dshd;
        }
        delete dsnv.nodes[i];
    }

    // Phục hồi dữ liệu
    dsnv.n = nextState->n;
    for (int i = 0; i < nextState->n; i++) {
        dsnv.nodes[i] = nextState->nodes[i];
    }

    nextState->n = 0;
    delete nextState;

    lamMoiBang();
    onHuyClicked();
    updateUndoRedoButtons();
    luuNhanVien(dsnv, FILE_NHANVIEN);
}

void NhanVienPage::saveState() {
    DS_NHANVIEN* state = cloneState(dsnv);
    pushState(undoStackTop, state);

    // Khi có hành động mới, xóa toàn bộ redoStack
    clearStack(redoStackTop);
    updateUndoRedoButtons();
}

void NhanVienPage::updateUndoRedoButtons() {
    ui->undoButton->setEnabled(undoStackTop != nullptr);
    ui->redoButton->setEnabled(redoStackTop != nullptr);
}

// Các hàm thao tác ngăn xếp lưu lịch sử bằng Danh sách liên kết đơn
void NhanVienPage::pushState(HistoryNode*& top, DS_NHANVIEN* state) {
    HistoryNode* node = new HistoryNode();
    node->state = state;
    node->next = top;
    top = node;
}

DS_NHANVIEN* NhanVienPage::popState(HistoryNode*& top) {
    if (top == nullptr) return nullptr;
    HistoryNode* temp = top;
    DS_NHANVIEN* state = temp->state;
    top = top->next;
    delete temp;
    return state;
}

void NhanVienPage::clearStack(HistoryNode*& top) {
    while (top != nullptr) {
        HistoryNode* temp = top;
        top = top->next;
        freeState(temp->state);
        delete temp;
    }
}

DS_NHANVIEN* NhanVienPage::cloneState(const DS_NHANVIEN &source) {
    DS_NHANVIEN* clone = new DS_NHANVIEN();
    clone->n = source.n;
    for (int i = 0; i < source.n; i++) {
        NHANVIEN* nv = new NHANVIEN();
        strcpy(nv->MANV, source.nodes[i]->MANV);
        strcpy(nv->HO, source.nodes[i]->HO);
        strcpy(nv->TEN, source.nodes[i]->TEN);
        strcpy(nv->PHAI, source.nodes[i]->PHAI);

        // Lưu trữ nông con trỏ hóa đơn hiện hành, không sao chép sâu để bảo toàn lịch sử hóa đơn
        nv->dshd = source.nodes[i]->dshd;
        clone->nodes[i] = nv;
    }
    return clone;
}

void NhanVienPage::freeState(DS_NHANVIEN* state) {
    if (!state) return;
    for (int i = 0; i < state->n; i++) {
        // Chỉ giải phóng vùng nhớ nhân viên của bản lưu lịch sử, không xóa dshd đang chạy
        delete state->nodes[i];
    }
    delete state;
}

void NhanVienPage::lamMoiBang() {
    QString keyword = ui->timKiemEdit->text().trimmed().toUpper();

    // Lọc các bản ghi khớp với từ khóa tìm kiếm
    std::vector<int> matchingIndices;
    for (int i = 0; i < dsnv.n; i++) {
        QString manv = QString::fromUtf8(dsnv.nodes[i]->MANV).toUpper();
        QString ho = QString::fromUtf8(dsnv.nodes[i]->HO).toUpper();
        QString ten = QString::fromUtf8(dsnv.nodes[i]->TEN).toUpper();

        if (keyword.isEmpty() || 
            manv.contains(keyword) || 
            ho.contains(keyword) || 
            ten.contains(keyword)) 
        {
            matchingIndices.push_back(i);
        }
    }

    ui->table->setRowCount(matchingIndices.size());
    for (size_t i = 0; i < matchingIndices.size(); i++) {
        int origIndex = matchingIndices[i];
        ui->table->setItem(i, 0, new QTableWidgetItem(dsnv.nodes[origIndex]->MANV));
        ui->table->setItem(i, 1, new QTableWidgetItem(dsnv.nodes[origIndex]->HO));
        ui->table->setItem(i, 2, new QTableWidgetItem(dsnv.nodes[origIndex]->TEN));
        ui->table->setItem(i, 3, new QTableWidgetItem(dsnv.nodes[origIndex]->PHAI));
    }
}
