#include "nhanvienpage.h"
#include "ui_nhanvienpage.h"
#include "nhanvienlogic.h"

#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <cstring>

NhanVienPage::NhanVienPage(DS_NHANVIEN &dsRef, QWidget *parent) 
    : QWidget(parent), ui(new Ui::NhanVienPage), dsnv(dsRef) 
{
    ui->setupUi(this);

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
    
    // Kết nối sự kiện chọn hàng trong bảng
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &NhanVienPage::onSelectionChanged);

    lamMoiBang();
}

NhanVienPage::~NhanVienPage() {
    // Giải phóng bộ nhớ của các ngăn xếp Undo/Redo
    for (DS_NHANVIEN* state : undoStack) {
        freeState(state);
    }
    for (DS_NHANVIEN* state : redoStack) {
        freeState(state);
    }
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
    saveState(); // Lưu trạng thái trước khi thêm mới
    
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
        freeState(undoStack.back());
        undoStack.pop_back();
        updateUndoRedoButtons();
        return;
    }
    
    ui->manvEdit->clear();
    ui->hoEdit->clear();
    ui->tenEdit->clear();
    lamMoiBang();
}

void NhanVienPage::onSuaClicked() {
    QString manv = ui->manvEdit->text().trimmed();
    if (manv.isEmpty()) return;

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
        freeState(undoStack.back());
        undoStack.pop_back();
        updateUndoRedoButtons();
        return;
    }

    onHuyClicked(); // Giải phóng form và kích hoạt lại Mã NV nhập liệu
    lamMoiBang();
}

void NhanVienPage::onXoaClicked() {
    int row = ui->table->currentRow();
    if (row < 0) return;
    QString manv = ui->table->item(row, 0)->text();
    
    saveState(); // Lưu trạng thái trước khi xóa
    
    std::string loi;
    bool ok = xoaNV(dsnv, manv.toStdString().c_str(), loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);

        // Hủy lưu trạng thái nếu xóa thất bại
        freeState(undoStack.back());
        undoStack.pop_back();
        updateUndoRedoButtons();
        return;
    }
    
    onHuyClicked();
    lamMoiBang();
}

void NhanVienPage::onHuyClicked() {
    // Bỏ chọn dòng trên bảng
    ui->table->clearSelection();
    ui->table->setCurrentCell(-1, -1);

    // Bật lại ô Mã nhân viên và xóa trắng form
    ui->manvEdit->setEnabled(true);
    ui->manvEdit->clear();
    ui->hoEdit->clear();
    ui->tenEdit->clear();
    ui->phaiCombo->setCurrentIndex(0);
    ui->errorLabel->setVisible(false);

    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
}

void NhanVienPage::onSelectionChanged() {
    int row = ui->table->currentRow();
    if (row < 0 || ui->table->selectedItems().isEmpty()) {
        ui->suaButton->setEnabled(false);
        ui->huyButton->setEnabled(false);
        return;
    }

    QTableWidgetItem* itemMa = ui->table->item(row, 0);
    QTableWidgetItem* itemHo = ui->table->item(row, 1);
    QTableWidgetItem* itemTen = ui->table->item(row, 2);
    QTableWidgetItem* itemPhai = ui->table->item(row, 3);

    if (itemMa && itemHo && itemTen && itemPhai) {
        ui->manvEdit->setText(itemMa->text());
        ui->manvEdit->setEnabled(false); // Không cho phép sửa Mã nhân viên (ID)
        ui->hoEdit->setText(itemHo->text());
        ui->tenEdit->setText(itemTen->text());
        ui->phaiCombo->setCurrentText(itemPhai->text());

        ui->suaButton->setEnabled(true);
        ui->huyButton->setEnabled(true);
        ui->themButton->setEnabled(false); // Đang sửa thì không thể bấm Thêm mới
    }
}

void NhanVienPage::onTimKiemChanged() {
    lamMoiBang();
}

void NhanVienPage::onUndoClicked() {
    if (undoStack.empty()) return;

    // Lưu trạng thái hiện tại vào redoStack
    DS_NHANVIEN* currentState = cloneState(dsnv);
    redoStack.push_back(currentState);

    // Lấy trạng thái trước đó từ undoStack
    DS_NHANVIEN* previousState = undoStack.back();
    undoStack.pop_back();

    // Giải phóng bộ nhớ động của danh sách dsnv hiện tại (Tránh rò rỉ RAM)
    for (int i = 0; i < dsnv.n; i++) {
        // Giải phóng danh sách liên kết hóa đơn
        nodeHD* current = dsnv.nodes[i]->dshd;
        while (current != nullptr) {
            nodeHD* temp = current;
            current = current->next;
            delete temp;
        }
        delete dsnv.nodes[i];
    }

    // Phục hồi dữ liệu
    dsnv.n = previousState->n;
    for (int i = 0; i < previousState->n; i++) {
        dsnv.nodes[i] = previousState->nodes[i]; // Nhận lại quyền sở hữu bộ nhớ
    }

    // Ngắt con trỏ trong previousState để khi xóa struct previousState không giải phóng nhầm các nhân viên
    previousState->n = 0;
    delete previousState;

    lamMoiBang();
    onHuyClicked();
    updateUndoRedoButtons();
}

void NhanVienPage::onRedoClicked() {
    if (redoStack.empty()) return;

    // Lưu trạng thái hiện tại vào undoStack
    DS_NHANVIEN* currentState = cloneState(dsnv);
    undoStack.push_back(currentState);

    // Lấy trạng thái tiếp theo từ redoStack
    DS_NHANVIEN* nextState = redoStack.back();
    redoStack.pop_back();

    // Giải phóng bộ nhớ động của danh sách dsnv hiện tại
    for (int i = 0; i < dsnv.n; i++) {
        nodeHD* current = dsnv.nodes[i]->dshd;
        while (current != nullptr) {
            nodeHD* temp = current;
            current = current->next;
            delete temp;
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
}

void NhanVienPage::saveState() {
    DS_NHANVIEN* state = cloneState(dsnv);
    undoStack.push_back(state);

    // Khi có hành động mới, xóa toàn bộ redoStack
    for (DS_NHANVIEN* s : redoStack) {
        freeState(s);
    }
    redoStack.clear();
    updateUndoRedoButtons();
}

void NhanVienPage::updateUndoRedoButtons() {
    ui->undoButton->setEnabled(!undoStack.empty());
    ui->redoButton->setEnabled(!redoStack.empty());
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

        // Sao chép sâu (Deep copy) danh sách liên kết đơn hóa đơn
        nv->dshd = nullptr;
        nodeHD* current = source.nodes[i]->dshd;
        nodeHD* tail = nullptr;
        while (current != nullptr) {
            nodeHD* newNode = new nodeHD();
            newNode->hd = current->hd; // Copy dữ liệu hóa đơn
            newNode->next = nullptr;

            if (nv->dshd == nullptr) {
                nv->dshd = newNode;
            } else {
                tail->next = newNode;
            }
            tail = newNode;
            current = current->next;
        }
        clone->nodes[i] = nv;
    }
    return clone;
}

void NhanVienPage::freeState(DS_NHANVIEN* state) {
    if (!state) return;
    for (int i = 0; i < state->n; i++) {
        nodeHD* current = state->nodes[i]->dshd;
        while (current != nullptr) {
            nodeHD* temp = current;
            current = current->next;
            delete temp;
        }
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