#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
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
#include <QGraphicsDropShadowEffect>
#include <QStyle>
#include <QColor>
#include <QTime>
#include <QSpinBox>
#include <QDoubleSpinBox>
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

static const char *STYLE = R"(
* { font-family: 'Segoe UI'; }
QLabel { color: #304638; }
QMainWindow, QWidget#root { background: #F5F0E4; color: #293B32; }
QFrame#sidebar { background: #254C3A; border: 1px solid #254C3A; border-radius: 18px; }
QLabel#brand { color: #F4F7EC; font-size: 21px; font-weight: 700; }
QLabel#subbrand { color: #D0E2D2; font-size: 11px; }
QPushButton#nav { text-align: left; padding: 12px 15px; border: 0; border-radius: 12px; color: #E4EFE4; font-size: 14px; background: transparent; }
QPushButton#nav:hover, QPushButton#nav[active="true"] { background: #4B8062; color: #FFFFFF; font-weight: 600; }
QFrame#panel, QFrame#stat { background: #FFFFFF; border: 1px solid #E8EEE5; border-radius: 18px; }
QLabel#pageTitle { font-size: 25px; font-weight: 700; color: #2D4938; }
QLabel#muted { color: #809187; font-size: 12px; }
QLabel#statTitle { color: #7A8B7E; font-size: 12px; }
QLabel#statValue { color: #2D513B; font-size: 25px; font-weight: 700; }
QPushButton#primary { color: white; background: #79A987; border: none; border-radius: 11px; padding: 11px 18px; font-weight: 600; }
QPushButton#primary:hover { background: #659875; }
QPushButton#soft { color: #416A4F; background: #EDF5ED; border: 1px solid #DCEADD; border-radius: 10px; padding: 9px 14px; }
QLineEdit, QComboBox, QDateEdit, QTimeEdit, QTextEdit, QSpinBox, QDoubleSpinBox { background: #FBFCF9; border: 1px solid #DDE7DC; border-radius: 10px; padding: 9px; color: #304638; selection-background-color: #BFDCC5; }
QTableWidget { background: white; border: 0; gridline-color: #EDF1EB; border-radius: 12px; selection-background-color: #E7F3E8; selection-color: #294C35; }
QHeaderView::section { background: #F0F6EF; color: #597260; border: 0; padding: 10px; font-weight: 600; }
QTableWidget::item { padding: 7px; }
QLabel#pill { background: #E5F3E7; color: #3D8050; border-radius: 9px; padding: 5px 9px; }
)";

static QLabel *label(const QString &text, const QString &id = QString()) {
    auto *l = new QLabel(text); if (!id.isEmpty()) l->setObjectName(id); return l;
}
static QFrame *panel() { auto *f = new QFrame; f->setObjectName("panel"); return f; }
static void shadow(QWidget *w) { auto *s = new QGraphicsDropShadowEffect(w); s->setBlurRadius(22); s->setOffset(0, 4); s->setColor(QColor(47, 82, 54, 14)); w->setGraphicsEffect(s); }

class RestaurantWindow : public QMainWindow {
public:
    explicit RestaurantWindow(const QString &username, const QString &role, std::function<void()> logoutCallback) : user(username), userRole(role), onLogout(std::move(logoutCallback)) {
        setWindowTitle("Tasty Restaurant | Quản lý nhà hàng"); resize(1440, 900); setMinimumSize(1120, 720); setStyleSheet(STYLE);
        auto *root = new QWidget; root->setObjectName("root"); setCentralWidget(root);
        auto *outer = new QHBoxLayout(root); outer->setContentsMargins(18,18,18,18); outer->setSpacing(18);
        auto *side = new QFrame; side->setObjectName("sidebar"); side->setFixedWidth(218); shadow(side);
        auto *sl = new QVBoxLayout(side); sl->setContentsMargins(14,20,14,16); sl->setSpacing(7);
        sl->addWidget(label("♧  Nhà Hàng", "brand")); sl->addWidget(label("      TASTY RESTAURANT", "subbrand")); sl->addSpacing(20);
        const QStringList names = {"⌂   Tổng quan", "▦   Đặt bàn", "▤   Quản lý bàn", "♨   Món ăn", "♧   Khách hàng", "♙   Nhân viên & tài khoản", "▣   Hóa đơn", "▥   Thống kê", "⚙   Tài khoản & nhà hàng", "▣   POS NHÂN VIÊN"};
        for (int i=0;i<names.size();++i) { auto *b=new QPushButton(names[i]); b->setObjectName("nav"); b->setProperty("active", i==0); b->setCursor(Qt::PointingHandCursor); sl->addWidget(b); navButtons.append(b); connect(b,&QPushButton::clicked,this,[this,i]{showPage(i);}); if(userRole!="Admin" && (i==5 || i==7 || i==8)) b->hide(); if(userRole=="Admin" && i==9) b->hide(); }
        sl->addStretch(); auto *welcome=panel(); welcome->setStyleSheet("QFrame#panel{background:#315B46;border:0;color:#F4F7EC;} QLabel{color:#F4F7EC;}"); auto *wl=new QVBoxLayout(welcome); wl->addWidget(label("❧", "brand")); wl->addWidget(label("Không gian xanh\nTrải nghiệm tuyệt vời")); sl->addWidget(welcome);
        outer->addWidget(side);
        auto *right = new QWidget; auto *rv = new QVBoxLayout(right); rv->setContentsMargins(0,0,0,0); rv->setSpacing(16);
        auto *top = new QHBoxLayout; auto *search=new QLineEdit; search->setPlaceholderText("⌕   Tìm kiếm món ăn, bàn, khách hàng..."); search->setMinimumHeight(42); top->addWidget(search,1); auto *profile=new QLabel("●   "+user+"   ·   "+(role=="Admin"?QString("Quản trị viên"):QString("Nhân viên"))); profile->setStyleSheet("background:#EAF2E8;border-radius:12px;padding:12px;color:#45664D;font-weight:600;"); top->addWidget(profile); auto *logoutButton=new QPushButton("Đăng xuất"); logoutButton->setObjectName("soft"); logoutButton->setCursor(Qt::PointingHandCursor); top->addWidget(logoutButton); connect(logoutButton,&QPushButton::clicked,this,[this]{ if(QMessageBox::question(this,"Đăng xuất","Bạn có chắc chắn muốn đăng xuất khỏi tài khoản này?",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::Yes){ hide(); if(onLogout) onLogout(); } }); rv->addLayout(top);
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
        rv->addWidget(pages,1); outer->addWidget(right,1);
    }
private:
    QString user, userRole; QVector<QPushButton*> navButtons; QStackedWidget *pages{}; std::function<void()> onLogout;
    void showPage(int index) { if(index<pages->count()) pages->setCurrentIndex(index); for(int i=0;i<navButtons.size();++i) navButtons[i]->setProperty("active",i==index); for(auto*b:navButtons){b->style()->unpolish(b);b->style()->polish(b);} }
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
        const QStringList vals={"20","28","5.320.000đ","5"};
        const QStringList icons={"♧","♙","＄","♨"};
        const QStringList colors={"#E4F2E5","#FBEBDD","#FFF3D7","#F9E5E8"};
        const QStringList details={"12 trống | 8 đã đặt","↑ 12% so với hôm qua","↑ 18% so với hôm qua","Dựa trên số lượng gọi món"};
        for(int i=0;i<4;++i){
            auto*f=new QFrame; f->setObjectName("stat"); shadow(f); auto*row=new QHBoxLayout(f);
            auto*ic=label(icons[i]); ic->setAlignment(Qt::AlignCenter); ic->setFixedSize(58,58);
            ic->setStyleSheet(QString("background:%1;border-radius:29px;color:#568564;font-size:23px;").arg(colors[i]));
            auto*col=new QVBoxLayout; col->addWidget(label(titles[i],"statTitle")); col->addWidget(label(vals[i],"statValue")); col->addWidget(label(details[i],"muted"));
            row->addWidget(ic); row->addLayout(col,1); stats->addWidget(f,0,i);
        }
        v->addLayout(stats);

        auto *content=new QHBoxLayout; content->setSpacing(12);
        auto *left=new QWidget; auto *leftCol=new QVBoxLayout(left); leftCol->setContentsMargins(0,0,0,0); leftCol->setSpacing(12);
        auto *map=panel(); shadow(map); auto*mv=new QVBoxLayout(map);
        auto*mh=new QHBoxLayout; mh->addWidget(label("▦  Sơ đồ bàn", "pageTitle")); mh->addStretch();
        mh->addWidget(label("● Trống   ● Đã đặt   ● Đang sử dụng   ● Bảo trì", "muted")); mv->addLayout(mh);
        auto *floors=new QHBoxLayout; for(const QString &floorName : {"Tầng 1","Tầng 2","Phòng VIP","Ngoài trời"}) { auto *fb=new QPushButton(floorName); fb->setObjectName("soft"); floors->addWidget(fb); } mv->addLayout(floors);
        auto*floor=new QGridLayout; floor->setSpacing(10);
        for(int i=0;i<12;++i){
            auto*b=new QPushButton(QString("B%1\n%2 người").arg(i+1,2,10,QChar('0')).arg(i%3==0?4:i%3==1?2:6)); b->setMinimumHeight(62);
            QString bg=(i==2||i==5)?"#F3C8C7":(i==7?"#F8E3AF":(i==9?"#D9D9D6":"#BFE2C5"));
            b->setStyleSheet(QString("QPushButton{background:%1;border:1px solid #B8D6BD;border-radius:13px;color:#294C35;font-weight:600;} QPushButton:hover{border:2px solid #78A985;}").arg(bg));
            floor->addWidget(b,i/4,i%4);
        }
        mv->addLayout(floor); leftCol->addWidget(map,3);

        auto *foodPanel=panel(); auto*foodV=new QVBoxLayout(foodPanel); auto*foodHead=new QHBoxLayout;
        foodHead->addWidget(label("♨  Món ăn phổ biến hôm nay", "pageTitle")); foodHead->addStretch(); foodHead->addWidget(label("Xem tất cả", "muted")); foodV->addLayout(foodHead);
        auto*foods=new QHBoxLayout; const QStringList foodNames={"Bò lúc lắc","Gà chiên mắm","Mì xào hải sản","Lẩu Thái","Cơm chiên hải sản"};
        const QStringList prices={"125.000đ","95.000đ","85.000đ","250.000đ","75.000đ"}; const QStringList emojis={"🍲","🍗","🍜","🥘","🍛"};
        for(int i=0;i<foodNames.size();++i){auto*fc=new QFrame;fc->setStyleSheet("QFrame{background:#F7FAF5;border:1px solid #E7EEE4;border-radius:13px;}");auto*fl=new QVBoxLayout(fc);auto*emoji=label(emojis[i]);emoji->setAlignment(Qt::AlignCenter);emoji->setStyleSheet("font-size:25px;background:#EAF3E7;border-radius:10px;padding:6px;");fl->addWidget(emoji);fl->addWidget(label(foodNames[i]));auto*price=label(prices[i]);price->setStyleSheet("color:#B56C65;font-weight:700;");fl->addWidget(price);foods->addWidget(fc);}
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
        connect(book,&QPushButton::clicked,this,[this,dt,tm,people,area,note]{QMessageBox::information(this,"Đặt bàn","Thông tin đã nhập trên giao diện. Chức năng lưu đặt bàn cần được nối với DatBanService và SQL Server.");});
        rightV->addWidget(quick);

        auto *customers=panel(); auto*cv=new QVBoxLayout(customers); auto*ch=new QHBoxLayout; ch->addWidget(label("♧  Khách hàng gần đây", "pageTitle")); ch->addStretch(); ch->addWidget(label("Xem tất cả","muted")); cv->addLayout(ch);
        const QStringList cn={"Nguyễn Văn A","Trần Thị B","Lê Minh C"}; const QStringList cp={"0123 456 789","0987 654 321","0369 258 147"}; const QStringList ct={"Bàn B03 · 4 người\n19:00 - 21:00","Bàn B06 · 2 người\n18:30 - 20:00","Bàn B08 · 6 người\n19:30 - 21:30"};
        for(int i=0;i<cn.size();++i){auto*r=new QHBoxLayout;auto*initial=label(QString(cn[i].at(0)));initial->setAlignment(Qt::AlignCenter);initial->setFixedSize(34,34);initial->setStyleSheet(QString("background:%1;border-radius:17px;color:#456A50;font-weight:700;").arg(QStringList({"#DCEBE0","#F1DFF0","#F5DDDC"})[i]));auto*info=new QVBoxLayout;info->addWidget(label(cn[i]));info->addWidget(label(cp[i],"muted"));auto*booking=label(ct[i]);booking->setStyleSheet("background:#EEF6ED;color:#456A50;border-radius:8px;padding:5px;");r->addWidget(initial);r->addLayout(info,1);r->addWidget(booking);cv->addLayout(r);}
        rightV->addWidget(customers);

        auto *invoices=panel(); auto*iv=new QVBoxLayout(invoices); auto*ih=new QHBoxLayout; ih->addWidget(label("▣  Hóa đơn gần đây", "pageTitle")); ih->addStretch(); ih->addWidget(label("Xem tất cả","muted")); iv->addLayout(ih);
        const QStringList invoiceRows={"HD001    Bàn B03 · 4 món    520.000đ    Đã thanh toán","HD002    Bàn B06 · 3 món    385.000đ    Đang xử lý","HD003    Bàn B08 · 5 món    750.000đ    Chưa thanh toán"};
        for(int i=0;i<invoiceRows.size();++i){auto*il=label(invoiceRows[i]);il->setStyleSheet(QString("padding:6px;color:%1;").arg(i==0?"#477B55":(i==1?"#B77B35":"#B75D5D")));iv->addWidget(il);}
        rightV->addWidget(invoices); rightV->addStretch(); content->addWidget(rightCol,4); v->addLayout(content,1); return w;
    }
    QWidget *makeBookingPage(){auto*w=new QWidget;auto*v=new QVBoxLayout(w);v->addWidget(label("Quản lý đặt bàn", "pageTitle"));v->addWidget(label("Theo dõi và tạo yêu cầu đặt bàn", "muted"));auto*f=panel();auto*l=new QVBoxLayout(f);auto*form=new QGridLayout;auto*name=new QLineEdit;name->setPlaceholderText("Tên khách hàng");auto*phone=new QLineEdit;phone->setPlaceholderText("Số điện thoại");auto*date=new QDateEdit(QDate::currentDate());date->setCalendarPopup(true);auto*time=new QTimeEdit(QTime(19,0));auto*count=new QComboBox;count->addItems({"2 người","4 người","6 người","8 người"});auto*area=new QComboBox;area->addItems({"Tầng 1","Tầng 2","Phòng VIP","Ngoài trời"});form->addWidget(label("Khách hàng"),0,0);form->addWidget(name,1,0);form->addWidget(label("Điện thoại"),0,1);form->addWidget(phone,1,1);form->addWidget(label("Ngày đặt"),2,0);form->addWidget(date,3,0);form->addWidget(label("Giờ đến"),2,1);form->addWidget(time,3,1);form->addWidget(label("Số người"),4,0);form->addWidget(count,5,0);form->addWidget(label("Khu vực"),4,1);form->addWidget(area,5,1);l->addLayout(form);auto*btn=new QPushButton("＋  Tạo đặt bàn");btn->setObjectName("primary");l->addWidget(btn,0,Qt::AlignLeft);connect(btn,&QPushButton::clicked,this,[this]{QMessageBox::information(this,"Đặt bàn","Biểu mẫu giao diện đã sẵn sàng. Cần nối với DatBanService để lưu dữ liệu thật.");});v->addWidget(f);v->addStretch();return w;}
    QWidget *makeTablesPage(){auto*w=new QWidget;auto*v=new QVBoxLayout(w);v->addWidget(label("Quản lý bàn ăn", "pageTitle"));v->addWidget(label("Theo dõi trạng thái bàn theo khu vực", "muted"));auto*f=panel();auto*l=new QVBoxLayout(f);auto*filters=new QHBoxLayout;auto*search=new QLineEdit;search->setPlaceholderText("Tìm mã bàn...");auto*area=new QComboBox;area->addItems({"Tất cả khu vực","Tầng 1","Tầng 2","Phòng VIP","Ngoài trời"});auto*add=new QPushButton("＋  Thêm bàn");add->setObjectName("primary");filters->addWidget(search,1);filters->addWidget(area);filters->addWidget(add);l->addLayout(filters);auto*t=new QTableWidget(8,4);t->setHorizontalHeaderLabels({"Mã bàn","Khu vực","Số chỗ","Trạng thái"});t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);for(int i=0;i<8;++i){t->setItem(i,0,new QTableWidgetItem(QString("B%1").arg(i+1,2,10,QChar('0'))));t->setItem(i,1,new QTableWidgetItem(i<4?"Tầng 1":"Tầng 2"));t->setItem(i,2,new QTableWidgetItem(i%3==0?"4":"2"));t->setItem(i,3,new QTableWidgetItem(i==2||i==5?"Đã đặt":"Trống"));}t->verticalHeader()->setVisible(false);l->addWidget(t);connect(search,&QLineEdit::textChanged,this,[t](const QString&s){for(int r=0;r<t->rowCount();++r)t->setRowHidden(r,!t->item(r,0)->text().contains(s,Qt::CaseInsensitive));});connect(add,&QPushButton::clicked,this,[this]{QMessageBox::information(this,"Thêm bàn","Cần kết nối nút này với BanService để ghi vào cơ sở dữ liệu.");});v->addWidget(f);return w;}
    QWidget *makeEmployeePage() {
        auto *w = new QWidget;
        auto *root = new QVBoxLayout(w);
        root->setContentsMargins(0,0,0,0);
        root->setSpacing(12);
        root->addWidget(label("Nhân viên & tài khoản", "pageTitle"));
        root->addWidget(label("Admin tạo tài khoản, cấp mật khẩu ban đầu, cập nhật trạng thái hoặc xóa tài khoản không còn sử dụng. Dữ liệu được lưu trực tiếp vào SQL Server.", "muted"));

        auto *formPanel = panel();
        auto *formLayout = new QVBoxLayout(formPanel);
        formLayout->addWidget(label("＋  Tạo nhân viên mới", "pageTitle"));
        auto *form = new QGridLayout;
        auto *fullName = new QLineEdit; fullName->setPlaceholderText("Họ và tên nhân viên");
        auto *username = new QLineEdit; username->setPlaceholderText("Tên đăng nhập duy nhất");
        auto *password = new QLineEdit; password->setPlaceholderText("Mật khẩu ban đầu (ít nhất 10 ký tự)"); password->setEchoMode(QLineEdit::Password);
        auto *phone = new QLineEdit; phone->setPlaceholderText("Số điện thoại");
        auto *email = new QLineEdit; email->setPlaceholderText("Email");
        auto *gender = new QComboBox; gender->addItems({"Chưa cập nhật", "Nam", "Nữ", "Khác"});
        auto *birthDate = new QDateEdit(QDate(2000,1,1)); birthDate->setCalendarPopup(true); birthDate->setDisplayFormat("dd/MM/yyyy"); birthDate->setSpecialValueText("Chưa cập nhật"); birthDate->setMinimumDate(QDate(1900,1,1));
        auto *position = new QComboBox; position->addItems({"Nhân viên phục vụ", "Thu ngân", "Pha chế", "Bếp", "Quản lý ca"});
        auto *salary = new QDoubleSpinBox; salary->setRange(0,1000000000); salary->setDecimals(0); salary->setSuffix(" đ/tháng"); salary->setSingleStep(500000);
        form->addWidget(label("Họ và tên"),0,0); form->addWidget(fullName,1,0);
        form->addWidget(label("Tên đăng nhập"),0,1); form->addWidget(username,1,1);
        form->addWidget(label("Mật khẩu ban đầu"),0,2); form->addWidget(password,1,2);
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
        toolbar->addWidget(filter,1); toolbar->addWidget(refresh); toolbar->addWidget(toggle); toolbar->addWidget(remove); listLayout->addLayout(toolbar);
        auto *table = new QTableWidget(0,8); table->setHorizontalHeaderLabels({"Mã NV","Họ tên","Tên đăng nhập","Điện thoại","Email","Chức vụ","Vai trò","Trạng thái"}); table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); table->verticalHeader()->setVisible(false); table->setSelectionBehavior(QAbstractItemView::SelectRows); table->setSelectionMode(QAbstractItemView::SingleSelection); table->setEditTriggers(QAbstractItemView::NoEditTriggers); listLayout->addWidget(table,1); root->addWidget(listPanel,1);

        auto loadEmployees = [this,table]() {
            table->setRowCount(0);
            if (!DatabaseManager::connect()) { QMessageBox::critical(this,"SQL Server","Không kết nối được cơ sở dữ liệu."); return; }
            QSqlQuery q(DatabaseManager::database());
            if (!q.exec("SELECT nv.MaNV,nv.HoTen,tk.MaTK,tk.TenDangNhap,ISNULL(nv.SoDienThoai,''),ISNULL(nv.Email,''),ISNULL(nv.ChucVu,''),tk.VaiTro,tk.TrangThai FROM dbo.TaiKhoan tk INNER JOIN dbo.NhanVien nv ON nv.MaNV=tk.MaNV WHERE tk.VaiTro IN (N'Nhân viên',N'NhanVien') ORDER BY nv.HoTen")) { QMessageBox::critical(this,"Không tải được nhân viên",q.lastError().text()); return; }
            while(q.next()) {
                int r=table->rowCount(); table->insertRow(r);
                const QList<QString> values={q.value(0).toString(),q.value(1).toString(),q.value(3).toString(),q.value(4).toString(),q.value(5).toString(),q.value(6).toString(),q.value(7).toString(),q.value(8).toString()};
                for(int c=0;c<values.size();++c) { auto *it=new QTableWidgetItem(values[c]); if(c==0) it->setData(Qt::UserRole,q.value(2).toString()); table->setItem(r,c,it); }
            }
        };
        connect(refresh,&QPushButton::clicked,this,loadEmployees);
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

    QWidget *makeGenericPage(const QString &name){auto*w=new QWidget;auto*v=new QVBoxLayout(w);v->addWidget(label(name,"pageTitle"));v->addWidget(label("Giao diện đồng bộ màu xanh lá pastel. Phần dữ liệu sẽ được nối với service tương ứng.","muted"));auto*f=panel();auto*l=new QVBoxLayout(f);auto*search=new QLineEdit;search->setPlaceholderText("Tìm kiếm...");l->addWidget(search);auto*t=new QTableWidget(0,3);t->setHorizontalHeaderLabels({"Mã","Thông tin","Trạng thái"});t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);t->verticalHeader()->setVisible(false);l->addWidget(t);v->addWidget(f,1);return w;}
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
