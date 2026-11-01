// Giao tiếp với chip đọc thẻ MFRC522.
#include "rc522.h"
#include "RFID_Scan.h"
#include "Systick.h"
#include "AppTypes.h"

// Chỉ số bản ghi người dùng đang được đối chiếu.
uint8_t RfidUserData_Index;

// Block dữ liệu hiện tại trên thẻ.
unsigned char block = 2;
// Bộ đệm phụ trợ cho dữ liệu RFID.
unsigned char buff[4];
// Số byte UID được dùng khi kiểm tra.
uint8_t total_uid = 5;
// Bộ đệm lưu loại thẻ trả về từ MFRC522.
uint8_t tagtype[2];
// Biến lưu mã định danh phụ trợ.
int id = 0;
// UID thẻ được cấu hình sẵn để đối chiếu.
unsigned char list_uid[5] = {0xFA, 0x94, 0x34, 0xE9, 0x0E};
// UID đọc được từ thẻ hiện tại.
unsigned char uid[4];
// Dữ liệu đọc từ block RFID.
unsigned char block_address[16] = {0};
// Kích thước dữ liệu trả về khi chọn thẻ.
unsigned char size;
// Bộ đệm phụ trợ cho dữ liệu đọc từ thẻ.
unsigned char read_buff[16];
// Danh sách biển số mẫu.
char *license_list[] = {"none",
						"59F2-2322",
						"89F2-2413",
						"23A1-2129",
						"12K1-2312",
						"59H2-8922"
						"67h3-1291",
						"12h8-1231"};

// Tìm biển số đã đọc trong danh sách người dùng từ thẻ SD.
static bool verify_data(const char *buff)
{
	for (RfidUserData_Index = 0; RfidUserData_Index < count; RfidUserData_Index++)
	{
		// Trả về đúng khi tìm thấy biển số trùng khớp.
		if (strcmp(buff, list_RfidUserData[RfidUserData_Index].license_plate) == 0)
		{
			return true;
		}
	}
	// Không có bản ghi nào khớp.
	return false;
}

// Kiểm tra các byte UID đọc được so với giá trị đã cấu hình.
static bool verify_uid(unsigned char *uid)
{
	while (total_uid)
	{
		static int index = 0;
		// Trả về đúng ngay khi byte UID tại vị trí hiện tại trùng khớp.
		if (uid[index] == list_uid[index])
		{
			return true;
		}
		index++;
		total_uid--;
	}
	// Không tìm thấy byte UID trùng khớp trong các lượt kiểm tra.
	return false;
}

// Đọc thông tin thẻ, kiểm tra quyền truy cập và cập nhật chế độ hệ thống.
void RFID_Scan()
{
	// Khóa mặc định dùng khi xác thực block dữ liệu.
	static unsigned char sectorKey[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	// Chờ đến khi phát hiện một thẻ RFID.
	while (MFRC522_Request(PICC_REQIDL, tagtype) != MI_OK)
		;

	// Đọc UID của thẻ.
	while (MFRC522_Anticoll(uid) != MI_OK)
		;

	// Chọn thẻ vừa được phát hiện.
	size = MFRC522_SelectTag(uid);

	// Đọc block để xác nhận giao tiếp với thẻ.
	while (MFRC522_Read(block, block_address) != MI_OK)
		;

	// Xác thực quyền truy cập block bằng khóa A.
	while (MFRC522_Auth(PICC_AUTHENT1A, block, sectorKey, uid) != MI_OK)
		;

	// Đọc lại block sau khi xác thực.
	while (MFRC522_Read(block, block_address) != MI_OK)
		;

	// Dữ liệu mẫu được ghi vào block hiện tại.
	static unsigned char data[] = "Pham Cao Duy";

	// Ghi dữ liệu mẫu lên thẻ.
	while (MFRC522_Write(block, data) != MI_OK)
		;

	// Đọc các block tiếp theo cho đến block 3.
	do
	{
		while (MFRC522_Read(block, block_address) != MI_OK)
			;
		block++;
	} while (block < 4);

	block = 0;
	MFRC522_Halt();
	// Tạm chờ trước khi đọc dữ liệu cần kiểm tra.
	delay_ms(500);

	// Đọc block 4 và truyền con trỏ nhận dữ liệu cho driver.
	char *pRev = NULL;
	while (MFRC522_Read(4, (unsigned char *)pRev) != MI_OK)
		;

	// Sao chép dữ liệu nhận được sang bộ đệm để đối chiếu.
	uint8_t len = strlen(pRev);
	unsigned char *buff = (unsigned char *)malloc(sizeof(unsigned char) * len + 1);
	strcpy((char *)buff, pRev);

	// Nếu UID hợp lệ, tiếp tục kiểm tra dữ liệu thẻ.
	if (verify_uid(uid))
	{
		// Dữ liệu khớp cơ sở dữ liệu thì cho phép vận hành.
		if (verify_data((char *)buff))
		{
			ModeStatusCfg.apptype = RUN_MODE;
		}
		// UID hợp lệ nhưng dữ liệu không có trong cơ sở dữ liệu.
		else
		{
			ModeStatusCfg.apptype = ERROR_MODE;
			ModeStatusCfg.errortype = RFID_SCAN_EMPTY;
		}
		// Yêu cầu LCD hiển thị trạng thái mới.
		ModeStatusCfg.IsLcdModeUpdate = true;
	}
	// UID không hợp lệ thì báo lỗi không tìm thấy thẻ.
	else
	{
		ModeStatusCfg.apptype = ERROR_MODE;
		ModeStatusCfg.errortype = UID_NOTFOUND;
	}

	free(pRev);
}

// Khởi tạo chip đọc thẻ MFRC522.
void RFID_ModuleInit()
{
	MFRC522_Init();
}
