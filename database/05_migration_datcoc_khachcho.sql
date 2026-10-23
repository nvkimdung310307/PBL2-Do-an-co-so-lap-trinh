/* ============================================================
   05_migration_datcoc_khachcho.sql
   Bo sung bang/procedure cho DatCocService va KhachChoService.
   An toan chay lai: khong xoa bang, khong xoa du lieu.
   Chay trong SQL Server Management Studio tren database RestaurantManagement.
   ============================================================ */
USE RestaurantManagement;
GO

IF OBJECT_ID(N'dbo.DatCoc', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.DatCoc
    (
        MaDatCoc VARCHAR(10) NOT NULL CONSTRAINT PK_DatCoc PRIMARY KEY,
        MaDatBan VARCHAR(10) NOT NULL,
        TienCoc DECIMAL(18,2) NOT NULL,
        NgayDatCoc DATE NOT NULL CONSTRAINT DF_DatCoc_Ngay DEFAULT (CONVERT(date, GETDATE())),
        TrangThai VARCHAR(20) NOT NULL CONSTRAINT DF_DatCoc_TrangThai DEFAULT ('DaCoc'),
        CONSTRAINT CK_DatCoc_TienCoc CHECK (TienCoc > 0),
        CONSTRAINT CK_DatCoc_TrangThai CHECK (TrangThai IN ('DaCoc','DaHoan','DaTru')),
        CONSTRAINT FK_DatCoc_DatBan FOREIGN KEY (MaDatBan) REFERENCES dbo.DatBan(MaDatBan)
    );
    PRINT N'Da tao bang DatCoc.';
END
ELSE PRINT N'Bang DatCoc da ton tai; bo qua.';
GO

IF OBJECT_ID(N'dbo.KhachCho', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.KhachCho
    (
        MaKhachCho VARCHAR(10) NOT NULL CONSTRAINT PK_KhachCho PRIMARY KEY,
        TenKhach NVARCHAR(100) NOT NULL,
        SoDienThoai VARCHAR(15) NOT NULL,
        SoLuongKhach INT NOT NULL,
        KhuVucMongMuon NVARCHAR(100) NULL,
        TrangThai VARCHAR(20) NOT NULL CONSTRAINT DF_KhachCho_TrangThai DEFAULT ('DangCho'),
        ThoiDiemTao DATETIME2 NOT NULL CONSTRAINT DF_KhachCho_ThoiDiem DEFAULT (SYSDATETIME()),
        CONSTRAINT CK_KhachCho_SoLuong CHECK (SoLuongKhach > 0),
        CONSTRAINT CK_KhachCho_TrangThai CHECK (TrangThai IN ('DangCho','DaGoi','DaHuy'))
    );
    PRINT N'Da tao bang KhachCho.';
END
ELSE PRINT N'Bang KhachCho da ton tai; bo qua.';
GO

-- sp_ThemDatCoc duoc dinh nghia duy nhat trong 08_priority2_hoadon_datcoc.sql
-- Chay script 08 sau script nay de cap nhat phien ban co transaction va khoa dong.
GO

/*
   Phan bo tien coc cho duy nhat hoa don CHUA THANH TOAN som nhat
   co mon an cua mot lich dat. Cac hoa don tach/gop khac khong duoc tru lap tien coc.
   Neu tien coc vuot so tien phai tra cua hoa don duoc ap dung, rollback de tranh mat tien coc.
*/
CREATE OR ALTER PROCEDURE dbo.sp_LamMoiTienHoaDonTheoDatBan
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    IF NOT EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaDatBan=@MaDatBan)
        THROW 50230, N'Không tìm thấy lịch đặt bàn khi tính lại tiền hóa đơn.', 1;

    DECLARE @MaHDNhanCoc VARCHAR(10), @TienCoc DECIMAL(18,2)=0,
            @TongTienNhanCoc DECIMAL(18,2)=0, @TienGiamNhanCoc DECIMAL(18,2)=0;

    SELECT @TienCoc=COALESCE(SUM(TienCoc),0)
    FROM dbo.DatCoc WITH (UPDLOCK,HOLDLOCK)
    WHERE MaDatBan=@MaDatBan AND TrangThai='DaCoc';

    SELECT TOP (1) @MaHDNhanCoc=h.MaHD
    FROM dbo.HoaDon h WITH (UPDLOCK,HOLDLOCK)
    WHERE h.MaDatBan=@MaDatBan
      AND h.TrangThai=N'Chưa thanh toán'
      AND EXISTS (SELECT 1 FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD)
    ORDER BY h.NgayLap, h.MaHD;

    IF @MaHDNhanCoc IS NOT NULL
    BEGIN
        SELECT @TongTienNhanCoc=COALESCE(SUM(CONVERT(DECIMAL(18,2),SoLuong*DonGia)),0)
        FROM dbo.ChiTietHoaDon WHERE MaHD=@MaHDNhanCoc;
        SELECT @TienGiamNhanCoc=TienGiam FROM dbo.HoaDon WHERE MaHD=@MaHDNhanCoc;

        IF @TienGiamNhanCoc IS NULL OR @TienGiamNhanCoc<0 OR @TienGiamNhanCoc>@TongTienNhanCoc
            THROW 50231, N'Tiền giảm của hóa đơn không hợp lệ; không thể áp dụng tiền cọc.', 1;
        IF @TienCoc>@TongTienNhanCoc-@TienGiamNhanCoc
            THROW 50232, N'Tiền cọc lớn hơn số tiền phải trả của hóa đơn đầu tiên. Hãy điều chỉnh/hoàn cọc hoặc gộp hóa đơn trước khi thanh toán.', 1;
    END

    IF EXISTS (
        SELECT 1
        FROM dbo.HoaDon h
        OUTER APPLY (
            SELECT COALESCE(SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)),0) AS TongTien
            FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD
        ) t
        WHERE h.MaDatBan=@MaDatBan
          AND h.TrangThai=N'Chưa thanh toán'
          AND (h.TienGiam<0 OR h.TienGiam>t.TongTien)
    )
        THROW 50233, N'Tiền giảm không hợp lệ ở ít nhất một hóa đơn chưa thanh toán.', 1;

    UPDATE h
       SET h.TongTien=t.TongTien,
           h.ThanhTien=t.TongTien-h.TienGiam-
               CASE WHEN h.MaHD=@MaHDNhanCoc THEN @TienCoc ELSE 0 END
    FROM dbo.HoaDon h
    OUTER APPLY (
        SELECT COALESCE(SUM(CONVERT(DECIMAL(18,2),ct.SoLuong*ct.DonGia)),0) AS TongTien
        FROM dbo.ChiTietHoaDon ct WHERE ct.MaHD=h.MaHD
    ) t
    WHERE h.MaDatBan=@MaDatBan AND h.TrangThai=N'Chưa thanh toán';
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_HoanDatCoc @MaDatCoc VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaDatBan VARCHAR(10);
        SELECT @MaDatBan=MaDatBan
        FROM dbo.DatCoc WITH (UPDLOCK,HOLDLOCK)
        WHERE MaDatCoc=@MaDatCoc AND TrangThai='DaCoc';
        IF @MaDatBan IS NULL
            THROW 50006, N'Không tìm thấy tiền cọc đang ở trạng thái DaCoc.', 1;

        /* Hoan coc chi duoc phep khi lich da huy/khong den; khong hoan coc cho lich dang dien ra. */
        IF NOT EXISTS (SELECT 1 FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK)
                       WHERE MaDatBan=@MaDatBan AND TrangThai IN (N'Đã hủy',N'Không đến'))
            THROW 50009, N'Chỉ được hoàn cọc khi lịch đặt đã hủy hoặc khách không đến.', 1;

        /* Du lieu cu co the da thanh toan hoa don nhung chua cap nhat trang thai coc.
           Chan hoan tien trong truong hop nay de tranh hoan coc hai lan. */
        IF EXISTS (
            SELECT 1
            FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK)
            WHERE MaDatBan=@MaDatBan AND TrangThai=N'Đã thanh toán'
        )
            THROW 50011, N'Lịch đặt đã có hóa đơn thanh toán; cần đối soát hóa đơn và tiền cọc trước khi hoàn.', 1;

        UPDATE dbo.DatCoc SET TrangThai='DaHoan'
        WHERE MaDatCoc=@MaDatCoc AND TrangThai='DaCoc';
        IF @@ROWCOUNT<>1 THROW 50010, N'Không thể cập nhật trạng thái hoàn cọc.', 1;
        EXEC dbo.sp_LamMoiTienHoaDonTheoDatBan @MaDatBan=@MaDatBan;
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT>0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_CapNhatTienCoc @MaDatCoc VARCHAR(10), @TienMoi DECIMAL(18,2)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        IF @TienMoi IS NULL OR @TienMoi<=0 THROW 50007, N'Tiền cọc phải lớn hơn 0.', 1;
        DECLARE @MaDatBan VARCHAR(10);
        SELECT @MaDatBan=MaDatBan
        FROM dbo.DatCoc WITH (UPDLOCK,HOLDLOCK)
        WHERE MaDatCoc=@MaDatCoc AND TrangThai='DaCoc';
        IF @MaDatBan IS NULL THROW 50008, N'Không tìm thấy tiền cọc hoặc tiền cọc không còn hiệu lực.', 1;
        UPDATE dbo.DatCoc SET TienCoc=@TienMoi WHERE MaDatCoc=@MaDatCoc AND TrangThai='DaCoc';
        IF @@ROWCOUNT<>1 THROW 50012, N'Không thể cập nhật tiền cọc.', 1;
        EXEC dbo.sp_LamMoiTienHoaDonTheoDatBan @MaDatBan=@MaDatBan;
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT>0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DanhSachDatCoc
AS
BEGIN
    SET NOCOUNT ON;
    SELECT dc.MaDatCoc, dc.MaDatBan, dc.TienCoc, dc.NgayDatCoc, dc.TrangThai
    FROM dbo.DatCoc dc ORDER BY dc.NgayDatCoc DESC, dc.MaDatCoc;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_ThemKhachCho
    @MaKhachCho VARCHAR(10), @TenKhach NVARCHAR(100), @SoDienThoai VARCHAR(15),
    @SoLuongKhach INT, @KhuVucMongMuon NVARCHAR(100)=NULL
AS
BEGIN
    SET NOCOUNT ON;
    IF NULLIF(LTRIM(RTRIM(@MaKhachCho)), '') IS NULL THROW 50011, N'Mã khách chờ không được trống.', 1;
    IF NULLIF(LTRIM(RTRIM(@TenKhach)), N'') IS NULL THROW 50012, N'Tên khách không được trống.', 1;
    IF NULLIF(LTRIM(RTRIM(@SoDienThoai)), '') IS NULL THROW 50013, N'Số điện thoại không được trống.', 1;
    IF @SoLuongKhach IS NULL OR @SoLuongKhach<=0 THROW 50014, N'Số lượng khách phải lớn hơn 0.', 1;
    IF EXISTS(SELECT 1 FROM dbo.KhachCho WHERE MaKhachCho=@MaKhachCho) THROW 50015, N'Mã khách chờ đã tồn tại.', 1;
    INSERT dbo.KhachCho(MaKhachCho,TenKhach,SoDienThoai,SoLuongKhach,KhuVucMongMuon,TrangThai)
    VALUES(@MaKhachCho,@TenKhach,@SoDienThoai,@SoLuongKhach,@KhuVucMongMuon,'DangCho');
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DanhSachKhachCho @ChiDangCho BIT=0
AS
BEGIN
    SET NOCOUNT ON;
    SELECT MaKhachCho,TenKhach,SoDienThoai,SoLuongKhach,KhuVucMongMuon,TrangThai,ThoiDiemTao
    FROM dbo.KhachCho
    WHERE @ChiDangCho=0 OR TrangThai='DangCho'
    ORDER BY CASE WHEN TrangThai='DangCho' THEN 0 ELSE 1 END, ThoiDiemTao, MaKhachCho;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_GoiKhachCho @MaKhachCho VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    UPDATE dbo.KhachCho SET TrangThai='DaGoi' WHERE MaKhachCho=@MaKhachCho AND TrangThai='DangCho';
    IF @@ROWCOUNT=0 THROW 50016, N'Không tìm thấy khách đang chờ.', 1;
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_HuyKhachCho @MaKhachCho VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    UPDATE dbo.KhachCho SET TrangThai='DaHuy' WHERE MaKhachCho=@MaKhachCho AND TrangThai='DangCho';
    IF @@ROWCOUNT=0 THROW 50017, N'Không tìm thấy khách đang chờ để hủy.', 1;
END;
GO

PRINT N'Da cap nhat bang va procedure DatCoc/KhachCho. Khong co du lieu nao bi xoa.';
GO
