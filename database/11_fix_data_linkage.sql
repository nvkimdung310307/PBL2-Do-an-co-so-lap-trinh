/* Fixes for persistent table merge/split tracking and payment method consistency.
   Run after 02_create_tables.sql, 05_migration_datcoc_khachcho.sql and 07/08/09 scripts.
   Safe to rerun: tables are created only when absent. */
USE RestaurantManagement;
GO
IF OBJECT_ID(N'dbo.NhomBan', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.NhomBan (
        MaNhomBan VARCHAR(36) NOT NULL CONSTRAINT PK_NhomBan PRIMARY KEY,
        TrangThai VARCHAR(20) NOT NULL CONSTRAINT DF_NhomBan_TrangThai DEFAULT ('DangGop'),
        NgayTao DATETIME2 NOT NULL CONSTRAINT DF_NhomBan_NgayTao DEFAULT (SYSDATETIME()),
        NgayTach DATETIME2 NULL,
        CONSTRAINT CK_NhomBan_TrangThai CHECK (TrangThai IN ('DangGop','DaTach'))
    );
END;
GO
IF OBJECT_ID(N'dbo.ChiTietNhomBan', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.ChiTietNhomBan (
        MaNhomBan VARCHAR(36) NOT NULL,
        MaBan VARCHAR(10) NOT NULL,
        CONSTRAINT PK_ChiTietNhomBan PRIMARY KEY (MaNhomBan, MaBan),
        CONSTRAINT FK_ChiTietNhomBan_NhomBan FOREIGN KEY (MaNhomBan) REFERENCES dbo.NhomBan(MaNhomBan),
        CONSTRAINT FK_ChiTietNhomBan_Ban FOREIGN KEY (MaBan) REFERENCES dbo.Ban(MaBan)
    );
END;
GO
IF OBJECT_ID(N'dbo.ChiTietNhomBan', N'U') IS NOT NULL
   AND NOT EXISTS (SELECT 1 FROM sys.indexes WHERE object_id=OBJECT_ID(N'dbo.ChiTietNhomBan') AND name=N'UX_ChiTietNhomBan_MaBan')
BEGIN
    CREATE UNIQUE INDEX UX_ChiTietNhomBan_MaBan ON dbo.ChiTietNhomBan(MaBan);
END;
GO
/* Mark group as split only when both named tables belong to the same active group. */
CREATE OR ALTER PROCEDURE dbo.sp_TachNhomBan
    @MaBanGoc VARCHAR(10), @MaBanTach VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON; SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaNhomBan VARCHAR(36);
        SELECT TOP (1) @MaNhomBan=ct1.MaNhomBan
        FROM dbo.ChiTietNhomBan ct1 WITH (UPDLOCK,HOLDLOCK)
        JOIN dbo.NhomBan n WITH (UPDLOCK,HOLDLOCK) ON n.MaNhomBan=ct1.MaNhomBan AND n.TrangThai='DangGop'
        JOIN dbo.ChiTietNhomBan ct2 ON ct2.MaNhomBan=ct1.MaNhomBan
        WHERE ct1.MaBan=@MaBanGoc AND ct2.MaBan=@MaBanTach AND @MaBanGoc<>@MaBanTach;
        IF @MaNhomBan IS NULL THROW 51101, N'Hai bàn không thuộc cùng một nhóm bàn đang gộp.', 1;
        DELETE dbo.ChiTietNhomBan WHERE MaNhomBan=@MaNhomBan AND MaBan=@MaBanTach;
        IF (SELECT COUNT(*) FROM dbo.ChiTietNhomBan WHERE MaNhomBan=@MaNhomBan) < 2
        BEGIN
            DELETE dbo.ChiTietNhomBan WHERE MaNhomBan=@MaNhomBan;
            UPDATE dbo.NhomBan SET TrangThai='DaTach', NgayTach=SYSDATETIME() WHERE MaNhomBan=@MaNhomBan;
        END;
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE()<>0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO
PRINT N'Da tao bang theo doi nhom ban de luu thao tac gop/tach.';
GO
