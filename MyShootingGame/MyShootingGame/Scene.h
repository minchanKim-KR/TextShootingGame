#pragma once

// 각 Scene들의 Scene 변경을 위한 헤더.

enum SceneId
{
	scene_TITLE, 
	scene_LOAD, 
	scene_GAME, 
	scene_CLEAR, 
	scene_OVER, 
	scene_EXIT
};

extern int g_Scene;