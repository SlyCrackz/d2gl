/*
	D2GL: Diablo 2 LoD Glide/DDraw to OpenGL Wrapper.
	Copyright (C) 2023  Bayaraa

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "pch.h"
#include "mini_map.h"
#include "d2/common.h"
#include "hd_text.h"

namespace d2gl::modules {

MiniMap::MiniMap()
{
	m_map = std::make_unique<Object>();
	m_bg = std::make_unique<Object>();
	m_bg->setColor(0x00000099, 1);
	m_bg->setColor(0x222222EE, 2);
}

void MiniMap::resize()
{
	const glm::vec2 zoom = App.viewport.scale;
	const glm::vec2 tex_size = { (float)App.game.size.x, (float)App.game.size.y };

	glm::vec2 padding = 3.0f / zoom;
	m_size = { (float)App.mini_map.width.value, (float)App.mini_map.height.value };
	m_pos = { (float)App.game.size.x - m_size.x - 5.0f - padding.x * 2.0f, 18.0f };

	float r_x = (tex_size.x / zoom.x - m_size.x) / 2 / (tex_size.x / zoom.x);
	float r_y = (tex_size.y / zoom.y - m_size.y) / 2 / (tex_size.y / zoom.y);

	m_bg->setSize(m_size + padding * 2.0f);
	m_bg->setExtra(m_size + padding * 2.0f);
	m_bg->setPosition(m_pos);

	m_map->setSize(m_size);
	m_map->setPosition(m_pos + padding);
	m_map->setTexCoord({ r_x, r_y, 1.0f - r_x, 1.0f - r_y });
}

void MiniMap::draw()
{
	static wchar_t time_str[20] = { 0 };
	static tm gmt_time;

	if (*d2::screen_shift == SCREENPANEL_NONE) {
		if (!d2::isEscMenuOpen() && !*d2::help_menu_open && !*d2::automap_on) {
			m_bg->setFlags(2, 3, 100);
			m_map->setFlags(5, 0, 100);
			App.context->pushObject(m_bg);
			App.context->pushObject(m_map);
		}

		if (App.hd_text.active && !*d2::help_menu_open) {
			time_t now = time(0);
			localtime_s(&gmt_time, &now);
			int hour = gmt_time.tm_hour % 12;
			if (hour == 0)
				hour = 12;
			swprintf_s(time_str, L" | �c\x34%.2d:%.2d %s", hour, gmt_time.tm_min, (gmt_time.tm_hour >= 12) ? L"PM" : L"AM");

			const auto old_size = modules::HDText::Instance().getTextSize();
			d2::setTextSizeHooked(19);
			m_time_width = d2::getNormalTextWidthHooked(time_str);
			d2::drawNormalTextHooked(time_str, App.game.size.x - m_time_width - 5, 13, 5, 0);
			d2::setTextSizeHooked(old_size);
		}
	}
}

}