#include "nhanvienlogic.h"
#include <cstring>

using namespace std;

// Khởi tạo danh sách nhân viên
void khoiTaoDSNV(DS_NHANVIEN &ds) {
    ds.n = 0;
}

// Tìm kiếm vị trí nhân viên theo Mã NV
int timViTriNV(const DS_NHANVIEN &ds, const char *manv) {
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.nodes[i]->MANV, manv) == 0) {
            return i;
        }
    }
    return -1;
}

// Tìm vị trí chèn để đảm bảo thứ tự tăng dần theo Tên -> Họ
int timViTriChen(const DS_NHANVIEN &ds, const char *ten, const char *ho) {
    int i = 0;
    while (i < ds.n) {
        int cmpTen = strcmp(ds.nodes[i]->TEN, ten);
        if (cmpTen > 0) {
            break; 
        } else if (cmpTen == 0) {
            int cmpHo = strcmp(ds.nodes[i]->HO, ho);
            if (cmpHo > 0) {
                break;
            }
        }
        i++;
    }
    return i;
}

// Thêm nhân viên mới
bool themNV(DS_NHANVIEN &ds, const char *manv, const char *ho, const char *ten, const char *phai, string &loi) {
    if (ds.n >= MAX_NV) {
        loi = "Danh sách nhân viên đã đầy!";
        return false;
    }
    if (strlen(ho) == 0 || strlen(ten) == 0) {
        loi = "Họ/Tên không được rỗng!";
        return false;
    }
    if (timViTriNV(ds, manv) != -1) {
        loi = "Mã nhân viên đã tồn tại!";
        return false;
    }

    NHANVIEN* nv = new NHANVIEN();
    strcpy(nv->MANV, manv);
    strcpy(nv->HO, ho);
    strcpy(nv->TEN, ten);
    strcpy(nv->PHAI, phai);
    nv->CoHD = false;
    nv->dshd = nullptr;

    int vitri = timViTriChen(ds, ten, ho);
    for (int i = ds.n; i > vitri; i--) {
        ds.nodes[i] = ds.nodes[i - 1];
    }
    ds.nodes[vitri] = nv;
    ds.n++;
    return true;
}

// Hiệu chỉnh (Sửa) nhân viên
bool suaNV(DS_NHANVIEN &ds, const char *manv, const char *hoMoi, const char *tenMoi, const char *phaiMoi, string &loi) {
    int vitri = timViTriNV(ds, manv);
    if (vitri == -1) {
        loi = "Không tìm thấy nhân viên!";
        return false;
    }
    if (strlen(hoMoi) == 0 || strlen(tenMoi) == 0) {
        loi = "Họ/Tên không được rỗng!";
        return false;
    }
    
    NHANVIEN *nv = ds.nodes[vitri];
    bool doiHoTen = (strcmp(nv->TEN, tenMoi) != 0 || strcmp(nv->HO, hoMoi) != 0);

    if (doiHoTen) {
        // Rút ra khỏi mảng
        for (int i = vitri; i < ds.n - 1; i++) {
            ds.nodes[i] = ds.nodes[i + 1];
        }
        ds.n--;

        // Ghi thông tin mới
        strcpy(nv->HO, hoMoi);
        strcpy(nv->TEN, tenMoi);
        strcpy(nv->PHAI, phaiMoi);

        // Tìm vị trí chèn mới
        int viTriMoi = timViTriChen(ds, tenMoi, hoMoi);
        for (int i = ds.n; i > viTriMoi; i--) {
            ds.nodes[i] = ds.nodes[i - 1];
        }
        ds.nodes[viTriMoi] = nv;
        ds.n++;
    } else {
        strcpy(nv->PHAI, phaiMoi);
    }
    return true;
}

// Xóa nhân viên (Chỉ xóa nếu nhân viên chưa lập hóa đơn nào)
bool xoaNV(DS_NHANVIEN &ds, const char *manv, string &loi) {
    int vitri = timViTriNV(ds, manv);
    if (vitri == -1) {
        loi = "Không tìm thấy nhân viên!";
        return false;
    }

    // Kiểm tra nếu nhân viên đã có hóa đơn trong hệ thống
    if (ds.nodes[vitri]->CoHD || ds.nodes[vitri]->dshd != nullptr) {
        loi = "Nhân viên này đã lập hóa đơn trong hệ thống, không được phép xóa!";
        return false;
    }

    // Giải phóng bộ nhớ nhân viên
    delete ds.nodes[vitri];

    // Dồn mảng dồn con trỏ
    for (int i = vitri; i < ds.n - 1; i++) {
        ds.nodes[i] = ds.nodes[i + 1];
    }
    ds.n--;
    return true;
}

// Giải phóng bộ nhớ toàn bộ danh sách khi đóng ứng dụng
void huyDSNV(DS_NHANVIEN &ds) {
    for (int i = 0; i < ds.n; i++) {
        nodeHD* current = ds.nodes[i]->dshd;
        while (current != nullptr) {
            nodeHD* temp = current;
            current = current->next;
            delete temp;
        }
        delete ds.nodes[i];
    }
    ds.n = 0;
}


