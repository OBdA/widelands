/*
 * Copyright (C) 2006-2019 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "graphic/text_layout.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "graphic/font_handler.h"
#include "graphic/graphic.h"
#include "graphic/image.h"
#include "graphic/style_manager.h"
#include "graphic/text/font_set.h"

namespace {
bool is_paragraph(const std::string& text) {
	return boost::starts_with(text, "<p");
}

bool is_div(const std::string& text) {
	return boost::starts_with(text, "<div");
}

std::string as_richtext_paragraph(const std::string& text, UI::Align align) {
	std::string alignment = "left";
	switch (align) {
	case UI::Align::kCenter:
		alignment = "center";
		break;
	case UI::Align::kRight:
		alignment = "right";
		break;
	case UI::Align::kLeft:
		alignment = "left";
		break;
	}

	static boost::format f("<rt><p align=%s>%s</p></rt>");
	f % alignment;
	f % text;
	return f.str();
}
}  // namespace

int text_width(const std::string& text, const UI::FontStyleInfo& style, float scale) {
	UI::FontStyleInfo info(style);
	info.set_size(info.size() * scale - UI::g_fh->fontset()->size_offset());
	return UI::g_fh
	   ->render(as_editor_richtext_paragraph(text, info))->width();
}

void replace_entities(std::string* text) {
	boost::replace_all(*text, "&gt;", ">");
	boost::replace_all(*text, "&lt;", "<");
	boost::replace_all(*text, "&nbsp;", " ");
	boost::replace_all(*text, "&amp;", "&");  // Must be performed last
}

int text_height(const UI::FontStyleInfo& style, float scale) {
	UI::FontStyleInfo info(style);
	info.set_size(info.size() * scale - UI::g_fh->fontset()->size_offset());
	return UI::g_fh
	   ->render(as_richtext_paragraph(UI::g_fh->fontset()->representative_character(), info))
	   ->height();
}

int text_height(UI::FontStyle style, float scale) {
	return text_height(g_gr->styles().font_style(style), scale);
}

std::string richtext_escape(const std::string& given_text) {
	std::string text = given_text;
	boost::replace_all(text, "&", "&amp;");  // Must be performed first
	boost::replace_all(text, ">", "&gt;");
	boost::replace_all(text, "<", "&lt;");
	return text;
}

/// Bullet list item
std::string as_listitem(const std::string& txt, UI::FontStyle style) {
	static boost::format f("<div width=100%%><div><p><font size=%d "
	                       "color=%s>•</font></p></div><div><p><space gap=6></p></div><div "
	                       "width=*><p><font size=%d color=%s>%s<vspace "
	                       "gap=6></font></p></div></div>");
	const UI::FontStyleInfo& font_style = g_gr->styles().font_style(style);
	f % font_style.size() % font_style.color().hex_value() % font_style.size() % font_style.color().hex_value() % txt;
	return f.str();
}

std::string as_richtext(const std::string& txt) {
	static boost::format f("<rt>%s</rt>");
	f % txt;
	return f.str();
}

std::string as_richtext_paragraph(const std::string& text, UI::FontStyle style, UI::Align align) {
	return as_richtext_paragraph(text, g_gr->styles().font_style(style), align);
}

std::string as_richtext_paragraph(const std::string& text, const UI::FontStyleInfo& style, UI::Align align) {
	return as_richtext_paragraph(style.as_font_tag(text), align);
}
// NOCOM use FontStyle instead?
std::string as_editor_richtext_paragraph(const std::string& text, const UI::FontStyleInfo& style) {
	static boost::format f("<rt keep_spaces=1><p>%s</p></rt>");
	f % style.as_font_tag(text);
	return f.str();
}


std::string as_game_tip(const std::string& txt) {
	static boost::format f("<rt padding_l=48 padding_t=28 padding_r=48 padding_b=28>"
	                       "<p align=center>%s</p></rt>");
	f % g_gr->styles().font_style(UI::FontStyle::kFsMenuGameTip).as_font_tag(txt);
	return f.str();
}

std::string as_message(const std::string& heading, const std::string& body) {
	return (
	   (boost::format(
	       "<rt><p>%s<br></p><vspace gap=6>%s</rt>") %
	    g_gr->styles().font_style(UI::FontStyle::kWuiMessageHeading).as_font_tag(heading) %
	    (is_paragraph(body) || is_div(body) ?
	        body :
	        (boost::format("<p>%s</p>") %
	         g_gr->styles().font_style(UI::FontStyle::kWuiMessageParagraph).as_font_tag(body))
	           .str()))
	      .str());
}

std::shared_ptr<const UI::RenderedText> autofit_text(const std::string& text,
																	  const UI::FontStyleInfo& font_info,
																	  int width) {
	const std::string text_to_render = richtext_escape(text);
	std::shared_ptr<const UI::RenderedText> rendered_text =
	   UI::g_fh->render(as_richtext_paragraph(text_to_render, font_info));

	// Autoshrink if it doesn't fit
	if (width > 0 && rendered_text->width() > width) {
		const int minimum_size = g_gr->styles().minimum_font_size();
		// We take a copy, because we are changing values during the autofit.
		UI::FontStyleInfo temp_font_info(font_info);
		temp_font_info.make_condensed();
		while (rendered_text->width() > width && temp_font_info.size() >= minimum_size) {
			rendered_text = UI::g_fh->render(as_richtext_paragraph(text_to_render, temp_font_info));
			temp_font_info.set_size(temp_font_info.size() - 1);
		}
	}
	return rendered_text;
}

std::string as_heading_with_content(const std::string& header,
                                    const std::string& content,
                                    UI::PanelStyle style,
                                    bool is_first,
                                    bool noescape) {
	switch (style) {
	case UI::PanelStyle::kFsMenu:
		return (boost::format("<p>%s%s %s</p>") %
		         (is_first ? "" : "<vspace gap=9>") %
		        g_gr->styles().font_style(UI::FontStyle::kFsMenuInfoPanelHeading).as_font_tag(noescape ? header : richtext_escape(header)) %
		        g_gr->styles().font_style(UI::FontStyle::kFsMenuInfoPanelParagraph).as_font_tag(noescape ? content : richtext_escape(content)))
		   .str();
	case UI::PanelStyle::kWui:
		return (boost::format("<p>%s%s %s</p>") %
		        (is_first ? "" : "<vspace gap=6>") %
		        g_gr->styles().font_style(UI::FontStyle::kWuiInfoPanelHeading).as_font_tag(noescape ? header : richtext_escape(header)) %
		        g_gr->styles().font_style(UI::FontStyle::kWuiInfoPanelParagraph).as_font_tag(noescape ? content : richtext_escape(content)))
		   .str();
	}
	NEVER_HERE();
}

std::string as_heading(const std::string& txt, UI::PanelStyle style, bool is_first) {
	switch (style) {
	case UI::PanelStyle::kFsMenu:
		return (boost::format("<p>%s%s</p>") % (is_first ? "" : "<vspace gap=9>") %
		        g_gr->styles()
		           .font_style(UI::FontStyle::kFsMenuInfoPanelHeading)
		           .as_font_tag(richtext_escape(txt)))
		   .str();
	case UI::PanelStyle::kWui:
		return (boost::format("<p>%s%s</p>") % (is_first ? "" : "<vspace gap=6>") %
		        g_gr->styles()
		           .font_style(UI::FontStyle::kWuiInfoPanelHeading)
		           .as_font_tag(richtext_escape(txt)))
		   .str();
	}
	NEVER_HERE();
}

std::string as_content(const std::string& txt, UI::PanelStyle style) {
	switch (style) {
	case UI::PanelStyle::kFsMenu:
		return (boost::format("<p><vspace gap=2>%s</p>") %
		        g_gr->styles()
		           .font_style(UI::FontStyle::kFsMenuInfoPanelParagraph)
		           .as_font_tag(richtext_escape(txt)))
		   .str();
	case UI::PanelStyle::kWui:
		return (boost::format("<p><vspace gap=2>%s</p>") %
		        g_gr->styles()
		           .font_style(UI::FontStyle::kWuiInfoPanelParagraph)
		           .as_font_tag(richtext_escape(txt)))
		   .str();
	}
	NEVER_HERE();
}
