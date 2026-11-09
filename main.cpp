#include <QApplication>
#include <QMessageBox>
#include <QMainWindow>
#include <memory>
#include <functional>
#include <utility>

#include "services/DatBanService.h"
#include "widgets/common/AppStyle.h"
#include "widgets/login/DangNhapWidget.h"
#include "widgets/admin/AdminWidget.h"
#include "widgets/nhanvien/NhanVienWidget.h"
#include "taikhoan/PhanQuyen.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Dzgn");
    app.setStyle("Fusion");

    // Đồng bộ trạng thái đặt bàn và bàn khi khởi động ứng dụng.
    DatBanService startupBookingService;
    if (!startupBookingService.capNhatTrangThaiTheoThoiGian()) {
        QMessageBox::warning(nullptr,
            "Không thể đồng bộ trạng thái đặt bàn",
            "Hệ thống chưa thể cập nhật trạng thái đặt bàn và bàn từ dữ liệu TXT. "
            "Vui lòng kiểm tra quyền truy cập thư mục database trước khi thao tác nghiệp vụ.");
    }

    LoginWidget login(appStyleSheet());
    std::unique_ptr<QMainWindow> window;

    login.onLoginSuccess = [&login, &window](const QString &username, const QString &role) {
        PhanQuyen::datVaiTroHienTai(role);
        std::function<void()> logoutCallback = [&login]() {
            PhanQuyen::dangXuat();
            login.clearPassword();
            login.setStatusMessage("Bạn đã đăng xuất. Vui lòng đăng nhập lại để tiếp tục.");
            login.show();
            login.raise();
            login.activateWindow();
        };

        if (role == "Admin")
            window.reset(createAdminWidget(username, std::move(logoutCallback)));
        else
            window.reset(createNhanVienWidget(username, std::move(logoutCallback)));

        window->show();
        login.hide();
    };

    login.show();
    const int result = app.exec();
    window.reset();
    return result;
}
