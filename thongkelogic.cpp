#include "thongkelogic.h"
#include "hoadonlogic.h"
#include "vattulogic.h"
#include <algorithm>
#include <cstring>
#include <map>

int soSanhNgay(const Date& a, const Date& b) {
    if (a.nam != b.nam) return a.nam - b.nam;
    if (a.thang != b.thang) return a.thang - b.thang;
    return a.ngay - b.ngay;
}

bool ngayTrongKhoang(const Date& ngay, const Date& tu, const Date& den) {
    return soSanhNgay(ngay, tu) >= 0 && soSanhNgay(ngay, den) <= 0;
}

std::vector<DongThongKeHoaDon> thongKeHoaDonTheoThoiGian(const DS_NHANVIEN& dsnv,
                                                          const Date& tuNgay,
                                                          const Date& denNgay) {
    std::vector<DongThongKeHoaDon> ketQua;

    for (int i = 0; i < dsnv.n; i++) {
        NHANVIEN* nv = dsnv.nodes[i];
        if (nv == nullptr) continue;

        std::string hoTenNV = std::string(nv->HO) + " " + nv->TEN;
        for (nodeHD* p = nv->dshd; p != nullptr; p = p->next) {
            if (!ngayTrongKhoang(p->hd.NgayLap, tuNgay, denNgay)) continue;

            DongThongKeHoaDon dong;
            std::strncpy(dong.soHD, p->hd.SoHD, 20);
            dong.soHD[20] = '\0';
            std::strncpy(dong.maNV, nv->MANV, 10);
            dong.maNV[10] = '\0';
            dong.ngayLap = p->hd.NgayLap;
            dong.loai = p->hd.Loai;
            dong.hoTenNV = hoTenNV;
            dong.triGia = tinhTongTriGiaHD(p->hd.dscthd);
            ketQua.push_back(dong);
        }
    }

    std::sort(ketQua.begin(), ketQua.end(), [](const DongThongKeHoaDon& a,
                                               const DongThongKeHoaDon& b) {
        int cmpNgay = soSanhNgay(a.ngayLap, b.ngayLap);
        if (cmpNgay != 0) return cmpNgay < 0;
        return std::strcmp(a.soHD, b.soHD) < 0;
    });

    return ketQua;
}

std::vector<DongTopVatTuDoanhThu> topVatTuDoanhThu(TreeVT root,
                                                    const DS_NHANVIEN& dsnv,
                                                    const Date& tuNgay,
                                                    const Date& denNgay,
                                                    int gioiHan) {
    struct TongHopVatTu {
        int soLuong = 0;
        double doanhThu = 0;
    };

    std::map<std::string, TongHopVatTu> bangTongHop;

    for (int i = 0; i < dsnv.n; i++) {
        NHANVIEN* nv = dsnv.nodes[i];
        if (nv == nullptr) continue;

        for (nodeHD* p = nv->dshd; p != nullptr; p = p->next) {
            if (p->hd.Loai != 'X') continue;
            if (!ngayTrongKhoang(p->hd.NgayLap, tuNgay, denNgay)) continue;

            for (int j = 0; j < p->hd.dscthd.n; j++) {
                const CT_HOADON& ct = p->hd.dscthd.nodes[j];
                TongHopVatTu& tongHop = bangTongHop[ct.MAVT];
                tongHop.soLuong += ct.SoLuong;
                tongHop.doanhThu += tinhTriGiaDong(ct);
            }
        }
    }

    std::vector<DongTopVatTuDoanhThu> ketQua;
    for (const auto& item : bangTongHop) {
        DongTopVatTuDoanhThu dong;
        std::strncpy(dong.maVT, item.first.c_str(), 10);
        dong.maVT[10] = '\0';

        nodeVT* vt = timVT(root, item.first.c_str());
        dong.tenVT = vt ? vt->vt.TENVT : "";
        dong.soLuong = item.second.soLuong;
        dong.doanhThu = item.second.doanhThu;
        ketQua.push_back(dong);
    }

    std::sort(ketQua.begin(), ketQua.end(), [](const DongTopVatTuDoanhThu& a,
                                               const DongTopVatTuDoanhThu& b) {
        if (a.doanhThu != b.doanhThu) return a.doanhThu > b.doanhThu;
        return std::strcmp(a.maVT, b.maVT) < 0;
    });

    if (gioiHan > 0 && static_cast<int>(ketQua.size()) > gioiHan) {
        ketQua.resize(gioiHan);
    }

    return ketQua;
}
