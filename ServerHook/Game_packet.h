#pragma once

#pragma pack(push, enter_game)
#pragma pack(1)

struct _sHeader
{
	u_short	sLength : 12;		// -2048 - 2047
	u_short	sCrypto : 2;		// 0 : false, nếu khác thì true
	u_short	sCompressed : 2;	// 0 : flase, nếu khác thì true

	_sHeader() {}
	_sHeader(u_short len) : sLength(len), sCrypto(0), sCompressed(0) {}
};

#define _MAX_QUEST_INVENTORY_				5


// Cấu trúc này chứa thông tin đầy đủ về nhân vật trong danh sách
struct _CharacList // # [Thông tin cơ bản của nhân vật] # 
{
	int				iUniqueID;				// ID duy nhất của nhân vật 
	char			cName[13];				// Tên nhân vật 
	char			cNick[13];				// Biệt danh nhân vật

	char			cZone;

	char			cSex;					// Giới tính 0: Nam, 1: Nữ 
	char			cFace;					// Khuôn mặt: tối đa 16 loại
	char			cHair;					// Kiểu tóc: tối đa 16 loại 
	char			cGroup;				    // Phái

	char			cAmorType;				// Loại áo 0
	char			cPantsType;				// Loại quần 1
	char			cCapType;				// Loại mũ 2
	char			cAdaeType;				// Loại tay áo 3
	char			cShoesType;				// Loại giày 4
	char			cBackPackType;			// Loại túi 
	char            cWeaponType;            // Loại vũ khí
	char			cMaskType;				// Loại mặt nạ

	short			sAmor;					// ID áo 0 
	short			sPants;				    // ID quần 1 
	short			sCap;					// ID mũ 4 
	short			sAdae;					// ID tay áo 2 
	short			sShoes;				    // ID giày 3 
	short			sBackPack;				// ID túi 7 

	short			sSelectWeaponID;		// ID vũ khí được chọn. 0, 1 (vũ khí 1, 2) 

	short			sMask;					// ID mặt nạ 8
	u_char			ucWeaponStrength;		// Độ bền vũ khí hiện tại

	short			sStr;					// Sức mạnh
	short			sZen;					// Tinh thần
	short			sIntelligence;			// Trí tuệ
	short			sConstitution;			// Thể chất
	short			sDexterity;			    // Nhanh nhẹn

	short			sInnerLevel;			// Cấp độ nội công

	// 2004/11/10 Ray thêm. class, classgrade
	char			cClass;				    // Vai trò 
	char			cClassGrade;			// Cấp độ vai trò

	char			cGM;					// Trạng thái GM

	// 2005/01/07 Ray thêm: vị trí nhân vật trong menu chọn
	float			fPosX;					// Vị trí X cuối cùng
	float			fPosZ;					// Vị trí Z cuối cùng

#ifdef _CHARAC_BLOCKING
	int				iBlockingEndDate;		// Thời gian hết hạn chặn
#endif	
//#ifdef _XTS_SERVERUNIFICATION	// Vẫn sử dụng sau khi hoàn thành hợp nhất
#ifdef _XTS_NEWCHARACLIST
	u_char				ucChangeNameFlag : 2;	// 0 - Không, 1 - Có thể đổi tên
	u_char				ucLeftFlag : 6;		    // Chưa sử dụng
#endif
};

// Thông tin nhiệm vụ
struct _sCharacQuestState
{
	bool	bReword : 1;	// Đã nhận phần thưởng chưa
	u_char	ucNodeID : 7;	// ID nút hiện tại (phải là unsigned char)

	// Thêm mới
	u_short usQuestID;      // ID nhiệm vụ hiệp khách, kỳ duyên, võ lâm

	_sCharacQuestState()
	{
		bReword = false;
		ucNodeID = 0;
	}
};

// Cấu trúc lưu nhiệm vụ chính
struct _sCharacMainQuestState
{
	bool			bReword : 1;	// Đã nhận phần thưởng chưa
	unsigned char	ucNodeID : 7;	// ID nút hiện tại
	unsigned short  sQuestID;       // ID nhiệm vụ chế long hành hiện tại
	u_char			padding;        // Phần đệm dự phòng

	_sCharacMainQuestState()
	{
		sQuestID = 0;
		ucNodeID = 0;
		bReword = false;
	}
};

// Cấu trúc vật phẩm nhiệm vụ
struct _sQuestItem
{
	short			sID;
	unsigned int	uiCount;

	_sQuestItem() : sID(0), uiCount(0)
	{}
};


#define QUEST_HISTORY_COUNT 50 

struct _sQuestHistory
{
	char cNodeHistory;
	char cBranchHistory;
};

struct _sRunningQuest
{
	u_short usQuestID;
	char  cNodeID;
	int   dwQuestStartTime;
	int	  dwQuestTimer;
	char  cTimerNodeNo;
	bool  bBeUse;

	char   cTFRetry;       // Kiểu retry khi nhận thưởng lại (true action hay false action)
	u_char ucHistoryCount; // Giới hạn 255 mục

	_sQuestHistory QuestHistory[QUEST_HISTORY_COUNT]; // Giảm xuống còn 50 mục
	_sRunningQuest()
	{
		usQuestID = 0;
		dwQuestTimer = 0;
		dwQuestStartTime = 0;
		ucHistoryCount = 0;  // Số lượng kết quả trigger được lưu trong lịch sử
		cTFRetry = 0;  // Loại retry (false hay true action)						    
		cNodeID = 0;
		cTimerNodeNo = 0;
		bBeUse = false;
	}
};



// Cấu trúc dữ liệu nhiệm vụ đang chạy trong gói tin, bao gồm cả lịch sử
// Dữ liệu lịch sử được gửi độc lập với nhánh
struct _sRunningQuestPacketData
{
	u_short usQuestID;
	int   dwQuestStartTime;
	int	  dwQuestTimer;
	char  cTimerNodeNo;
	u_char ucSelectNo;
	u_char ucCounter[4];

	_sRunningQuestPacketData()
	{
		usQuestID = 0;
		dwQuestStartTime = 0;
		dwQuestTimer = 0;
		cTimerNodeNo = 0;
		ucSelectNo = 0;
		ucCounter[0] = 0;
		ucCounter[1] = 0;
		ucCounter[2] = 0;
		ucCounter[3] = 0;
	}
};

#ifdef _XDEF_EXPAND_ORGANIZATIONNAME
#define					_XDEF_ORGANIZATIONNAMELENGTH			24
#else
#define					_XDEF_ORGANIZATIONNAMELENGTH			8
#endif
// Cấu trúc xếp hạng nhiệm vụ
struct QUEST_RANK_RECORD
{
	unsigned char	m_ucClan;
	char			m_acOrgName[_XDEF_ORGANIZATIONNAMELENGTH + 1];		// Cá nhân: Vô, Đồng hành: Vô, Đoàn: Tên đoàn hùng
	char			m_acName[13];		// Cá nhân: Tên, Đồng hành: Tên hàng trưởng, Đoàn: Tên đoàn trưởng
	unsigned short	m_usLevelOrCount;	// Cá nhân: Cấp độ thành tựu, Đồng hành: Số người, Đoàn: Số thành viên
	unsigned short	m_usQuestData;		// KillCount, Time, ItemCount
};

#define MAX_QUEST_RANK_RECORD	(10)

struct _QUEST_RANK_DATA
{
	QUEST_RANK_RECORD	m_aRankRecord[MAX_QUEST_RANK_RECORD];
};


// Từ 0 đến 15
struct _sCharac_Skill
{
	short			id;
	char			cStep;				// Giai đoạn phát triển võ công

	u_char			ucSkillState : 2;	// 0: Đã hoàn thành, 1: Chưa hoàn thành
	u_char			ucPassiveState : 1;	// Bật/tắt Passive
	u_char			ucCurMission : 5;	// Đếm tiến trình nhiệm vụ 0-31

	int				iCurSkillExp;
};

struct _sSkill_pair
{
	short	    sSkillID; // ID kỹ năng
	char        number;   // Cấp độ kỹ năng (-1 là xóa)
};

// Liên quan đến vật phẩm
union __item_or_money
{
	struct
	{
		u_short	 usCurDur;			  // Độ bền hiện tại
		char	 cIdentify : 4;	  // Trạng thái giám định
		u_char   ucSocketCount : 4;   // Số lỗ socket có thể tạo
		u_char   ucCount;			  // Số lượng tối đa 255
	} S_NormalItem;
	struct
	{
		u_int	 uiCount;	 		  // Lượng tiền
	} S_MoneyItem;
};

struct __inven_uselimit_info_v1	// Thông tin giới hạn sử dụng
{
	u_char				ucFlag1;		// Flag1 = Có giới hạn thời gian hay không
#ifdef _XDEF_SMELT_ITEM //Author : Dương Hy Vương
	u_short				usResaved : 1;	// Đã thiết lập độ bền cơ bản chưa
	u_short				usUsedProtect : 1;	// Đã sử dụng bảo hộ chưa
	u_short				usCurProtect : 5;	// Số lượng bảo hộ hiện tại (vũ khí)
	u_short				usCurMaxProtect : 5;	// Độ bảo hộ tối đa

	u_short				usReserved : 4;	// Dự phòng
#else
	u_char				ucFlag2;		// 
	u_char				ucFlag3;
#endif
	u_short				usValue1;		// 

	u_char				ucYear;		// Năm - 2000 : bắt đầu từ 5
	u_char				ucMonth;		// Tháng
	u_char				ucDay;			// Ngày
	u_char				ucHour;		// Giờ
	u_char				ucMinute;		// Phút 
};

// Thông tin vật phẩm không phải vũ khí/trang phục
struct _sUser_InvenItem
{
#ifdef _XDEF_CASHBACKPACK_080404_MAGVIPER //Author : Dương Hy Vương //breif : Thêm túi đồ cao cấp
	u_char					cSlot;			// Vị trí slot
#else
	char					cSlot;			// Vị trí slot
#endif
	char					cType;			// Phân loại lớn 9
	char					cSecond;		// Được thiết lập lại mỗi khi khởi động
	short					sID;			// ID vật phẩm 12

	__item_or_money			Item_Money;		// 16 		

	u_char					ucIncludeLimit : 1;// Có thông tin giới hạn sử dụng không
	u_char					ucBooleanRes : 7;	// 
};

// ---------------------------------------------------------------------=
// Cấu trúc cho vật phẩm không phải vũ khí/trang phục có giới hạn sử dụng (ucIncludeLimit = 1)
struct _sUser_Inven_v1_limit : public _sUser_InvenItem
{
	__inven_uselimit_info_v1	uselimit;
};
// ---------------------------------------------------------------------=


// ---------------------------------------------------------------------=
// Cấu trúc cho vũ khí/trang phục
struct _sUser_Inven_v1_extension : public _sUser_InvenItem
{
	short					sSocket[4];		// 24

	u_char					ucStrength;	// Độ mạnh (với vũ khí)

	//Author : Dương Hy Vương
	//u_char	ucCurProtect : 4;	// Số lượng bảo vệ (0-15)
	//u_char	ucIncProtect : 3;	// Tăng độ bảo vệ tối đa
	//u_char	ucUsedProtect : 1;	// Đã sử dụng bảo vệ chưa
	u_char					ucSharpness;	// Độ sắc bén (với vũ khí)
};

// Cấu trúc cho vũ khí/trang phục có giới hạn sử dụng (ucIncludeLimit = 1)
struct _sUser_Inven_v1_all : public _sUser_Inven_v1_extension
{
	__inven_uselimit_info_v1	uselimit;
};
// ---------------------------------------------------------------------=
struct _sUser_Inven
{
	u_char ucIdx;
	_sUser_Inven_v1_all Item;
};

// Kho đồ
struct _SM_Storage_Inven
{
#ifdef _XDEF_CASHBACKPACK_080404_MAGVIPER //Author : Dương Hy Vương //breif : Thêm túi đồ cao cấp
	u_char				cIdx;			// Chỉ số kho đồ hoặc chỉ số túi đồ người dùng
#else
	char				cIdx;			// Chỉ số kho đồ hoặc chỉ số túi đồ người dùng
#endif
	u_char      		usNumber;		// Số lượng lưu trữ
	char                cIsstackable;   // Có thể xếp chồng không
};

// Vật phẩm rơi từ quái vật
struct _sItem_Drop						// Một mục vật phẩm trên mặt đất
{
	int				iItemWorldID;
	// 2004.05.31->oneway48 modify
	float			fX;
	float			fZ;
	//	short			fX;
	//	short			fZ;
	// modify end 
	u_short			usOwner;
	char			cType;				// Phân loại lớn 2^4 : 16

	union {
		struct {
			char				cSecond;		// Phân loại trung
			char				cIdentify;		// Chỉ từ 0-3
			short				sID;			// ID vật phẩm (phân loại nhỏ)
		} S_detail;
		u_int			uiCount;
	};

	u_char				ucCount;		// Số lượng
};

struct _sitem_pair
{
	u_char			slot;				// Slot
	u_char			number;				// Số lượng
};

struct _tradeitem
{
	char			index;
	unsigned short	quantity;
};

// Dữ liệu sưu tầm
#define COLLECTIONDATA_COMPLETEFLAG_INDEX	(15)
extern WORD							g_CollectonWordFilter[16];
class ckCollectionData
{
private:
	unsigned short	m_usData;
public:
	inline unsigned short	GetData() { return m_usData; }
	inline void ckCollectionData::SetTrue(unsigned char index) { m_usData = m_usData | g_CollectonWordFilter[index]; };
	inline void ckCollectionData::SetFalse(unsigned char index) { m_usData = m_usData & (g_CollectonWordFilter[index] ^ USHRT_MAX); };
	inline void ckCollectionData::SetCompleted() { m_usData = m_usData | g_CollectonWordFilter[COLLECTIONDATA_COMPLETEFLAG_INDEX]; };

	inline bool ckCollectionData::IsThisTrue(unsigned char index)
	{
		if ((m_usData & g_CollectonWordFilter[index]) == 0)
			return false;
		return true;
	};
	inline bool ckCollectionData::IsThisCompleted()
	{
		if ((m_usData & g_CollectonWordFilter[COLLECTIONDATA_COMPLETEFLAG_INDEX]) == 0)
			return false;
		return true;
	};
};

#define MAX_COLLECTION_INDEX_COUNT	(512)
struct _CHARAC_COLLECTION_DATA	// Đơn vị dữ liệu lưu trong DB
{
	ckCollectionData		m_aData[MAX_COLLECTION_INDEX_COUNT];
};

union __charac_state
{
	short				sWoundRate;			// Tỷ lệ thương tích ngoài 0-100
	short				sInsideWoundRate;		// Tỷ lệ thương tích trong 0-100
	u_short				usFatigueRate;			// Mức độ mệt mỏi 0-100

	short				sRetribution;			// Nghiệp báo
	int					sHonor;

	short				sShowdowm;				// Thành tích tỷ võ
	short				sFuryParameter;			// Tham số phẫn nộ (giá trị phẫn nộ hiện tại)

	short				power;					// Giá trị Power
	short				stat;					// Giá trị Stat

	int					iMatchRecord;			// Giá trị liên quan đến tỷ võ
};

enum EN_STATE_PACKET_FLAG
{
	charac_state_en_wound_rate = 0,		// Thương tích ngoài
	charac_state_en_inside_rate,		// Thương tích trong 1
	charac_state_en_fatigue, 			// Mệt mỏi 2
	charac_state_en_retribution,		// Nghiệp báo 3
	charac_state_en_honor,				// Danh vọng 4
	charac_state_en_showdown,			// Thành tích tỷ võ 5
	charac_state_en_fury,				// Phẫn nộ 6
	charac_state_en_fame,				// 7 Ác danh

	charac_state_en_res2,				// 8
	charac_state_en_res3,				// 9
	charac_state_en_res4,				// 10
	charac_state_en_life_power,			// Cập nhật sinh lực 11 
	charac_state_en_force_power,		// Cập nhật nội lực 12
	charac_state_en_concentration_power,// Cập nhật ý chí 13
	charac_state_en_max_life,			// Sinh lực tối đa 14
	charac_state_en_max_force,			// Nội lực tối đa 15
	charac_state_en_max_concentration,	// Ý chí tối đa 16
	charac_state_en_constitution,		// Thể chất 17
	charac_state_en_zen,				// Tinh thần 18
	charac_state_en_intelligence,		// Trí tuệ 19
	charac_state_en_dexterity,			// Nhanh nhẹn 20
	charac_state_en_strength,			// Sức mạnh 21

	charac_state_en_match_grade = 30,		// Cập nhật phù hiệu cấp độ tỷ võ 30
	charac_state_en_match_win,			// Cập nhật số trận thắng cá nhân 31
	charac_state_en_match_lose,			// Cập nhật số trận thua cá nhân 32
	charac_state_en_match_tie,			// Cập nhật số trận hòa cá nhân 33
	charac_state_en_match_max_win_sr,	// Cập nhật số trận thắng liên tiếp tối đa cá nhân 34
	charac_state_en_match_cur_win_sr,	// Cập nhật số trận thắng liên tiếp hiện tại cá nhân 35


	charac_state_en_match_gr_win = 40,		// Cập nhật số trận thắng nhóm
	charac_state_en_match_gr_lose,			// Cập nhật số trận thua nhóm
	charac_state_en_match_gr_tie			// Cập nhật số trận hòa nhóm
};

struct _sND_TimeUnit
{
	// Năm/tháng/ngày/giờ/phút/giây EndTime
	u_int		uiYear : 5;		// base 2000
	u_int		uiMonth : 4;
	u_int		uiDay : 6;
	u_int		uiHour : 5;
	u_int		uiMinute : 6;
	u_int		uiSecond : 6;
};


#include "packet_for_login.h"
#include "packet_for_lobby.h"
#include "packet_for_zone_monster.h"
#include "packet_for_battle.h"
#include "packet_for_quest.h"
#include "packet_for_skill.h"
#include "packet_for_level.h"
#include "packet_for_ctrl_gms.h"

#include "packet_for_items_trade.h"
#include "packet_for_messenger.h"
#include "packet_for_svrmove.h"
#include "packet_for_party.h"
#include "packet_for_boss.h"	// Gói tin liên quan đến boss
#include "packet_for_nickname.h" // Gói tin liên quan đến biệt hiệu
#include "packet_for_organization.h"	// Gói tin liên quan đến bang phái
#include "packet_for_personalstore.h"	// Gói tin liên quan đến cửa hàng cá nhân

#include "packet_for_event.h"			// Gói tin sự kiện
#include "packet_for_collection.h"		// Sưu tập
#include "packet_for_unigrouplobby.h"	// Liên quan đến hợp nhất máy chủ
#include "packet_for_cashitem.h"		// Gói tin liên quan đến vật phẩm cash
#include "packet_for_cb.h"				// Gói tin liên quan đến chiến trường trang viên
#include "packet_for_xtrap.h"
#include "packet_for_pk.h"				// Gói tin liên quan đến PK (theo dõi vị trí PK...)
#include "packet_for_match.h"			// Tỷ võ


#pragma pack(pop, enter_game)