/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_GAMECLIENT_H
#define GAME_CLIENT_GAMECLIENT_H

#include <base/vmath.h>
#include <engine/client.h>
#include <engine/console.h>
#include <game/layers.h>
#include <game/localization.h>
#include <game/gamecore.h>

#include <game/teamscore.h>

#include <cstdio>

#define MIN3(x,y,z)  ((y) <= (z) ? \
	((x) <= (y) ? (x) : (y)) \
	: \
	((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
	((x) >= (y) ? (x) : (y)) \
	: \
	((x) >= (z) ? (x) : (z)))

class CGameClient;

class CWeaponData
{
public:
	int m_Tick;
	vec2 m_Pos;
	vec2 m_Direction;
	vec2 StartPos() { return m_Pos + m_Direction * 28.0f * 0.75f; }
};

class CLocalProjectile
{
public:
	int m_Active;
	CGameClient *m_pGameClient;
	CWorldCore *m_pWorld;
	CCollision *m_pCollision;

	vec2 m_Direction;
	vec2 m_Pos;
	int m_StartTick;
	int m_Type;

	int m_Owner;
	int m_Weapon;
	bool m_Explosive;
	int m_Bouncing;
	bool m_Freeze;
	bool m_ExtraInfo;

	vec2 GetPos(float Time);
	void CreateExplosion(vec2 Pos, int LocalClientID);
	void Tick(int CurrentTick, int GameTickSpeed, int LocalClientID);
	void Init(CGameClient *pGameClient, CWorldCore *pWorld, CCollision *pCollision, const CNetObj_Projectile *pProj);
	void Init(CGameClient *pGameClient, CWorldCore *pWorld, CCollision *pCollision, vec2 Vel, vec2 Pos, int StartTick, int Type, int Owner, int Weapon, bool Explosive, int Bouncing, bool Freeze, bool ExtraInfo);
	bool GameLayerClipped(vec2 CheckPos);
	void Deactivate() { m_Active = 0; }
};

class CGameClient : public IGameClient
{
	class CStack
	{
	public:
		enum
		{
			MAX_COMPONENTS = 64,
		};

		CStack();
		void Add(class CComponent *pComponent);

		class CComponent *m_paComponents[MAX_COMPONENTS];
		int m_Num;
	};

	CStack m_All;
	CStack m_Input;
	mutable CNetObjHandler m_NetObjHandler;

	class IEngine *m_pEngine;
	class IInput *m_pInput;
	class IGraphics *m_pGraphics;
	class ITextRender *m_pTextRender;
	class IClient *m_pClient;
	class ISound *m_pSound;
	class IConsole *m_pConsole;
	class IStorage *m_pStorage;
	class IDemoPlayer *m_pDemoPlayer;
	class IServerBrowser *m_pServerBrowser;
	class IEditor *m_pEditor;
	class IFriends *m_pFriends;
	class IFriends *m_pFoes;
	class IUpdater *m_pUpdater;

	CLayers m_Layers;
	class CCollision m_Collision;

	void ProcessEvents();
	void UpdatePositions();

	int m_PredictedTick;
	int m_LastNewPredictedTick[2];

	int m_LastRoundStartTick;

	int m_LastFlagCarrierRed;
	int m_LastFlagCarrierBlue;

	int m_CheckInfo[2];

	static void ConTeam(IConsole::IResult *pResult, void *pUserData);
	static void ConKill(IConsole::IResult *pResult, void *pUserData);

	static void ConchainSpecialInfoupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);
	static void ConchainSpecialDummyInfoupdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);
	static void ConchainSpecialDummy(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

public:
	IKernel *Kernel() { return IInterface::Kernel(); }
	IEngine *Engine() const { return m_pEngine; }
	class IGraphics *Graphics() const { return m_pGraphics; }
	class IClient *Client() const { return m_pClient; }
	class ISound *Sound() const { return m_pSound; }
	class IInput *Input() const { return m_pInput; }
	class IStorage *Storage() const { return m_pStorage; }
	class IConsole *Console() { return m_pConsole; }
	class ITextRender *TextRender() const { return m_pTextRender; }
	class IDemoPlayer *DemoPlayer() const { return m_pDemoPlayer; }
	class IDemoRecorder *DemoRecorder(int Recorder) const { return Client()->DemoRecorder(Recorder); }
	class IServerBrowser *ServerBrowser() const { return m_pServerBrowser; }
	class CLayers *Layers() { return &m_Layers; };
	class CCollision *Collision() { return &m_Collision; };
	class IEditor *Editor() { return m_pEditor; }
	class IFriends *Friends() { return m_pFriends; }
	class IFriends *Foes() { return m_pFoes; }
	class IUpdater *Updater() { return m_pUpdater; }

	int NetobjNumCorrections() { return m_NetObjHandler.NumObjCorrections(); }
	const char *NetobjCorrectedOn() { return m_NetObjHandler.CorrectedObjOn(); }

	bool m_SuppressEvents;
	bool m_NewTick;
	bool m_NewPredictedTick;
	int m_FlagDropTick[2];

	// TODO: move this
	CTuningParams m_Tuning[2];

	enum
	{
		SERVERMODE_PURE=0,
		SERVERMODE_MOD,
		SERVERMODE_PUREMOD,
	};
	int m_ServerMode;

	int m_AllowTimeScore[2];

	int m_DemoSpecID;

	vec2 m_LocalCharacterPos;

	// predicted players
	CCharacterCore m_PredictedPrevChar;
	CCharacterCore m_PredictedChar;

	// snap pointers
	struct CSnapState
	{
		const CNetObj_Character *m_pLocalCharacter;
		const CNetObj_Character *m_pLocalPrevCharacter;
		const CNetObj_PlayerInfo *m_pLocalInfo;
		const CNetObj_SpectatorInfo *m_pSpectatorInfo;
		const CNetObj_SpectatorInfo *m_pPrevSpectatorInfo;
		const CNetObj_Flag *m_paFlags[2];
		const CNetObj_GameInfo *m_pGameInfoObj;
		const CNetObj_GameData *m_pGameDataObj;
		int m_GameDataSnapID;

		const CNetObj_PlayerInfo *m_paPlayerInfos[MAX_CLIENTS];
		const CNetObj_PlayerInfo *m_paInfoByScore[MAX_CLIENTS];
		const CNetObj_PlayerInfo *m_paInfoByName[MAX_CLIENTS];
		//const CNetObj_PlayerInfo *m_paInfoByTeam[MAX_CLIENTS];
		const CNetObj_PlayerInfo *m_paInfoByDDTeam[MAX_CLIENTS];

		int m_LocalClientID;
		int m_NumPlayers;
		int m_aTeamSize[2];

		// spectate data
		struct CSpectateInfo
		{
			bool m_Active;
			int m_SpectatorID;
			bool m_UsePosition;
			vec2 m_Position;
		} m_SpecInfo;

		//
		struct CCharacterInfo
		{
			bool m_Active;

			// snapshots
			CNetObj_Character m_Prev;
			CNetObj_Character m_Cur;

			// interpolated position
			vec2 m_Position;
		};

		CCharacterInfo m_aCharacters[MAX_CLIENTS];
	};

	CSnapState m_Snap;

	// client data
	struct CClientData
	{
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;

		char m_aName[MAX_NAME_LENGTH];
		char m_aClan[MAX_CLAN_LENGTH];
		int m_Country;
		char m_aSkinName[64];
		int m_SkinID;
		int m_SkinColor;
		int m_Team;
		int m_Emoticon;
		int m_EmoticonStart;
		CCharacterCore m_Predicted;
		CCharacterCore m_PrevPredicted;

		float m_Angle;
		bool m_Active;
		bool m_ChatIgnore;
		bool m_Friend;
		bool m_Foe;

		void UpdateRenderInfo();
		void Reset();

		// DDRace

		int m_Score;
	};

	CClientData m_aClients[MAX_CLIENTS];

	class CClientStats
	{
		int m_IngameTicks;
		int m_JoinTick;
		bool m_Active;
		
	public:
		CClientStats();

		int m_aFragsWith[NUM_WEAPONS];
		int m_aDeathsFrom[NUM_WEAPONS];
		int m_Frags;
		int m_Deaths;
		int m_Suicides;
		int m_BestSpree;
		int m_CurrentSpree;

		int m_FlagGrabs;
		int m_FlagCaptures;

		void Reset();
		
		bool IsActive() const { return m_Active; }
		void JoinGame(int Tick) { m_Active = true; m_JoinTick = Tick; };
		void JoinSpec(int Tick) { m_Active = false; m_IngameTicks += Tick - m_JoinTick; };
		int GetIngameTicks(int Tick) const { return m_IngameTicks + Tick - m_JoinTick; };
		float GetFPM(int Tick, int TickSpeed) const { return (float)(m_Frags * TickSpeed * 60) / GetIngameTicks(Tick); };
	};

	CClientStats m_aStats[MAX_CLIENTS];

	void OnReset();

	// hooks
	virtual void OnConnected();
	virtual void OnRender();
	virtual void OnUpdate();
	virtual void OnDummyDisconnect();
	virtual void OnRelease();
	virtual void OnInit();
	virtual void OnConsoleInit();
	virtual void OnStateChange(int NewState, int OldState);
	virtual void OnMessage(int MsgId, CUnpacker *pUnpacker, bool IsDummy = 0);
	virtual void OnNewSnapshot();
	virtual void OnPredict();
	virtual void OnActivateEditor();
	virtual void OnDummySwap();
	virtual int OnSnapInput(int *pData, bool Dummy, bool Force);
	virtual void OnShutdown();
	virtual void OnEnterGame();
	virtual void OnRconType(bool UsernameReq);
	virtual void OnRconLine(const char *pLine);
	virtual void OnTimeScore(int AllowTimeScore, bool Dummy);
	virtual void OnGameOver();
	virtual void OnStartGame();
	virtual void OnFlagGrab(int TeamID);

	void OnWindowResize();
	static void OnWindowResizeCB(void *pUser);

	const char *GetItemName(int Type) const;
	const char *Version() const override;
	const char *NetVersion() const override;
	const char *NetVersion7() const override;
	int DDNetVersion() const override;
	const char *DDNetVersionStr() const override;
	virtual int ClientVersion7() const override;
	char m_aDDNetVersionStr[64];

	// actions
	// TODO: move these
	void SendSwitchTeam(int Team);
	void SendInfo(bool Start);
	virtual void SendDummyInfo(bool Start);
	void SendKill(int ClientID);

	// pointers to all systems
	class CGameConsole *m_pGameConsole;
	class CBinds *m_pBinds;
	class CParticles *m_pParticles;
	class CMenus *m_pMenus;
	class CSkins *m_pSkins;
	class CCountryFlags *m_pCountryFlags;
	class CFlow *m_pFlow;
	class CChat *m_pChat;
	class CDamageInd *m_pDamageind;
	class CCamera *m_pCamera;
	class CControls *m_pControls;
	class CEffects *m_pEffects;
	class CSounds *m_pSounds;
	class CMotd *m_pMotd;
	class CMapImages *m_pMapimages;
	class CVoting *m_pVoting;
	class CScoreboard *m_pScoreboard;
	class CStatboard *m_pStatboard;
	class CItems *m_pItems;
	class CMapLayers *m_pMapLayersBackGround;
	class CMapLayers *m_pMapLayersForeGround;
	class CBackground *m_pBackGround;

	class CMapSounds *m_pMapSounds;
	class CPlayers *m_pPlayers;

	// DDRace

	int m_LocalIDs[2];
	CNetObj_PlayerInput m_DummyInput;
	CNetObj_PlayerInput m_HammerInput;
	int m_DummyFire;

	class CRaceDemo *m_pRaceDemo;
	class CGhost *m_pGhost;
	class CTeamsCore m_Teams;

	class CChillerBotUX *m_pChillerBotUX;
	class CChatHelper *m_pChatHelper;

	int IntersectCharacter(vec2 Pos0, vec2 Pos1, vec2& NewPos, int ownID);
	int IntersectCharacter(vec2 OldPos, vec2 NewPos, float Radius, vec2* NewPos2, int ownID, CWorldCore *World);

	CWeaponData m_aWeaponData[150];
	CWeaponData *GetWeaponData(int Tick) { return &m_aWeaponData[((Tick%150)+150)%150]; }
	CWeaponData *FindWeaponData(int TargetTick);

	void FindWeaker(bool IsWeaker[2][MAX_CLIENTS]);

	bool AntiPingPlayers() { return g_Config.m_ClAntiPing && g_Config.m_ClAntiPingPlayers && !m_Snap.m_SpecInfo.m_Active && Client()->State() != IClient::STATE_DEMOPLAYBACK && (m_Tuning[g_Config.m_ClDummy].m_PlayerCollision || m_Tuning[g_Config.m_ClDummy].m_PlayerHooking); }
	bool AntiPingGrenade() { return g_Config.m_ClAntiPing && g_Config.m_ClAntiPingGrenade && !m_Snap.m_SpecInfo.m_Active && Client()->State() != IClient::STATE_DEMOPLAYBACK; }
	bool AntiPingWeapons() { return g_Config.m_ClAntiPing && g_Config.m_ClAntiPingWeapons && !m_Snap.m_SpecInfo.m_Active && Client()->State() != IClient::STATE_DEMOPLAYBACK; }

private:
	bool m_DDRaceMsgSent[2];
	int m_ShowOthers[2];

	// chillerbot-ng
	void ConsoleKeyInput();
	void StartInputThread(int mode);
	void ChillerCommands(const char *pCmd);
	void ShowServerList();
	void PenetrateServer();
	void ChillerBotTick();
	const char *GetPentestCommand(const char *pFileName);
	const char *GetRandomChatCommand();
	int m_PenDelay;
	int m_InputMode;
	int64 m_EnterGameTime;

	enum {
		INPUT_CHAT,
		INPUT_LOCAL_CONSOLE,
		INPUT_RCON_CONSOLE,
		INPUT_CHILLER_CONSOLE
	};

public:
	std::vector<char*> m_vChatCmds;
	int64 m_RequestCmdlist;
};

// chillerbot-ng class less thread stuff
void ConsoleKeyInputThread(void *pArg);

inline float HueToRgb(float v1, float v2, float h)
{
	if(h < 0.0f) h += 1;
	if(h > 1.0f) h -= 1;
	if((6.0f * h) < 1.0f) return v1 + (v2 - v1) * 6.0f * h;
	if((2.0f * h) < 1.0f) return v2;
	if((3.0f * h) < 2.0f) return v1 + (v2 - v1) * ((2.0f/3.0f) - h) * 6.0f;
	return v1;
}

inline vec3 HslToRgb(vec3 HSL)
{
	if(HSL.s == 0.0f)
		return vec3(HSL.l, HSL.l, HSL.l);
	else
	{
		float v2 = HSL.l < 0.5f ? HSL.l * (1.0f + HSL.s) : (HSL.l+HSL.s) - (HSL.s*HSL.l);
		float v1 = 2.0f * HSL.l - v2;

		return vec3(HueToRgb(v1, v2, HSL.h + (1.0f/3.0f)), HueToRgb(v1, v2, HSL.h), HueToRgb(v1, v2, HSL.h - (1.0f/3.0f)));
	}
}

inline vec3 RgbToHsl(vec3 RGB)
{
	vec3 HSL;
	float MaxColor = MAX3(RGB.r, RGB.g, RGB.b);
	float MinColor = MIN3(RGB.r, RGB.g, RGB.b);
	if (MinColor == MaxColor)
		return vec3(0.0f, 0.0f, RGB.g * 255.0f);
	else
	{
		HSL.l = (MinColor + MaxColor) / 2;

		if (HSL.l < 0.5)
			HSL.s = (MaxColor - MinColor) / (MaxColor + MinColor);
		else
			HSL.s = (MaxColor - MinColor) / (2.0 - MaxColor - MinColor);

		if (RGB.r == MaxColor)
			HSL.h = (RGB.g - RGB.b) / (MaxColor - MinColor);
		else if (RGB.g == MaxColor)
			HSL.h = 2.0 + (RGB.b - RGB.r) / (MaxColor - MinColor);
		else
			HSL.h = 4.0 + (RGB.r - RGB.g) / (MaxColor - MinColor);

		HSL.h /= 6; //to bring it to a number between 0 and 1
		if (HSL.h < 0) HSL.h++;
	}
	HSL.h = int(HSL.h * 255.0);
	HSL.s = int(HSL.s * 255.0);
	HSL.l = int(HSL.l * 255.0);
	return HSL;

}

vec3 CalculateNameColor(vec3 TextColorHSL);

#endif
