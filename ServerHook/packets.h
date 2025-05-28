#pragma once
#include <stdint.h>
#include <winsock.h>
#define _XTS_NEWCHARACLIST
#define _CHARAC_BLOCKING


struct _sHeader {
	uint16_t sLength : 12;		// -2048 ~ 2047
	uint16_t sCrypto : 2;		// 0 : false , còn lại là true // ³ª¸ÓÁö´Â true
	uint16_t sCompressed : 2;	// 0 : false , còn lại là true // ³ª¸ÓÁö´Â true

	_sHeader() {}
	_sHeader(uint16_t len) : sLength(len), sCrypto(0), sCompressed(0) {}
};


#pragma pack(push,1)
struct _CharacList // # [Nội bộ] Thông tin cơ bản của nhân vật # // # [내부] 캐릭터의 기본 정보 # 
{
	int				iUniqueID;				// ID độc nhất của nhân vật // 캐릭터의 유니크 아이디 
	char			cName[13];				// Tên của nhân vật // 캐릭터 의 이름 
	char			cNick[13];				// Biệt danh của nhân vật // 캐릭터 별칭 

	char			cZone;

	char			cSex;					// Giới tính 0: Nam, 1: Nữ // 성별 0 : 남자, 1 : 여자 
	char			cFace;					// Khuôn mặt: tối đa 16 loại // 얼굴형 : max 16종 
	char			cHair;					// Kiểu tóc: tối đa 16 loại // 머리 스타일 : max 16종 
	char			cGroup;				    // Môn phái liên kết // 소속 문파

	char			cAmorType;				// Áo 0 // 상의	0
	char			cPantsType;				// Quần 1 // 하의	1
	char			cCapType;				// Mũ 2 // 모자 2
	char			cAdaeType;				// Găng tay 3 // 아대 3
	char			cShoesType;				// Giày 4 // 신발 4
	char			cBackPackType;			// Ba lô // 배낭 
	char            cWeaponType;            // Loại vũ khí // 무기 타입
	char			cMaskType;				// Loại mặt nạ // 치장 타입

	short			sAmor;					// Áo 0 // 상의 0 
	short			sPants;				    // Quần 1 // 하의 1 
	short			sCap;					// Mũ 4 // 모자 4 
	short			sAdae;					// Găng tay 2 // 아대 2 
	short			sShoes;				    // Giày 3 // 신발 3 
	short			sBackPack;				// Ba lô 7 // 배낭 7 

	short			sSelectWeaponID;		// Chỉ số vũ khí đã chọn. 0, 1 (tương ứng với vũ khí 1, 2) // 선택한 무기 인덱스. 0, 1 ( 각각 무기1, 2 ) 

	short			sMask;					// Mặt nạ 8 // 치장 8
	u_char			ucWeaponStrength;		// Mức độ rèn luyện của vũ khí hiện tại. // 현재 무기의 제련 정도.

	short			sStr;					// Sức mạnh <= Khí chính // 근력 <= 원랙는 정기 
	short			sZen;					// Khí chân <= Giữ nguyên // 진기 <= 그대로 
	short			sIntelligence;			// Trí tuệ <= Khí linh // 지혜 <= 영기 
	short			sConstitution;			// Sức khỏe <= Khí sinh // 건강 <= // 건강 <= 생기 
	short			sDexterity;			    // Nhanh nhẹn <= Khí tâm // 민첩 <= 심기 

	short			sInnerLevel;			// Mức nội công // 내공 수위

	// 2004/11/10 Ray bổ sung. class, classgrade // 2004/11/10 Ray 추가. class, classgrade
	char			cClass;				    // Vai trò // 역할 
	char			cClassGrade;			// Cấp độ vai trò // 역할 등급

	char			cGM;					// Có phải GM không.. // GM 여부..

	// 2005/01/07 Ray bổ sung: Hiển thị vị trí nhân vật trong màn hình lựa chọn. // 2005/01/07 Ray 추가 : 선택창에서 캐릭터의 위치 표시.
	float			fPosX;					// Vị trí cuối cùng. // 최종 위치.
	float			fPosZ;					//

#ifdef _CHARAC_BLOCKING
	int				iBlockingEndDate;		// Thời gian hết hạn khóa // 블럭킹 만료 시간
#endif	
//#ifdef _XTS_SERVERUNIFICATION	// Vẫn được sử dụng sau khi hoàn thành việc thống nhất // 통합 완료 이후에도 계속 쓰임
#ifdef _XTS_NEWCHARACLIST
	u_char				ucChangeNameFlag : 2;	// 0 - Không, 1 - Có thể thay đổi tên // 0 - Not, 1 - Possible Change Name
	u_char				ucLeftFlag : 6;		    // Chưa được sử dụng. // 아직 안쓰임.
#endif
};
#pragma pack(pop)

struct CharacInfo {
	_sHeader header = _sHeader(0x5b); //0
	uint8_t ucMessage = 0x1B; //2
	uint8_t name[0x0D] = { 0 }; //3
	uint8_t padGM[0x1a - 0x03 - 13 * sizeof(uint8_t)] = { 0 }; //16
	uint8_t gm_sex = 0;
	//uint8_t gm : 5; //26
	//uint8_t sex : 3;
	uint8_t padStruct[0x56 - 0x1a - sizeof(uint8_t)]; //27
	uint16_t visualEffect1 = 0; //0x56
	uint16_t visualEffect2 = 0; //0x58
	uint8_t pad; //0x5a
	//0x5b
};


struct MSG_CHARAC_LIST
{
	_sHeader			header;
	unsigned char				ucMessage;

	char				nCharac;						// Số lượng nhân vật // 캐릭터 개수
	_CharacList		List[3];
	int				uiStateFlag;					// Thông tin trạng thái 32bit của cụm máy chủ (1: Mở cổng) // 서버군의 32bit상태정보(1: 봉문해제)
};



#pragma pack(push,1)
struct _CharacListRef // # [Nội bộ] Thông tin cơ bản của nhân vật # // # [내부] 캐릭터의 기본 정보 # 
{
	int32_t				iUniqueID;				// ID độc nhất của nhân vật // 캐릭터의 유니크 아이디 //4
	char			cName[13];				// Tên của nhân vật // 캐릭터 의 이름  //17
	char			cNick[13];				// Biệt danh của nhân vật // 캐릭터 별칭 30 bytes //30

	int8_t			cZone; //31
	//31

	int8_t			cSex;					// Giới tính 0: Nam, 1: Nữ // 성별 0 : 남자, 1 : 여자  //32
	int8_t			cFace;					// Khuôn mặt: tối đa 16 loại // 얼굴형 : max 16종  //33
	int8_t			cHair;					// Kiểu tóc: tối đa 16 loại // 머리 스타일 : max 16종  //34
	int8_t			cGroup;				    // Môn phái liên kết // 소속 문파 //35
	//35
	int8_t			cAmorType;				// Áo 0 // 상의	0 //36
	int8_t			cPantsType;				// Quần 1 // 하의	1 //37
	int8_t			cCapType;				// Mũ 2 // 모자 2 //38
	int8_t			cAdaeType;				// Găng tay 3 // 아대 3 //39
	int8_t			cShoesType;				// Giày 4 // 신발 4 //40
	int8_t			cBackPackType;			// Ba lô // 배낭  //41
	int8_t            cWeaponType;           // Loại vũ khí // 무기 타입 //42
	int8_t			cMaskType;				// Loại mặt nạ // 치장 타입  //43
	//43
	int16_t			sAmor;					// Áo 0 // 상의 0 //45
	int16_t			sPants;				    // Quần 1 // 하의 1 //47
	int16_t			sCap;					// Mũ 4 // 모자 4 //49
	int16_t			sAdae;					// Găng tay 2 // 아대 2 //51
	int16_t			sShoes;				    // Giày 3 // 신발 3 //53
	int16_t			sBackPack;				// Ba lô 7 // 배낭 7 //55



	//55
	int16_t			sSelectWeaponID;		// Chỉ số vũ khí đã chọn. 0, 1 (tương ứng với vũ khí 1, 2) // 선택한 무기 인덱스. 0, 1 ( 각각 무기1, 2 ) 
	//57
	int16_t			sMask;					// Mặt nạ 8 // 치장 8
	uint8_t ucWeaponStrength;		        // Mức độ rèn luyện của vũ khí hiện tại. // 현재 무기의 제련 정도. //60

	int8_t hatRefine;
	int8_t jacketRefine;
	int8_t pantsRefine;
	int8_t glovesRefine;
	int8_t bootsRefine;
	//60
	int16_t			sStr;					// Sức mạnh <= Khí chính // 근력 <= 원랙는 정기 
	int16_t			sZen;					// Khí chân <= Giữ nguyên // 진기 <= 그대로 
	int16_t			sIntelligence;			// Trí tuệ <= Khí linh // 지혜 <= 영기 
	int16_t			sConstitution;			// Sức khỏe <= Khí sinh // 건강 <= // 건강 <= 생기 
	int16_t			sDexterity;			    // Nhanh nhẹn <= Khí tâm // 민첩 <= 심기 
	//70
	int16_t			sInnerLevel;			// Mức nội công // 내공 수위 72
	//72
	// 2004/11/10 Ray bổ sung. class, classgrade // 2004/11/10 Ray 추가. class, classgrade
	int8_t			cClass;				    // Vai trò // 역할 
	int8_t			cClassGrade;			// Cấp độ vai trò // 역할 등급

	int8_t			cGM;					// Có phải GM không.. // GM 여부.. //
	//75rom D
	float			fPosX;					// Vị trí cuối cùng. // 최종 위치.
	float			fPosZ;					// 
	//83

	int32_t				iBlockingEndDate;		// Thời gian hết hạn khóa // 블럭킹 만료 시간  85

	//87

	uint8_t				ucChangeNameFlag : 2;	// 0 - Không, 1 - Có thể thay đổi tên // 0 - Not, 1 - Possible Change Name
	uint8_t				ucLeftFlag : 6;		    // Chưa được sử dụng. // 아직 안쓰임.

};
#pragma pack(pop)


#pragma pack(push,1)

struct MSG_CHARAC_LIST_REF
{
	_sHeader			header;
	unsigned char				ucMessage;

	char				nCharac;						// Số lượng nhân vật // 캐릭터 개수
	_CharacListRef		List[3];
	int				uiStateFlag;				    // Thông tin trạng thái 32bit của cụm máy chủ (1: Mở cổng) // 서버군의 32bit상태정보(1: 봉문해제)
};
#pragma pack(pop)