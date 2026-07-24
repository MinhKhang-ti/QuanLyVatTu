#ifndef CAUTRUCDULIEU_H
#define CAUTRUCDULIEU_H

#include <cstddef>

/* ============================================================================
 * QUY UOC: chi them field neu la du lieu NOI TAI can thiet cho ban than danh
 * sach do. Khong them field de cache du lieu cua danh sach khac.
 * ========================================================================== */

// ================= 1) VAT TU - Cay nhi phan tim kiem (BST) =================
struct VATTU {
    char MAVT[11];
    char TENVT[51];
    char DVT[11];
    int SoLuongTon;
};
struct nodeVT {
    VATTU vt;
    nodeVT *left;
    nodeVT *right;
};
typedef nodeVT* TreeVT;


// ========== 2) CHI TIET HOA DON - danh sach tuyen tinh (mang, toi da 20) ==========
const int SO_VT_TOIDA_MOI_HD = 20;

struct CT_HOADON {
    char MAVT[11];
    int SoLuong;
    float DonGia;
    float VAT;
};
struct DS_CTHD {
    CT_HOADON nodes[SO_VT_TOIDA_MOI_HD];
    int n = 0;
};


// ================= 3) HOA DON - danh sach lien ket don =================
struct Date {
    int ngay;
    int thang;
    int nam;
};

struct HOADON {
    char SoHD[21];
    Date NgayLap;
    char Loai;
    DS_CTHD dscthd;
};
struct nodeHD {
    HOADON hd;
    nodeHD *next;
};
typedef nodeHD* PTRHD;


// ============ 4) NHAN VIEN - danh sach tuyen tinh la mang con tro ============
struct NHANVIEN {
    char MANV[11];
    char HO[31];
    char TEN[21];
    char PHAI[4];
    bool CoHD;
    PTRHD dshd = NULL;
};

const int MAX_NV = 500;
struct DS_NHANVIEN {
    int n = 0;
    NHANVIEN* nodes[MAX_NV];
};

#endif // CAUTRUCDULIEU_H
