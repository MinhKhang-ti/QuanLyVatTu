#include "hoadonlogic.h"
#include "vattulogic.h"
#include <cstring>
#include <vector>
#include <cctype>
using namespace std;

nodeHD* taoHoaDon(const char* soHD, Date ngay, char loai) {
    nodeHD* p = new nodeHD();
    strncpy(p->hd.SoHD, soHD, 20); p->hd.SoHD[20] = '\0';
    p->hd.NgayLap = ngay;
    p->hd.Loai = loai;
    p->hd.dscthd.n = 0;
    p->next = nullptr;
    return p;
}

void themHoaDonVaoDS(PTRHD& dshd, nodeHD* hdMoi) {
    hdMoi->next = dshd;
    dshd = hdMoi;
}

nodeHD* timHoaDon(PTRHD dshd, const char* soHD) {
    nodeHD* p = dshd;
    while (p) {
        if (strcmp(p->hd.SoHD, soHD) == 0) return p;
        p = p->next;
    }
    return nullptr;
}

bool themCTHD(DS_CTHD& ds, const char* mavt, int soLuong, float donGia, float vat, string& loi) {
    if (ds.n >= SO_VT_TOIDA_MOI_HD) {
        loi = "Hóa đơn đã đủ tối đa" + to_string(SO_VT_TOIDA_MOI_HD) + " vật tư";
        return false;
    }
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.nodes[i].MAVT, mavt) == 0) {
            loi = "Vật tư này đã có trong hóa đơn";
            return false;
        }
    }
    CT_HOADON& ct = ds.nodes[ds.n];
    strncpy(ct.MAVT, mavt, 10); ct.MAVT[10] = '\0';
    ct.SoLuong = soLuong;
    ct.DonGia = donGia;
    ct.VAT = vat;
    ds.n++;
    return true;
}

bool xoaCTHD(DS_CTHD& ds, const char* mavt, string& loi) {
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.nodes[i].MAVT, mavt) == 0) {
            for (int j = i; j < ds.n - 1; j++) ds.nodes[j] = ds.nodes[j + 1];
            ds.n--;
            return true;
        }
    }
    loi = "Không tìm thấy vật tư trong hóa đơn ";
    return false;
}

double tinhTriGiaDong(const CT_HOADON& ct) {
    double thanhTien = ct.SoLuong * ct.DonGia;
    return thanhTien + thanhTien * (ct.VAT / 100.0);
}

double tinhTongTriGiaHD(const DS_CTHD& ds) {
    double tong = 0;
    for (int i = 0; i < ds.n; i++) tong += tinhTriGiaDong(ds.nodes[i]);
    return tong;
}

void huyDSHoaDon(PTRHD& dshd) {
    while (dshd) {
        nodeHD* t = dshd;
        dshd = dshd->next;
        delete t;
    }
}

bool isSoHDTonTaiHeThong(const DS_NHANVIEN& dsnv, const char* soHD) {
    for (int i = 0; i < dsnv.n; i++) {
        if (timHoaDon(dsnv.nodes[i]->dshd, soHD) != nullptr) {
            return true;
        }
    }
    return false;
}

bool kiemTraKhaNangXuatKho(TreeVT root, const DS_CTHD& dsTam, const char* mavt, int soLuongThem, int& tonKho, string& loi) {
    nodeVT* node = timVT(root, mavt);
    if (!node) {
        loi = "Vật tư không tồn tại trong hệ thống!";
        return false;
    }
    tonKho = node->vt.SoLuongTon;

    int soLuongDaChon = 0;
    for (int i = 0; i < dsTam.n; i++) {
        if (strcmp(dsTam.nodes[i].MAVT, mavt) == 0) {
            soLuongDaChon += dsTam.nodes[i].SoLuong;
        }
    }

    if (soLuongDaChon + soLuongThem > tonKho) {
        loi = "LỖI XUẤT HÀNG: Số lượng xuất vượt quá tồn kho!\nVật tư [" + string(node->vt.MAVT) + " - " + string(node->vt.TENVT) + "] hiện chỉ còn tồn: " + to_string(tonKho) + " trong kho.";
        return false;
    }
    return true;
}

void capNhatTonKhoKhiGhiHD(TreeVT root, const DS_CTHD& ds, char loaiHD) {
    for (int i = 0; i < ds.n; i++) {
        nodeVT* node = timVT(root, ds.nodes[i].MAVT);
        if (node) {
            if (loaiHD == 'N') {
                node->vt.SoLuongTon += ds.nodes[i].SoLuong;
            } else if (loaiHD == 'X') {
                node->vt.SoLuongTon -= ds.nodes[i].SoLuong;
            }
            node->vt.DaXuatHienTrongHD = true;
        }
    }
}

bool ghiNhanHoaDon(DS_NHANVIEN& dsnv, int idxNV, TreeVT root, const char* soHD, Date ngayLap, char loaiHD, const DS_CTHD& dscthd) {
    if (idxNV < 0 || idxNV >= dsnv.n) return false;

    // 1. Tạo node hóa đơn mới và gán dữ liệu chi tiết
    nodeHD* hdMoi = taoHoaDon(soHD, ngayLap, loaiHD);
    hdMoi->hd.dscthd = dscthd;

    // 2. Thêm vào danh sách liên kết của nhân viên
    themHoaDonVaoDS(dsnv.nodes[idxNV]->dshd, hdMoi);
    dsnv.nodes[idxNV]->CoHD = true;

    // 3. Tự động cập nhật số lượng tồn trên cây BST Vật tư
    capNhatTonKhoKhiGhiHD(root, dscthd, loaiHD);

    return true;
}

nodeHD* timHoaDonTrongHeThong(const DS_NHANVIEN& dsnv, const char* soHD, int& idxNV) {
    for (int i = 0; i < dsnv.n; i++) {
        nodeHD* hd = timHoaDon(dsnv.nodes[i]->dshd, soHD);
        if (hd) { idxNV = i; return hd; }
    }
    idxNV = -1;
    return nullptr;
}

static const string CHUSO_VT[10] = {"không","một","hai","ba","bốn","năm","sáu","bảy","tám","chín"};

static string vietHoaChuCai(const string& s) {
    if (s.empty()) return s;
    string r = s;
    r[0] = toupper((unsigned char)r[0]);
    return r;
}

static string docBaChuSo(int so) {
    string kq = "";
    int tram = so / 100;
    int chuc = (so % 100) / 10;
    int donvi = so % 10;

    if (tram > 0) {
        kq += CHUSO_VT[tram] + " trăm";
        if (chuc == 0 && donvi > 0) kq += " linh";
    }
    if (chuc >= 2) {
        if (!kq.empty()) kq += " ";
        kq += CHUSO_VT[chuc] + " mươi";
        if (donvi == 1) kq += " mốt";
        else if (donvi == 4) kq += " tư";
        else if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    } else if (chuc == 1) {
        if (!kq.empty()) kq += " ";
        kq += "mười";
        if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    } else if (chuc == 0 && donvi > 0) {
        if (!kq.empty()) kq += " ";
        kq += CHUSO_VT[donvi];
    }
    return kq;
}

static string docNhom(int gia, bool batBuocDu3ChuSo) {
    if (!batBuocDu3ChuSo) return docBaChuSo(gia);
    int tram = gia / 100;
    int chuc = (gia % 100) / 10;
    int donvi = gia % 10;
    string kq = (tram == 0 ? "không" : CHUSO_VT[tram]) + " trăm";
    if (chuc == 0 && donvi > 0) kq += " linh " + CHUSO_VT[donvi];
    else if (chuc == 1) {
        kq += " mười";
        if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    } else if (chuc >= 2) {
        kq += " " + CHUSO_VT[chuc] + " mươi";
        if (donvi == 1) kq += " mốt";
        else if (donvi == 4) kq += " tư";
        else if (donvi == 5) kq += " lăm";
        else if (donvi > 0) kq += " " + CHUSO_VT[donvi];
    }
    return kq;
}

static const string DONVI_NHOM[8] = {"", "nghìn", "triệu", "tỷ", "nghìn tỷ", "triệu tỷ", "tỷ tỷ", ""};

string docSoThanhChu(long long soTien) {
    if (soTien == 0) return "Không đồng";
    bool am = soTien < 0;
    if (am) soTien = -soTien;

    // Mang tinh co dinh thay cho std::vector - toi da 7 nhom 3 chu so la du cho pham vi long long
    int nhom[8];
    int soNhom = 0;
    long long n = soTien;
    while (n > 0 && soNhom < 8) {
        nhom[soNhom] = (int)(n % 1000);
        soNhom++;
        n /= 1000;
    }

    string ketQua = "";
    for (int i = soNhom - 1; i >= 0; i--) {
        int gia = nhom[i];
        bool laNhomDauTien = (i == soNhom - 1);
        if (gia == 0) continue;
        string phan = docNhom(gia, !laNhomDauTien);
        if (!ketQua.empty()) ketQua += " ";
        ketQua += phan;
        if (i < 8 && !DONVI_NHOM[i].empty()) ketQua += " " + DONVI_NHOM[i];
    }
    ketQua += " đồng";
    if (am) ketQua = "Âm " + ketQua;
    else ketQua = vietHoaChuCai(ketQua);
    return ketQua;
}