#include "vattulogic.h"
#include <cstring>
#include <algorithm>

nodeVT* taoNodeVT(const char* mavt, const char* tenvt, const char* dvt, int soLuongTon) {
    nodeVT* p = new nodeVT();
    strncpy(p->vt.MAVT, mavt, 10); p->vt.MAVT[10] = '\0';
    strncpy(p->vt.TENVT, tenvt, 50); p->vt.TENVT[50] = '\0';
    strncpy(p->vt.DVT, dvt, 10); p->vt.DVT[10] = '\0';
    p->vt.SoLuongTon = soLuongTon;
    p->left = p->right = nullptr;
    return p;
}

nodeVT* timVT(TreeVT root, const char* mavt) {
    if (!root) return nullptr;
    int cmp = strcmp(mavt, root->vt.MAVT);
    if (cmp == 0) return root;
    return cmp < 0 ? timVT(root->left, mavt) : timVT(root->right, mavt);
}

bool themVT(TreeVT& root, const char* mavt, const char* tenvt, const char* dvt,
            int soLuongTon, std::string& loi) {
    if (timVT(root, mavt) != nullptr) {
        loi = "Ma vat tu da ton tai";
        return false;
    }
    if (!root) {
        root = taoNodeVT(mavt, tenvt, dvt, soLuongTon);
        return true;
    }
    int cmp = strcmp(mavt, root->vt.MAVT);
    if (cmp < 0) return themVT(root->left, mavt, tenvt, dvt, soLuongTon, loi);
    return themVT(root->right, mavt, tenvt, dvt, soLuongTon, loi);
}

bool suaVT(TreeVT root, const char* mavt, const char* tenvtMoi, const char* dvtMoi,
           std::string& loi) {
    nodeVT* p = timVT(root, mavt);
    if (!p) { loi = "Khong tim thay vat tu"; return false; }
    strncpy(p->vt.TENVT, tenvtMoi, 50); p->vt.TENVT[50] = '\0';
    strncpy(p->vt.DVT, dvtMoi, 10); p->vt.DVT[10] = '\0';
    return true;
}

static nodeVT* timMin(nodeVT* node) {
    while (node && node->left) node = node->left;
    return node;
}

bool xoaVT(TreeVT& root, const char* mavt, std::string& loi) {
    if (!root) { loi = "Khong tim thay vat tu"; return false; }
    int cmp = strcmp(mavt, root->vt.MAVT);
    if (cmp < 0) return xoaVT(root->left, mavt, loi);
    if (cmp > 0) return xoaVT(root->right, mavt, loi);

    if (!root->left && !root->right) {
        delete root; root = nullptr;
    } else if (!root->left) {
        nodeVT* t = root; root = root->right; delete t;
    } else if (!root->right) {
        nodeVT* t = root; root = root->left; delete t;
    } else {
        nodeVT* succ = timMin(root->right);
        root->vt = succ->vt;
        std::string tmp;
        xoaVT(root->right, succ->vt.MAVT, tmp);
    }
    return true;
}

int demSoVT(TreeVT root) {
    if (!root) return 0;
    return 1 + demSoVT(root->left) + demSoVT(root->right);
}

static void duyetInorderVaoMang(nodeVT* root, VATTU mang[], int& idx) {
    if (!root) return;
    duyetInorderVaoMang(root->left, mang, idx);
    mang[idx] = root->vt;
    idx++;
    duyetInorderVaoMang(root->right, mang, idx);
}

VATTU* duyetTheoTen(TreeVT root, int& soLuong) {
    soLuong = demSoVT(root);
    VATTU* mang = new VATTU[soLuong];
    int idx = 0;
    duyetInorderVaoMang(root, mang, idx);

    std::sort(mang, mang + soLuong, [](const VATTU& a, const VATTU& b) {
        return strcmp(a.TENVT, b.TENVT) < 0;
    });
    return mang;
}

void huyCayVT(TreeVT& root) {
    if (!root) return;
    huyCayVT(root->left);
    huyCayVT(root->right);
    delete root;
    root = nullptr;
}

nodeVT* cloneCayVT(nodeVT* root) {
    if (!root) return nullptr;
    nodeVT* newNode = new nodeVT();
    newNode->vt = root->vt;
    newNode->left = cloneCayVT(root->left);
    newNode->right = cloneCayVT(root->right);
    return newNode;
}