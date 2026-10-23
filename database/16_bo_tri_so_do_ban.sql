USE RestaurantManagement;
GO
IF OBJECT_ID(N'dbo.BoTriBan', N'U') IS NULL
BEGIN
    CREATE TABLE dbo.BoTriBan
    (
        MaBan VARCHAR(10) NOT NULL PRIMARY KEY,
        Cot INT NOT NULL CONSTRAINT DF_BoTriBan_Cot DEFAULT 0,
        Hang INT NOT NULL CONSTRAINT DF_BoTriBan_Hang DEFAULT 0,
        ThuTu INT NOT NULL CONSTRAINT DF_BoTriBan_ThuTu DEFAULT 0,
        CONSTRAINT FK_BoTriBan_Ban FOREIGN KEY (MaBan) REFERENCES dbo.Ban(MaBan) ON DELETE CASCADE,
        CONSTRAINT CK_BoTriBan_Cot CHECK (Cot BETWEEN 0 AND 19),
        CONSTRAINT CK_BoTriBan_Hang CHECK (Hang BETWEEN 0 AND 19)
    );
END;
GO
INSERT dbo.BoTriBan(MaBan,Cot,Hang,ThuTu)
SELECT b.MaBan,
       (ROW_NUMBER() OVER(PARTITION BY b.MaKhuVuc ORDER BY b.MaBan)-1)%4,
       ((ROW_NUMBER() OVER(PARTITION BY b.MaKhuVuc ORDER BY b.MaBan)-1)/4),
       ROW_NUMBER() OVER(PARTITION BY b.MaKhuVuc ORDER BY b.MaBan)
FROM dbo.Ban b
WHERE NOT EXISTS (SELECT 1 FROM dbo.BoTriBan p WHERE p.MaBan=b.MaBan);
GO
PRINT N'Đã tạo cấu hình bố trí sơ đồ bàn.';
GO
