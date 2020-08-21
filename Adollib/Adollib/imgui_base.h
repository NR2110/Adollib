#pragma once

class imgui_base {
public:
	bool draw_flag;
	virtual void render() = 0;
};