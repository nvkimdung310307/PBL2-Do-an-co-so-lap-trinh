#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QTableWidget>
#include <QAbstractItemView>
#include <QRegularExpression>
#include <QLocale>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QRandomGenerator>
#include <QDate>
#include <QDateTime>
#include <QDateEdit>
#include <QTimeEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>
#include <QStyle>
#include <QColor>
#include <QTime>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QSqlDatabase>
#include <QDateEdit>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QObject>
#include <QSettings>
#include <QDir>
#include <memory>
#include <functional>
#include <utility>
#include "DatabaseManager.h"
#include "taikhoan/TaiKhoanService.h"
#include "taikhoan/DangNhapService.h"
#include "oop/TaiKhoan.h"
#include "taikhoan/PasswordUtils.h"

// Nút bàn hỗ trợ kéo-thả trực tiếp trên sơ đồ. Không cần Q_OBJECT/moc.
class DraggableTableButton : public QPushButton {
public:
    using QPushButton::QPushButton;
    std::function<void(const QPoint &)> onDrop;
protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            pressPosition = event->position().toPoint();
            dragging = false;
        }
        QPushButton::mousePressEvent(event);
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        if ((event->buttons() & Qt::LeftButton) &&
            (event->position().toPoint() - pressPosition).manhattanLength() >= QApplication::startDragDistance()) {
            dragging = true;
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
        QPushButton::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton && dragging) {
            dragging = false;
            unsetCursor();
            if (onDrop) onDrop(event->globalPosition().toPoint());
            event->accept();
            return; // Không phát clicked sau khi kéo thả.
        }
        QPushButton::mouseReleaseEvent(event);
    }
private:
    QPoint pressPosition;
    bool dragging = false;
};

static const char *STYLE = R"(
* { font-family: 'Segoe UI'; }
QLabel { color: #000000; }
/* Dialogs and alerts: consistently light background + dark text for clear contrast. */
QDialog, QMessageBox, QInputDialog { background-color: #F7FAF5; color: #202820; }
QDialog QLabel, QMessageBox QLabel, QInputDialog QLabel { color: #202820; background-color: transparent; }
QDialog QLineEdit, QDialog QTextEdit, QDialog QComboBox, QDialog QSpinBox,
QInputDialog QLineEdit { background-color: #FFFFFF; color: #202820; border: 1px solid #C9DCCB; border-radius: 7px; selection-background-color: #CDE8D0; selection-color: #17251B; padding: 6px; }
QDialog QPushButton, QMessageBox QPushButton, QInputDialog QPushButton { color: #202820; background-color: #E4EFE2; border: 1px solid #C7DCC8; border-radius: 7px; padding: 7px 16px; min-width: 64px; }
QDialog QPushButton:hover, QMessageBox QPushButton:hover, QInputDialog QPushButton:hover { background-color: #CDE3CE; border-color: #9EBFA2; }
QDialog QPushButton:pressed, QMessageBox QPushButton:pressed, QInputDialog QPushButton:pressed { background-color: #B8D5BB; }
QMainWindow, QWidget#root { background: #F5F0E4; color: #000000; }
QFrame#sidebar { background: #B9D3BA; border: 1px solid #A8C8AA; border-radius: 18px; }
QLabel#brand { color: #000000; font-size: 21px; font-weight: 700; }
QLabel#subbrand { color: #000000; font-size: 11px; }
QPushButton#nav { text-align: left; padding: 12px 15px; border: 0; border-radius: 12px; color: #000000; font-size: 14px; background: transparent; }
QPushButton#nav:hover, QPushButton#nav[active="true"] { background: #91BA96; color: #17251B; font-weight: 600; }
QFrame#panel, QFrame#stat { background: #FFFFFF; border: 1px solid #E8EEE5; border-radius: 18px; }
QLabel#pageTitle { font-size: 25px; font-weight: 700; color: #000000; }
QLabel#muted { color: #000000; font-size: 12px; }
QLabel#statTitle { color: #000000; font-size: 12px; }
QLabel#statValue { color: #000000; font-size: 25px; font-weight: 700; }
QPushButton#primary { color: #000000; background: #79A987; border: none; border-radius: 11px; padding: 11px 18px; font-weight: 600; }
QPushButton#primary:hover { background: #659875; }
QPushButton#soft { color: #000000; background: #EDF5ED; border: 1px solid #DCEADD; border-radius: 10px; padding: 9px 14px; }
QLineEdit, QComboBox, QDateEdit, QTimeEdit, QTextEdit, QSpinBox, QDoubleSpinBox { background: #FBFCF9; border: 1px solid #DDE7DC; border-radius: 10px; padding: 9px; color: #17251B; selection-background-color: #CDE8D0; selection-color: #17251B; }
QComboBox QAbstractItemView { background: #FFFFFF; color: #17251B; selection-background-color: #DDEFE0; selection-color: #17251B; border: 1px solid #D4E4D5; outline: 0; padding: 4px; }
QComboBox QAbstractItemView::item { min-height: 28px; padding: 6px 8px; background: #FFFFFF; color: #17251B; }
QComboBox QAbstractItemView::item:hover { background: #EDF7ED; color: #17251B; }
QComboBox QAbstractItemView::item:selected { background: #DDEFE0; color: #17251B; }
QComboBox::drop-down, QDateEdit::drop-down, QTimeEdit::drop-down { background: #EDF5ED; border: 0; border-left: 1px solid #DDE7DC; border-top-right-radius: 9px; border-bottom-right-radius: 9px; width: 26px; }
QCalendarWidget QWidget { background: #FFFFFF; color: #17251B; alternate-background-color: #F3F8F2; }
QCalendarWidget QToolButton { background: #E4F1E3; color: #17251B; border: 0; padding: 6px; }
QCalendarWidget QMenu { background: #FFFFFF; color: #17251B; }
QScrollArea { background: #FFFFFF; border: 0; }
QScrollArea QWidget#qt_scrollarea_viewport { background: #FFFFFF; }
QToolTip { background: #F8FCF7; color: #17251B; border: 1px solid #D4E4D5; }
QTableWidget { background: #FFFFFF; alternate-background-color: #F5FAF4; border: 1px solid #E3EDE0; gridline-color: #E3EDE0; border-radius: 12px; color: #000000; selection-background-color: #D9EED9; selection-color: #000000; }
QHeaderView::section { background: #EAF4E8; color: #000000; border: 0; border-bottom: 1px solid #DCE9D9; padding: 11px; font-weight: 600; }
QTableWidget::item { padding: 8px; color: #000000; background-color: transparent; }
QTableWidget::item:hover { background: #F0F8EF; }
QLabel#pill { background: #E5F3E7; color: #000000; border-radius: 9px; padding: 5px 9px; }
)";

static QLabel *label(const QString &text, const QString &id = QString()) {
    auto *l = new QLabel(text); if (!id.isEmpty()) l->setObjectName(id); return l;
}
static QFrame *panel() { auto *f = new QFrame; f->setObjectName("panel"); return f; }
static void shadow(QWidget *w) { auto *s = new QGraphicsDropShadowEffect(w); s->setBlurRadius(22); s->setOffset(0, 4); s->setColor(QColor(47, 82, 54, 14)); w->setGraphicsEffect(s); }

class RestaurantWindow : public QMainWindow {
public:
    explicit RestaurantWindow(const QString &username, const QString &role, std::function<void()> logoutCallback) : user(username), userRole(role), onLogout(std::move(logoutCallback)) {
        setWindowTitle("Quản lý nhà hàng | Admin & Nhân viên"); resize(1440, 900); setMinimumSize(1120, 720); setStyleSheet(STYLE);
        auto *root = new QWidget; root->setObjectName("root"); setCentralWidget(root);
        auto *outer = new QHBoxLayout(root); outer->setContentsMargins(18,18,18,18); outer->setSpacing(18);
        auto *side = new QFrame; side->setObjectName("sidebar"); side->setFixedWidth(218); shadow(side);
        auto *sl = new QVBoxLayout(side); sl->setContentsMargins(14,20,14,16); sl->setSpacing(7);
        // Use the same database.ini location as registration/login so the registered
        // restaurant name is always shown even when VS Code starts from another cwd.
        QSettings restaurantSettings(QDir(QApplication::applicationDirPath()).filePath("database.ini"), QSettings::IniFormat);
        restaurantSettings.beginGroup("restaurant");
        QString restaurantName = restaurantSettings.value("name").toString().trimmed();
        QString restaurantCode = restaurantSettings.value("code").toString().trimmed();
        restaurantSettings.endGroup();
        if (restaurantName.isEmpty()) restaurantName = "Chưa thiết lập nhà hàng";
        auto *brandLabel = label("♧  " + restaurantName, "brand");
        brandLabel->setWordWrap(true);
        brandLabel->setToolTip(restaurantName);
        sl->addWidget(brandLabel);
        auto *codeLabel = label(restaurantCode.isEmpty() ? "MÃ NHÀ HÀNG: CHƯA CẤP" : ("MÃ NHÀ HÀNG: " + restaurantCode), "subbrand");
        codeLabel->setWordWrap(true);
        sl->addWidget(codeLabel);
        sl->addSpacing(20);

        // Page indices are kept unchanged for QStackedWidget; only visual menu order changes.
        // Daily operations first, administration/configuration at the bottom.
        struct MenuEntry { int page; const char *title; };
        const MenuEntry adminMenu[] = {
            {0, "⌂   Tổng quan"}, {1, "▦   Đặt bàn"}, {2, "▤   Quản lý bàn"},
            {4, "♧   Khách hàng"}, {3, "♨   Món ăn"}, {6, "▣   Hóa đơn"},
            {7, "▥   Thống kê"}, {5, "♙   Nhân viên & tài khoản"},
            {10, "▦   Thiết kế sơ đồ bàn"}, {8, "⚙   Tài khoản & nhà hàng"}
        };
        const MenuEntry staffMenu[] = {
            {0, "⌂   Tổng quan"}, {1, "▦   Đặt bàn"}, {9, "▣   POS NHÂN VIÊN"},
            {2, "▤   Quản lý bàn"}, {4, "♧   Khách hàng"}, {3, "♨   Món ăn"}, {6, "▣   Hóa đơn"}
        };
        navButtons.fill(nullptr, 11);
        auto addMenuEntry = [this, sl](int page, const QString &title) {
            auto *b = new QPushButton(title);
            b->setObjectName("nav");
            b->setProperty("active", page == 0);
            b->setCursor(Qt::PointingHandCursor);
            sl->addWidget(b);
            navButtons[page] = b;
            connect(b, &QPushButton::clicked, this, [this, page] { showPage(page); });
        };
        if (userRole == "Admin") {
            for (const auto &entry : adminMenu) addMenuEntry(entry.page, QString::fromUtf8(entry.title));
        } else {
            for (const auto &entry : staffMenu) addMenuEntry(entry.page, QString::fromUtf8(entry.title));
        }
        sl->addStretch(); auto *welcome=panel(); welcome->setStyleSheet("QFrame#panel{background:#AFCDAF;border:1px solid #9FBEA0;color:#17251B;} QLabel{color:#17251B;}"); auto *wl=new QVBoxLayout(welcome); wl->addWidget(label("❧", "brand")); wl->addWidget(label("Không gian xanh\nTrải nghiệm tuyệt vời")); sl->addWidget(welcome);
        outer->addWidget(side);
        auto *right = new QWidget; auto *rv = new QVBoxLayout(right); rv->setContentsMargins(0,0,0,0); rv->setSpacing(16);
        auto *top = new QHBoxLayout; auto *search=new QLineEdit; search->setPlaceholderText("⌕   Tìm kiếm món ăn, bàn, khách hàng..."); search->setMinimumHeight(42); top->addWidget(search,1); auto *profile=new QLabel("●   "+user+"   ·   "+(role=="Admin"?QString("Quản trị viên"):QString("Nhân viên"))); profile->setStyleSheet("background:#EAF2E8;border-radius:12px;padding:12px;color:#000000;font-weight:600;"); top->addWidget(profile); auto *logoutButton=new QPushButton("Đăng xuất"); logoutButton->setObjectName("soft"); logoutButton->setCursor(Qt::PointingHandCursor); top->addWidget(logoutButton); connect(logoutButton,&QPushButton::clicked,this,[this]{ if(QMessageBox::question(this,"Đăng xuất","Bạn có chắc chắn muốn đăng xuất khỏi tài khoản này?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes){ hide(); if(onLogout) onLogout(); } }); rv->addLayout(top);
        pages = new QStackedWidget;
        pages->addWidget(makeDashboard());
        pages->addWidget(makeBookingPage());
        pages->addWidget(makeTablesPage());
        pages->addWidget(makeGenericPage("Món ăn"));
        pages->addWidget(makeGenericPage("Khách hàng"));
        pages->addWidget(makeEmployeePage());
        pages->addWidget(makeGenericPage("Hóa đơn"));
        pages->addWidget(makeGenericPage("Thống kê"));
        pages->addWidget(makeAccountPage());
        pages->addWidget(makePosPage());
        // Menu index 10: Thiết kế sơ đồ bàn. Must be added to QStackedWidget
        // so clicking the sidebar entry actually opens this page.
        pages->addWidget(makeFloorDesignPage());
        rv->addWidget(pages,1); outer->addWidget(right,1);
    }
private:
    QString user, userRole; QVector<QPushButton*> navButtons; QStackedWidget *pages{}; std::function<void()> onLogout;
    void showPage(int index) {
        if (index >= 0 && index < pages->count()) pages->setCurrentIndex(index);
        for (int i = 0; i < navButtons.size(); ++i) {
            auto *b = navButtons[i];
            if (!b) continue;
            b->setProperty("active", i == index);
            b->style()->unpolish(b);
            b->style()->polish(b);
        }
    }
    QWidget *makeDashboard() {
        auto *w=new QWidget; auto *v=new QVBoxLayout(w); v->setContentsMargins(0,0,0,0); v->setSpacing(12);
        auto *heading=new QHBoxLayout; auto *ht=new QVBoxLayout;
        ht->addWidget(label("Chào mừng trở lại, "+user+"!", "pageTitle"));
        ht->addWidget(label("Cùng quản lý nhà hàng hiệu quả hơn hôm nay  ✿", "muted"));
        heading->addLayout(ht); heading->addStretch();
        auto *date=label("▣   "+QDate::currentDate().toString("dddd, dd/MM/yyyy"));
        date->setStyleSheet("background:white;border:1px solid #E5ECE2;border-radius:11px;padding:12px;"); heading->addWidget(date); v->addLayout(heading);

        auto *stats=new QGridLayout; stats->setSpacing(12);
        const QStringList titles={"Tổng bàn","Khách hàng hôm nay","Doanh thu hôm nay","Món ăn phổ biến"};
        QStringList vals={"0","0","0đ","0"};
        QStringList details={"Chưa có dữ liệu bàn","Khách đặt hôm nay","Doanh thu đã thanh toán hôm nay","Món đang bán"};
        if (DatabaseManager::isOpen()) {
            QSqlQuery q(DatabaseManager::database());
            if (q.exec("SELECT COUNT(*) FROM dbo.Ban") && q.next()) vals[0]=q.value(0).toString();
            if (q.exec("SELECT COUNT(DISTINCT db.MaKH) FROM dbo.DatBan db WHERE db.NgayDat=CAST(GETDATE() AS DATE) AND db.TrangThai IN (N'Đã đặt',N'Đã nhận bàn')") && q.next()) vals[1]=q.value(0).toString();
            if (q.exec("SELECT COALESCE(SUM(ThanhTien),0) FROM dbo.HoaDon WHERE CAST(NgayLap AS DATE)=CAST(GETDATE() AS DATE) AND TrangThai=N'Đã thanh toán'") && q.next()) vals[2]=QLocale(QLocale::Vietnamese).toString(q.value(0).toDouble(),'f',0)+"đ";
            if (q.exec("SELECT COUNT(*) FROM dbo.MonAn WHERE TrangThai=N'Đang bán'") && q.next()) vals[3]=q.value(0).toString();
        }
        const QStringList icons={"♧","♙","＄","♨"};
        const QStringList colors={"#E4F2E5","#FBEBDD","#FFF3D7","#F9E5E8"};
        for(int i=0;i<4;++i){
            auto*f=new QFrame; f->setObjectName("stat"); shadow(f); auto*row=new QHBoxLayout(f);
            auto*ic=label(icons[i]); ic->setAlignment(Qt::AlignCenter); ic->setFixedSize(58,58);
            ic->setStyleSheet(QString("background:%1;border-radius:29px;color:#000000;font-size:23px;").arg(colors[i]));
            auto*col=new QVBoxLayout; col->addWidget(label(titles[i],"statTitle")); col->addWidget(label(vals[i],"statValue")); col->addWidget(label(details[i],"muted"));
            row->addWidget(ic); row->addLayout(col,1); stats->addWidget(f,0,i);
        }
        v->addLayout(stats);

        auto *content=new QHBoxLayout; content->setSpacing(12);
        auto *left=new QWidget; auto *leftCol=new QVBoxLayout(left); leftCol->setContentsMargins(0,0,0,0); leftCol->setSpacing(12);
        auto *map=panel(); shadow(map); auto*mv=new QVBoxLayout(map);
        auto*mh=new QHBoxLayout; mh->addWidget(label("▦  Sơ đồ bàn", "pageTitle")); mh->addStretch();
        // Chú giải trạng thái dùng các chấm màu pastel tương ứng với màu bàn bên dưới.
        const QList<QPair<QString, QString>> statusLegend = {
            {QStringLiteral("Trống"), QStringLiteral("#BFE2C5")},
            {QStringLiteral("Đã đặt"), QStringLiteral("#F8E3AF")},
            {QStringLiteral("Đang sử dụng"), QStringLiteral("#F3C8C7")},
            {QStringLiteral("Bảo trì"), QStringLiteral("#D9D9D6")}
        };
        for (const auto &item : statusLegend) {
            auto *legendItem = new QWidget;
            auto *legendLayout = new QHBoxLayout(legendItem);
            legendLayout->setContentsMargins(0, 0, 0, 0);
            legendLayout->setSpacing(4);
            auto *dot = new QLabel(QStringLiteral("●"));
            dot->setStyleSheet(QString("color:%1;font-size:14px;background:transparent;").arg(item.second));
            auto *caption = new QLabel(item.first);
            caption->setStyleSheet("color:#202820;background:transparent;font-size:12px;");
            legendLayout->addWidget(dot);
            legendLayout->addWidget(caption);
            mh->addWidget(legendItem);
        }
        mv->addLayout(mh);
        auto *floorTabsRow = new QHBoxLayout;
        floorTabsRow->setSpacing(8);
        auto *refreshFloorMap = new QPushButton("↻ Làm mới sơ đồ");
        refreshFloorMap->setObjectName("soft");
        floorTabsRow->addStretch();
        floorTabsRow->addWidget(refreshFloorMap);
        mv->addLayout(floorTabsRow);

        auto *floors = new QHBoxLayout;
        floors->setSpacing(8);
        auto *tabGroup = new QButtonGroup(w);
        tabGroup->setExclusive(true);
        auto *areaStack = new QStackedWidget;
        areaStack->setMinimumHeight(190);
        areaStack->setStyleSheet("QStackedWidget{background:transparent;border:0;}");
        auto *areaScroll = new QScrollArea;
        areaScroll->setWidgetResizable(true);
        areaScroll->setFrameShape(QFrame::NoFrame);
        areaScroll->setStyleSheet("QScrollArea{background:transparent;border:0;} QWidget#qt_scrollarea_viewport{background:transparent;}");
        areaScroll->setWidget(areaStack);
        mv->addLayout(floors);
        mv->addWidget(areaScroll, 1);

        auto reloadDashboardAreas = [this, w, floors, tabGroup, areaStack]() {
            // Remove the previous dynamic tabs and panels before re-reading SQL Server.
            const auto oldButtons = tabGroup->buttons();
            for (auto *button : oldButtons) {
                tabGroup->removeButton(button);
                floors->removeWidget(button);
                button->deleteLater();
            }
            while (areaStack->count() > 0) {
                QWidget *oldPage = areaStack->widget(0);
                areaStack->removeWidget(oldPage);
                oldPage->deleteLater();
            }

            if (!DatabaseManager::connect()) {
                auto *message = new QWidget;
                auto *layout = new QVBoxLayout(message);
                auto *text = label("Chưa kết nối SQL Server. Kiểm tra database.ini để tải sơ đồ bàn.", "muted");
                text->setWordWrap(true);
                layout->addWidget(text);
                areaStack->addWidget(message);
                return;
            }

            QSqlQuery areaQuery(DatabaseManager::database());
            if (!areaQuery.exec("SELECT kv.MaKhuVuc, kv.TenKhuVuc FROM dbo.KhuVuc kv ORDER BY kv.TenKhuVuc")) {
                auto *message = new QWidget;
                auto *layout = new QVBoxLayout(message);
                auto *text = label("Không tải được danh sách khu vực: " + areaQuery.lastError().text(), "muted");
                text->setWordWrap(true);
                layout->addWidget(text);
                areaStack->addWidget(message);
                return;
            }

            int areaIndex = 0;
            while (areaQuery.next()) {
                const QString areaId = areaQuery.value(0).toString();
                const QString areaName = areaQuery.value(1).toString();
                auto *tab = new QPushButton(areaName);
                tab->setObjectName("soft");
                tab->setCheckable(true);
                tab->setMinimumHeight(38);
                tab->setStyleSheet("QPushButton{background:#EDF5ED;color:#202820;border:1px solid #DCEADD;border-radius:10px;padding:9px 14px;} QPushButton:hover{background:#E2F0E2;} QPushButton:checked{background:#C7E0CE;color:#202820;border:1px solid #8BB79A;font-weight:700;}");
                tabGroup->addButton(tab, areaIndex);
                floors->addWidget(tab, 1);

                auto *areaPanel = new QWidget;
                areaPanel->setStyleSheet("background:transparent;color:#202820;");
                auto *areaLayout = new QGridLayout(areaPanel);
                areaLayout->setContentsMargins(4, 8, 4, 8);
                areaLayout->setSpacing(8);

                QSqlQuery tableQuery(DatabaseManager::database());
                tableQuery.prepare("SELECT b.MaBan,b.TenBan,b.SoCho,b.TrangThai,ISNULL(p.Cot,-1),ISNULL(p.Hang,-1) FROM dbo.Ban b LEFT JOIN dbo.BoTriBan p ON p.MaBan=b.MaBan WHERE b.MaKhuVuc=? ORDER BY ISNULL(p.Hang,9999),ISNULL(p.Cot,9999),b.MaBan");
                tableQuery.addBindValue(areaId);
                int col = 0, row = 0, tableCount = 0;
                if (tableQuery.exec()) {
                    while (tableQuery.next()) {
                        ++tableCount;
                        const QString id = tableQuery.value(0).toString();
                        const QString name = tableQuery.value(1).toString();
                        const int seats = tableQuery.value(2).toInt();
                        const QString status = tableQuery.value(3).toString();
                        QString bg = "#BFE2C5";
                        if (status == QStringLiteral("Đã đặt")) bg = "#F8E3AF";
                        else if (status == QStringLiteral("Đang sử dụng")) bg = "#F3C8C7";
                        else if (status == QStringLiteral("Bảo trì")) bg = "#D9D9D6";

                        auto *tableButton = new DraggableTableButton(name + "\n" + QString::number(seats) + " chỗ");
                        tableButton->setToolTip("Mã: " + id + "\nTrạng thái: " + status + "\nKhu vực: " + areaName);
                        tableButton->setMinimumSize(125, 72);
                        tableButton->setStyleSheet(QString("QPushButton{background:%1;border:1px solid #B8D6BD;border-radius:12px;color:#000000;font-weight:600;padding:8px;} QPushButton:hover{border:2px solid #78A985;}").arg(bg));
                        const int savedCol = tableQuery.value(4).toInt() >= 0 ? tableQuery.value(4).toInt() : col;
                        const int savedRow = tableQuery.value(5).toInt() >= 0 ? tableQuery.value(5).toInt() : row;
                        areaLayout->addWidget(tableButton, savedRow, savedCol);
                        tableButton->setProperty("tableId", id);
                        tableButton->setProperty("gridCol", savedCol);
                        tableButton->setProperty("gridRow", savedRow);
                        tableButton->setCursor(Qt::OpenHandCursor);
                        tableButton->setToolTip("Kéo thả bàn đến vị trí mới để di chuyển. Thả lên bàn khác để đổi chỗ.");
                        tableButton->onDrop = [this, areaId, areaPanel, areaLayout, tableButton, id](const QPoint &globalPos) {
                            if (userRole != QStringLiteral("Admin")) {
                                QMessageBox::information(this, "Chỉ dành cho Admin", "Chỉ Admin mới có thể di chuyển bàn trên sơ đồ.");
                                return;
                            }
                            const int oldCol = tableButton->property("gridCol").toInt();
                            const int oldRow = tableButton->property("gridRow").toInt();
                            const QPoint dropPos = areaPanel->mapFromGlobal(globalPos);
                            DraggableTableButton *targetButton = nullptr;
                            for (int i = 0; i < areaLayout->count(); ++i) {
                                auto *candidate = dynamic_cast<DraggableTableButton *>(areaLayout->itemAt(i)->widget());
                                if (candidate && candidate != tableButton && candidate->geometry().contains(dropPos)) {
                                    targetButton = candidate;
                                    break;
                                }
                            }

                            int newCol = oldCol;
                            int newRow = oldRow;
                            QString otherId;
                            int otherCol = -1, otherRow = -1;
                            if (targetButton) {
                                newCol = targetButton->property("gridCol").toInt();
                                newRow = targetButton->property("gridRow").toInt();
                                otherId = targetButton->property("tableId").toString();
                                otherCol = newCol;
                                otherRow = newRow;
                            } else {
                                // Thả vào ô trống: suy ra chỉ số hàng/cột từ kích thước ô hiện có.
                                DraggableTableButton *reference = nullptr;
                                for (int i = 0; i < areaLayout->count(); ++i) {
                                    reference = dynamic_cast<DraggableTableButton *>(areaLayout->itemAt(i)->widget());
                                    if (reference) break;
                                }
                                if (!reference) return;
                                const int spacingX = areaLayout->horizontalSpacing() >= 0 ? areaLayout->horizontalSpacing() : 8;
                                const int spacingY = areaLayout->verticalSpacing() >= 0 ? areaLayout->verticalSpacing() : 8;
                                const int stepX = qMax(1, reference->width() + spacingX);
                                const int stepY = qMax(1, reference->height() + spacingY);
                                const int originX = reference->geometry().left() - reference->property("gridCol").toInt() * stepX;
                                const int originY = reference->geometry().top() - reference->property("gridRow").toInt() * stepY;
                                newCol = qBound(0, (dropPos.x() - originX) / stepX, 19);
                                newRow = qBound(0, (dropPos.y() - originY) / stepY, 19);
                                if (newCol == oldCol && newRow == oldRow) return;
                            }
                            if (newCol == oldCol && newRow == oldRow) return;

                            QSqlDatabase db = DatabaseManager::database();
                            if (!db.transaction()) {
                                QMessageBox::critical(this, "Lỗi SQL", db.lastError().text());
                                return;
                            }
                            // Kiểm tra va chạm từ SQL để vẫn an toàn nếu sơ đồ đang có dữ liệu vị trí cũ.
                            QSqlQuery occupied(db);
                            occupied.prepare("SELECT TOP 1 b.MaBan FROM dbo.Ban b JOIN dbo.BoTriBan p ON p.MaBan=b.MaBan WHERE b.MaKhuVuc=? AND p.Cot=? AND p.Hang=? AND b.MaBan<>?");
                            occupied.addBindValue(areaId); occupied.addBindValue(newCol); occupied.addBindValue(newRow); occupied.addBindValue(id);
                            if (!occupied.exec()) {
                                db.rollback();
                                QMessageBox::critical(this, "Không kiểm tra được vị trí", occupied.lastError().text());
                                return;
                            }
                            if (occupied.next()) {
                                const QString occupiedId = occupied.value(0).toString();
                                if (otherId.isEmpty()) otherId = occupiedId;
                                if (otherId != occupiedId) {
                                    db.rollback();
                                    QMessageBox::warning(this, "Vị trí không khả dụng", "Ô này đã có bàn. Hãy thả trực tiếp lên bàn đó để đổi chỗ.");
                                    return;
                                }
                                if (!targetButton) {
                                    for (int i = 0; i < areaLayout->count(); ++i) {
                                        auto *candidate = dynamic_cast<DraggableTableButton *>(areaLayout->itemAt(i)->widget());
                                        if (candidate && candidate->property("tableId").toString() == occupiedId) {
                                            targetButton = candidate;
                                            otherCol = candidate->property("gridCol").toInt();
                                            otherRow = candidate->property("gridRow").toInt();
                                            break;
                                        }
                                    }
                                }
                            }
                            occupied.finish();

                            if (!otherId.isEmpty() && targetButton) {
                                // Di chuyển bàn đang kéo vào tọa độ tạm để tránh va chạm khóa/vị trí duy nhất.
                                QSqlQuery temporary(db);
                                temporary.prepare("UPDATE dbo.BoTriBan SET Cot=-100,Hang=-100 WHERE MaBan=?");
                                temporary.addBindValue(id);
                                if (!temporary.exec()) { db.rollback(); QMessageBox::critical(this, "Không đổi được vị trí", temporary.lastError().text()); return; }
                                QSqlQuery swap(db);
                                swap.prepare("IF EXISTS(SELECT 1 FROM dbo.BoTriBan WHERE MaBan=?) UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=? ELSE INSERT dbo.BoTriBan(MaBan,Cot,Hang) VALUES(?,?,?)");
                                swap.addBindValue(otherId); swap.addBindValue(oldCol); swap.addBindValue(oldRow); swap.addBindValue(otherId);
                                swap.addBindValue(otherId); swap.addBindValue(oldCol); swap.addBindValue(oldRow);
                                if (!swap.exec()) { db.rollback(); QMessageBox::critical(this, "Không đổi được vị trí", swap.lastError().text()); return; }
                            }
                            QSqlQuery move(db);
                            move.prepare("IF EXISTS(SELECT 1 FROM dbo.BoTriBan WHERE MaBan=?) UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=? ELSE INSERT dbo.BoTriBan(MaBan,Cot,Hang) VALUES(?,?,?)");
                            move.addBindValue(id); move.addBindValue(newCol); move.addBindValue(newRow); move.addBindValue(id);
                            move.addBindValue(id); move.addBindValue(newCol); move.addBindValue(newRow);
                            if (!move.exec() || !db.commit()) {
                                db.rollback();
                                QMessageBox::critical(this, "Không lưu được vị trí", move.lastError().text().isEmpty() ? db.lastError().text() : move.lastError().text());
                                return;
                            }
                            areaLayout->removeWidget(tableButton);
                            tableButton->setProperty("gridCol", newCol);
                            tableButton->setProperty("gridRow", newRow);
                            areaLayout->addWidget(tableButton, newRow, newCol);
                            if (!otherId.isEmpty() && targetButton) {
                                areaLayout->removeWidget(targetButton);
                                targetButton->setProperty("gridCol", oldCol);
                                targetButton->setProperty("gridRow", oldRow);
                                areaLayout->addWidget(targetButton, oldRow, oldCol);
                            }
                            areaPanel->updateGeometry();
                        };
                        if (tableQuery.value(4).toInt() < 0 || tableQuery.value(5).toInt() < 0) {
                            if (++col == 4) { col = 0; ++row; }
                        }
                        // Admin can edit a table's position directly from the dashboard map.
                        connect(tableButton, &QPushButton::clicked, this,
                                [this, id, name, status, areaName, areaId, tableButton, areaLayout, savedCol, savedRow] {
                            if (userRole != QStringLiteral("Admin")) {
                                QMessageBox::information(this, "Thông tin bàn", name + " (" + id + ")\nKhu vực: " + areaName + "\nTrạng thái: " + status);
                                return;
                            }
                            QDialog dialog(this);
                            dialog.setWindowTitle("Chỉnh vị trí bàn — " + name);
                            dialog.setMinimumWidth(330);
                            auto *form = new QFormLayout(&dialog);
                            auto *colEdit = new QSpinBox(&dialog); colEdit->setRange(0, 19); colEdit->setValue(tableButton->property("gridCol").toInt());
                            auto *rowEdit = new QSpinBox(&dialog); rowEdit->setRange(0, 19); rowEdit->setValue(tableButton->property("gridRow").toInt());
                            auto *hint = new QLabel("Đổi Cột/Hàng để di chuyển bàn trên sơ đồ.\nCột 0–19, Hàng 0–19.", &dialog);
                            hint->setWordWrap(true);
                            form->addRow("Bàn", new QLabel(name + " (" + id + ")", &dialog));
                            form->addRow("Khu vực", new QLabel(areaName, &dialog));
                            form->addRow("Cột", colEdit);
                            form->addRow("Hàng", rowEdit);
                            form->addRow(hint);
                            auto *buttons = new QHBoxLayout;
                            auto *save = new QPushButton("Lưu vị trí", &dialog); save->setObjectName("primary");
                            auto *cancel = new QPushButton("Hủy", &dialog); cancel->setObjectName("soft");
                            buttons->addWidget(save); buttons->addWidget(cancel); form->addRow(buttons);
                            QObject::connect(save, &QPushButton::clicked, &dialog, &QDialog::accept);
                            QObject::connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
                            if (dialog.exec() != QDialog::Accepted) return;

                            QSqlDatabase db = DatabaseManager::database();
                            if (!db.transaction()) { QMessageBox::critical(this, "Lỗi SQL", db.lastError().text()); return; }
                            QSqlQuery occupied(db);
                            occupied.prepare("SELECT TOP 1 p.MaBan,b.TenBan FROM dbo.BoTriBan p JOIN dbo.Ban b ON b.MaBan=p.MaBan WHERE b.MaKhuVuc=? AND p.Cot=? AND p.Hang=? AND p.MaBan<>?");
                            occupied.addBindValue(areaId); occupied.addBindValue(colEdit->value()); occupied.addBindValue(rowEdit->value()); occupied.addBindValue(id);
                            QString otherId, otherName;
                            if (!occupied.exec()) { db.rollback(); QMessageBox::critical(this, "Lỗi SQL", occupied.lastError().text()); return; }
                            if (occupied.next()) { otherId = occupied.value(0).toString(); otherName = occupied.value(1).toString(); }
                            occupied.finish();
                            if (!otherId.isEmpty()) {
                                if (QMessageBox::question(this, "Vị trí đã có bàn", "Ô này đang có " + otherName + ". Bạn muốn đổi chỗ hai bàn không?", QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) { db.rollback(); return; }
                                QSqlQuery swap(db);
                                swap.prepare("UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=?");
                                swap.addBindValue(savedCol); swap.addBindValue(savedRow); swap.addBindValue(otherId);
                                if (!swap.exec()) { db.rollback(); QMessageBox::critical(this, "Không đổi được vị trí", swap.lastError().text()); return; }
                            }
                            QSqlQuery update(db);
                            update.prepare("IF EXISTS(SELECT 1 FROM dbo.BoTriBan WHERE MaBan=?) UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=? ELSE INSERT dbo.BoTriBan(MaBan,Cot,Hang) VALUES(?,?,?)");
                            update.addBindValue(id); update.addBindValue(colEdit->value()); update.addBindValue(rowEdit->value()); update.addBindValue(id);
                            update.addBindValue(id); update.addBindValue(colEdit->value()); update.addBindValue(rowEdit->value());
                            if (!update.exec() || !db.commit()) { db.rollback(); QMessageBox::critical(this, "Không lưu được vị trí", update.lastError().text().isEmpty() ? db.lastError().text() : update.lastError().text()); return; }
                            // Update the visible map immediately; no need to restart the app.
                            areaLayout->removeWidget(tableButton);
                            areaLayout->addWidget(tableButton, rowEdit->value(), colEdit->value());
                            QMessageBox::information(this, "Đã lưu", "Đã cập nhật vị trí bàn trên sơ đồ.");
                        });
                    }
                }

                if (tableCount == 0) {
                    auto *emptyArea = label("Khu vực này chưa có bàn. Admin có thể thêm bàn trong mục Quản lý bàn hoặc Thiết kế sơ đồ bàn.", "muted");
                    emptyArea->setWordWrap(true);
                    emptyArea->setMinimumHeight(60);
                    areaLayout->addWidget(emptyArea, 0, 0, 1, 4);
                }

                areaStack->addWidget(areaPanel);
                QObject::connect(tab, &QPushButton::clicked, areaStack, [areaStack, tabGroup, tab, areaIndex]() {
                    areaStack->setCurrentIndex(areaIndex);
                    tab->setChecked(true);
                    for (auto *other : tabGroup->buttons()) {
                        other->setProperty("activeArea", other == tab);
                        other->style()->unpolish(other);
                        other->style()->polish(other);
                    }
                });
                ++areaIndex;
            }

            if (areaIndex == 0) {
                auto *message = new QWidget;
                auto *layout = new QVBoxLayout(message);
                auto *text = label("Chưa có khu vực nào. Admin có thể tạo Tầng 1, Tầng 2, Phòng VIP hoặc khu vực mới trong mục Thiết kế sơ đồ bàn.", "muted");
                text->setWordWrap(true);
                layout->addWidget(text);
                areaStack->addWidget(message);
            } else {
                if (QAbstractButton *first = tabGroup->button(0)) first->click();
            }
        };
        refreshFloorMap->setToolTip("Tải lại khu vực và bàn từ SQL Server. Khu vực mới do Admin thêm sẽ xuất hiện tại đây.");
        connect(refreshFloorMap, &QPushButton::clicked, w, reloadDashboardAreas);
        reloadDashboardAreas();
        leftCol->addWidget(map,3);

        auto *foodPanel=panel(); auto*foodV=new QVBoxLayout(foodPanel); auto*foodHead=new QHBoxLayout;
        foodHead->addWidget(label("♨  Món ăn phổ biến hôm nay", "pageTitle")); foodHead->addStretch(); auto *allFoods=new QPushButton("Xem tất cả  ›"); allFoods->setObjectName("soft"); foodHead->addWidget(allFoods); connect(allFoods,&QPushButton::clicked,this,[this]{showPage(3);}); foodV->addLayout(foodHead);
        auto*foods=new QHBoxLayout;
        QSqlQuery foodQuery(DatabaseManager::database());
        if(DatabaseManager::isOpen() && foodQuery.exec("SELECT TOP 5 TenMon, DonGia FROM dbo.MonAn WHERE TrangThai=N'Đang bán' ORDER BY TenMon")) {
            int count=0;
            while(foodQuery.next()) { auto*fc=new QFrame;fc->setStyleSheet("QFrame{background:#F7FAF5;border:1px solid #E7EEE4;border-radius:13px;}");auto*fl=new QVBoxLayout(fc);auto*emoji=label("♨");emoji->setAlignment(Qt::AlignCenter);emoji->setStyleSheet("font-size:25px;background:#EAF3E7;border-radius:10px;padding:6px;");fl->addWidget(emoji);fl->addWidget(label(foodQuery.value(0).toString()));auto*price=label(QLocale(QLocale::Vietnamese).toString(foodQuery.value(1).toDouble(),'f',0)+"đ");price->setStyleSheet("color:#000000;font-weight:700;");fl->addWidget(price);foods->addWidget(fc);++count;}
            if(count==0) foods->addWidget(label("Chưa có món đang bán. Hãy thêm món ăn trong mục Món ăn.","muted"));
        } else foods->addWidget(label("Chưa có dữ liệu món ăn hoặc chưa kết nối SQL Server.","muted"));
        foodV->addLayout(foods); leftCol->addWidget(foodPanel,1); content->addWidget(left,7);

        auto *rightCol=new QWidget; auto *rightV=new QVBoxLayout(rightCol); rightV->setContentsMargins(0,0,0,0); rightV->setSpacing(12);
        auto *quick=panel(); shadow(quick); auto*qv=new QVBoxLayout(quick); qv->addWidget(label("▦  Đặt bàn nhanh", "pageTitle"));
        auto*dt=new QDateEdit(QDate::currentDate()); dt->setCalendarPopup(true); auto*tm=new QTimeEdit(QTime(19,0));
        auto*dateTime=new QHBoxLayout; dateTime->addWidget(dt); dateTime->addWidget(tm); qv->addLayout(dateTime);
        auto*people=new QComboBox; people->addItems({"2 người","3 người","4 người","5 người","6 người","8 người"});
        auto*area=new QComboBox; area->addItems({"Tất cả khu vực","Tầng 1","Tầng 2","Phòng VIP","Ngoài trời"});
        auto*choices=new QHBoxLayout; choices->addWidget(people); choices->addWidget(area); qv->addLayout(choices);
        auto*note=new QLineEdit; note->setPlaceholderText("Ghi chú (nếu có)"); qv->addWidget(note);
        auto*book=new QPushButton("⊕   Đặt bàn"); book->setObjectName("primary"); qv->addWidget(book);
        connect(book,&QPushButton::clicked,this,[this,dt,tm,people,area,note]{showPage(1);QMessageBox::information(this,"Đặt bàn nhanh","Đã mở trang Quản lý đặt bàn. Vui lòng nhập thông tin khách và lưu tại trang đó.");});
        rightV->addWidget(quick);

        auto *customers=panel(); auto*cv=new QVBoxLayout(customers); auto*ch=new QHBoxLayout; ch->addWidget(label("♧  Khách hàng gần đây", "pageTitle")); ch->addStretch(); auto *allCustomers=new QPushButton("Xem tất cả  ›"); allCustomers->setObjectName("soft"); ch->addWidget(allCustomers); connect(allCustomers,&QPushButton::clicked,this,[this]{showPage(4);}); cv->addLayout(ch);
        QSqlQuery recentCustomers(DatabaseManager::database());
        if(DatabaseManager::isOpen() && recentCustomers.exec("SELECT TOP 3 kh.HoTen, kh.SoDienThoai, db.MaBan, db.SoNguoi, db.GioDat FROM dbo.DatBan db JOIN dbo.KhachHang kh ON kh.MaKH=db.MaKH ORDER BY db.NgayDat DESC, db.GioDat DESC")) {
            int idx=0;
            while(recentCustomers.next()) {QString n=recentCustomers.value(0).toString(); auto*r=new QHBoxLayout;auto*initial=label(n.isEmpty()?"?":QString(n.at(0)));initial->setAlignment(Qt::AlignCenter);initial->setFixedSize(34,34);initial->setStyleSheet("background:#DCEBE0;border-radius:17px;color:#000000;font-weight:700;");auto*info=new QVBoxLayout;info->addWidget(label(n));info->addWidget(label(recentCustomers.value(1).toString(),"muted"));auto*booking=label(recentCustomers.value(2).toString()+" · "+recentCustomers.value(3).toString()+" người\n"+recentCustomers.value(4).toString().left(5));booking->setStyleSheet("background:#EEF6ED;color:#000000;border-radius:8px;padding:5px;");r->addWidget(initial);r->addLayout(info,1);r->addWidget(booking);cv->addLayout(r);++idx;}
            if(idx==0) cv->addWidget(label("Chưa có lượt đặt bàn. Dữ liệu sẽ xuất hiện sau khi phát sinh đặt bàn.","muted"));
        } else cv->addWidget(label("Chưa có dữ liệu khách hàng hoặc chưa kết nối SQL Server.","muted"));
        rightV->addWidget(customers);

        auto *invoices=panel(); auto*iv=new QVBoxLayout(invoices); auto*ih=new QHBoxLayout; ih->addWidget(label("▣  Hóa đơn gần đây", "pageTitle")); ih->addStretch(); auto *allInvoices=new QPushButton("Xem tất cả  ›"); allInvoices->setObjectName("soft"); ih->addWidget(allInvoices); connect(allInvoices,&QPushButton::clicked,this,[this]{showPage(6);}); iv->addLayout(ih);
        const QStringList invoiceRows={"HD001    Bàn B03 · 4 món    520.000đ    Đã thanh toán","HD002    Bàn B06 · 3 món    385.000đ    Đang xử lý","HD003    Bàn B08 · 5 món    750.000đ    Chưa thanh toán"};
        for(int i=0;i<invoiceRows.size();++i){auto*il=label(invoiceRows[i]);il->setStyleSheet(QString("padding:6px;color:%1;").arg(i==0?"#477B55":(i==1?"#B77B35":"#B75D5D")));iv->addWidget(il);}
        rightV->addWidget(invoices); rightV->addStretch(); content->addWidget(rightCol,4); v->addLayout(content,1); return w;
    }
    QWidget *makeBookingPage(){
        auto*w=new QWidget; auto*v=new QVBoxLayout(w);
        v->addWidget(label("Quản lý đặt bàn", "pageTitle"));
        v->addWidget(label("Tạo lượt đặt bàn và lưu trực tiếp vào SQL Server", "muted"));
        auto*f=panel(); auto*l=new QVBoxLayout(f); auto*form=new QGridLayout;
        auto*name=new QLineEdit; name->setPlaceholderText("Tên khách hàng");
        auto*phone=new QLineEdit; phone->setPlaceholderText("Số điện thoại");
        auto*date=new QDateEdit(QDate::currentDate()); date->setCalendarPopup(true); date->setMinimumDate(QDate::currentDate());
        auto*time=new QTimeEdit(QTime(19,0));
        auto*count=new QSpinBox; count->setRange(1,30); count->setValue(2);
        auto*tableBox=new QComboBox;
        auto*note=new QLineEdit; note->setPlaceholderText("Ghi chú (không bắt buộc)");
        form->addWidget(label("Khách hàng"),0,0); form->addWidget(name,1,0);
        form->addWidget(label("Điện thoại"),0,1); form->addWidget(phone,1,1);
        form->addWidget(label("Ngày đặt"),2,0); form->addWidget(date,3,0);
        form->addWidget(label("Giờ đến"),2,1); form->addWidget(time,3,1);
        form->addWidget(label("Số người"),4,0); form->addWidget(count,5,0);
        form->addWidget(label("Bàn phù hợp"),4,1); form->addWidget(tableBox,5,1);
        form->addWidget(label("Ghi chú"),6,0); form->addWidget(note,7,0,1,2); l->addLayout(form);
        auto*buttons=new QHBoxLayout; auto*refresh=new QPushButton("↻  Tải bàn trống"); refresh->setObjectName("soft");
        auto*save=new QPushButton("＋  Tạo đặt bàn"); save->setObjectName("primary"); buttons->addWidget(refresh); buttons->addWidget(save); buttons->addStretch(); l->addLayout(buttons);
        auto*hint=label("Chọn ngày, giờ và số người trước khi tải danh sách bàn phù hợp.","muted"); l->addWidget(hint);
        auto loadTables=[this,tableBox,date,time,count,hint](){
            tableBox->clear(); if(!DatabaseManager::connect()){hint->setText("Không kết nối được SQL Server.");return;}
            QSqlQuery q(DatabaseManager::database());
            q.prepare("SELECT b.MaBan,b.TenBan,b.SoCho,kv.TenKhuVuc FROM dbo.Ban b JOIN dbo.KhuVuc kv ON kv.MaKhuVuc=b.MaKhuVuc WHERE b.TrangThai<>N'Bảo trì' AND b.SoCho>=? AND NOT EXISTS (SELECT 1 FROM dbo.DatBan d WHERE d.MaBan=b.MaBan AND d.NgayDat=? AND d.TrangThai IN (N'Đã đặt',N'Đã nhận bàn') AND ABS(DATEDIFF(MINUTE,CAST(d.GioDat AS datetime),CAST(? AS datetime)))<120) ORDER BY b.SoCho,b.MaBan");
            q.addBindValue(count->value()); q.addBindValue(date->date()); q.addBindValue(time->time());
            if(!q.exec()){hint->setText("Không tải được bàn: "+q.lastError().text());return;}
            while(q.next()) tableBox->addItem(QString("%1 · %2 · %3 chỗ").arg(q.value(1).toString(),q.value(3).toString()).arg(q.value(2).toInt()),q.value(0).toString());
            hint->setText(tableBox->count()?QString("Có %1 bàn phù hợp.").arg(tableBox->count()):"Không có bàn phù hợp với ngày, giờ và số người đã chọn.");
        };
        connect(refresh,&QPushButton::clicked,w,loadTables); connect(date,&QDateEdit::dateChanged,w,[loadTables](const QDate&){loadTables();}); connect(time,&QTimeEdit::timeChanged,w,[loadTables](const QTime&){loadTables();}); connect(count,QOverload<int>::of(&QSpinBox::valueChanged),w,[loadTables](int){loadTables();});
        connect(save,&QPushButton::clicked,w,[this,name,phone,date,time,count,tableBox,note,loadTables,hint](){
            QString ten=name->text().trimmed(),sdt=phone->text().trimmed(),maBan=tableBox->currentData().toString();
            if(ten.isEmpty()||sdt.isEmpty()||maBan.isEmpty()){QMessageBox::warning(this,"Thiếu thông tin","Nhập tên, số điện thoại và chọn bàn phù hợp.");return;}
            if(sdt.size()<9||sdt.size()>15){QMessageBox::warning(this,"Số điện thoại không hợp lệ","Số điện thoại phải có từ 9 đến 15 ký tự.");return;}
            QSqlDatabase db=DatabaseManager::database(); if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}
            QSqlQuery q(db); q.prepare("SELECT MaKH FROM dbo.KhachHang WHERE SoDienThoai=?"); q.addBindValue(sdt);
            if(!q.exec()){db.rollback();QMessageBox::critical(this,"Lỗi SQL",q.lastError().text());return;}
            QString maKH; if(q.next()) maKH=q.value(0).toString(); q.finish();
            if(maKH.isEmpty()){
                QSqlQuery idq(db); if(!idq.exec("SELECT ISNULL(MAX(TRY_CONVERT(int,SUBSTRING(MaKH,3,8))),0)+1 FROM dbo.KhachHang WHERE MaKH LIKE 'KH%'")||!idq.next()){db.rollback();QMessageBox::critical(this,"Lỗi tạo mã khách hàng",idq.lastError().text());return;}
                maKH="KH"+QString::number(idq.value(0).toInt()).rightJustified(6,'0'); idq.finish();
                QSqlQuery ins(db); ins.prepare("INSERT dbo.KhachHang(MaKH,HoTen,SoDienThoai) VALUES(?,?,?)"); ins.addBindValue(maKH);ins.addBindValue(ten);ins.addBindValue(sdt);
                if(!ins.exec()){db.rollback();QMessageBox::critical(this,"Không tạo được khách hàng",ins.lastError().text());return;}
            } else {QSqlQuery up(db);up.prepare("UPDATE dbo.KhachHang SET HoTen=? WHERE MaKH=?");up.addBindValue(ten);up.addBindValue(maKH);if(!up.exec()){db.rollback();QMessageBox::critical(this,"Không cập nhật khách hàng",up.lastError().text());return;}}
            QSqlQuery idq(db); if(!idq.exec("SELECT ISNULL(MAX(TRY_CONVERT(int,SUBSTRING(MaDatBan,3,8))),0)+1 FROM dbo.DatBan WHERE MaDatBan LIKE 'DB%'")||!idq.next()){db.rollback();QMessageBox::critical(this,"Lỗi tạo mã đặt bàn",idq.lastError().text());return;}
            QString maDB="DB"+QString::number(idq.value(0).toInt()).rightJustified(6,'0');idq.finish();
            QSqlQuery ins(db);ins.prepare("INSERT dbo.DatBan(MaDatBan,MaKH,MaBan,NgayDat,GioDat,SoNguoi,TrangThai,GhiChu) VALUES(?,?,?,?,?,?,N'Đã đặt',?)");ins.addBindValue(maDB);ins.addBindValue(maKH);ins.addBindValue(maBan);ins.addBindValue(date->date());ins.addBindValue(time->time());ins.addBindValue(count->value());ins.addBindValue(note->text().trimmed());
            if(!ins.exec()){db.rollback();QMessageBox::critical(this,"Không tạo được đặt bàn",ins.lastError().text());return;}
            if(date->date()==QDate::currentDate()){QSqlQuery up(db);up.prepare("UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=? AND TrangThai=N'Trống'");up.addBindValue(maBan);if(!up.exec()){db.rollback();QMessageBox::critical(this,"Không cập nhật trạng thái bàn",up.lastError().text());return;}}
            if(!db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được đặt bàn",db.lastError().text());return;}
            QMessageBox::information(this,"Đặt bàn thành công","Đã lưu lượt đặt "+maDB+" vào SQL Server. Khi khách đến, chọn lịch bên dưới và bấm Xác nhận khách đến / Nhận bàn.");name->clear();phone->clear();note->clear();loadTables();
        });
        v->addWidget(f);

        // Danh sach dat ban de nhan vien xac nhan khach den va cap nhat trang thai ban.
        auto *reservationPanel = panel();
        auto *reservationLayout = new QVBoxLayout(reservationPanel);
        auto *reservationHeader = new QHBoxLayout;
        reservationHeader->addWidget(label("Lịch đặt bàn / Xác nhận khách đến", "pageTitle"));
        reservationHeader->addStretch();
        auto *refreshReservations = new QPushButton("↻ Làm mới lịch");
        refreshReservations->setObjectName("soft");
        auto *confirmArrival = new QPushButton("✓ Xác nhận khách đến / Nhận bàn");
        confirmArrival->setObjectName("primary");
        reservationHeader->addWidget(refreshReservations);
        reservationHeader->addWidget(confirmArrival);
        reservationLayout->addLayout(reservationHeader);
        reservationLayout->addWidget(label("Chọn một lịch đang 'Đã đặt' rồi xác nhận khách đến. Bàn sẽ chuyển sang 'Đang sử dụng' và POS có thể tiếp tục phục vụ.", "muted"));
        auto *reservationTable = new QTableWidget(0, 7);
        reservationTable->setHorizontalHeaderLabels({"Mã đặt", "Khách hàng", "Bàn", "Ngày", "Giờ", "Số người", "Trạng thái"});
        reservationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        reservationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        reservationTable->setSelectionMode(QAbstractItemView::SingleSelection);
        reservationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        reservationTable->verticalHeader()->setVisible(false);
        reservationTable->setMinimumHeight(190);
        reservationLayout->addWidget(reservationTable);
        v->addWidget(reservationPanel, 1);

        auto loadReservations = [this, reservationTable]() {
            reservationTable->setRowCount(0);
            if (!DatabaseManager::connect()) {
                QMessageBox::warning(this, "SQL Server", "Không kết nối được cơ sở dữ liệu.");
                return;
            }
            QSqlQuery q(DatabaseManager::database());
            q.prepare("SELECT d.MaDatBan, kh.HoTen, b.TenBan, CONVERT(varchar(10), d.NgayDat, 23), "
                      "CONVERT(varchar(5), d.GioDat, 108), d.SoNguoi, d.TrangThai "
                      "FROM dbo.DatBan d JOIN dbo.KhachHang kh ON kh.MaKH=d.MaKH "
                      "JOIN dbo.Ban b ON b.MaBan=d.MaBan "
                      "WHERE d.NgayDat >= DATEADD(day,-1,CAST(GETDATE() AS date)) "
                      "ORDER BY d.NgayDat DESC, d.GioDat DESC, d.MaDatBan DESC");
            if (!q.exec()) {
                QMessageBox::warning(this, "Không tải được lịch đặt", q.lastError().text());
                return;
            }
            while (q.next()) {
                const int row = reservationTable->rowCount();
                reservationTable->insertRow(row);
                for (int col = 0; col < 7; ++col) {
                    auto *item = new QTableWidgetItem(q.value(col).toString());
                    if (col == 0) item->setData(Qt::UserRole, q.value(0).toString());
                    reservationTable->setItem(row, col, item);
                }
                const QString status = q.value(6).toString();
                const QColor bg = status == QStringLiteral("Đã nhận bàn") ? QColor("#F3C8C7") :
                                  status == QStringLiteral("Đã đặt") ? QColor("#F8E3AF") :
                                  status == QStringLiteral("Đã hủy") || status == QStringLiteral("Không đến") ? QColor("#E5E6E2") : QColor("#E8F2E6");
                for (int col = 0; col < 7; ++col) {
                    if (reservationTable->item(row, col)) {
                        reservationTable->item(row, col)->setBackground(bg);
                        reservationTable->item(row, col)->setForeground(QColor("#202820"));
                    }
                }
            }
        };
        connect(refreshReservations, &QPushButton::clicked, w, loadReservations);
        connect(confirmArrival, &QPushButton::clicked, w, [this, reservationTable, loadReservations]() {
            const int row = reservationTable->currentRow();
            if (row < 0 || !reservationTable->item(row, 0)) {
                QMessageBox::information(this, "Chưa chọn lịch", "Hãy chọn lịch đặt bàn cần xác nhận khách đến.");
                return;
            }
            const QString bookingId = reservationTable->item(row, 0)->text();
            const QString status = reservationTable->item(row, 6)->text();
            if (status != QStringLiteral("Đã đặt")) {
                QMessageBox::information(this, "Không thể xác nhận", "Chỉ lịch đang ở trạng thái 'Đã đặt' mới có thể xác nhận khách đến. Trạng thái hiện tại: " + status);
                return;
            }
            if (QMessageBox::question(this, "Xác nhận khách đến",
                    "Xác nhận khách của lượt " + bookingId + " đã đến và nhận bàn?\nBàn sẽ chuyển sang trạng thái 'Đang sử dụng'.",
                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
            QSqlQuery q(DatabaseManager::database());
            q.prepare("EXEC dbo.sp_DatBan_XacNhanDen @MaDatBan=?");
            q.addBindValue(bookingId);
            if (!q.exec()) {
                QMessageBox::critical(this, "Không xác nhận được khách", q.lastError().text());
                return;
            }
            q.finish();
            loadReservations();
            QMessageBox::information(this, "Đã nhận bàn",
                "Đã cập nhật lượt đặt thành 'Đã nhận bàn' và bàn thành 'Đang sử dụng'.\n"
                "Hãy bấm 'Làm mới sơ đồ' ở Tổng quan nếu sơ đồ chưa cập nhật.");
        });

        loadTables();
        loadReservations();
        return w;
    }
    QWidget *makeTablesPage(){
        auto*w=new QWidget;auto*v=new QVBoxLayout(w);v->addWidget(label("Quản lý bàn ăn","pageTitle"));v->addWidget(label("Dữ liệu bàn lấy trực tiếp từ SQL Server","muted"));
        auto*f=panel();auto*l=new QVBoxLayout(f);auto*filters=new QHBoxLayout;auto*search=new QLineEdit;search->setPlaceholderText("Tìm mã bàn hoặc tên bàn...");auto*area=new QComboBox;area->addItem("Tất cả khu vực","");auto*refresh=new QPushButton("↻ Làm mới");refresh->setObjectName("soft");auto*add=new QPushButton("＋ Thêm bàn");add->setObjectName("primary");filters->addWidget(search,1);filters->addWidget(area);filters->addWidget(refresh);filters->addWidget(add);l->addLayout(filters);
        auto*t=new QTableWidget(0,5);t->setHorizontalHeaderLabels({"Mã bàn","Tên bàn","Khu vực","Số chỗ","Trạng thái"});t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);t->setSelectionBehavior(QAbstractItemView::SelectRows);t->setEditTriggers(QAbstractItemView::NoEditTriggers);t->verticalHeader()->setVisible(false);l->addWidget(t,1);
        auto loadAreas=[this,area](){QString old=area->currentData().toString();area->clear();area->addItem("Tất cả khu vực","");QSqlQuery q(DatabaseManager::database());if(q.exec("SELECT MaKhuVuc,TenKhuVuc FROM dbo.KhuVuc ORDER BY TenKhuVuc"))while(q.next())area->addItem(q.value(1).toString(),q.value(0).toString());int i=area->findData(old);if(i>=0)area->setCurrentIndex(i);};
        auto load=[this,t,area](){t->setRowCount(0);if(!DatabaseManager::connect()){QMessageBox::warning(this,"SQL Server","Không kết nối được cơ sở dữ liệu.");return;}QSqlQuery q(DatabaseManager::database());QString sql="SELECT b.MaBan,b.TenBan,kv.TenKhuVuc,b.SoCho,b.TrangThai FROM dbo.Ban b JOIN dbo.KhuVuc kv ON kv.MaKhuVuc=b.MaKhuVuc";if(!area->currentData().toString().isEmpty())sql+=" WHERE b.MaKhuVuc=?";sql+=" ORDER BY kv.TenKhuVuc,b.MaBan";if(!q.prepare(sql)){QMessageBox::warning(this,"Lỗi SQL",q.lastError().text());return;}if(!area->currentData().toString().isEmpty())q.addBindValue(area->currentData());if(!q.exec()){QMessageBox::warning(this,"Không tải được bàn",q.lastError().text());return;}while(q.next()){int r=t->rowCount();t->insertRow(r);for(int c=0;c<5;++c)t->setItem(r,c,new QTableWidgetItem(q.value(c).toString()));}};
        connect(refresh,&QPushButton::clicked,w,[loadAreas,load](){loadAreas();load();});connect(area,QOverload<int>::of(&QComboBox::currentIndexChanged),w,[load](int){load();});connect(search,&QLineEdit::textChanged,w,[t](const QString&s){for(int r=0;r<t->rowCount();++r){bool ok=false;for(int c=0;c<2;++c)if(t->item(r,c)&&t->item(r,c)->text().contains(s,Qt::CaseInsensitive))ok=true;t->setRowHidden(r,!ok);}});
        connect(add,&QPushButton::clicked,w,[this,loadAreas,load](){if(!DatabaseManager::connect()){QMessageBox::warning(this,"SQL Server","Không kết nối được cơ sở dữ liệu.");return;}QDialog d(this);d.setWindowTitle("Thêm bàn ăn");QFormLayout form(&d);QLineEdit ma,ten;QSpinBox cho;cho.setRange(1,30);cho.setValue(4);QComboBox kv;QSqlQuery q(DatabaseManager::database());if(!q.exec("SELECT MaKhuVuc,TenKhuVuc FROM dbo.KhuVuc ORDER BY TenKhuVuc")){QMessageBox::warning(this,"Lỗi SQL",q.lastError().text());return;}while(q.next())kv.addItem(q.value(1).toString(),q.value(0).toString());if(kv.count()==0){QMessageBox::warning(this,"Thiếu khu vực","Hãy tạo khu vực trong database trước.");return;}QComboBox st;st.addItems({"Trống","Bảo trì"});form.addRow("Mã bàn",&ma);form.addRow("Tên bàn",&ten);form.addRow("Số chỗ",&cho);form.addRow("Khu vực",&kv);form.addRow("Trạng thái",&st);QPushButton save("Lưu"),cancel("Hủy");QHBoxLayout buttons;buttons.addWidget(&save);buttons.addWidget(&cancel);form.addRow(&buttons);QObject::connect(&cancel,&QPushButton::clicked,&d,&QDialog::reject);QObject::connect(&save,&QPushButton::clicked,&d,&QDialog::accept);if(d.exec()!=QDialog::Accepted)return;QString id=ma.text().trimmed(),nm=ten.text().trimmed();if(id.isEmpty()||nm.isEmpty()){QMessageBox::warning(this,"Thiếu thông tin","Nhập mã và tên bàn.");return;}QSqlQuery ins(DatabaseManager::database());ins.prepare("INSERT dbo.Ban(MaBan,TenBan,SoCho,TrangThai,MaKhuVuc) VALUES(?,?,?,?,?)");ins.addBindValue(id);ins.addBindValue(nm);ins.addBindValue(cho.value());ins.addBindValue(st.currentText());ins.addBindValue(kv.currentData());if(!ins.exec()){QMessageBox::critical(this,"Không thêm được bàn",ins.lastError().text());return;}loadAreas();load();QMessageBox::information(this,"Thành công","Đã thêm bàn vào SQL Server.");});
        v->addWidget(f,1);loadAreas();load();return w;
    }
    QWidget *makeFloorDesignPage() {
        auto *w = new QWidget;
        auto *root = new QVBoxLayout(w);
        root->setContentsMargins(0,0,0,0); root->setSpacing(12);
        root->addWidget(label("Thiết kế sơ đồ bàn", "pageTitle"));
        root->addWidget(label("Chỉ Admin được cấu hình khu vực, số bàn và vị trí. Cấu hình được lưu trong SQL Server.", "muted"));

        if (!DatabaseManager::isOpen()) DatabaseManager::connect();
        if (DatabaseManager::isOpen()) {
            QSqlQuery schema(DatabaseManager::database());
            schema.exec("IF OBJECT_ID(N'dbo.BoTriBan',N'U') IS NULL BEGIN CREATE TABLE dbo.BoTriBan(MaBan VARCHAR(10) NOT NULL PRIMARY KEY REFERENCES dbo.Ban(MaBan) ON DELETE CASCADE, Cot INT NOT NULL CONSTRAINT DF_BoTriBan_Cot DEFAULT 0, Hang INT NOT NULL CONSTRAINT DF_BoTriBan_Hang DEFAULT 0, ThuTu INT NOT NULL CONSTRAINT DF_BoTriBan_ThuTu DEFAULT 0); END");
            schema.exec("INSERT dbo.BoTriBan(MaBan,Cot,Hang,ThuTu) SELECT b.MaBan,(ROW_NUMBER() OVER(PARTITION BY b.MaKhuVuc ORDER BY b.MaBan)-1)%4,((ROW_NUMBER() OVER(PARTITION BY b.MaKhuVuc ORDER BY b.MaBan)-1)/4),ROW_NUMBER() OVER(PARTITION BY b.MaKhuVuc ORDER BY b.MaBan) FROM dbo.Ban b WHERE NOT EXISTS(SELECT 1 FROM dbo.BoTriBan p WHERE p.MaBan=b.MaBan)");
        }

        auto *areaPanel = panel(); auto *areaRow = new QHBoxLayout(areaPanel);
        auto *areaBox = new QComboBox; areaBox->setMinimumWidth(210);
        auto *areaName = new QLineEdit; areaName->setPlaceholderText("Tên khu vực mới, ví dụ: Tầng 3");
        auto *areaId = new QLineEdit; areaId->setPlaceholderText("Mã khu vực, ví dụ: T3"); areaId->setMaximumWidth(150);
        auto *addArea = new QPushButton("＋ Thêm khu vực"); addArea->setObjectName("primary");
        auto *renameArea = new QPushButton("Đổi tên khu vực"); renameArea->setObjectName("soft");
        auto *deleteArea = new QPushButton("Xóa khu vực"); deleteArea->setObjectName("soft");
        areaRow->addWidget(label("Khu vực")); areaRow->addWidget(areaBox); areaRow->addWidget(areaId); areaRow->addWidget(areaName,1); areaRow->addWidget(addArea); areaRow->addWidget(renameArea); areaRow->addWidget(deleteArea);
        root->addWidget(areaPanel);

        auto *body = new QHBoxLayout;
        auto *left = panel(); auto *leftLayout = new QVBoxLayout(left);
        leftLayout->addWidget(label("Danh sách bàn trong khu vực", "sectionTitle"));
        auto *table = new QTableWidget(0,5); table->setHorizontalHeaderLabels({"Mã bàn","Tên bàn","Số chỗ","Cột","Hàng"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); table->setSelectionBehavior(QAbstractItemView::SelectRows); table->setSelectionMode(QAbstractItemView::SingleSelection); table->setEditTriggers(QAbstractItemView::NoEditTriggers); table->verticalHeader()->setVisible(false); leftLayout->addWidget(table,1);
        auto *tableForm = new QGridLayout;
        auto *tableId = new QLineEdit; tableId->setPlaceholderText("B01");
        auto *tableName = new QLineEdit; tableName->setPlaceholderText("Bàn 01");
        auto *seats = new QSpinBox; seats->setRange(1,100); seats->setValue(4);
        auto *col = new QSpinBox; col->setRange(0,19); col->setValue(0);
        auto *row = new QSpinBox; row->setRange(0,19); row->setValue(0);
        tableForm->addWidget(label("Mã bàn"),0,0); tableForm->addWidget(tableId,1,0);
        tableForm->addWidget(label("Tên bàn"),0,1); tableForm->addWidget(tableName,1,1);
        tableForm->addWidget(label("Số chỗ"),0,2); tableForm->addWidget(seats,1,2);
        tableForm->addWidget(label("Cột (0–19)"),2,0); tableForm->addWidget(col,3,0);
        tableForm->addWidget(label("Hàng (0–19)"),2,1); tableForm->addWidget(row,3,1);
        leftLayout->addLayout(tableForm);
        auto *actions = new QHBoxLayout;
        auto *addTable = new QPushButton("＋ Thêm bàn"); addTable->setObjectName("primary");
        auto *moreTables = new QPushButton("▾ Xem thêm / Tạo nhanh bàn"); moreTables->setObjectName("soft");
        auto *saveLayout = new QPushButton("Lưu vị trí bàn"); saveLayout->setObjectName("soft");
        auto *deleteTable = new QPushButton("Xóa bàn đã chọn"); deleteTable->setObjectName("soft");
        auto *refresh = new QPushButton("↻ Làm mới"); refresh->setObjectName("soft");
        actions->addWidget(addTable); actions->addWidget(moreTables); actions->addWidget(saveLayout); actions->addWidget(deleteTable); actions->addWidget(refresh); leftLayout->addLayout(actions);
        body->addWidget(left,3);

        auto *previewPanel = panel(); auto *previewLayout = new QVBoxLayout(previewPanel);
        previewLayout->addWidget(label("Xem trước sơ đồ", "sectionTitle"));
        auto *previewScroll = new QScrollArea; previewScroll->setWidgetResizable(true); previewScroll->setFrameShape(QFrame::NoFrame); previewScroll->setStyleSheet("QScrollArea{background:#F8FBF7;border:0;} QWidget#qt_scrollarea_viewport{background:#F8FBF7;}");
        auto *preview = new QWidget; preview->setStyleSheet("background:#F8FBF7;color:#17251B;"); preview->setAutoFillBackground(true); auto *previewGrid = new QGridLayout(preview); previewGrid->setSpacing(10); previewGrid->setContentsMargins(6,6,6,6); previewScroll->setWidget(preview); previewLayout->addWidget(previewScroll,1);
        previewLayout->addWidget(label("Admin có thể kéo-thả bàn trực tiếp trên sơ đồ; thả lên bàn khác để đổi chỗ.","muted")); body->addWidget(previewPanel,2); root->addLayout(body,1);

        auto reloadAreas = [this,areaBox](){QString current=areaBox->currentData().toString(); areaBox->clear(); QSqlQuery q(DatabaseManager::database()); if(q.exec("SELECT MaKhuVuc,TenKhuVuc FROM dbo.KhuVuc ORDER BY TenKhuVuc")) while(q.next()) areaBox->addItem(q.value(1).toString(),q.value(0).toString()); int i=areaBox->findData(current); if(i>=0) areaBox->setCurrentIndex(i);};
        auto reload = [this,areaBox,table,preview,previewGrid,tableId,tableName,seats,col,row](){
            while(QLayoutItem *it=previewGrid->takeAt(0)){if(it->widget()) it->widget()->deleteLater(); delete it;}
            table->setRowCount(0); if(!DatabaseManager::isOpen()||areaBox->currentData().toString().isEmpty()) return;
            QSqlQuery q(DatabaseManager::database()); q.prepare("SELECT b.MaBan,b.TenBan,b.SoCho,ISNULL(p.Cot,0),ISNULL(p.Hang,0),b.TrangThai FROM dbo.Ban b LEFT JOIN dbo.BoTriBan p ON p.MaBan=b.MaBan WHERE b.MaKhuVuc=? ORDER BY ISNULL(p.ThuTu,9999),b.MaBan"); q.addBindValue(areaBox->currentData()); if(!q.exec()){QMessageBox::warning(nullptr,"Không tải được sơ đồ",q.lastError().text());return;}
            while(q.next()) {int r=table->rowCount(); table->insertRow(r); for(int c=0;c<5;++c)table->setItem(r,c,new QTableWidgetItem(q.value(c).toString()));
                auto *b = new DraggableTableButton(QString("%1\n%2 chỗ\n%3").arg(q.value(1).toString()).arg(q.value(2).toInt()).arg(q.value(5).toString()));
                b->setMinimumSize(100,72);
                QString bg="#BFE2C5";
                if(q.value(5).toString()=="Đã đặt") bg="#F8E3AF";
                else if(q.value(5).toString()=="Đang sử dụng") bg="#F3C8C7";
                else if(q.value(5).toString()=="Bảo trì") bg="#D9D9D6";
                b->setStyleSheet(QString("QPushButton{background:%1;border:1px solid #B7D7BE;border-radius:12px;color:#000000;padding:8px;} QPushButton:hover{border:2px solid #4F8963;}").arg(bg));
                int x=q.value(3).toInt(), y=q.value(4).toInt();
                previewGrid->addWidget(b,y,x);
                const QString id=q.value(0).toString();
                b->setProperty("tableId",id); b->setProperty("gridCol",x); b->setProperty("gridRow",y);
                b->setCursor(Qt::OpenHandCursor);
                b->setToolTip(userRole==QStringLiteral("Admin") ? "Giữ chuột trái và kéo bàn đến vị trí mới. Thả lên bàn khác để đổi chỗ." : "Chỉ Admin mới được di chuyển bàn.");
                b->onDrop = [this, areaBox, table, preview, previewGrid, b, id](const QPoint &globalPos) {
                    if (userRole != QStringLiteral("Admin")) {
                        QMessageBox::information(this,"Chỉ dành cho Admin","Chỉ Admin mới có thể di chuyển bàn trên sơ đồ.");
                        return;
                    }
                    const int oldCol=b->property("gridCol").toInt();
                    const int oldRow=b->property("gridRow").toInt();
                    const QPoint dropPos=preview->mapFromGlobal(globalPos);
                    DraggableTableButton *target=nullptr;
                    for(int i=0;i<previewGrid->count();++i){
                        auto *candidate=dynamic_cast<DraggableTableButton*>(previewGrid->itemAt(i)->widget());
                        if(candidate && candidate!=b && candidate->geometry().contains(dropPos)){target=candidate;break;}
                    }
                    int newCol=oldCol,newRow=oldRow; QString otherId;
                    if(target){newCol=target->property("gridCol").toInt();newRow=target->property("gridRow").toInt();otherId=target->property("tableId").toString();}
                    else {
                        DraggableTableButton *reference=nullptr;
                        for(int i=0;i<previewGrid->count();++i){reference=dynamic_cast<DraggableTableButton*>(previewGrid->itemAt(i)->widget());if(reference)break;}
                        if(!reference)return;
                        const int sx=qMax(1,reference->width()+(previewGrid->horizontalSpacing()>=0?previewGrid->horizontalSpacing():10));
                        const int sy=qMax(1,reference->height()+(previewGrid->verticalSpacing()>=0?previewGrid->verticalSpacing():10));
                        const int ox=reference->geometry().left()-reference->property("gridCol").toInt()*sx;
                        const int oy=reference->geometry().top()-reference->property("gridRow").toInt()*sy;
                        newCol=qBound(0,(dropPos.x()-ox)/sx,19); newRow=qBound(0,(dropPos.y()-oy)/sy,19);
                        if(newCol==oldCol && newRow==oldRow)return;
                    }
                    if(newCol==oldCol && newRow==oldRow)return;
                    QSqlDatabase db=DatabaseManager::database();
                    if(!db.transaction()){QMessageBox::critical(this,"Lỗi SQL",db.lastError().text());return;}
                    QSqlQuery occupied(db);
                    occupied.prepare("SELECT TOP 1 b.MaBan FROM dbo.Ban b JOIN dbo.BoTriBan p ON p.MaBan=b.MaBan WHERE b.MaKhuVuc=? AND p.Cot=? AND p.Hang=? AND b.MaBan<>?");
                    occupied.addBindValue(areaBox->currentData().toString()); occupied.addBindValue(newCol); occupied.addBindValue(newRow); occupied.addBindValue(id);
                    if(!occupied.exec()){db.rollback();QMessageBox::critical(this,"Không kiểm tra được vị trí",occupied.lastError().text());return;}
                    if(occupied.next()){
                        const QString occupiedId=occupied.value(0).toString();
                        if(otherId.isEmpty() || otherId!=occupiedId){db.rollback();QMessageBox::warning(this,"Vị trí đã có bàn","Thả trực tiếp lên bàn cần đổi chỗ, hoặc chọn một ô trống.");return;}
                    }
                    occupied.finish();
                    if(!otherId.isEmpty()){
                        QSqlQuery temp(db);temp.prepare("UPDATE dbo.BoTriBan SET Cot=-100,Hang=-100 WHERE MaBan=?");temp.addBindValue(id);
                        if(!temp.exec()){db.rollback();QMessageBox::critical(this,"Không đổi được vị trí",temp.lastError().text());return;}
                        QSqlQuery swap(db);swap.prepare("IF EXISTS(SELECT 1 FROM dbo.BoTriBan WHERE MaBan=?) UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=? ELSE INSERT dbo.BoTriBan(MaBan,Cot,Hang) VALUES(?,?,?)");
                        swap.addBindValue(otherId);swap.addBindValue(oldCol);swap.addBindValue(oldRow);swap.addBindValue(otherId);swap.addBindValue(otherId);swap.addBindValue(oldCol);swap.addBindValue(oldRow);
                        if(!swap.exec()){db.rollback();QMessageBox::critical(this,"Không đổi được vị trí",swap.lastError().text());return;}
                    }
                    QSqlQuery move(db);move.prepare("IF EXISTS(SELECT 1 FROM dbo.BoTriBan WHERE MaBan=?) UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=? ELSE INSERT dbo.BoTriBan(MaBan,Cot,Hang) VALUES(?,?,?)");
                    move.addBindValue(id);move.addBindValue(newCol);move.addBindValue(newRow);move.addBindValue(id);move.addBindValue(id);move.addBindValue(newCol);move.addBindValue(newRow);
                    if(!move.exec() || !db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được vị trí",move.lastError().text().isEmpty()?db.lastError().text():move.lastError().text());return;}
                    previewGrid->removeWidget(b);b->setProperty("gridCol",newCol);b->setProperty("gridRow",newRow);previewGrid->addWidget(b,newRow,newCol);
                    if(target){previewGrid->removeWidget(target);target->setProperty("gridCol",oldCol);target->setProperty("gridRow",oldRow);previewGrid->addWidget(target,oldRow,oldCol);}
                    for(int rr=0;rr<table->rowCount();++rr){
                        if(table->item(rr,0)&&table->item(rr,0)->text()==id){table->item(rr,3)->setText(QString::number(newCol));table->item(rr,4)->setText(QString::number(newRow));}
                        if(!otherId.isEmpty()&&table->item(rr,0)&&table->item(rr,0)->text()==otherId){table->item(rr,3)->setText(QString::number(oldCol));table->item(rr,4)->setText(QString::number(oldRow));}
                    }
                    preview->updateGeometry();
                };
                QObject::connect(b,&QPushButton::clicked,table,[table,id,tableId,tableName,seats,col,row](){for(int rr=0;rr<table->rowCount();++rr)if(table->item(rr,0)&&table->item(rr,0)->text()==id){table->selectRow(rr);tableId->setText(table->item(rr,0)->text());tableName->setText(table->item(rr,1)->text());seats->setValue(table->item(rr,2)->text().toInt());col->setValue(table->item(rr,3)->text().toInt());row->setValue(table->item(rr,4)->text().toInt());break;}});
            }
        };
        QObject::connect(areaBox,QOverload<int>::of(&QComboBox::currentIndexChanged),w,[reload](int){reload();});
        QObject::connect(refresh,&QPushButton::clicked,w,[reloadAreas,reload](){reloadAreas();reload();});
        QObject::connect(addArea,&QPushButton::clicked,w,[this,areaId,areaName,reloadAreas](){QString id=areaId->text().trimmed(),name=areaName->text().trimmed();if(id.isEmpty()||name.isEmpty()){QMessageBox::warning(this,"Thiếu thông tin","Nhập mã và tên khu vực.");return;}QSqlQuery q(DatabaseManager::database());q.prepare("INSERT dbo.KhuVuc(MaKhuVuc,TenKhuVuc) VALUES(?,?)");q.addBindValue(id);q.addBindValue(name);if(!q.exec()){QMessageBox::critical(this,"Không thêm được khu vực",q.lastError().text());return;}areaId->clear();areaName->clear();reloadAreas();QMessageBox::information(this,"Thành công","Đã tạo khu vực.");});
        QObject::connect(renameArea,&QPushButton::clicked,w,[this,areaBox,areaName,reloadAreas](){QString id=areaBox->currentData().toString(),name=areaName->text().trimmed();if(id.isEmpty()||name.isEmpty()){QMessageBox::warning(this,"Thiếu thông tin","Chọn khu vực và nhập tên mới.");return;}QSqlQuery q(DatabaseManager::database());q.prepare("UPDATE dbo.KhuVuc SET TenKhuVuc=? WHERE MaKhuVuc=?");q.addBindValue(name);q.addBindValue(id);if(!q.exec()){QMessageBox::critical(this,"Không đổi được tên",q.lastError().text());return;}reloadAreas();areaName->clear();QMessageBox::information(this,"Thành công","Đã đổi tên khu vực.");});
        QObject::connect(deleteArea,&QPushButton::clicked,w,[this,areaBox,reloadAreas,reload](){QString id=areaBox->currentData().toString();if(id.isEmpty())return;if(QMessageBox::question(this,"Xóa khu vực","Chỉ xóa được khu vực chưa có bàn. Tiếp tục?",QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes)return;QSqlQuery q(DatabaseManager::database());q.prepare("DELETE dbo.KhuVuc WHERE MaKhuVuc=? AND NOT EXISTS(SELECT 1 FROM dbo.Ban WHERE MaKhuVuc=?)");q.addBindValue(id);q.addBindValue(id);if(!q.exec()||q.numRowsAffected()==0){QMessageBox::warning(this,"Không xóa được","Khu vực đang có bàn hoặc lỗi SQL: "+q.lastError().text());return;}reloadAreas();reload();});
        QObject::connect(addTable,&QPushButton::clicked,w,[this,areaBox,tableId,tableName,seats,col,row,reload](){QString id=tableId->text().trimmed(),name=tableName->text().trimmed(),area=areaBox->currentData().toString();if(id.isEmpty()||name.isEmpty()||area.isEmpty()){QMessageBox::warning(this,"Thiếu thông tin","Chọn khu vực và nhập mã/tên bàn.");return;}QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(this,"SQL",db.lastError().text());return;}QSqlQuery q(db);q.prepare("INSERT dbo.Ban(MaBan,TenBan,SoCho,TrangThai,MaKhuVuc) VALUES(?,?,?,N'Trống',?)");q.addBindValue(id);q.addBindValue(name);q.addBindValue(seats->value());q.addBindValue(area);if(!q.exec()){db.rollback();QMessageBox::critical(this,"Không thêm được bàn",q.lastError().text());return;}QSqlQuery p(db);p.prepare("INSERT dbo.BoTriBan(MaBan,Cot,Hang,ThuTu) VALUES(?,?,?,(SELECT COUNT(*)+1 FROM dbo.Ban WHERE MaKhuVuc=?))");p.addBindValue(id);p.addBindValue(col->value());p.addBindValue(row->value());p.addBindValue(area);if(!p.exec()){db.rollback();QMessageBox::critical(this,"Không lưu được vị trí",p.lastError().text());return;}if(!db.commit()){db.rollback();QMessageBox::critical(this,"SQL",db.lastError().text());return;}reload();QMessageBox::information(this,"Thành công","Đã thêm bàn và lưu vị trí vào SQL Server.");});
        QObject::connect(moreTables,&QPushButton::clicked,w,[this,areaBox,reload](){
            QString area=areaBox->currentData().toString();
            if(area.isEmpty()){QMessageBox::information(this,"Chọn khu vực","Hãy chọn khu vực muốn tạo bàn trước.");return;}
            QDialog d(this); d.setWindowTitle("Xem thêm — Tạo nhanh nhiều bàn"); d.setMinimumWidth(430);
            QFormLayout form(&d);
            QLineEdit prefix; prefix.setText("B"); prefix.setMaxLength(6);
            QSpinBox startNo; startNo.setRange(1,9999); startNo.setValue(1);
            QSpinBox count; count.setRange(1,100); count.setValue(10);
            QSpinBox seatCount; seatCount.setRange(1,100); seatCount.setValue(4);
            QSpinBox columns; columns.setRange(1,10); columns.setValue(4);
            QLineEdit namePrefix; namePrefix.setText("Bàn");
            form.addRow("Tiền tố mã bàn",&prefix);
            form.addRow("Số bắt đầu",&startNo);
            form.addRow("Số bàn cần tạo",&count);
            form.addRow("Tên hiển thị",&namePrefix);
            form.addRow("Số chỗ mỗi bàn",&seatCount);
            form.addRow("Số cột trên sơ đồ",&columns);
            auto *note=new QLabel("Ví dụ: tiền tố B, bắt đầu 1, tạo 10 bàn → B01…B10.\\nBàn sẽ được lưu vào SQL Server và tự xuất hiện trên sơ đồ.");
            note->setWordWrap(true); form.addRow(note);
            auto *buttons=new QHBoxLayout; auto *create=new QPushButton("Tạo bàn và lưu"); create->setObjectName("primary"); auto *cancel=new QPushButton("Hủy"); cancel->setObjectName("soft"); buttons->addWidget(create); buttons->addWidget(cancel); form.addRow(buttons);
            QObject::connect(cancel,&QPushButton::clicked,&d,&QDialog::reject);
            QObject::connect(create,&QPushButton::clicked,&d,&QDialog::accept);
            if(d.exec()!=QDialog::Accepted)return;
            QString pfx=prefix.text().trimmed(), nmPrefix=namePrefix.text().trimmed();
            if(pfx.isEmpty()||nmPrefix.isEmpty()){QMessageBox::warning(this,"Thiếu thông tin","Nhập tiền tố mã và tên bàn.");return;}
            if(!DatabaseManager::connect()){QMessageBox::critical(this,"SQL Server","Không kết nối được SQL Server.");return;}
            QSqlDatabase db=DatabaseManager::database();
            if(!db.transaction()){QMessageBox::critical(this,"SQL Server",db.lastError().text());return;}
            int created=0, colCount=columns.value();
            for(int i=0;i<count.value();++i){
                int number=startNo.value()+i;
                QString id=pfx+QString("%1").arg(number,2,10,QChar('0'));
                QString nm=nmPrefix+" "+QString("%1").arg(number,2,10,QChar('0'));
                QSqlQuery ins(db); ins.prepare("INSERT dbo.Ban(MaBan,TenBan,SoCho,TrangThai,MaKhuVuc) VALUES(?,?,?,N'Trống',?)");
                ins.addBindValue(id); ins.addBindValue(nm); ins.addBindValue(seatCount.value()); ins.addBindValue(area);
                if(!ins.exec()){db.rollback();QMessageBox::critical(this,"Không tạo được bàn",QString("Lỗi ở mã %1:\\n%2").arg(id,ins.lastError().text()));return;}
                QSqlQuery pos(db); pos.prepare("INSERT dbo.BoTriBan(MaBan,Cot,Hang,ThuTu) VALUES(?,?,?,?)");
                pos.addBindValue(id); pos.addBindValue(i%colCount); pos.addBindValue(i/colCount); pos.addBindValue(number);
                if(!pos.exec()){db.rollback();QMessageBox::critical(this,"Không lưu được vị trí",QString("Lỗi ở mã %1:\\n%2").arg(id,pos.lastError().text()));return;}
                ++created;
            }
            if(!db.commit()){db.rollback();QMessageBox::critical(this,"SQL Server",db.lastError().text());return;}
            d.accept(); reload(); QMessageBox::information(this,"Đã tạo bàn",QString("Đã tạo %1 bàn trong khu vực đã chọn. Sơ đồ được cập nhật tự động.").arg(created));
        });
        QObject::connect(table,&QTableWidget::itemSelectionChanged,w,[table,tableId,tableName,seats,col,row](){int r=table->currentRow();if(r<0)return;tableId->setText(table->item(r,0)->text());tableName->setText(table->item(r,1)->text());seats->setValue(table->item(r,2)->text().toInt());col->setValue(table->item(r,3)->text().toInt());row->setValue(table->item(r,4)->text().toInt());});
        QObject::connect(saveLayout,&QPushButton::clicked,w,[this,table,tableId,tableName,seats,col,row,reload](){int r=table->currentRow();if(r<0){QMessageBox::information(this,"Chọn bàn","Chọn một bàn trong danh sách trước.");return;}QString id=table->item(r,0)->text();QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(this,"SQL",db.lastError().text());return;}QSqlQuery u(db);u.prepare("UPDATE dbo.Ban SET TenBan=?,SoCho=? WHERE MaBan=?");u.addBindValue(tableName->text().trimmed());u.addBindValue(seats->value());u.addBindValue(id);if(!u.exec()){db.rollback();QMessageBox::critical(this,"Không lưu được thông tin bàn",u.lastError().text());return;}QSqlQuery p(db);p.prepare("IF EXISTS(SELECT 1 FROM dbo.BoTriBan WHERE MaBan=?) UPDATE dbo.BoTriBan SET Cot=?,Hang=? WHERE MaBan=? ELSE INSERT dbo.BoTriBan(MaBan,Cot,Hang) VALUES(?,?,?)");p.addBindValue(id);p.addBindValue(col->value());p.addBindValue(row->value());p.addBindValue(id);p.addBindValue(id);p.addBindValue(col->value());p.addBindValue(row->value());if(!p.exec()){db.rollback();QMessageBox::critical(this,"Không lưu được vị trí",p.lastError().text());return;}if(!db.commit()){db.rollback();QMessageBox::critical(this,"SQL",db.lastError().text());return;}reload();QMessageBox::information(this,"Đã lưu","Đã lưu thông tin và vị trí bàn.");});
        QObject::connect(deleteTable,&QPushButton::clicked,w,[this,table,reload](){int r=table->currentRow();if(r<0){QMessageBox::information(this,"Chọn bàn","Chọn bàn cần xóa.");return;}QString id=table->item(r,0)->text();if(QMessageBox::question(this,"Xóa bàn","Chỉ xóa bàn chưa có lịch đặt/hóa đơn liên quan. Tiếp tục?",QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes)return;QSqlQuery q(DatabaseManager::database());q.prepare("DELETE dbo.Ban WHERE MaBan=? AND NOT EXISTS(SELECT 1 FROM dbo.DatBan WHERE MaBan=?)");q.addBindValue(id);q.addBindValue(id);if(!q.exec()||q.numRowsAffected()==0){QMessageBox::warning(this,"Không xóa được","Bàn đang có lịch đặt liên quan hoặc lỗi SQL: "+q.lastError().text());return;}reload();});
        reloadAreas(); reload(); return w;
    }

    QWidget *makeEmployeePage() {
        auto *w = new QWidget;
        auto *root = new QVBoxLayout(w);
        root->setContentsMargins(0,0,0,0);
        root->setSpacing(12);
        root->addWidget(label("Nhân viên & tài khoản", "pageTitle"));
        root->addWidget(label("Admin tạo tài khoản, xác thực Admin trước khi cấp lại mật khẩu, cập nhật trạng thái hoặc xóa tài khoản. Mật khẩu cũ được mã hóa một chiều nên không thể khôi phục; mật khẩu mới được hiển thị sau khi cấp.", "muted"));

        auto *formPanel = panel();
        auto *formLayout = new QVBoxLayout(formPanel);
        formLayout->addWidget(label("＋  Tạo nhân viên mới", "pageTitle"));
        auto *form = new QGridLayout;
        auto *fullName = new QLineEdit; fullName->setPlaceholderText("Họ và tên nhân viên");
        auto *username = new QLineEdit; username->setPlaceholderText("Tên đăng nhập duy nhất");
        auto *password = new QLineEdit; password->setPlaceholderText("Mật khẩu ban đầu (ít nhất 10 ký tự)"); password->setEchoMode(QLineEdit::Password);
        auto *showInitialPassword = new QPushButton("Hiện"); showInitialPassword->setObjectName("soft"); showInitialPassword->setCheckable(true);
        connect(showInitialPassword, &QPushButton::toggled, password, [password, showInitialPassword](bool visible) {
            password->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
            showInitialPassword->setText(visible ? "Ẩn" : "Hiện");
        });
        auto *passwordField = new QWidget; auto *passwordFieldLayout = new QHBoxLayout(passwordField);
        passwordFieldLayout->setContentsMargins(0,0,0,0); passwordFieldLayout->setSpacing(6);
        passwordFieldLayout->addWidget(password,1); passwordFieldLayout->addWidget(showInitialPassword);
        auto *phone = new QLineEdit; phone->setPlaceholderText("Số điện thoại");
        auto *email = new QLineEdit; email->setPlaceholderText("Email");
        auto *gender = new QComboBox; gender->addItems({"Chưa cập nhật", "Nam", "Nữ", "Khác"});
        auto *birthDate = new QDateEdit(QDate(2000,1,1)); birthDate->setCalendarPopup(true); birthDate->setDisplayFormat("dd/MM/yyyy"); birthDate->setSpecialValueText("Chưa cập nhật"); birthDate->setMinimumDate(QDate(1900,1,1));
        auto *position = new QComboBox; position->addItems({"Nhân viên phục vụ", "Thu ngân", "Pha chế", "Bếp", "Quản lý ca"});
        auto *salary = new QDoubleSpinBox; salary->setRange(0,1000000000); salary->setDecimals(0); salary->setSuffix(" đ/tháng"); salary->setSingleStep(500000);
        form->addWidget(label("Họ và tên"),0,0); form->addWidget(fullName,1,0);
        form->addWidget(label("Tên đăng nhập"),0,1); form->addWidget(username,1,1);
        form->addWidget(label("Mật khẩu ban đầu"),0,2); form->addWidget(passwordField,1,2);
        form->addWidget(label("Số điện thoại"),2,0); form->addWidget(phone,3,0);
        form->addWidget(label("Email"),2,1); form->addWidget(email,3,1);
        form->addWidget(label("Giới tính"),2,2); form->addWidget(gender,3,2);
        form->addWidget(label("Ngày sinh"),4,0); form->addWidget(birthDate,5,0);
        form->addWidget(label("Chức vụ"),4,1); form->addWidget(position,5,1);
        form->addWidget(label("Lương"),4,2); form->addWidget(salary,5,2);
        formLayout->addLayout(form);
        auto *createButton = new QPushButton("Tạo nhân viên và cấp tài khoản"); createButton->setObjectName("primary");
        formLayout->addWidget(createButton,0,Qt::AlignLeft);
        root->addWidget(formPanel);

        auto *listPanel = panel(); auto *listLayout = new QVBoxLayout(listPanel);
        auto *toolbar = new QHBoxLayout;
        auto *filter = new QLineEdit; filter->setPlaceholderText("Tìm nhân viên, tên đăng nhập, điện thoại...");
        auto *refresh = new QPushButton("Làm mới"); refresh->setObjectName("soft");
        auto *toggle = new QPushButton("Khóa / mở tài khoản"); toggle->setObjectName("soft");
        auto *remove = new QPushButton("Xóa tài khoản"); remove->setObjectName("primary"); remove->setStyleSheet("QPushButton#primary{background:#C96F68;}");
        auto *resetPassword = new QPushButton("Đặt lại mật khẩu"); resetPassword->setObjectName("soft");
        toolbar->addWidget(filter,1); toolbar->addWidget(refresh); toolbar->addWidget(toggle); toolbar->addWidget(resetPassword); toolbar->addWidget(remove); listLayout->addLayout(toolbar);
        auto *table = new QTableWidget(0,10); table->setHorizontalHeaderLabels({"Mã NV","Họ tên","Tên đăng nhập","Điện thoại","Email","Chức vụ","Vai trò","Trạng thái","Mật khẩu","Thao tác"}); table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); table->verticalHeader()->setVisible(false); table->setSelectionBehavior(QAbstractItemView::SelectRows); table->setSelectionMode(QAbstractItemView::SingleSelection); table->setEditTriggers(QAbstractItemView::NoEditTriggers); listLayout->addWidget(table,1); root->addWidget(listPanel,1);

        auto loadEmployees = [this,table]() {
            table->setRowCount(0);
            if (!DatabaseManager::connect()) { QMessageBox::critical(this,"SQL Server","Không kết nối được cơ sở dữ liệu."); return; }
            QSqlQuery q(DatabaseManager::database());
            if (!q.exec("SELECT nv.MaNV,nv.HoTen,tk.MaTK,tk.TenDangNhap,ISNULL(nv.SoDienThoai,''),ISNULL(nv.Email,''),ISNULL(nv.ChucVu,''),tk.VaiTro,tk.TrangThai FROM dbo.TaiKhoan tk INNER JOIN dbo.NhanVien nv ON nv.MaNV=tk.MaNV WHERE tk.VaiTro IN (N'Nhân viên',N'NhanVien') ORDER BY nv.HoTen")) { QMessageBox::critical(this,"Không tải được nhân viên",q.lastError().text()); return; }
            while(q.next()) {
                int r=table->rowCount(); table->insertRow(r);
                const QList<QString> values={q.value(0).toString(),q.value(1).toString(),q.value(3).toString(),q.value(4).toString(),q.value(5).toString(),q.value(6).toString(),q.value(7).toString(),q.value(8).toString(),QStringLiteral("••••••••••")};
                for(int c=0;c<values.size();++c) { auto *it=new QTableWidgetItem(values[c]); if(c==0) it->setData(Qt::UserRole,q.value(2).toString()); if(c==8) { it->setToolTip("Mật khẩu được lưu bằng PBKDF2 nên không thể xem ngược mật khẩu cũ. Nhấn nút Xem để xác thực Admin và cấp mật khẩu mới nếu cần."); it->setForeground(QColor("#597260")); } table->setItem(r,c,it); }
                auto *viewPassword = new QPushButton("👁 Xem / cấp lại"); viewPassword->setObjectName("soft"); viewPassword->setCursor(Qt::PointingHandCursor);
                const QString accountId=q.value(2).toString();
                QObject::connect(viewPassword,&QPushButton::clicked,this,[this,accountId,table,r]() {
                    if(userRole!="Admin") { QMessageBox::warning(this,"Không đủ quyền","Chỉ Admin mới được xem/cấp lại mật khẩu nhân viên."); return; }
                    bool ok=false;
                    const QString adminUsername=QInputDialog::getText(this,"Xác thực Admin","Nhập tên đăng nhập Admin:",QLineEdit::Normal,QString(),&ok).trimmed();
                    if(!ok || adminUsername.isEmpty()) return;
                    const QString adminPassword=QInputDialog::getText(this,"Xác thực Admin","Nhập mật khẩu Admin để tiếp tục:",QLineEdit::Password,QString(),&ok);
                    if(!ok) return;
                    TaiKhoanService accountService; DangNhapService loginService(accountService);
                    const TaiKhoan *verified=loginService.dangNhap(adminUsername.toStdString(),adminPassword.toStdString());
                    if(!verified || (verified->getVaiTro()!="Admin" && verified->getVaiTro()!="Quản lý")) {
                        QMessageBox::warning(this,"Xác thực thất bại","Tên đăng nhập/mật khẩu Admin không đúng hoặc tài khoản không có quyền Admin."); return;
                    }
                    QMessageBox::information(this,"Không thể khôi phục mật khẩu cũ","Mật khẩu cũ được lưu bằng PBKDF2 nên không thể giải mã. Bạn có thể cấp mật khẩu mới rồi xem mật khẩu mới một lần để gửi cho nhân viên.");
                    const QString newPassword=QInputDialog::getText(this,"Cấp mật khẩu mới","Nhập mật khẩu mới (ít nhất 10 ký tự):",QLineEdit::Password,QString(),&ok);
                    if(!ok) return;
                    if(newPassword.size()<10) { QMessageBox::warning(this,"Mật khẩu chưa hợp lệ","Mật khẩu phải có ít nhất 10 ký tự."); return; }
                    QSqlQuery update(DatabaseManager::database()); update.prepare("UPDATE dbo.TaiKhoan SET MatKhau=? WHERE MaTK=? AND VaiTro IN (N'Nhân viên',N'NhanVien')"); update.addBindValue(PasswordUtils::hashPassword(newPassword)); update.addBindValue(accountId);
                    if(!update.exec() || update.numRowsAffected()!=1) { QMessageBox::critical(this,"Không cấp được mật khẩu",update.lastError().text()); return; }
                    if(table->item(r,8)) table->item(r,8)->setText(newPassword);
                    QMessageBox::information(this,"Mật khẩu mới đã được cấp",QString("Mật khẩu mới của nhân viên là:\n\n%1\n\nHãy chuyển mật khẩu này trực tiếp cho nhân viên. Mật khẩu sẽ không thể xem lại sau khi đóng/mở lại trang.").arg(newPassword));
                });
                table->setCellWidget(r,9,viewPassword);
            }
        };
        connect(refresh,&QPushButton::clicked,this,loadEmployees);
        connect(resetPassword,&QPushButton::clicked,this,[this,selectedAccountId=std::function<QString()>([table](){int r=table->currentRow(); if(r<0||!table->item(r,0)) return QString(); return table->item(r,0)->data(Qt::UserRole).toString();}),loadEmployees](){
            if(userRole!="Admin"){QMessageBox::warning(this,"Không đủ quyền","Chỉ Admin được đặt lại mật khẩu nhân viên.");return;}
            const QString id=selectedAccountId();
            if(id.isEmpty()){QMessageBox::information(this,"Chọn nhân viên","Hãy chọn một dòng nhân viên trước.");return;}
            bool ok=false; const QString pw=QInputDialog::getText(this,"Đặt lại mật khẩu","Nhập mật khẩu mới (ít nhất 10 ký tự):",QLineEdit::Password,QString(),&ok);
            if(!ok)return;
            if(pw.size()<10){QMessageBox::warning(this,"Mật khẩu chưa hợp lệ","Mật khẩu phải có ít nhất 10 ký tự.");return;}
            if(!DatabaseManager::connect()){QMessageBox::critical(this,"SQL Server","Không kết nối được cơ sở dữ liệu.");return;}
            QSqlQuery q(DatabaseManager::database()); q.prepare("UPDATE dbo.TaiKhoan SET MatKhau=? WHERE MaTK=? AND VaiTro IN (N'Nhân viên',N'NhanVien')"); q.addBindValue(PasswordUtils::hashPassword(pw)); q.addBindValue(id);
            if(!q.exec()||q.numRowsAffected()!=1){QMessageBox::critical(this,"Không đặt lại được mật khẩu",q.lastError().text());return;}
            loadEmployees(); QMessageBox::information(this,"Thành công","Đã đặt lại mật khẩu. Mật khẩu mới không được hiển thị trong danh sách.");
        });
        connect(filter,&QLineEdit::textChanged,this,[table](const QString &text){for(int r=0;r<table->rowCount();++r){bool match=false;for(int c=0;c<table->columnCount();++c) if(table->item(r,c)&&table->item(r,c)->text().contains(text,Qt::CaseInsensitive)) match=true; table->setRowHidden(r,!match);}});
        connect(createButton,&QPushButton::clicked,this,[=]() {
            const QString name=fullName->text().trimmed(), login=username->text().trimmed(), pw=password->text();
            if(name.isEmpty()||login.isEmpty()||pw.size()<10){QMessageBox::warning(this,"Thiếu thông tin","Nhập họ tên, tên đăng nhập và mật khẩu từ 10 ký tự trở lên.");return;}
            if(login.contains(QRegularExpression("\\s"))){QMessageBox::warning(this,"Tên đăng nhập không hợp lệ","Tên đăng nhập không được chứa khoảng trắng.");return;}
            if(!DatabaseManager::connect()){QMessageBox::critical(this,"SQL Server","Không kết nối được cơ sở dữ liệu.");return;}
            QSqlDatabase db=DatabaseManager::database();
            if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}
            auto nextId=[&db](const QString &tableName,const QString &column,const QString &prefix)->QString {
                QSqlQuery idQuery(db); const QString sql=QString("SELECT ISNULL(MAX(TRY_CONVERT(int,SUBSTRING(%1,3,8))),0)+1 FROM dbo.%2 WITH (UPDLOCK,HOLDLOCK) WHERE %1 LIKE '%3%' ").arg(column,tableName,prefix);
                if(!idQuery.exec(sql)||!idQuery.next()) return QString();
                return prefix+QString::number(idQuery.value(0).toInt()).rightJustified(8,'0');
            };
            const QString maNV=nextId("NhanVien","MaNV","NV"); const QString maTK=nextId("TaiKhoan","MaTK","TK");
            if(maNV.isEmpty()||maTK.isEmpty()){db.rollback();QMessageBox::critical(this,"Lỗi tạo mã","Không tạo được mã nhân viên/tài khoản.");return;}
            QSqlQuery emp(db); emp.prepare("INSERT dbo.NhanVien(MaNV,HoTen,GioiTinh,NgaySinh,SoDienThoai,Email,ChucVu,Luong,TrangThai) VALUES(?,?,?,?,?,?,?,?,N'Đang làm')");
            emp.addBindValue(maNV); emp.addBindValue(name); emp.addBindValue(gender->currentText()=="Chưa cập nhật"?QVariant():QVariant(gender->currentText())); emp.addBindValue(birthDate->date()==QDate(2000,1,1)?QVariant():QVariant(birthDate->date())); emp.addBindValue(phone->text().trimmed().isEmpty()?QVariant():QVariant(phone->text().trimmed())); emp.addBindValue(email->text().trimmed().isEmpty()?QVariant():QVariant(email->text().trimmed())); emp.addBindValue(position->currentText()); emp.addBindValue(salary->value());
            if(!emp.exec()){db.rollback();QMessageBox::critical(this,"Không tạo được nhân viên",emp.lastError().text());return;}
            QSqlQuery acc(db); acc.prepare("INSERT dbo.TaiKhoan(MaTK,TenDangNhap,MatKhau,VaiTro,MaNV,TrangThai) VALUES(?,?,?,N'Nhân viên',?,N'Hoạt động')"); acc.addBindValue(maTK); acc.addBindValue(login); acc.addBindValue(PasswordUtils::hashPassword(pw)); acc.addBindValue(maNV);
            if(!acc.exec()){db.rollback();QMessageBox::critical(this,"Không tạo được tài khoản",acc.lastError().text()+"\nKiểm tra tên đăng nhập có bị trùng không.");return;}
            if(!db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được",db.lastError().text());return;}
            fullName->clear();username->clear();password->clear();phone->clear();email->clear();salary->setValue(0);
            loadEmployees(); QMessageBox::information(this,"Đã tạo tài khoản",QString("Đã lưu nhân viên và tài khoản vào SQL Server.\nMã nhân viên: %1\nTên đăng nhập: %2\nVai trò: Nhân viên").arg(maNV,login));
        });
        auto selectedAccountId=[table]()->QString {int r=table->currentRow();if(r<0||!table->item(r,0))return QString();return table->item(r,0)->data(Qt::UserRole).toString();};
        connect(toggle,&QPushButton::clicked,this,[this,selectedAccountId,loadEmployees](){QString id=selectedAccountId();if(id.isEmpty()){QMessageBox::information(this,"Chọn nhân viên","Hãy chọn một dòng nhân viên trước.");return;}QSqlQuery q(DatabaseManager::database());q.prepare("UPDATE dbo.TaiKhoan SET TrangThai=CASE WHEN TrangThai=N'Hoạt động' THEN N'Bị khóa' ELSE N'Hoạt động' END WHERE MaTK=? AND VaiTro IN (N'Nhân viên',N'NhanVien')");q.addBindValue(id);if(!q.exec()||q.numRowsAffected()!=1){QMessageBox::critical(this,"Không cập nhật được",q.lastError().text());return;}loadEmployees();});
        connect(remove,&QPushButton::clicked,this,[this,selectedAccountId,loadEmployees](){QString id=selectedAccountId();if(id.isEmpty()){QMessageBox::information(this,"Chọn nhân viên","Hãy chọn tài khoản cần xóa.");return;}if(QMessageBox::question(this,"Xóa tài khoản","Xóa tài khoản đăng nhập này? Thông tin nhân viên được giữ lại để bảo toàn lịch sử hóa đơn/đặt bàn.",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;QSqlQuery q(DatabaseManager::database());q.prepare("DELETE FROM dbo.TaiKhoan WHERE MaTK=? AND VaiTro IN (N'Nhân viên',N'NhanVien')");q.addBindValue(id);if(!q.exec()||q.numRowsAffected()!=1){QMessageBox::critical(this,"Không xóa được tài khoản",q.lastError().text());return;}loadEmployees();QMessageBox::information(this,"Đã xóa","Đã xóa tài khoản đăng nhập. Hồ sơ nhân viên được giữ lại cho dữ liệu lịch sử.");});
        if(userRole=="Admin") loadEmployees();
        return w;
    }

    QWidget *makeAccountPage() {
        auto *w = new QWidget;
        auto *root = new QVBoxLayout(w);
        root->setContentsMargins(0,0,0,0); root->setSpacing(12);
        root->addWidget(label("Quản lý tài khoản Admin & nhà hàng", "pageTitle"));
        root->addWidget(label("Chỉ Admin được thao tác. Không thể xóa tài khoản đang đăng nhập hoặc Admin hoạt động cuối cùng.", "muted"));

        auto *accountPanel = panel(); auto *av = new QVBoxLayout(accountPanel);
        auto *buttons = new QHBoxLayout;
        auto *reload = new QPushButton("Làm mới danh sách"); reload->setObjectName("soft");
        auto *removeAdmin = new QPushButton("Xóa tài khoản Admin đã chọn"); removeAdmin->setObjectName("primary");
        buttons->addWidget(reload); buttons->addStretch(); buttons->addWidget(removeAdmin); av->addLayout(buttons);
        auto *table = new QTableWidget(0,4);
        table->setHorizontalHeaderLabels({"Mã tài khoản","Tên đăng nhập","Vai trò","Trạng thái"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); table->setSelectionBehavior(QAbstractItemView::SelectRows); table->setSelectionMode(QAbstractItemView::SingleSelection); table->setEditTriggers(QAbstractItemView::NoEditTriggers); table->verticalHeader()->setVisible(false); av->addWidget(table,1);
        auto *restaurantPanel = panel(); auto *rv = new QVBoxLayout(restaurantPanel);
        rv->addWidget(label("Đăng ký nhà hàng trên máy này", "pageTitle"));
        rv->addWidget(label("Xóa đăng ký sẽ xóa mã/tên nhà hàng lưu trong database.ini trên máy này. Không xóa dữ liệu nhà hàng trong SQL Server.", "muted"));
        auto *removeRestaurant = new QPushButton("Xóa đăng ký nhà hàng trên máy này"); removeRestaurant->setObjectName("soft"); rv->addWidget(removeRestaurant,0,Qt::AlignLeft);
        root->addWidget(accountPanel,1); root->addWidget(restaurantPanel);

        auto loadAdmins = [table]() {
            table->setRowCount(0);
            if(!DatabaseManager::connect()) { QMessageBox::critical(table,"SQL Server","Không kết nối được cơ sở dữ liệu."); return; }
            QSqlQuery q(DatabaseManager::database());
            if(!q.exec("SELECT MaTK,TenDangNhap,VaiTro,TrangThai FROM dbo.TaiKhoan WHERE VaiTro IN (N'Admin',N'Quản lý') ORDER BY TenDangNhap")) { QMessageBox::critical(table,"Không tải được tài khoản Admin",q.lastError().text()); return; }
            while(q.next()) { int r=table->rowCount(); table->insertRow(r); for(int c=0;c<4;++c)table->setItem(r,c,new QTableWidgetItem(q.value(c).toString())); }
        };
        connect(reload,&QPushButton::clicked,this,loadAdmins);
        connect(removeAdmin,&QPushButton::clicked,this,[this,table,loadAdmins]() {
            int row=table->currentRow(); if(row<0){QMessageBox::information(this,"Chưa chọn tài khoản","Chọn một tài khoản Admin trong danh sách.");return;}
            QString id=table->item(row,0)->text(), username=table->item(row,1)->text();
            if(username.compare(user,Qt::CaseInsensitive)==0){QMessageBox::warning(this,"Không thể xóa","Bạn không thể xóa tài khoản Admin đang đăng nhập.");return;}
            if(QMessageBox::question(this,"Xóa tài khoản Admin",QString("Xóa quyền đăng nhập của tài khoản %1? Hồ sơ nhân viên và dữ liệu lịch sử sẽ được giữ lại.").arg(username),QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;
            QSqlDatabase db=DatabaseManager::database(); if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}
            QSqlQuery lock(db); if(!lock.exec("SELECT COUNT(*) FROM dbo.TaiKhoan WITH (UPDLOCK,HOLDLOCK) WHERE VaiTro IN (N'Admin',N'Quản lý') AND TrangThai=N'Hoạt động'")||!lock.next()){db.rollback();QMessageBox::critical(this,"Không kiểm tra được Admin",lock.lastError().text());return;}
            int activeAdmins=lock.value(0).toInt();
            QSqlQuery target(db); target.prepare("SELECT TrangThai FROM dbo.TaiKhoan WITH (UPDLOCK,HOLDLOCK) WHERE MaTK=? AND VaiTro IN (N'Admin',N'Quản lý')"); target.addBindValue(id);
            if(!target.exec()||!target.next()){db.rollback();QMessageBox::warning(this,"Không tìm thấy","Tài khoản đã thay đổi hoặc không còn tồn tại.");return;}
            bool targetActive=target.value(0).toString()==QStringLiteral("Hoạt động");
            if(targetActive && activeAdmins<=1){db.rollback();QMessageBox::warning(this,"Không thể xóa","Không thể xóa Admin hoạt động cuối cùng.");return;}
            QSqlQuery del(db);del.prepare("DELETE FROM dbo.TaiKhoan WHERE MaTK=? AND VaiTro IN (N'Admin',N'Quản lý')");del.addBindValue(id);
            if(!del.exec()||del.numRowsAffected()!=1||!db.commit()){db.rollback();QMessageBox::critical(this,"Xóa thất bại",del.lastError().text().isEmpty()?db.lastError().text():del.lastError().text());return;}
            loadAdmins();QMessageBox::information(this,"Đã xóa","Đã xóa tài khoản đăng nhập. Hồ sơ nhân viên và lịch sử nghiệp vụ được giữ lại.");
        });
        connect(removeRestaurant,&QPushButton::clicked,this,[this]() {
            QSettings cfg(QDir(QApplication::applicationDirPath()).filePath("database.ini"),QSettings::IniFormat);
            QString code=cfg.value("restaurant/code","").toString();
            if(code.isEmpty()){QMessageBox::information(this,"Chưa đăng ký","Máy này chưa có đăng ký nhà hàng để xóa.");return;}
            if(QMessageBox::warning(this,"Xóa đăng ký nhà hàng", "Xóa mã và tên đăng ký nhà hàng trên máy này? Thao tác này KHÔNG xóa database hoặc hóa đơn trong SQL Server; muốn đăng nhập lại cần đăng ký nhà hàng lại.",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;
            cfg.beginGroup("restaurant");cfg.remove("code");cfg.remove("name");cfg.remove("adminUsername");cfg.endGroup();cfg.sync();
            if(cfg.status()!=QSettings::NoError){QMessageBox::critical(this,"Không xóa được","Không ghi được database.ini. Hãy kiểm tra quyền ghi thư mục ứng dụng.");return;}
            QMessageBox::information(this,"Đã xóa đăng ký","Đã xóa mã đăng ký trên máy này. Dữ liệu SQL Server vẫn được giữ nguyên.");
        });
        if(userRole=="Admin") loadAdmins();
        return w;
    }

    QWidget *makePosPage() {
        auto *w=new QWidget; auto *root=new QVBoxLayout(w); root->setContentsMargins(0,0,0,0); root->setSpacing(12);
        root->addWidget(label("POS NHÂN VIÊN — BÁN HÀNG", "pageTitle"));
        root->addWidget(label("Chức năng riêng cho nhân viên: nhận khách, chọn bàn, gọi món, xem tổng tiền và thanh toán. Dữ liệu được lưu vào SQL Server.","muted"));
        auto *selection=panel(); auto *sv=new QGridLayout(selection);
        auto *bookingBox=new QComboBox; auto *invoiceBox=new QComboBox; auto *refresh=new QPushButton("Làm mới lượt khách");refresh->setObjectName("soft");auto *createInvoice=new QPushButton("＋ Tạo hóa đơn");createInvoice->setObjectName("primary");auto *walkIn=new QPushButton("＋ Nhận khách vãng lai");walkIn->setObjectName("soft");
        sv->addWidget(label("Lượt khách đã nhận bàn"),0,0);sv->addWidget(bookingBox,1,0,1,2);sv->addWidget(label("Hóa đơn đang mở"),0,2);sv->addWidget(invoiceBox,1,2,1,2);sv->addWidget(refresh,1,4);sv->addWidget(createInvoice,1,5);sv->addWidget(walkIn,1,6);root->addWidget(selection);
        auto *body=new QHBoxLayout;body->setSpacing(12);
        auto *menuPanel=panel();auto *mv=new QVBoxLayout(menuPanel);mv->addWidget(label("Thực đơn", "pageTitle"));auto *menuSearch=new QLineEdit;menuSearch->setPlaceholderText("Tìm món ăn...");mv->addWidget(menuSearch);auto *menuTable=new QTableWidget(0,4);menuTable->setHorizontalHeaderLabels({"Mã món","Tên món","Loại","Đơn giá"});menuTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);menuTable->verticalHeader()->setVisible(false);menuTable->setSelectionBehavior(QAbstractItemView::SelectRows);menuTable->setSelectionMode(QAbstractItemView::SingleSelection);menuTable->setEditTriggers(QAbstractItemView::NoEditTriggers);mv->addWidget(menuTable,1);auto *qtyRow=new QHBoxLayout;auto *qty=new QSpinBox;qty->setRange(1,999);qty->setValue(1);auto *addItem=new QPushButton("Thêm món vào hóa đơn");addItem->setObjectName("primary");qtyRow->addWidget(label("Số lượng"));qtyRow->addWidget(qty);qtyRow->addWidget(addItem,1);mv->addLayout(qtyRow);
        auto *billPanel=panel();auto *bv=new QVBoxLayout(billPanel);bv->addWidget(label("Hóa đơn hiện tại", "pageTitle"));auto *billInfo=label("Chọn lượt khách và hóa đơn để xem chi tiết.","muted");bv->addWidget(billInfo);auto *billTable=new QTableWidget(0,5);billTable->setHorizontalHeaderLabels({"Mã món","Tên món","Số lượng","Đơn giá","Thành tiền"});billTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);billTable->verticalHeader()->setVisible(false);billTable->setSelectionBehavior(QAbstractItemView::SelectRows);billTable->setSelectionMode(QAbstractItemView::SingleSelection);billTable->setEditTriggers(QAbstractItemView::NoEditTriggers);bv->addWidget(billTable,1);auto *editRow=new QHBoxLayout;auto *newQty=new QSpinBox;newQty->setRange(1,999);auto *updateQty=new QPushButton("Cập nhật SL");updateQty->setObjectName("soft");auto *deleteItem=new QPushButton("Xóa món");deleteItem->setObjectName("soft");editRow->addWidget(newQty);editRow->addWidget(updateQty);editRow->addWidget(deleteItem);bv->addLayout(editRow);auto *total=label("Tổng thanh toán: 0 đ","statValue");bv->addWidget(total);auto *payRow=new QHBoxLayout;auto *method=new QComboBox;method->addItems({"Tiền mặt","Chuyển khoản","Ví điện tử","QR"});auto *pay=new QPushButton("Thanh toán hóa đơn");pay->setObjectName("primary");payRow->addWidget(method,1);payRow->addWidget(pay);bv->addLayout(payRow);body->addWidget(menuPanel,5);body->addWidget(billPanel,6);root->addLayout(body,1);

        auto currentBooking=[bookingBox](){return bookingBox->currentData().toString();};
        auto currentInvoice=[invoiceBox](){return invoiceBox->currentData().toString();};
        auto employeeId=[this]()->QString {QSqlQuery q(DatabaseManager::database());q.prepare("SELECT MaNV FROM dbo.TaiKhoan WHERE TenDangNhap=? AND TrangThai=N'Hoạt động'");q.addBindValue(user);if(!q.exec()||!q.next())return QString();return q.value(0).toString();};
        auto loadMenu=[this,menuTable,menuSearch](){menuTable->setRowCount(0);if(!DatabaseManager::connect())return;QSqlQuery q(DatabaseManager::database());if(!q.exec("SELECT MaMon,TenMon,ISNULL(LoaiMon,N''),DonGia FROM dbo.MonAn WHERE TrangThai=N'Đang bán' ORDER BY LoaiMon,TenMon")){QMessageBox::critical(this,"Không tải được thực đơn",q.lastError().text());return;}while(q.next()){int r=menuTable->rowCount();menuTable->insertRow(r);for(int c=0;c<4;++c)menuTable->setItem(r,c,new QTableWidgetItem(c==3?QLocale::system().toString(q.value(c).toDouble(),'f',0)+" đ":q.value(c).toString()));menuTable->item(r,0)->setData(Qt::UserRole,q.value(0).toString());}menuSearch->setText(QString());};
        connect(menuSearch,&QLineEdit::textChanged,this,[menuTable](const QString&s){for(int r=0;r<menuTable->rowCount();++r){bool match=false;for(int c=0;c<3;++c)if(menuTable->item(r,c)->text().contains(s,Qt::CaseInsensitive))match=true;menuTable->setRowHidden(r,!match);}});
        auto loadInvoices=[this,bookingBox,invoiceBox,billTable,billInfo,total]() {invoiceBox->clear();billTable->setRowCount(0);total->setText("Tổng thanh toán: 0 đ");QString dbId=bookingBox->currentData().toString();if(dbId.isEmpty())return;QSqlQuery q(DatabaseManager::database());q.prepare("SELECT MaHD,ThanhTien FROM dbo.HoaDon WHERE MaDatBan=? AND TrangThai=N'Chưa thanh toán' ORDER BY NgayLap DESC");q.addBindValue(dbId);if(!q.exec()){QMessageBox::critical(this,"Không tải được hóa đơn",q.lastError().text());return;}while(q.next())invoiceBox->addItem(q.value(0).toString()+" · "+QLocale::system().toString(q.value(1).toDouble(),'f',0)+" đ",q.value(0).toString());q.finish();billInfo->setText(invoiceBox->count()?"Hóa đơn chưa thanh toán đã được lưu trên SQL Server.":"Chưa có hóa đơn mở. Nhấn ‘Tạo hóa đơn’ để bắt đầu.");};
        auto loadBookings=[this,bookingBox,loadInvoices]() {QString previous=bookingBox->currentData().toString();bookingBox->clear();QSqlQuery q(DatabaseManager::database());if(!q.exec("SELECT d.MaDatBan,kh.HoTen,b.TenBan,d.SoNguoi,d.NgayDat,d.GioDat FROM dbo.DatBan d JOIN dbo.KhachHang kh ON kh.MaKH=d.MaKH JOIN dbo.Ban b ON b.MaBan=d.MaBan WHERE d.TrangThai=N'Đã nhận bàn' ORDER BY d.NgayDat,d.GioDat")){QMessageBox::critical(this,"Không tải được lượt khách",q.lastError().text());return;}while(q.next()){QString id=q.value(0).toString();QString text=QString("%1 · %2 · %3 · %4 người").arg(id,q.value(1).toString(),q.value(2).toString()).arg(q.value(3).toInt());bookingBox->addItem(text,id);}q.finish();int idx=bookingBox->findData(previous);if(idx>=0)bookingBox->setCurrentIndex(idx);loadInvoices();};
        auto refreshBill=[this,invoiceBox,billTable,billInfo,total]() {billTable->setRowCount(0);QString id=invoiceBox->currentData().toString();if(id.isEmpty()){total->setText("Tổng thanh toán: 0 đ");return;}QSqlQuery q(DatabaseManager::database());q.prepare("SELECT ct.MaMon,m.TenMon,ct.SoLuong,ct.DonGia,ct.ThanhTien,h.TongTien,h.TienGiam,h.ThanhTien FROM dbo.ChiTietHoaDon ct JOIN dbo.MonAn m ON m.MaMon=ct.MaMon JOIN dbo.HoaDon h ON h.MaHD=ct.MaHD WHERE ct.MaHD=? ORDER BY m.TenMon");q.addBindValue(id);if(!q.exec()){QMessageBox::critical(this,"Không tải được chi tiết",q.lastError().text());return;}double amount=0;while(q.next()){int r=billTable->rowCount();billTable->insertRow(r);billTable->setItem(r,0,new QTableWidgetItem(q.value(0).toString()));billTable->setItem(r,1,new QTableWidgetItem(q.value(1).toString()));billTable->setItem(r,2,new QTableWidgetItem(q.value(2).toString()));billTable->setItem(r,3,new QTableWidgetItem(QLocale::system().toString(q.value(3).toDouble(),'f',0)+" đ"));billTable->setItem(r,4,new QTableWidgetItem(QLocale::system().toString(q.value(4).toDouble(),'f',0)+" đ"));amount=q.value(7).toDouble();}q.finish();QSqlQuery totalQ(DatabaseManager::database());totalQ.prepare("SELECT TongTien,TienGiam,ThanhTien FROM dbo.HoaDon WHERE MaHD=?");totalQ.addBindValue(id);if(totalQ.exec()&&totalQ.next()){amount=totalQ.value(2).toDouble();billInfo->setText(QString("Mã hóa đơn: %1  |  Tổng món: %2 đ  |  Giảm: %3 đ").arg(id).arg(QLocale::system().toString(totalQ.value(0).toDouble(),'f',0),QLocale::system().toString(totalQ.value(1).toDouble(),'f',0)));}total->setText("Tổng thanh toán: "+QLocale::system().toString(amount,'f',0)+" đ");};
        connect(refresh,&QPushButton::clicked,this,loadBookings);connect(bookingBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[loadInvoices](int){loadInvoices();});connect(invoiceBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,[refreshBill](int){refreshBill();});
        connect(walkIn,&QPushButton::clicked,this,[this,employeeId,loadBookings]() {
            if(!DatabaseManager::connect()){QMessageBox::critical(this,"SQL Server","Không kết nối được cơ sở dữ liệu.");return;}
            QDialog dialog(this);dialog.setWindowTitle("Nhận khách vãng lai tại POS");dialog.setMinimumWidth(440);dialog.setStyleSheet(STYLE);
            auto *layout=new QVBoxLayout(&dialog);layout->setContentsMargins(24,22,24,22);layout->setSpacing(10);
            layout->addWidget(label("Nhận khách và mở lượt phục vụ","pageTitle"));
            layout->addWidget(label("Thông tin khách, bàn và lượt phục vụ sẽ được lưu vào SQL Server.","muted"));
            auto *guestName=new QLineEdit;guestName->setPlaceholderText("Họ tên khách hàng");
            auto *guestPhone=new QLineEdit;guestPhone->setPlaceholderText("Số điện thoại khách hàng");
            auto *tableBox=new QComboBox;
            QSqlQuery tables(DatabaseManager::database());
            if(!tables.exec("SELECT MaBan,TenBan,SoCho FROM dbo.Ban WHERE TrangThai=N'Trống' ORDER BY TenBan")){QMessageBox::critical(this,"Không tải được bàn",tables.lastError().text());return;}
            while(tables.next())tableBox->addItem(QString("%1 · %2 chỗ").arg(tables.value(1).toString()).arg(tables.value(2).toInt()),tables.value(0).toString());
            tables.finish();
            auto *guestCount=new QSpinBox;guestCount->setRange(1,50);guestCount->setValue(2);
            auto *form=new QFormLayout;form->addRow("Tên khách",guestName);form->addRow("Điện thoại",guestPhone);form->addRow("Bàn trống",tableBox);form->addRow("Số khách",guestCount);layout->addLayout(form);
            auto *buttons=new QHBoxLayout;auto *cancel=new QPushButton("Hủy");cancel->setObjectName("soft");auto *save=new QPushButton("Xác nhận nhận bàn");save->setObjectName("primary");buttons->addStretch();buttons->addWidget(cancel);buttons->addWidget(save);layout->addLayout(buttons);
            connect(cancel,&QPushButton::clicked,&dialog,&QDialog::reject);
            connect(save,&QPushButton::clicked,&dialog,[&,this]() {
                const QString name=guestName->text().trimmed(),phone=guestPhone->text().trimmed(),tableId=tableBox->currentData().toString();
                if(name.isEmpty()||phone.isEmpty()||tableId.isEmpty()){QMessageBox::warning(&dialog,"Thiếu thông tin","Nhập tên, số điện thoại và chọn bàn trống.");return;}
                const QString nv=employeeId();if(nv.isEmpty()){QMessageBox::critical(&dialog,"Tài khoản chưa liên kết","Không tìm thấy hồ sơ nhân viên của tài khoản hiện tại.");return;}
                QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(&dialog,"Lỗi giao dịch",db.lastError().text());return;}
                QSqlQuery tableCheck(db);tableCheck.prepare("SELECT SoCho FROM dbo.Ban WITH (UPDLOCK,HOLDLOCK) WHERE MaBan=? AND TrangThai=N'Trống'");tableCheck.addBindValue(tableId);
                if(!tableCheck.exec()||!tableCheck.next()){tableCheck.finish();db.rollback();QMessageBox::warning(&dialog,"Bàn không còn trống","Bàn vừa được sử dụng hoặc không tồn tại. Hãy làm mới rồi chọn bàn khác.");return;}
                const int seats=tableCheck.value(0).toInt();tableCheck.finish();if(guestCount->value()>seats){db.rollback();QMessageBox::warning(&dialog,"Bàn không đủ chỗ",QString("Bàn này chỉ có %1 chỗ.").arg(seats));return;}
                QString customerId;QSqlQuery customer(db);customer.prepare("SELECT MaKH FROM dbo.KhachHang WITH (UPDLOCK,HOLDLOCK) WHERE SoDienThoai=?");customer.addBindValue(phone);
                if(!customer.exec()){db.rollback();QMessageBox::critical(&dialog,"Lỗi tra cứu khách",customer.lastError().text());return;}
                if(customer.next()){customerId=customer.value(0).toString();customer.finish();QSqlQuery updateCustomer(db);updateCustomer.prepare("UPDATE dbo.KhachHang SET HoTen=? WHERE MaKH=?");updateCustomer.addBindValue(name);updateCustomer.addBindValue(customerId);if(!updateCustomer.exec()){db.rollback();QMessageBox::critical(&dialog,"Không cập nhật được khách",updateCustomer.lastError().text());return;}}
                else {customer.finish();QSqlQuery idq(db);if(!idq.exec("SELECT ISNULL(MAX(TRY_CONVERT(int,SUBSTRING(MaKH,3,8))),0)+1 FROM dbo.KhachHang WITH (UPDLOCK,HOLDLOCK) WHERE MaKH LIKE 'KH%'")||!idq.next()){db.rollback();QMessageBox::critical(&dialog,"Lỗi tạo mã khách",idq.lastError().text());return;}customerId="KH"+QString::number(idq.value(0).toInt()).rightJustified(8,'0');idq.finish();QSqlQuery addCustomer(db);addCustomer.prepare("INSERT dbo.KhachHang(MaKH,HoTen,SoDienThoai,NgayTao) VALUES(?,?,?,GETDATE())");addCustomer.addBindValue(customerId);addCustomer.addBindValue(name);addCustomer.addBindValue(phone);if(!addCustomer.exec()){db.rollback();QMessageBox::critical(&dialog,"Không lưu được khách hàng",addCustomer.lastError().text());return;}}
                QSqlQuery idBooking(db);if(!idBooking.exec("SELECT ISNULL(MAX(TRY_CONVERT(int,SUBSTRING(MaDatBan,3,8))),0)+1 FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan LIKE 'DB%'")||!idBooking.next()){db.rollback();QMessageBox::critical(&dialog,"Lỗi tạo mã lượt khách",idBooking.lastError().text());return;}QString bookingId="DB"+QString::number(idBooking.value(0).toInt()).rightJustified(8,'0');idBooking.finish();
                QSqlQuery insertBooking(db);insertBooking.prepare("INSERT dbo.DatBan(MaDatBan,MaKH,MaBan,MaNV,NgayDat,GioDat,SoNguoi,TrangThai,GhiChu) VALUES(?,?,?,?,CONVERT(date,GETDATE()),CONVERT(time(0),GETDATE()),?,N'Đã nhận bàn',N'Khách vãng lai từ POS')");insertBooking.addBindValue(bookingId);insertBooking.addBindValue(customerId);insertBooking.addBindValue(tableId);insertBooking.addBindValue(nv);insertBooking.addBindValue(guestCount->value());
                if(!insertBooking.exec()){db.rollback();QMessageBox::critical(&dialog,"Không tạo được lượt phục vụ",insertBooking.lastError().text());return;}
                QSqlQuery updateTable(db);updateTable.prepare("UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=? AND TrangThai=N'Trống'");updateTable.addBindValue(tableId);if(!updateTable.exec()||updateTable.numRowsAffected()==0){db.rollback();QMessageBox::critical(&dialog,"Không cập nhật được bàn",updateTable.lastError().text());return;}
                if(!db.commit()){db.rollback();QMessageBox::critical(&dialog,"Không lưu được lượt phục vụ",db.lastError().text());return;}
                dialog.accept();loadBookings();QMessageBox::information(this,"Đã nhận khách",QString("Đã lưu lượt phục vụ %1. Bạn có thể tạo hóa đơn và gọi món trong POS.").arg(bookingId));
            });
            dialog.exec();
        });
        connect(createInvoice,&QPushButton::clicked,this,[this,currentBooking,employeeId,loadInvoices,invoiceBox,refreshBill](){QString booking=currentBooking();if(booking.isEmpty()){QMessageBox::information(this,"Chưa có lượt khách","Chỉ tạo hóa đơn khi có lượt khách đã xác nhận nhận bàn.");return;}QString nv=employeeId();if(nv.isEmpty()){QMessageBox::critical(this,"Không xác định được nhân viên","Tài khoản hiện tại chưa liên kết với hồ sơ nhân viên trong SQL Server.");return;}QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}QSqlQuery existing(db);existing.prepare("SELECT TOP 1 MaHD FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan=? AND TrangThai=N'Chưa thanh toán' ORDER BY NgayLap DESC");existing.addBindValue(booking);if(!existing.exec()){db.rollback();QMessageBox::critical(this,"Lỗi SQL",existing.lastError().text());return;}if(existing.next()){existing.finish();db.rollback();QMessageBox::information(this,"Đã có hóa đơn", "Lượt khách này đã có hóa đơn chưa thanh toán. Hãy chọn hóa đơn đó để tiếp tục.");loadInvoices();return;}existing.finish();QSqlQuery idq(db);if(!idq.exec("SELECT ISNULL(MAX(TRY_CONVERT(int,SUBSTRING(MaHD,3,8))),0)+1 FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD LIKE 'HD%' ")||!idq.next()){db.rollback();QMessageBox::critical(this,"Lỗi tạo mã",idq.lastError().text());return;}QString hd="HD"+QString::number(idq.value(0).toInt()).rightJustified(8,'0');idq.finish();QSqlQuery ins(db);ins.prepare("INSERT dbo.HoaDon(MaHD,MaDatBan,MaNV,TongTien,TienGiam,ThanhTien,TrangThai) VALUES(?,?,?,0,0,0,N'Chưa thanh toán')");ins.addBindValue(hd);ins.addBindValue(booking);ins.addBindValue(nv);if(!ins.exec()){db.rollback();QMessageBox::critical(this,"Không tạo được hóa đơn",ins.lastError().text());return;}if(!db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được hóa đơn",db.lastError().text());return;}loadInvoices();int idx=invoiceBox->findData(hd);if(idx>=0)invoiceBox->setCurrentIndex(idx);refreshBill();QMessageBox::information(this,"Đã tạo hóa đơn","Hóa đơn đã được lưu vào SQL Server: "+hd);});
        connect(addItem,&QPushButton::clicked,this,[this,menuTable,qty,invoiceBox,refreshBill](){QString hd=invoiceBox->currentData().toString();int row=menuTable->currentRow();if(hd.isEmpty()){QMessageBox::information(this,"Chưa chọn hóa đơn","Chọn hoặc tạo hóa đơn trước khi gọi món.");return;}if(row<0||!menuTable->item(row,0)){QMessageBox::information(this,"Chưa chọn món","Chọn một món trong thực đơn.");return;}QString maMon=menuTable->item(row,0)->data(Qt::UserRole).toString();QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}QSqlQuery check(db);check.prepare("SELECT SoLuong FROM dbo.ChiTietHoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD=? AND MaMon=?");check.addBindValue(hd);check.addBindValue(maMon);if(!check.exec()){db.rollback();QMessageBox::critical(this,"Lỗi SQL",check.lastError().text());return;}QSqlQuery q(db);if(check.next()){int old=check.value(0).toInt();check.finish();q.prepare("UPDATE dbo.ChiTietHoaDon SET SoLuong=? WHERE MaHD=? AND MaMon=? AND EXISTS(SELECT 1 FROM dbo.HoaDon WHERE MaHD=? AND TrangThai=N'Chưa thanh toán')");q.addBindValue(old+qty->value());q.addBindValue(hd);q.addBindValue(maMon);q.addBindValue(hd);}else{check.finish();q.prepare("INSERT dbo.ChiTietHoaDon(MaHD,MaMon,SoLuong,DonGia) SELECT ?,m.MaMon,?,m.DonGia FROM dbo.MonAn m JOIN dbo.HoaDon h ON h.MaHD=? WHERE m.MaMon=? AND m.TrangThai=N'Đang bán' AND h.TrangThai=N'Chưa thanh toán'");q.addBindValue(hd);q.addBindValue(qty->value());q.addBindValue(hd);q.addBindValue(maMon);}if(!q.exec()||q.numRowsAffected()==0){db.rollback();QMessageBox::critical(this,"Không thêm được món",q.lastError().text().isEmpty()?"Hóa đơn đã đóng hoặc món không còn bán.":q.lastError().text());return;}q.finish();QSqlQuery recalc(db);recalc.prepare("EXEC dbo.sp_TinhTongHoaDon @MaHD=?, @TienGiamMoi=NULL");recalc.addBindValue(hd);if(!recalc.exec()){db.rollback();QMessageBox::critical(this,"Không tính lại được tiền",recalc.lastError().text());return;}recalc.finish();if(!db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được",db.lastError().text());return;}refreshBill();});
        connect(updateQty,&QPushButton::clicked,this,[this,billTable,newQty,invoiceBox,refreshBill](){int row=billTable->currentRow();QString hd=invoiceBox->currentData().toString();if(row<0||hd.isEmpty()){QMessageBox::information(this,"Chọn món","Chọn món trong hóa đơn trước.");return;}QString mon=billTable->item(row,0)->text();QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}QSqlQuery q(db);q.prepare("UPDATE dbo.ChiTietHoaDon SET SoLuong=? WHERE MaHD=? AND MaMon=? AND EXISTS(SELECT 1 FROM dbo.HoaDon WHERE MaHD=? AND TrangThai=N'Chưa thanh toán')");q.addBindValue(newQty->value());q.addBindValue(hd);q.addBindValue(mon);q.addBindValue(hd);if(!q.exec()||q.numRowsAffected()!=1){db.rollback();QMessageBox::critical(this,"Không cập nhật được",q.lastError().text());return;}q.finish();QSqlQuery recalc(db);recalc.prepare("EXEC dbo.sp_TinhTongHoaDon @MaHD=?, @TienGiamMoi=NULL");recalc.addBindValue(hd);if(!recalc.exec()){db.rollback();QMessageBox::critical(this,"Không tính lại được",recalc.lastError().text());return;}recalc.finish();if(!db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được",db.lastError().text());return;}refreshBill();});
        connect(deleteItem,&QPushButton::clicked,this,[this,billTable,invoiceBox,refreshBill](){int row=billTable->currentRow();QString hd=invoiceBox->currentData().toString();if(row<0||hd.isEmpty()){QMessageBox::information(this,"Chọn món","Chọn món cần xóa.");return;}if(QMessageBox::question(this,"Xóa món","Xóa món khỏi hóa đơn chưa thanh toán?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;QString mon=billTable->item(row,0)->text();QSqlDatabase db=DatabaseManager::database();if(!db.transaction()){QMessageBox::critical(this,"Lỗi giao dịch",db.lastError().text());return;}QSqlQuery q(db);q.prepare("DELETE ct FROM dbo.ChiTietHoaDon ct JOIN dbo.HoaDon h ON h.MaHD=ct.MaHD WHERE ct.MaHD=? AND ct.MaMon=? AND h.TrangThai=N'Chưa thanh toán'");q.addBindValue(hd);q.addBindValue(mon);if(!q.exec()||q.numRowsAffected()!=1){db.rollback();QMessageBox::critical(this,"Không xóa được món",q.lastError().text());return;}q.finish();QSqlQuery recalc(db);recalc.prepare("EXEC dbo.sp_TinhTongHoaDon @MaHD=?, @TienGiamMoi=NULL");recalc.addBindValue(hd);if(!recalc.exec()){db.rollback();QMessageBox::critical(this,"Không tính lại được tiền",recalc.lastError().text());return;}recalc.finish();if(!db.commit()){db.rollback();QMessageBox::critical(this,"Không lưu được",db.lastError().text());return;}refreshBill();});
        connect(pay,&QPushButton::clicked,this,[this,invoiceBox,method,loadInvoices,refreshBill,bookingBox](){QString hd=invoiceBox->currentData().toString();if(hd.isEmpty()){QMessageBox::information(this,"Chưa chọn hóa đơn","Chọn hóa đơn cần thanh toán.");return;}if(QMessageBox::question(this,"Xác nhận thanh toán", "Xác nhận thanh toán hóa đơn "+hd+"?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)!=QMessageBox::Yes)return;QSqlQuery q(DatabaseManager::database());q.prepare("EXEC dbo.sp_ThanhToanHoaDon @MaHD=?, @PhuongThucThanhToan=?");q.addBindValue(hd);q.addBindValue(method->currentText());if(!q.exec()){QMessageBox::critical(this,"Thanh toán thất bại",q.lastError().text());return;}q.finish();QMessageBox::information(this,"Thanh toán thành công","Hóa đơn "+hd+" đã được lưu trạng thái thanh toán trong SQL Server.");loadInvoices();refreshBill();});
        if(userRole!="Admin"){loadMenu();loadBookings();}
        return w;
    }

    QWidget *makeGenericPage(const QString &name){
        auto *w=new QWidget; auto *v=new QVBoxLayout(w);
        auto *heading=new QHBoxLayout; heading->addWidget(label(name,"pageTitle")); heading->addStretch();
        auto *refresh=new QPushButton("↻  Làm mới"); refresh->setObjectName("soft"); heading->addWidget(refresh); v->addLayout(heading);
        v->addWidget(label("Dữ liệu được tải trực tiếp từ SQL Server. Chọn một dòng để xem thông tin chi tiết.","muted"));
        auto *f=panel(); auto *l=new QVBoxLayout(f); auto *search=new QLineEdit; search->setPlaceholderText("Tìm theo mã, tên hoặc trạng thái..."); l->addWidget(search);
        auto *t=new QTableWidget(0,4); t->setHorizontalHeaderLabels({"Mã","Tên / Thông tin","Giá trị / Liên hệ","Trạng thái"}); t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); t->setSelectionBehavior(QAbstractItemView::SelectRows); t->setEditTriggers(QAbstractItemView::NoEditTriggers); t->verticalHeader()->setVisible(false); l->addWidget(t,1);
        auto *detail=new QLabel("Chưa chọn bản ghi."); detail->setObjectName("muted"); detail->setWordWrap(true); l->addWidget(detail);
        // Luôn hiển thị bảng và tải dữ liệu ngay khi trang được tạo/mở.
        // Nếu database chưa có bản ghi, hiển thị thông báo rõ ràng thay vì bảng trống.
        t->setMinimumHeight(300);
        t->verticalHeader()->setDefaultSectionSize(34);
        t->verticalHeader()->setMinimumSectionSize(28);
        t->setAlternatingRowColors(true);
        t->setShowGrid(true);
        t->setWordWrap(false);
        t->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        auto load=[this,name,t,detail,w](){
            t->setRowCount(0);
            detail->setText("Đang tải dữ liệu...");
            if(!DatabaseManager::connect()){
                detail->setText("Không thể tải dữ liệu: chưa kết nối được SQL Server.");
                t->setRowCount(1);
                t->setSpan(0,0,1,t->columnCount());
                t->setItem(0,0,new QTableWidgetItem("Không kết nối được SQL Server. Kiểm tra database.ini và trạng thái SQL Server."));
                return;
            }
            QString sql;
            if(name=="Món ăn") sql="SELECT TOP 500 MaMon,TenMon,CONVERT(nvarchar(50),CAST(DonGia AS money),1),TrangThai FROM dbo.MonAn ORDER BY TenMon";
            else if(name=="Khách hàng") sql="SELECT TOP 500 MaKH,HoTen,SoDienThoai,ISNULL(Email,N'') FROM dbo.KhachHang ORDER BY NgayTao DESC";
            else if(name=="Hóa đơn") sql="SELECT TOP 500 MaHD,MaDatBan,CONVERT(nvarchar(50),CAST(ThanhTien AS money),1),TrangThai FROM dbo.HoaDon ORDER BY NgayLap DESC";
            else if(name=="Thống kê") sql="SELECT N'Tổng lượt đặt',CONVERT(nvarchar(50),COUNT(*)),N'Lượt',N'Tất cả' FROM dbo.DatBan UNION ALL SELECT N'Đang đặt',CONVERT(nvarchar(50),COUNT(*)),N'Lượt',N'Đã đặt' FROM dbo.DatBan WHERE TrangThai=N'Đã đặt' UNION ALL SELECT N'Đã nhận bàn',CONVERT(nvarchar(50),COUNT(*)),N'Lượt',N'Đã nhận bàn' FROM dbo.DatBan WHERE TrangThai=N'Đã nhận bàn' UNION ALL SELECT N'Đã hủy',CONVERT(nvarchar(50),COUNT(*)),N'Lượt',N'Đã hủy' FROM dbo.DatBan WHERE TrangThai=N'Đã hủy' UNION ALL SELECT N'Không đến',CONVERT(nvarchar(50),COUNT(*)),N'Lượt',N'Không đến' FROM dbo.DatBan WHERE TrangThai=N'Không đến' UNION ALL SELECT N'Doanh thu đã thanh toán',CONVERT(nvarchar(50),CAST(ISNULL(SUM(ThanhTien),0) AS money),1),N'VNĐ',N'Đã thanh toán' FROM dbo.HoaDon WHERE TrangThai=N'Đã thanh toán'";
            else sql="SELECT TOP 500 MaDatBan,MaKH,MaBan,TrangThai FROM dbo.DatBan ORDER BY NgayDat DESC,GioDat DESC";
            QSqlQuery q(DatabaseManager::database());
            if(!q.exec(sql)){
                detail->setText("Truy vấn dữ liệu thất bại. Chi tiết lỗi SQL Server: " + q.lastError().text());
                t->setRowCount(1);
                t->setSpan(0,0,1,t->columnCount());
                t->setItem(0,0,new QTableWidgetItem("Không tải được dữ liệu: " + q.lastError().text()));
                return;
            }
            while(q.next()){
                int r=t->rowCount(); t->insertRow(r);
                for(int c=0;c<4;++c) {
                    auto *item=new QTableWidgetItem(q.value(c).toString());
                    item->setToolTip(item->text());
                    t->setItem(r,c,item);
                }
            }
            if(t->rowCount()==0){
                t->insertRow(0);
                t->setSpan(0,0,1,t->columnCount());
                t->setItem(0,0,new QTableWidgetItem("Chưa có dữ liệu. Hãy thêm bản ghi hoặc kiểm tra database đang kết nối."));
                detail->setText("Danh sách hiện đang trống; dữ liệu sẽ tự xuất hiện sau khi được lưu vào SQL Server.");
            } else {
                detail->setText(QString("Đã tải %1 bản ghi từ SQL Server. Chọn một dòng để xem chi tiết.").arg(t->rowCount()));
            }
        };
        connect(refresh,&QPushButton::clicked,w,load);
        connect(search,&QLineEdit::textChanged,w,[t](const QString &text){for(int r=0;r<t->rowCount();++r){bool found=false;for(int c=0;c<t->columnCount();++c)if(t->item(r,c)&&t->item(r,c)->text().contains(text,Qt::CaseInsensitive)){found=true;break;}t->setRowHidden(r,!found);}});
        connect(t,&QTableWidget::itemSelectionChanged,w,[t,detail,name](){int r=t->currentRow();if(r<0)return;QStringList parts;for(int c=0;c<t->columnCount();++c)if(t->item(r,c))parts<<t->horizontalHeaderItem(c)->text()+": "+t->item(r,c)->text();detail->setText(name+" — "+parts.join("  |  "));});
        v->addWidget(f,1); load(); return w;
    }
};

int main(int argc,char *argv[])
{
    QApplication app(argc,argv);
    app.setApplicationName("Tasty Restaurant");
    app.setStyle("Fusion");

    QString err;
    if(!DatabaseManager::connect(&err)){
        QMessageBox::critical(nullptr,"Không kết nối được SQL Server",
            "Không thể kết nối cơ sở dữ liệu:\n"+err+
            "\n\nKiểm tra database.ini và SQL Server rồi mở lại ứng dụng.");
        return 1;
    }

    TaiKhoanService accountService;
    DangNhapService loginService(accountService);

    QSettings config(QDir(QApplication::applicationDirPath()).filePath("database.ini"), QSettings::IniFormat);
    config.beginGroup("restaurant");
    QString configuredRestaurantCode = config.value("code", "").toString().trimmed();
    QString configuredAdminUsername = config.value("adminUsername", "").toString().trimmed();
    config.endGroup();

    QMainWindow login;
    login.setWindowTitle("Đăng nhập | Tasty Restaurant");
    login.resize(520,700);
    login.setMinimumSize(480,650);
    login.setStyleSheet(STYLE);

    auto *root=new QWidget;
    root->setObjectName("root");
    auto *v=new QVBoxLayout(root);
    v->setContentsMargins(42,32,42,30);
    v->setSpacing(13);
    v->addWidget(label("♧  Nhà Hàng", "brand"),0,Qt::AlignHCenter);
    v->addWidget(label("Chào mừng bạn quay lại", "pageTitle"),0,Qt::AlignHCenter);
    v->addWidget(label("Đăng nhập vào hệ thống quản lý nhà hàng", "muted"),0,Qt::AlignHCenter);
    v->addSpacing(8);

    v->addWidget(label("Mã nhà hàng"));
    auto *restaurantCode=new QLineEdit;
    restaurantCode->setPlaceholderText("Nhập mã nhà hàng");
    restaurantCode->setMinimumHeight(44);
    restaurantCode->setText(configuredRestaurantCode);
    v->addWidget(restaurantCode);
    auto *restaurantRegistration = new QPushButton("Đăng ký nhà hàng / Lấy mã nhà hàng");
    restaurantRegistration->setObjectName("soft");
    v->addWidget(restaurantRegistration);

    v->addWidget(label("Đăng nhập với vai trò"));
    auto *roleBox=new QComboBox;
    roleBox->addItem("Quản trị viên (Admin)", "Admin");
    roleBox->addItem("Nhân viên", "NhanVien");
    roleBox->setMinimumHeight(44);
    v->addWidget(roleBox);

    v->addWidget(label("Tên đăng nhập"));
    auto *u=new QLineEdit;
    u->setPlaceholderText("Nhập tên đăng nhập");
    u->setMinimumHeight(44);
    v->addWidget(u);

    v->addWidget(label("Mật khẩu"));
    auto *p=new QLineEdit;
    p->setPlaceholderText("Nhập mật khẩu");
    p->setEchoMode(QLineEdit::Password);
    p->setMinimumHeight(44);
    v->addWidget(p);

    auto *showPassword=new QPushButton("Hiện mật khẩu");
    showPassword->setObjectName("soft");
    showPassword->setCheckable(true);
    QObject::connect(showPassword, &QPushButton::toggled, p, [p, showPassword](bool visible) {
        p->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
        showPassword->setText(visible ? "Ẩn mật khẩu" : "Hiện mật khẩu");
    });
    v->addWidget(showPassword,0,Qt::AlignRight);

    auto *btn=new QPushButton("Đăng nhập");
    btn->setObjectName("primary");
    btn->setMinimumHeight(46);
    v->addWidget(btn);
    auto *status=label("", "muted");
    status->setAlignment(Qt::AlignCenter);
    v->addWidget(status);
    v->addStretch();
    v->addWidget(label("Tasty Restaurant  •  Pastel Green UI", "muted"),0,Qt::AlignHCenter);
    login.setCentralWidget(root);

    QObject::connect(restaurantRegistration, &QPushButton::clicked, &login, [&]{
        QDialog dialog(&login); dialog.setWindowTitle("Đăng ký nhà hàng"); dialog.setMinimumWidth(430); dialog.setStyleSheet(STYLE);
        auto *dv=new QVBoxLayout(&dialog); dv->setContentsMargins(26,24,26,24); dv->setSpacing(12);
        dv->addWidget(label("Đăng ký nhà hàng", "pageTitle"));
        dv->addWidget(label("Nhập thông tin nhà hàng và xác thực bằng tài khoản Admin đang hoạt động trong SQL Server trước khi cấp mã.", "muted"));
        auto *restaurantName=new QLineEdit; restaurantName->setPlaceholderText("Tên nhà hàng");
        auto *adminName=new QLineEdit; adminName->setPlaceholderText("Tên đăng nhập Admin");
        auto *adminPassword=new QLineEdit; adminPassword->setPlaceholderText("Mật khẩu Admin"); adminPassword->setEchoMode(QLineEdit::Password);
        dv->addWidget(label("Tên nhà hàng")); dv->addWidget(restaurantName);
        dv->addWidget(label("Tài khoản Admin")); dv->addWidget(adminName);
        dv->addWidget(label("Mật khẩu Admin")); dv->addWidget(adminPassword);
        auto *saveCode=new QPushButton("Xác thực Admin và cấp mã nhà hàng"); saveCode->setObjectName("primary"); dv->addWidget(saveCode);
        QObject::connect(saveCode,&QPushButton::clicked,&dialog,[&,restaurantName,adminName,adminPassword]{
            const QString rn=restaurantName->text().trimmed(); const QString an=adminName->text().trimmed();
            const QString ap=adminPassword->text();
            if(rn.isEmpty()||an.isEmpty()||ap.isEmpty()){
                QMessageBox::warning(&dialog,"Thiếu thông tin","Nhập tên nhà hàng, tài khoản Admin và mật khẩu.");return;
            }
            const TaiKhoan* adminAccount=loginService.dangNhap(an.toStdString(),ap.toStdString());
            if(!adminAccount){
                QMessageBox::warning(&dialog,"Xác thực thất bại","Không xác thực được tài khoản. Kiểm tra tên đăng nhập, mật khẩu và trạng thái tài khoản trong SQL Server.");return;
            }
            QString verifiedRole=QString::fromStdString(adminAccount->getVaiTro());
            if(verifiedRole=="Quản lý") verifiedRole="Admin";
            if(verifiedRole!="Admin"){
                QMessageBox::warning(&dialog,"Không đủ quyền","Chỉ tài khoản có vai trò Admin/Quản lý mới được đăng ký nhà hàng.");return;
            }
            const QString verifiedUsername=QString::fromStdString(adminAccount->getTenDangNhap());
            const QString code="NH"+QDateTime::currentDateTime().toString("yyMMdd")+QString::number(QRandomGenerator::global()->bounded(1000,9999));
            QSettings saveConfig(QDir(QApplication::applicationDirPath()).filePath("database.ini"),QSettings::IniFormat);
            saveConfig.beginGroup("restaurant"); saveConfig.setValue("code",code); saveConfig.setValue("name",rn); saveConfig.setValue("adminUsername",verifiedUsername); saveConfig.endGroup(); saveConfig.sync();
            if(saveConfig.status()!=QSettings::NoError){
                QMessageBox::critical(&dialog,"Không lưu được","Không ghi được mã nhà hàng vào database.ini. Kiểm tra quyền ghi thư mục ứng dụng.");return;
            }
            configuredRestaurantCode=code; configuredAdminUsername=verifiedUsername; restaurantCode->setText(code);
            adminPassword->clear();
            QMessageBox::information(&dialog,"Đã cấp mã nhà hàng",QString("Nhà hàng: %1\nMã nhà hàng: %2\nTài khoản Admin: %3\n\nHãy lưu mã này để dùng khi đăng nhập trên máy này.").arg(rn,code,verifiedUsername)); dialog.accept();
        });
        dialog.exec();
    });

    std::unique_ptr<RestaurantWindow> window;
    QObject::connect(btn,&QPushButton::clicked,&login,[&]{
        const QString enteredCode=restaurantCode->text().trimmed();
        if(enteredCode.isEmpty()){
            QMessageBox::warning(&login,"Thiếu thông tin","Vui lòng nhập mã nhà hàng.");
            restaurantCode->setFocus();
            return;
        }
        if(configuredRestaurantCode.isEmpty()){
            QMessageBox::warning(&login,"Chưa đăng ký nhà hàng","Hãy chọn 'Đăng ký nhà hàng / Lấy mã nhà hàng' để tạo mã trước khi đăng nhập.");
            return;
        }
        if(enteredCode.compare(configuredRestaurantCode,Qt::CaseInsensitive)!=0){
            QMessageBox::warning(&login,"Mã nhà hàng không đúng","Mã nhà hàng không khớp với mã đã đăng ký trên máy này.");
            restaurantCode->setFocus();
            restaurantCode->selectAll();
            return;
        }
        if(u->text().trimmed().isEmpty() || p->text().isEmpty()){
            QMessageBox::warning(&login,"Thiếu thông tin","Vui lòng nhập tên đăng nhập và mật khẩu.");
            return;
        }

        const TaiKhoan* tk=loginService.dangNhap(u->text().trimmed().toStdString(),p->text().toStdString());
        if(!tk){
            QMessageBox::warning(&login,"Đăng nhập thất bại","Tên đăng nhập hoặc mật khẩu không đúng, hoặc tài khoản không hoạt động.");
            p->clear();
            p->setFocus();
            return;
        }

        QString actualRole=QString::fromStdString(tk->getVaiTro());
        if(actualRole=="Nhân viên") actualRole="NhanVien";
        if(actualRole=="Quản lý") actualRole="Admin";
        const QString requestedRole=roleBox->currentData().toString();
        if(actualRole!=requestedRole){
            QMessageBox::warning(&login,"Sai vai trò đăng nhập",
                requestedRole=="Admin"
                    ? "Tài khoản này không có quyền Quản trị viên. Hãy chọn đúng vai trò hoặc dùng tài khoản Admin."
                    : "Tài khoản này không có quyền Nhân viên. Hãy chọn đúng vai trò đăng nhập.");
            p->clear();
            return;
        }

        const QString username=QString::fromStdString(tk->getTenDangNhap());
        if(actualRole=="Admin" && !configuredAdminUsername.isEmpty() && username.compare(configuredAdminUsername,Qt::CaseInsensitive)!=0){
            QMessageBox::warning(&login,"Tài khoản Admin không khớp","Chỉ tài khoản Admin đã khai báo khi đăng ký nhà hàng mới được vào vai trò quản trị viên.");
            p->clear();
            return;
        }
        const QString displayRole=actualRole;
        window=std::make_unique<RestaurantWindow>(username,displayRole,[&login,u,p,status]{
            u->clear();
            p->clear();
            status->setText("Bạn đã đăng xuất. Vui lòng đăng nhập lại để tiếp tục.");
            login.show();
            login.raise();
            login.activateWindow();
            u->setFocus();
        });
        window->show();
        login.hide();
        p->clear();
        status->clear();
    });
    QObject::connect(p,&QLineEdit::returnPressed,btn,&QPushButton::click);
    QObject::connect(u,&QLineEdit::returnPressed,p,[p]{p->setFocus();});

    login.show();
    const int result=app.exec();
    window.reset();
    DatabaseManager::close();
    return result;
}
