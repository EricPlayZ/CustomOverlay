#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

class CustomOverlay : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	virtual void onLoad();
	virtual void onUnload();
	
	void disablePsyUI(bool disabled);

	int getBoostAmount();
	int getGameTime();

	TeamWrapper getMyTeam();
	TeamWrapper getOpposingTeam();
	UnrealColor getTeamColor(TeamWrapper team);

	void OnGameLoad(std::string eventName);
	void OnGameDestroy(std::string eventName);
	void OnOverlayChanged(std::string oldValue, CVarWrapper cvar);
	void Render(CanvasWrapper canvas);
private:
	bool hooked;
	bool loaded;
	std::shared_ptr<bool> overlayOn;

	int boost;

	std::string gameTime;

	UnrealColor myTeamColor;
	UnrealColor opposingTeamColor;

	Vector2 screenSize;

	Vector2 boostBoxSize;
	Vector2 boostBoxPosition;
	Vector2F boostTextSize;
	Vector2 boostTextPosition;

	Vector2 scoreBoxCenterSize;
	Vector2 scoreBoxCenterPosition;
	Vector2F scoreBoxCenterTextSize;
	Vector2 scoreBoxCenterTextPosition;

	Vector2 scoreBoxLeftSize;
	Vector2 scoreBoxLeftPosition;
	Vector2F scoreBoxLeftTextSize;
	Vector2 scoreBoxLeftTextPosition;

	Vector2 scoreBoxRightPosition;
	Vector2F scoreBoxRightTextSize;
	Vector2 scoreBoxRightTextPosition;

	inline std::string lead_zeros(int n, int len);
};
