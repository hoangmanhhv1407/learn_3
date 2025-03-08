#pragma once
#include <stdint.h>
#include <winsock.h>
#define _XTS_NEWCHARACLIST
#define _CHARAC_BLOCKING

 
struct _sHeader {
	uint16_t sLength : 12;		// -2048 ~ 2047
	uint16_t sCrypto : 2;		// 0 : false , ³ª¸ÓÁö´Â true
	uint16_t sCompressed : 2;	//0 : false , ³ª¸ÓÁö´Â true

	_sHeader() {}
	_sHeader(uint16_t len) : sLength(len), sCrypto(0), sCompressed(0) {}
};
 

#pragma pack(push,1)
struct _CharacList // # [내부] 캐릭터의 기본 정보 # 
{
	int				iUniqueID;				// 캐릭터의 유니크 아이디 
	char			cName[13];				// 캐릭터 의 이름 
	char			cNick[13];				// 캐릭터 별칭 

	char			cZone;

	char			cSex;					// 성별 0 : 남자, 1 : 여자 
	char			cFace;					// 얼굴형 : max 16종 
	char			cHair;					// 머리 스타일 : max 16종 
	char			cGroup;				// 소속 문파

	char			cAmorType;				// 상의	0
	char			cPantsType;				// 하의	1
	char			cCapType;				// 모자 2
	char			cAdaeType;				// 아대 3
	char			cShoesType;				// 신발 4
	char			cBackPackType;			// 배낭 
	char            cWeaponType;           // 무기 타입
	char			cMaskType;				// 치장 타입

	short			sAmor;					// 상의 0 
	short			sPants;				// 하의 1 
	short			sCap;					// 모자 4 
	short			sAdae;					// 아대 2 
	short			sShoes;				// 신발 3 
	short			sBackPack;				// 배낭 7 

	short			sSelectWeaponID;		// 선택한 무기 인덱스. 0, 1 ( 각각 무기1, 2 ) 

	short			sMask;					// 치장 8
	u_char			ucWeaponStrength;		// 현재 무기의 제련 정도.

	short			sStr;					// 근력 <= 원랙는 정기 
	short			sZen;					// 진기 <= 그대로 
	short			sIntelligence;			// 지혜 <= 영기 
	short			sConstitution;			// 건강 <= // 건강 <= 생기 
	short			sDexterity;			// 민첩 <= 심기 

	short			sInnerLevel;			// 내공 수위

	// 2004/11/10 Ray 추가. class, classgrade
	char			cClass;				// 역할 
	char			cClassGrade;			// 역할 등급

	char			cGM;					// GM 여부..

	// 2005/01/07 Ray 추가 : 선택창에서 캐릭터의 위치 표시.
	float			fPosX;					// 최종 위치.
	float			fPosZ;					//

#ifdef _CHARAC_BLOCKING
	int				iBlockingEndDate;		// 블럭킹 만료 시간
#endif	
//#ifdef _XTS_SERVERUNIFICATION	// 통합 완료 이후에도 계속 쓰임
#ifdef _XTS_NEWCHARACLIST
	u_char				ucChangeNameFlag : 2;	// 0 - Not, 1 - Possible Change Name
	u_char				ucLeftFlag : 6;		// 아직 안쓰임.
#endif
 };
#pragma pack(pop)

struct CharacInfo {
	_sHeader header = _sHeader(0x5b); //0
	uint8_t ucMessage = 0x1B; //2
	uint8_t name[0x0D] = { 0 }; //3
	uint8_t padGM[0x1a - 0x03 - 13 * sizeof(uint8_t) ] = { 0 }; //16
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

	char				nCharac;						// 캐릭터 개수
	_CharacList		List[3];
	int				uiStateFlag;					//서버군의 32bit상태정보(1: 봉문해제)
};
 


#pragma pack(push,1)
 struct _CharacListRef // # [내부] 캐릭터의 기본 정보 # 
 {
	 int32_t				iUniqueID;				// 캐릭터의 유니크 아이디 //4
	 char			cName[13];				// 캐릭터 의 이름  //17
	 char			cNick[13];				// 캐릭터 별칭 30 bytes //30

	 int8_t			cZone; //31
	 //31

	 int8_t			cSex;					// 성별 0 : 남자, 1 : 여자  //32
	 int8_t			cFace;					// 얼굴형 : max 16종  //33
	 int8_t			cHair;					// 머리 스타일 : max 16종  //34
	 int8_t			cGroup;				// 소속 문파 //35
	 //35
	 int8_t			cAmorType;				// 상의	0 //36
	 int8_t			cPantsType;				// 하의	1 //37
	 int8_t			cCapType;				// 모자 2 //38
	 int8_t			cAdaeType;				// 아대 3 //39
	 int8_t			cShoesType;				// 신발 4 //40
	 int8_t			cBackPackType;			// 배낭  //41
	 int8_t            cWeaponType;           // 무기 타입 //42
	 int8_t			cMaskType;				// 치장 타입  //43
	 //43
	 int16_t			sAmor;					// 상의 0 //45
	 int16_t			sPants;				// 하의 1 //47
	 int16_t			sCap;					// 모자 4 //49
	 int16_t			sAdae;					// 아대 2 //51
	 int16_t			sShoes;				// 신발 3 //53
	 int16_t			sBackPack;				// 배낭 7 //55



	 //55
	 int16_t			sSelectWeaponID;		// 선택한 무기 인덱스. 0, 1 ( 각각 무기1, 2 ) 
	 //57
	 int16_t			sMask;					// 치장 8
	 uint8_t ucWeaponStrength;		// 현재 무기의 제련 정도. //60

	 int8_t hatRefine;
	 int8_t jacketRefine;
	 int8_t pantsRefine;
	 int8_t glovesRefine;
	 int8_t bootsRefine;
	 //60
	 int16_t			sStr;					// 근력 <= 원랙는 정기 
	 int16_t			sZen;					// 진기 <= 그대로 
	 int16_t			sIntelligence;			// 지혜 <= 영기 
	 int16_t			sConstitution;			// 건강 <= // 건강 <= 생기 
	 int16_t			sDexterity;			// 민첩 <= 심기 
	 //70
	 int16_t			sInnerLevel;			// 내공 수위 72
	 //72
	 // 2004/11/10 Ray 추가. class, classgrade
	 int8_t			cClass;				// 역할 
	 int8_t			cClassGrade;			// 역할 등급

	 int8_t			cGM;					// GM 여부.. //
	 //75rom D
	 float			fPosX;					// 최종 위치.
	 float			fPosZ;					// 
	 //83

	 int32_t				iBlockingEndDate;		// 블럭킹 만료 시간  85

	 //87

	 uint8_t				ucChangeNameFlag : 2;	// 0 - Not, 1 - Possible Change Name
	 uint8_t				ucLeftFlag : 6;		// 아직 안쓰임.

 



 };
#pragma pack(pop)


#pragma pack(push,1)

 struct MSG_CHARAC_LIST_REF
 {
	 _sHeader			header;
	 unsigned char				ucMessage;

	 char				nCharac;						// 캐릭터 개수
	 _CharacListRef		List[3];
	 int				uiStateFlag;				//서버군의 32bit상태정보(1: 봉문해제)
 };
#pragma pack(pop)