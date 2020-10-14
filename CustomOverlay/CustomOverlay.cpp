#include "CustomOverlay.h"
#include <stdlib.h>

BAKKESMOD_PLUGIN(CustomOverlay, "Custom Overlay", "2.0", PERMISSION_CUSTOM_TRAINING | PERMISSION_FREEPLAY | PERMISSION_OFFLINE | PERMISSION_ONLINE | PERMISSION_MENU | PERMISSION_PAUSEMENU_CLOSED | PERMISSION_SOCCAR)

void CustomOverlay::onLoad()
{
	overlayOn = std::make_shared<bool>(false);

	cvarManager->registerCvar("overlayplugin_enabled", "0", "Enables the overlay plugin.", true, false, 0.0F, false, 0.0F).bindTo(overlayOn);
	cvarManager->getCvar("overlayplugin_enabled").addOnValueChanged(std::bind(&CustomOverlay::OnOverlayChanged, this, std::placeholders::_1, std::placeholders::_2));

	if (*overlayOn)
		HookAllEvents();

	DisablePsyUI(false);
}

void CustomOverlay::HookAllEvents()
{
	if (hooked)
		return;

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Active.StartRound", bind(&CustomOverlay::OnGameLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", bind(&CustomOverlay::OnGameDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&CustomOverlay::OnGameDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.EventExploded", bind(&CustomOverlay::OnGameDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Team_TA.AddPlayerToTeam", bind(&CustomOverlay::OnGameLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Team_TA.RemovePlayerFromTeam", bind(&CustomOverlay::OnGameDestroy, this, std::placeholders::_1));

	hooked = true;
}

void CustomOverlay::UnhookAllEvents()
{
	if (!hooked)
		return;

	gameWrapper->UnhookEvent("Function GameEvent_Soccar_TA.Active.StartRound");
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded");
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed");
	gameWrapper->UnhookEvent("Function TAGame.Ball_TA.EventExploded");
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Team_TA.AddPlayerToTeam");
	gameWrapper->UnhookEvent("Function TAGame.GameEvent_Team_TA.RemovePlayerFromTeam");

	hooked = false;
}

void CustomOverlay::DisablePsyUI(bool disabled)
{
	if (cvarManager->getCvar("cl_rendering_scaleform_disabled").getIntValue() != disabled)
		cvarManager->getCvar("cl_rendering_scaleform_disabled").setValue(disabled);
}

int CustomOverlay::getBoostAmount()
{
	CarWrapper localCar = gameWrapper->GetLocalCar();
	if (localCar.IsNull())
		return -1;

	BoostWrapper boostComponent = localCar.GetBoostComponent();
	if (boostComponent.IsNull())
		return -1;

	return boostComponent.GetCurrentBoostAmount() * 100;
}

int CustomOverlay::getGameTime()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (!gameWrapper->IsInGame())
	{
		if (!gameWrapper->IsInOnlineGame())
			return -1;
		else if (onlineServer.IsNull())
			return -1;

		return onlineServer.GetSecondsRemaining();
	}
	else if (localServer.IsNull())
		return -1;

	return localServer.GetSecondsRemaining();
}

TeamWrapper CustomOverlay::getMyTeam()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PlayerControllerWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer();

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : localServerTeams)
				if (localServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2())
					return team;
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams)
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2())
					return team;
	}
}

TeamWrapper CustomOverlay::getOpposingTeam()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PlayerControllerWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer();

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : localServerTeams)
				if (localServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2())
					return team;
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams)
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2())
					return team;
	}
}

LinearColor CustomOverlay::getTeamColor(TeamWrapper team)
{
	if (!team.IsNull())
		return team.GetCurrentColorList().Get(0);
}

void CustomOverlay::UpdateVars()
{
	if (!*overlayOn)
		return;

	boost = getBoostAmount();

	gameTime = (getGameTime() != -1) ? (std::to_string(getGameTime() / 60) + ":" + lead_zeros(getGameTime() % 60, 2)) : std::to_string(-1);

	myTeamColor = getTeamColor(getMyTeam());
	opposingTeamColor = getTeamColor(getOpposingTeam());
}

void CustomOverlay::OnGameLoad(std::string eventName)
{
	if (!gameWrapper->IsInGame())
		if (!gameWrapper->IsInOnlineGame())
			return;

	if (!loaded)
	{
		if (*overlayOn)
			gameWrapper->RegisterDrawable(std::bind(&CustomOverlay::Render, this, std::placeholders::_1));

		loaded = true;
	}

	DisablePsyUI(true);
}

void CustomOverlay::OnGameDestroy(std::string eventName)
{
	if (loaded)
	{
		gameWrapper->UnregisterDrawables();

		loaded = false;
	}

	DisablePsyUI(false);
}

void CustomOverlay::OnOverlayChanged(std::string oldValue, CVarWrapper cvar)
{
	if (oldValue.compare("0") == 0 && cvar.getBoolValue())
	{
		HookAllEvents();
		OnGameLoad("Load");
	}
	else if (oldValue.compare("1") == 0 && !cvar.getBoolValue())
	{
		UnhookAllEvents();
		OnGameDestroy("Destroy");
	}
}

void CustomOverlay::Render(CanvasWrapper canvas)
{
	if (!*overlayOn)
		return;

	if (!gameWrapper->IsInOnlineGame())
		if (!gameWrapper->IsInGame())
			return;

	UpdateVars();

	screenSize = canvas.GetSize();

	boostBoxSize.X = (int)((screenSize.X * 11.97F) / 100);
	boostBoxSize.Y = (int)((screenSize.X * 11.97F) / 100);
	boostBoxPosition.X = (screenSize.X - boostBoxSize.X) - (int)((screenSize.X * 2.6F) / 100);
	boostBoxPosition.Y = (screenSize.Y - boostBoxSize.Y) - (int)((screenSize.X * 2.6F) / 100);
	boostTextSize = canvas.GetStringSize(std::to_string(boost), (float)((int)((boostBoxSize.X / 24) * 10) / 10), (float)((int)((boostBoxSize.Y / 24) * 10) / 10));
	boostTextPosition.X = ((screenSize.X - (boostBoxSize.X / 2)) - (int)((screenSize.X * 3) / 100)) - (boostTextSize.X / 2);
	boostTextPosition.Y = ((screenSize.Y - (boostBoxSize.Y / 2)) - (int)((screenSize.X * 3) / 100)) - (boostTextSize.Y / 2);

	canvas.SetPosition(boostBoxPosition);
	if (boost > 80)
		canvas.SetColor(200, 55, 55, 235);
	else if (boost < 11)
		canvas.SetColor(127, 127, 127, 200);
	else
		canvas.SetColor(150, 90, 40, 200);
	canvas.FillBox(boostBoxSize);

	canvas.SetPosition(boostTextPosition);
	if (boost > 80)
		canvas.SetColor(255, 215, 180, 255);
	else
		canvas.SetColor(255, 235, 130, 255);
	canvas.DrawString(std::to_string(boost), (float)((int)((boostBoxSize.X / 24) * 10) / 10), (float)((int)((boostBoxSize.Y / 24) * 10) / 10));

	scoreBoxCenterSize.X = (int)((screenSize.X * 14.58F) / 100);
	scoreBoxCenterSize.Y = (int)((screenSize.Y * 8.33F) / 100);
	scoreBoxCenterPosition.X = (screenSize.X / 2) - (scoreBoxCenterSize.X / 2);
	scoreBoxCenterPosition.Y = 0;
	scoreBoxCenterTextSize = canvas.GetStringSize(gameTime, (float)((int)((scoreBoxCenterSize.X / 40) * 10) / 10), (float)((int)((scoreBoxCenterSize.X / 40) * 10) / 10));
	scoreBoxCenterTextPosition.X = (screenSize.X / 2) - (scoreBoxCenterTextSize.X / 2);
	scoreBoxCenterTextPosition.Y = (scoreBoxCenterSize.Y / 2) - (scoreBoxCenterTextSize.Y / 2);

	canvas.SetPosition(scoreBoxCenterPosition);
	canvas.SetColor(0, 0, 0, 200);
	canvas.FillBox(scoreBoxCenterSize);
	canvas.SetPosition(scoreBoxCenterTextPosition);
	canvas.SetColor(255, 255, 255, 255);
	canvas.DrawString(gameTime, (float)((int)((scoreBoxCenterSize.X / 40) * 10) / 10), (float)((int)((scoreBoxCenterSize.X / 40) * 10) / 10));

	scoreBoxLeftSize.X = (int)(((screenSize.X * 4.17F) / 100) * 1.25F);
	scoreBoxLeftSize.Y = scoreBoxCenterSize.Y;
	scoreBoxLeftPosition.X = scoreBoxCenterPosition.X - scoreBoxLeftSize.X;
	scoreBoxLeftPosition.Y = 0;
	scoreBoxLeftTextSize = canvas.GetStringSize(std::to_string(getMyTeam().GetScore()), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10));
	scoreBoxLeftTextPosition.X = (scoreBoxLeftPosition.X + (scoreBoxLeftSize.X / 2)) - (scoreBoxLeftTextSize.X / 2);
	scoreBoxLeftTextPosition.Y = (scoreBoxLeftSize.Y / 2) - (scoreBoxLeftTextSize.Y / 2);

	canvas.SetPosition(scoreBoxLeftPosition);
	canvas.SetColor(myTeamColor.R * 255, myTeamColor.G * 255, myTeamColor.B * 255, 200);
	canvas.FillBox(scoreBoxLeftSize);
	canvas.SetPosition(scoreBoxLeftTextPosition);
	canvas.SetColor(255, 252, 250, 255);
	canvas.DrawString(std::to_string(getMyTeam().GetScore()), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10));

	scoreBoxRightPosition.X = scoreBoxCenterPosition.X + scoreBoxCenterSize.X;
	scoreBoxRightPosition.Y = 0;
	scoreBoxRightTextSize = canvas.GetStringSize(std::to_string(getOpposingTeam().GetScore()), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10));
	scoreBoxRightTextPosition.X = (scoreBoxRightPosition.X + (scoreBoxLeftSize.X / 2)) - (scoreBoxRightTextSize.X / 2);
	scoreBoxRightTextPosition.Y = (scoreBoxLeftSize.Y / 2) - (scoreBoxRightTextSize.Y / 2);

	canvas.SetPosition(scoreBoxRightPosition);
	canvas.SetColor(opposingTeamColor.R * 255, opposingTeamColor.G * 255, opposingTeamColor.B * 255, 200);
	canvas.FillBox(scoreBoxLeftSize);
	canvas.SetPosition(scoreBoxRightTextPosition);
	canvas.SetColor(255, 252, 250, 255);
	canvas.DrawString(std::to_string(getOpposingTeam().GetScore()), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10), (float)((int)((scoreBoxLeftSize.X / 16) * 10) / 10));
}

void CustomOverlay::onUnload()
{
	cvarManager->log("mmmm yes despacito");
}

inline std::string CustomOverlay::lead_zeros(int n, int len)
{
	std::string result(len--, '0');

	for (int val = (n < 0) ? -n : n; len >= 0 && val != 0; --len, val /= 10)
		result[len] = '0' + val % 10;

	if (len >= 0 && n < 0)
		result[0] = '-';

	return result;
}
