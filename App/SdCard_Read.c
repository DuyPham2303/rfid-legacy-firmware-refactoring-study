// Khai báo giao diện thao tác thẻ SD.
#include "SdCard_Read.h"
#include "AppTypes.h"
#include <errno.h>
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

typedef struct
{
	const char *name;
	const char *licensePlate;
} SampleUserRecord;

// mảng tĩnh giả lập sample data ghi vào csv
static const SampleUserRecord sampleUserRecords[] = {
	{"Pham Cao Duy", "51K-482.16"},
	{"Trinh Tran Phuong Tuan", "70A-391.28"},
	{"Trinh Le Hoang", "60C-745.09"},
	{"Nguyen Tan Tung", "59A-638.41"},
	{"Le Quang Nhat", "30F-826.17"},
	{"Nguyen Huu Hung", "43A-590.62"},
	{"Pham Cao Duy", "51H-274.83"},
	{"Dinh Anh Tuan", "70B-918.36"},
	{"Nguyen Ho Duy", "61K-405.27"},
	{"Pham Cao Duy", "50E-731.94"},
	{"Nguyen Thi Thanh Thuy", "51G-286.53"},
	{"Nguyen Ho Duy", "72A-649.18"},
	{"Nguyen Thi Thanh Thu", "59C-357.20"},
	{"Pham Cao Duy", "60A-802.71"}};

#define MAX_RFID_USERS ((int)(sizeof(sampleUserRecords) / sizeof(sampleUserRecords[0])))
#define USER_DATA_LINE_SIZE 100U
#define STRING_TERMINATOR_SIZE 1U
#define EMPTY_RECORD_COUNT 0
#define SAMPLE_RECORD_START_INDEX 0U
#define MOUNT_IMMEDIATELY 1U
#define CLEARED_BYTE_VALUE 0
#define INITIAL_BUFFER_LENGTH 0
#define CSV_HEADER "ten,bien so xe\n"
#define FAT_RESERVED_CLUSTER_COUNT 2U
#define SECTOR_TO_KILOBYTE_RATIO 0.5f

// Số lượng bản ghi đã nạp từ tệp dữ liệu.
int count = EMPTY_RECORD_COUNT;
// Danh sách dữ liệu người dùng được nạp vào RAM.
static RfidUserDtypeCfg_s userDataPool[MAX_RFID_USERS];
static char userDataTextPool[MAX_RFID_USERS][USER_DATA_LINE_SIZE];
const RfidUserDtypeCfg_s *list_RfidUserData = userDataPool;
// Đường dẫn tệp CSV chứa dữ liệu người dùng.
#define FILE_SRC ".//Bsw//database//db.csv"

// Các hàm đọc, ghi và phân tích dữ liệu CSV.
static Status Load_UserDB(const char *path);
static Status WriteFile(const char *path);
static Status parse_line(char *line, RfidUserDtypeCfg_s *record, char *textBuffer);
static Status inspectCsvFile(const char *path, bool *needsSampleData);
static Status writeSampleCsv(FILE *file);
static Status skipCsvHeader(FILE *file, char *line, size_t lineSize);
static Status isCompleteCsvLine(FILE *file, const char *line);
static Status loadCsvRecords(FILE *file);

// Khởi tạo thẻ SD và nạp cơ sở dữ liệu người dùng.
void SDcard_ModuleInit()
{
#ifdef DISKO_H
	// Khởi tạo thẻ SD khi bật hỗ trợ FatFs.
	// Khởi tạo FatFs và gắn phân vùng gốc.
	MX_FATFS_Init();
	fresult = f_mount(&fs, "/", MOUNT_IMMEDIATELY);
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
	count = EMPTY_RECORD_COUNT;

	// ghi dữ liệu giả lập vào file
	if (WriteFile(path) != E_OK)
	{
		return NOT_OK;
	}

	// đọc ra dữ liệu trong file
	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		return NOT_OK;
	}

	// cấp phát stack lưu từng dòng file csv
	char line[USER_DATA_LINE_SIZE];

	// bỏ qua tiêu đề
	Status status = skipCsvHeader(file, line, sizeof(line));
	if (status == E_OK)
	{
		status = loadCsvRecords(file);
	}

	if (fclose(file) != 0)
	{
		count = EMPTY_RECORD_COUNT;
		return NOT_OK;
	}
	return status;
}

// Kiểm tra tệp có dữ liệu để tránh ghi đè.
static Status inspectCsvFile(const char *path, bool *needsSampleData)
{
	FILE *file = fopen(path, "r");
	if (file == NULL)
	{
		if (errno == ENOENT)
		{
			*needsSampleData = true;
			return E_OK;
		}
		return NOT_OK;
	}

	int firstChar = fgetc(file);
	bool readError = ferror(file) != 0;
	int closeResult = fclose(file);
	if (readError || closeResult != 0)
	{
		return NOT_OK;
	}

	*needsSampleData = firstChar == EOF;
	return E_OK;
}

// Ghi dữ liệu mẫu với tên và biển số giả lập.
static Status writeSampleCsv(FILE *file)
{
	// ghi phần tiêu đề
	if (fprintf(file, "%s", CSV_HEADER) < 0)
	{
		return NOT_OK;
	}

	// chép lần lượt từng bản ghi vào từng dòng của file csv
	for (size_t index = SAMPLE_RECORD_START_INDEX;
		 index < sizeof(sampleUserRecords) / sizeof(sampleUserRecords[0]);
		 index++)
	{
		if (fprintf(file, "%s,%s\n",
					sampleUserRecords[index].name,
					sampleUserRecords[index].licensePlate) < 0)
		{
			return NOT_OK;
		}
	}

	return ferror(file) ? NOT_OK : E_OK;
}

// Chỉ tạo tệp mẫu khi tệp chưa tồn tại hoặc đang rỗng.
static Status WriteFile(const char *path)
{
	bool needsSampleData;

	// kiểm tra file đã có sẵn dữ liệu chưa
	if (inspectCsvFile(path, &needsSampleData) != E_OK)
	{
		return NOT_OK;
	}

	// trả kết quả trạng thái file csv
	if (!needsSampleData)
	{
		return E_OK; // file đả có dữ liệu không cần ghi mới
	}

	// Quy trình khi dữ liệu vào file csv
	FILE *file = fopen(path, "w");
	if (file == NULL)
	{
		return NOT_OK;
	}
	// Ghi dữ liệu từ mảng tĩnh vào file
	Status status = writeSampleCsv(file);
	if (fclose(file) != 0)
	{
		return NOT_OK;
	}
	return status;
}

// Bỏ qua dòng tiêu đề trước khi đọc bản ghi.
static Status skipCsvHeader(FILE *file, char *line, size_t lineSize)
{
	return fgets(line, lineSize, file) == NULL ? NOT_OK : E_OK;
}

// Từ chối dòng bị cắt bởi bộ đệm đọc.
static Status isCompleteCsvLine(FILE *file, const char *line)
{
	if (strchr(line, '\n') != NULL)
	{
		return E_OK;
	}

	int nextChar = fgetc(file);
	return nextChar == EOF && !ferror(file) ? E_OK : NOT_OK;
}

// Đọc, kiểm tra giới hạn và phân tích các bản ghi CSV.
static Status loadCsvRecords(FILE *file)
{
	char line[USER_DATA_LINE_SIZE];

	// lặp qua từng dòng của file và đọc ra dữ liệu trỏ vào line
	while (fgets(line, sizeof(line), file) != NULL)
	{
		// lần lượt trích xuất từng block dữ liệu (name + bien so xe)
		if (isCompleteCsvLine(file, line) != E_OK ||
			count >= MAX_RFID_USERS ||
			parse_line(line, &userDataPool[count], userDataTextPool[count]) != E_OK)
		{
			// xử lý trường hợp nếu số lượng bản ghi thực tế lớn hơn quy định
			count = EMPTY_RECORD_COUNT;
			return NOT_OK;
		}
		count++; // chuyển sang block dữ liệu kế tiếp
	}

	if (ferror(file) || count == EMPTY_RECORD_COUNT)
	{
		count = EMPTY_RECORD_COUNT;
		return NOT_OK;
	}
	return E_OK;
}

// Tách tên và biển số từ một dòng CSV thành bản ghi người dùng.
static Status parse_line(char *line, RfidUserDtypeCfg_s *record, char *textBuffer)
{
	char *token; // Con trỏ giữ trường CSV vừa tách.

	// Tách trường đầu tiên làm tên người dùng.
	token = strtok(line, ","); // Tách trường đến dấu phẩy đầu tiên.
	if (token == NULL)
	{
		return NOT_OK;
	}
	char *name = token;
	size_t nameLength = strlen(token);

	// Tách trường kế tiếp làm biển số xe.
	token = strtok(NULL, ",");
	if (token == NULL)
	{
		return NOT_OK;
	}

	// đánh dấu điểm kết sau khi đọc xong để tránh ký tự rác
	token[strcspn(token, "\r\n")] = '\0';
	size_t licensePlateLength = strlen(token);

	// đảm bảo kích thước dữ liệu không vượt quá buffer quy định
	if (nameLength + STRING_TERMINATOR_SIZE +
			licensePlateLength + STRING_TERMINATOR_SIZE >
		USER_DATA_LINE_SIZE)
	{
		return NOT_OK;
	}

	// Lưu hai trường trong bộ đệm tĩnh của bản ghi hiện tại.
	record->name = textBuffer;
	memcpy(record->name, name, nameLength + STRING_TERMINATOR_SIZE);
	record->license_plate = textBuffer + nameLength + STRING_TERMINATOR_SIZE;
	memcpy(record->license_plate, token, licensePlateLength + STRING_TERMINATOR_SIZE);

	return E_OK;
}

// Các API thao tác trên SD card - tạm thời chưa sử dụng
#ifdef DISKO_H

// Tính độ dài chuỗi bao gồm ký tự kết thúc.
static int buffSize(char *pbuff)
{
	int size = INITIAL_BUFFER_LENGTH;
	while (pbuff[size++] != '\0')
		;
	return size;
}

// Đọc tổng dung lượng và dung lượng còn trống của thẻ.
static void check_free_space()
{
	f_getfree("", &fre_clust, &pfs);
	total = (uint32_t)((pfs->n_fatent - FAT_RESERVED_CLUSTER_COUNT) *
					   pfs->csize * SECTOR_TO_KILOBYTE_RATIO);
	sprintf(buffer, "SD CARD Total Size: \t%d\n", total);
	memset(buffer, CLEARED_BYTE_VALUE, sizeof(buffer));
	free_space = (uint32_t)(fre_clust * pfs->csize * SECTOR_TO_KILOBYTE_RATIO);
	memset(buffer, CLEARED_BYTE_VALUE, sizeof(buffer));
}

// Ghi dữ liệu thử nghiệm lên thẻ.
static void SDcard_write()
{
	// Mở hoặc tạo tệp thử nghiệm.
	fresult = f_open(&file, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);

	// Ghi nội dung mẫu.
	strcpy(buffer, "This is File2.txt, written using ...f_write... and it says Hello from Controllerstech\n");

	fresult = f_write(&file, buffer, buffSize(buffer), &bw);

	// Đóng tệp sau khi ghi.
	f_close(&file);

	memset(buffer, CLEARED_BYTE_VALUE, sizeof(buffer));
}

// Đọc nội dung tệp thử nghiệm.
static void SDcard_read()
{
	fresult = f_open(&file, "file2.txt", FA_READ);
	if (fresult == FR_OK)
		f_read(&file, buffer, f_size(&file), &br);
	// Đóng tệp sau khi đọc.
	f_close(&file);
	memset(buffer, CLEARED_BYTE_VALUE, sizeof(buffer));
}

// Ghi nối dữ liệu rồi đọc lại tệp thử nghiệm.
static void SDcard_updateFile()
{
	// Mở tệp ở chế độ đọc và ghi.
	fresult = f_open(&file, "filee2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);

	// Chuyển vị trí ghi đến cuối tệp.
	fresult = f_lseek(&file, f_size(&file));

	if (fresult == FR_OK)
	{
		// Ghi thêm nội dung.
		fresult = f_puts("This is updated data and it should be in the end", &file);

		f_close(&file);

		memset(buffer, CLEARED_BYTE_VALUE, buffSize(buffer));

		// Mở lại tệp để đọc.
		fresult = f_open(&file, "filee2.txt", FA_READ);

		// Đọc nội dung vào bộ đệm.
		fresult = f_read(&file, buffer, f_size(&file), &br);

		// Chờ đến khi đọc thành công.
		while (fresult != FR_OK)
			;
	}
	// Đóng tệp sau khi cập nhật.
	f_close(&file);
}

#endif