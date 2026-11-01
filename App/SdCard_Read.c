// Khai báo giao diện thao tác thẻ SD.
#include "SdCard_Read.h"
#include "AppTypes.h"
#ifdef DISKO_H
FATFS fs; // Đối tượng hệ thống tệp.
FIL file; // Đối tượng tệp đang thao tác.
// FILlist_LicensePlate fno;
FRESULT fresult; // Kết quả thao tác FatFs.
UINT br, bw;	 // Số byte đã đọc/ghi.

// Biến dùng để tính dung lượng thẻ SD.
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;
static int buffSize(char *pbuff);
static RfidUserDtypeCfg_s *read_data(char *path);
static void check_free_space();
static void SDcard_write();
static void SDcard_read();
static void SDcard_updateFile();

#endif

// Số lượng bản ghi đã nạp từ tệp dữ liệu.
int count = 0;
// Danh sách dữ liệu người dùng được nạp vào RAM.
RfidUserDtypeCfg_s *list_RfidUserData = NULL;
// Đường dẫn tệp CSV chứa dữ liệu người dùng.
#define FILE_SRC ".//Bsw//database//db.csv"

// Các hàm đọc, ghi và phân tích dữ liệu CSV.
static Status Load_UserDB(const char *path);
static Status WriteFile(const char *path);
static RfidUserDtypeCfg_s *parse_line(char *line);

// Khởi tạo thẻ SD và nạp cơ sở dữ liệu người dùng.
void SDcard_ModuleInit()
{
#ifdef DISKO_H
	// Khởi tạo FatFs và gắn phân vùng gốc.
	MX_FATFS_Init();
	fresult = f_mount(&fs, "/", 1);
	while (fresult != FR_OK)
		; // Chờ đến khi gắn thẻ nhớ thành công.

	// Kiểm tra dung lượng trống trước khi thao tác tệp.
	check_free_space();

	// Ghi dữ liệu mẫu lên thẻ SD.
	SDcard_write();
#endif

	// Nạp dữ liệu người dùng và chuyển sang trạng thái khởi tạo thành công.
	if (Load_UserDB(FILE_SRC) == E_OK)
	{
		ModeStatusCfg.apptype = INIT_MODE;
	}
	else
	{
		// Giữ nguyên trạng thái hiện tại nếu không nạp được dữ liệu.
		return;
	}
}

// Đọc CSV, phân tích từng dòng và lưu danh sách người dùng vào RAM.
static Status Load_UserDB(const char *path)
{
	// Chuẩn bị tệp dữ liệu trước khi mở để đọc.
	if (WriteFile(path) == E_OK)
	{
		// Mở tệp CSV ở chế độ chỉ đọc.
		FILE *file = fopen(path, "r");
		if (file == NULL)
		{
			return NOT_OK;
		}
		char line[100];
		// Bỏ qua dòng tiêu đề của tệp CSV.
		fgets(line, sizeof(line), file);
		// Cấp phát vùng nhớ ban đầu cho một bản ghi.
		list_RfidUserData = (RfidUserDtypeCfg_s *)malloc(sizeof(RfidUserDtypeCfg_s));

		// Dừng nếu không cấp phát được bộ nhớ.
		if (list_RfidUserData == NULL)
		{
			return NOT_OK;
		}

		// Đọc và phân tích lần lượt từng dòng dữ liệu.
		while (fgets(line, sizeof(line), file) != NULL)
		{
			// Tách dữ liệu của dòng hiện tại thành một bản ghi tạm.
			RfidUserDtypeCfg_s *temp = parse_line(line);
			// Tăng số lượng bản ghi đã đọc.
			count++;
			// Mở rộng vùng nhớ để chứa bản ghi mới.
			list_RfidUserData = (RfidUserDtypeCfg_s *)realloc(list_RfidUserData, sizeof(RfidUserDtypeCfg_s) * count);
			// Chép bản ghi tạm vào danh sách chính.
			list_RfidUserData[count - 1] = *temp;
			// Giải phóng vùng nhớ cấu trúc tạm sau khi chép.
			free(temp);
		}
		// Đóng tệp sau khi đã đọc xong.
		fclose(file);
		return E_OK;
	}
	// Báo lỗi nếu không thể chuẩn bị tệp dữ liệu.
	return NOT_OK;
}

// Tạo tệp CSV chứa dữ liệu người dùng mẫu.
static Status WriteFile(const char *path)
{
	// Mở tệp ở chế độ ghi mới, thay thế nội dung cũ nếu có.
	FILE *file = fopen(path, "w");
	if (file == NULL)
	{
		return NOT_OK;
	}
	fprintf(file, "ten,tuoi,so dien thoai,dia chi\n");
	fprintf(file, "Pham Cao Duy,25,0972665872,91 Pham Van Hai P3 Q Tan Binh\n");
	fprintf(file, "Trinh Tran Phuong Tuan,24,0908234588,TP Tay Ninh \n");
	fprintf(file, "Trinh Le Hoang,28,0376572677,12 Nguyen Xien Q12\n");
	fprintf(file, "Nguyen Tan Tung,21,038764589,45 Tran Binh Trong\n");
	fprintf(file, "Le Quang Nhat,26,0978278121,17 Binh Phuoc\n");
	fprintf(file, "Nguyen Huu Hung,22,0978565342,17 Q Binh Thanh\n");
	fprintf(file, "Pham Cao Duy,18,090395678,220/8 Nguyen Phuc Nguuyen P9 Q3\n");
	fprintf(file, "Dinh Anh Tuan,24,0903478211,TP Tay Ninh\n");
	fprintf(file, "Nguyen Ho Duy,17,0906733209,22/5 Binh Thuan\n");
	fprintf(file, "Pham Cao Duy,20,0376572231,18 Nguyen Thi Minh Khai Q1 TPHCM\n");
	fprintf(file, "Nguyen Thi Thanh Thuy,27,038764987,21 Nguyen Thien Thuat P12 QTan Phu\n");
	fprintf(file, "Nguyen Ho Duy,27,038764912,22 Nguyen Thien Thuat P13 Q Tan Binh\n");
	fprintf(file, "Nguyen Thi Thanh Thu,27,038764997,23 Nguyen Thien Thuat P14  Q Binh Thanh\n");
	fprintf(file, "Pham Cao Duy,27,038764910,24 Nguyen Thien Thuat P9 Q 10\n");
	fclose(file);
	return E_OK;
}

// Tách tên và biển số từ một dòng CSV thành bản ghi người dùng.
static RfidUserDtypeCfg_s *parse_line(char *line)
{
	// Cấp phát cấu trúc tạm cho bản ghi đang phân tích.
	RfidUserDtypeCfg_s *temp = (RfidUserDtypeCfg_s *)malloc(sizeof(RfidUserDtypeCfg_s));
	char *token; // Con trỏ giữ trường CSV vừa tách.

	// Tách trường đầu tiên làm tên người dùng.
	token = strtok(line, ","); // Tách trường đến dấu phẩy đầu tiên.
	temp->name = (char *)malloc(strlen(token) + 1);
	strcpy(temp->name, token); // Lưu tên vào vùng nhớ riêng.

	// Tách trường kế tiếp làm biển số xe.
	token = strtok(NULL, ",");
	temp->license_plate = (char *)malloc(strlen(token) + 1);
	strcpy(temp->license_plate, token); // Lưu biển số vào vùng nhớ riêng.

	return temp; // Trả về bản ghi vừa phân tích.
}

#ifdef DISKO_H

// Tính độ dài chuỗi bao gồm ký tự kết thúc.
static int buffSize(char *pbuff)
{
	int size = 0;
	while (pbuff[size++] != '\0')
		;
	return size;
}

// Đọc tổng dung lượng và dung lượng còn trống của thẻ.
static void check_free_space()
{
	f_getfree("", &fre_clust, &pfs);
	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	sprintf(buffer, "SD CARD Total Size: \t%d\n", total);
	memset(buffer, 0, sizeof(buffer));
	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
	memset(buffer, 0, sizeof(buffer));
}

// Tạo tệp thử nghiệm và ghi nội dung mẫu vào thẻ.
static void SDcard_write()
{
	/* Mở hoặc tạo tệp thứ hai ở chế độ ghi. */
	fresult = f_open(&file, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);

	/* Ghi nội dung mẫu vào tệp. */
	strcpy(buffer, "This is File2.txt, written using ...f_write... and it says Hello from Controllerstech\n");

	fresult = f_write(&file, buffer, buffSize(buffer), &bw);

	/* Đóng tệp sau khi ghi. */
	f_close(&file);

	memset(buffer, 0, sizeof(buffer));
}

// Đọc nội dung tệp thử nghiệm từ thẻ.
static void SDcard_read()
{
	fresult = f_open(&file, "file2.txt", FA_READ);
	if (fresult == FR_OK)
		f_read(&file, buffer, f_size(&file), &br);
	/* Đóng tệp sau khi đọc. */
	f_close(&file);
	memset(buffer, 0, sizeof(buffer));
}

// Ghi nối dữ liệu vào cuối tệp thử nghiệm rồi đọc lại nội dung.
static void SDcard_updateFile()
{
	/* Mở tệp hiện có ở chế độ đọc và ghi. */
	fresult = f_open(&file, "filee2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);

	/* Di chuyển vị trí ghi đến cuối tệp. */
	fresult = f_lseek(&file, f_size(&file));

	if (fresult == FR_OK)
	{

		/* Ghi chuỗi bổ sung vào tệp. */
		fresult = f_puts("This is updated data and it should be in the end", &file);

		f_close(&file);

		memset(buffer, 0, buffSize(buffer));

		/* Mở lại tệp ở chế độ đọc. */
		fresult = f_open(&file, "filee2.txt", FA_READ);

		/* Đọc nội dung tệp vào bộ đệm. */
		fresult = f_read(&file, buffer, f_size(&file), &br);

		// Chờ đến khi thao tác đọc thành công.
		while (fresult != FR_OK)
			;
	}
	/* Đóng tệp sau khi cập nhật. */
	f_close(&file);
}

#endif