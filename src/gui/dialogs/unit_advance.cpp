/*
   Copyright (C) 2016 - 2018 by the Battle for Wesnoth Project https://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/unit_advance.hpp"

#include "gui/auxiliary/find_widget.hpp"
#include "gui/widgets/button.hpp"
#include "gui/widgets/image.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/unit_preview_pane.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "units/unit.hpp"
#include "help/help.hpp"

#include "utils/functional.hpp"

namespace gui2
{
namespace dialogs
{

REGISTER_DIALOG(unit_advance)

unit_advance::unit_advance(const unit_ptr_vector& samples, size_t real)
	: previews_(samples)
	, selected_index_(0)
	, last_real_advancement_(real)
{
}

void unit_advance::pre_show(window& window)
{
	listbox& list = find_widget<listbox>(&window, "advance_choice", false);

	connect_signal_notify_modified(list, std::bind(&unit_advance::list_item_clicked, this, std::ref(window)));

	window.keyboard_capture(&list);

	connect_signal_mouse_left_click(
		find_widget<button>(&window, "show_help", false),
		std::bind(&unit_advance::show_help, this));

	for(size_t i = 0; i < previews_.size(); i++) {
		const unit& sample = *previews_[i];

		std::map<std::string, string_map> row_data;
		string_map column;

		std::string image_string, name = sample.type_name();

		// This checks if we've finished iterating over the last unit type advancements
		// and are into the modification-based advancements.
		if(i >= last_real_advancement_) {
			const auto& back = sample.get_modifications().child_range("advancement").back();

			if(back.has_attribute("image")) {
				image_string = back["image"].str();
			}

			name = back["description"].str();
		}

		if(image_string.empty()) {
			image_string = sample.type().image() + sample.image_mods();
		}

		column["label"] = image_string;
		row_data.emplace("advancement_image", column);

		column["label"] = name;
		row_data.emplace("advancement_name", column);

		list.add_row(row_data);
	}

	list_item_clicked(window);

	// Disable ESC existing
	window.set_escape_disabled(true);
}

void unit_advance::list_item_clicked(window& window)
{
	const int selected_row
		= find_widget<listbox>(&window, "advance_choice", false).get_selected_row();

	if(selected_row == -1) {
		return;
	}

	find_widget<unit_preview_pane>(&window, "advancement_details", false)
		.set_displayed_unit(*previews_[selected_row]);
}

void unit_advance::show_help()
{
	help::show_help("advancement");
}

void unit_advance::post_show(window& window)
{
	if(get_retval() == retval::OK) {
		selected_index_ = find_widget<listbox>(&window, "advance_choice", false)
			.get_selected_row();
	}
}

} // namespace dialogs
} // namespace gui2
