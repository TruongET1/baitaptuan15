#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Định nghĩa các loại mục trong sách
enum LoaiMuc {
    CHUONG,      // Chapter
    MUC,         // Section
    MUC_CON,     // Subsection
    MUC_CON_CON  // Subsubsection
};

// Cấu trúc một node (mục) trong cây
struct Node {
    string tieuDe;          // Tiêu đề của mục
    LoaiMuc loai;          // Loại mục (chương, mục,...)
    int trangBatDau;       // Trang bắt đầu
    int trangKetThuc;      // Trang kết thúc
    vector<Node*> danhSachCon;  // Danh sách các mục con
    Node* nodeCha;         // Con trỏ đến mục cha

    // Hàm khởi tạo
    Node(string td, LoaiMuc l, int batDau, int ketThuc, Node* cha = nullptr) {
        tieuDe = td;
        loai = l;
        trangBatDau = batDau;
        trangKetThuc = ketThuc;
        nodeCha = cha;
    }

    // Tính số trang của mục này
    int soTrang() {
        return trangKetThuc - trangBatDau + 1;
    }
};

class CaySach {
private:
    Node* goc;  // Node gốc của cây

    // Đếm số chương theo đệ quy
    int demSoChuongDeQuy(Node* node) {
        if (!node) return 0;
        
        int dem = (node->loai == CHUONG) ? 1 : 0;
        for (Node* con : node->danhSachCon) {
            dem += demSoChuongDeQuy(con);
        }
        return dem;
    }

    // Tìm chương dài nhất theo đệ quy
    Node* timChuongDaiNhatDeQuy(Node* node, int& soTrangMax) {
        if (!node) return nullptr;

        Node* chuongDaiNhat = nullptr;
        if (node->loai == CHUONG) {
            int tongSoTrang = node->soTrang();
            // Tính tổng số trang bao gồm cả các mục con
            for (Node* con : node->danhSachCon) {
                tongSoTrang += con->soTrang();
            }
            if (tongSoTrang > soTrangMax) {
                soTrangMax = tongSoTrang;
                chuongDaiNhat = node;
            }
        }

        // Tìm trong các nhánh con
        for (Node* con : node->danhSachCon) {
            Node* ketQuaCon = timChuongDaiNhatDeQuy(con, soTrangMax);
            if (ketQuaCon) {
                chuongDaiNhat = ketQuaCon;
            }
        }
        return chuongDaiNhat;
    }

    // Tìm một mục theo tiêu đề
    Node* timMucTheoTieuDe(Node* node, const string& tieuDe) {
        if (!node) return nullptr;
        
        if (node->tieuDe == tieuDe) return node;

        for (Node* con : node->danhSachCon) {
            Node* ketQua = timMucTheoTieuDe(con, tieuDe);
            if (ketQua) return ketQua;
        }
        return nullptr;
    }

    // Cập nhật số trang sau khi xóa một mục
    void capNhatSoTrang(Node* node, int soTrangCanTru) {
        if (!node) return;

        node->trangBatDau -= soTrangCanTru;
        node->trangKetThuc -= soTrangCanTru;

        for (Node* con : node->danhSachCon) {
            capNhatSoTrang(con, soTrangCanTru);
        }
    }

    // Xóa một node và toàn bộ cây con của nó
    void xoaNodeDeQuy(Node* node) {
        if (!node) return;
        
        for (Node* con : node->danhSachCon) {
            xoaNodeDeQuy(con);
        }
        delete node;
    }

public:
    // Hàm khởi tạo
    CaySach() {
        goc = new Node("Mục Lục Sách", CHUONG, 1, 1, nullptr);
    }

    // Thêm một mục mới vào sách
    void themMuc(const string& tieuDeCha, const string& tieuDe, 
                LoaiMuc loai, int trangBatDau, int trangKetThuc) {
        Node* nodeCha = timMucTheoTieuDe(goc, tieuDeCha);
        if (nodeCha) {
            Node* nodeMoi = new Node(tieuDe, loai, trangBatDau, trangKetThuc, nodeCha);
            nodeCha->danhSachCon.push_back(nodeMoi);
        }
    }

    // Đếm tổng số chương trong sách
    int demSoChuong() {
        return demSoChuongDeQuy(goc);
    }

    // Tìm chương dài nhất trong sách
    Node* timChuongDaiNhat() {
        int soTrangMax = 0;
        return timChuongDaiNhatDeQuy(goc, soTrangMax);
    }

    // Xóa một mục khỏi sách
    bool xoaMuc(const string& tieuDe) {
        Node* nodeCanXoa = timMucTheoTieuDe(goc, tieuDe);
        if (!nodeCanXoa || nodeCanXoa == goc) return false;

        // Tính số trang cần điều chỉnh
        int soTrangCanTru = nodeCanXoa->soTrang();

        // Xóa node khỏi danh sách con của node cha
        Node* nodeCha = nodeCanXoa->nodeCha;
        auto viTri = find(nodeCha->danhSachCon.begin(), nodeCha->danhSachCon.end(), nodeCanXoa);
        if (viTri != nodeCha->danhSachCon.end()) {
            nodeCha->danhSachCon.erase(viTri);
        }

        // Cập nhật số trang cho các mục sau node bị xóa
        capNhatSoTrang(goc, soTrangCanTru);

        // Xóa node và toàn bộ cây con của nó
        xoaNodeDeQuy(nodeCanXoa);
        return true;
    }

    // In cây để kiểm tra
    void inCay(Node* node = nullptr, int muc = 0) {
        if (!node) node = goc;
        
        string khoangTrang(muc * 2, ' ');
        cout << khoangTrang << node->tieuDe << " (Trang: " << node->trangBatDau 
             << "-" << node->trangKetThuc << ")" << endl;
        
        for (Node* con : node->danhSachCon) {
            inCay(con, muc + 1);
        }
    }

    // Hàm hủy
    ~CaySach() {
        xoaNodeDeQuy(goc);
    }
};

int main() {
    CaySach sach;

    // Thêm các mục vào sách
    sach.themMuc("Mục Lục Sách", "Chương 1: Giới thiệu", CHUONG, 1, 20);
    sach.themMuc("Chương 1: Giới thiệu", "1.1 Tổng quan", MUC, 2, 10);
    sach.themMuc("Chương 1: Giới thiệu", "1.2 Mục tiêu", MUC, 11, 20);

    sach.themMuc("Mục Lục Sách", "Chương 2: Tổng quan tài liệu", CHUONG, 21, 50);
    sach.themMuc("Chương 2: Tổng quan tài liệu", "2.1 Các nghiên cứu trước", MUC, 22, 35);
    sach.themMuc("2.1 Các nghiên cứu trước", "2.1.1 Nghiên cứu ban đầu", MUC_CON, 23, 28);
    sach.themMuc("Chương 2: Tổng quan tài liệu", "2.2 Tình trạng hiện tại", MUC, 36, 50);

    // In cấu trúc sách ban đầu
    cout << "Cấu trúc sách ban đầu:" << endl;
    sach.inCay();

    // Đếm số chương
    cout << "\nSố chương trong sách: " << sach.demSoChuong() << endl;

    // Tìm chương dài nhất
    Node* chuongDai = sach.timChuongDaiNhat();
    if (chuongDai) {
        cout << "Chương dài nhất: " << chuongDai->tieuDe << endl;
    }

    // Xóa một mục
    cout << "\nĐang xóa mục '2.1 Các nghiên cứu trước'..." << endl;
    sach.xoaMuc("2.1 Các nghiên cứu trước");

    // In cấu trúc sách sau khi xóa
    cout << "\nCấu trúc sách sau khi xóa:" << endl;
    sach.inCay();