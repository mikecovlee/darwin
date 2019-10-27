#define DARWIN_FORCE_BUILTIN
#define DARWIN_DISABLE_LOG
#include "../darwin/darwin.hpp"
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
using namespace darwin;

namespace keymap {
	constexpr int key_esc = 27;
	constexpr int key_enter = 10;
	constexpr int key_delete = 127;
	constexpr int key_up = 'w';
	constexpr int key_down = 's';
	constexpr int key_left = 'a';
	constexpr int key_right = 'd';
	constexpr int key_mode = 'i';
	constexpr int key_quit = 'q';
	constexpr int key_reload = 'r';
	constexpr int key_save = 'x';
} // namespace keymap

class texteditor final {
	std::size_t render_border = 0, render_offx = 0, render_offy = 0;
	std::size_t cursor_count = 0, cursor_x = 0, cursor_y = 0;
	std::size_t last_win_width = 0, last_win_height = 0;
	std::vector<std::string> file_buffer;
	darwin::drawable *pic = nullptr;
	bool text_modified = false;
	bool insert_mode = false;
	std::string char_buffer;
	std::string file_path;

	enum class await_process_type {
		null,
		quit,
		reload
	} await_process = await_process_type::null;

	enum class unsaved_status_type {
		null,
		asking,
		confirm
	} unsaved_status = unsaved_status_type::null;

public:
	int tab_indent = 4;
	void load(const std::string &path)
	{
		std::ifstream in(file_path);
		for (std::string line; std::getline(in, line); file_buffer.push_back(line)) {
			for (std::size_t i = 0; i < line.size(); ++i) {
				if (line[i] == '\t') {
					line[i] = ' ';
					for (std::size_t count = 0; count < tab_indent - 1; ++count)
						line.insert(line.begin() + i, ' ');
				}
			}
		}
	}
	texteditor(const std::string &path) : file_path(path)
	{
		load(file_path);
	}

private:
	std::size_t text_area_width()
	{
		return pic->get_width() - render_border - 5;
	}
	std::size_t text_area_height()
	{
		return pic->get_height() - 2;
	}
	std::size_t text_offset_x()
	{
		return render_offx + cursor_x;
	}
	std::size_t text_offset_y()
	{
		return render_offy + cursor_y;
	}
	std::string &current_line()
	{
		return file_buffer[text_offset_y()];
	}
	void save_file(const std::string &path)
	{
		std::ofstream out(path);
		for (std::size_t idx = 0; idx < file_buffer.size() - 1; ++idx)
			out << file_buffer[idx] << std::endl;
		out << file_buffer[file_buffer.size() - 1] << std::flush;
		text_modified = false;
	}
	void adjust_cursor(std::size_t x_offset)
	{
		if (x_offset < cursor_x + render_offx) {
			x_offset = cursor_x + render_offx - x_offset;
			for (; x_offset > 0 && cursor_x > 0; --x_offset, --cursor_x)
				;
			for (; x_offset > 0 && render_offx > 0; --x_offset, --render_offx)
				;
			if (cursor_x == 0 && render_offx < 4)
				std::swap(cursor_x, render_offx);
		}
		else {
			for (; cursor_x + render_offx < x_offset && cursor_x < pic->get_width() - 1; ++cursor_x)
				;
			for (; cursor_x + render_offx < x_offset && cursor_x + render_offx < current_line().size(); ++render_offx)
				;
		}
	}
	void key_up()
	{
		cursor_count = 0;
		if (cursor_y > 0)
			--cursor_y;
		else if (render_offy > 0)
			--render_offy;
		if (cursor_x + render_offx > current_line().size())
			adjust_cursor(current_line().size());
	}
	void key_down()
	{
		cursor_count = 0;
		if (cursor_y < text_area_height() - 1)
			++cursor_y;
		else if (render_offy + text_area_height() < file_buffer.size())
			++render_offy;
		if (cursor_x + render_offx > current_line().size())
			adjust_cursor(current_line().size());
	}
	void key_left()
	{
		cursor_count = 0;
		if (cursor_x > 0)
			--cursor_x;
		else if (render_offx > 0)
			--render_offx;
		else if (cursor_y + render_offy > 0) {
			if (render_offy > 0)
				--render_offy;
			else if (cursor_y > 0)
				--cursor_y;
			adjust_cursor(current_line().size());
		}
	}
	void key_right()
	{
		cursor_count = 0;
		if (cursor_x < text_area_width()) {
			if (text_offset_x() < current_line().size())
				++cursor_x;
			else if(text_offset_y() < file_buffer.size() - 1) {
				cursor_x = render_offx = 0;
				key_down();
			}
		}
		else if (text_offset_x() < current_line().size())
			++render_offx;
		else if (render_offy + text_area_height() < file_buffer.size()) {
			render_offx = 0;
			++render_offy;
		}
	}
	bool window_resized()
	{
		if (last_win_width != pic->get_width() || last_win_height != pic->get_height()) {
			last_win_width = pic->get_width();
			last_win_height = pic->get_height();
			while (cursor_x >= text_area_width()) {
				--cursor_x;
				++render_offx;
			}
			return true;
		}
		else
			return false;
	}
	bool keyboard_input()
	{
		if (await_process != await_process_type::null)
			return true;
		if (darwin::runtime.is_kb_hit()) {
			if (insert_mode) {
				int key = darwin::runtime.get_kb_hit();
				if (key != keymap::key_esc) {
					text_modified = true;
					auto &line = current_line();
					if (key == keymap::key_enter) {
						auto line_current = line.substr(0, text_offset_x());
						auto line_next = line.substr(text_offset_x());
						line = line_current;
						file_buffer.insert(file_buffer.begin() + text_offset_y() + 1, line_next);
						key_down();
						cursor_x = render_offx = 0;
					}
					else if (key == keymap::key_delete) {
						if (cursor_x + render_offx == 0) {
							key_up();
							adjust_cursor(current_line().size());
							current_line().append(line);
							file_buffer.erase(file_buffer.begin() + text_offset_y() + 1);
						}
						else {
							if (line.size() > 0) {
								line.erase(text_offset_x() - 1, 1);
								key_left();
							}
						}
					}
					else {
						line.insert(line.begin() + text_offset_x(), key);
						key_right();
					}
				}
				else
					insert_mode = false;
			}
			else {
				switch (std::tolower(darwin::runtime.get_kb_hit())) {
				case keymap::key_up:
					key_up();
					break;
				case keymap::key_down:
					key_down();
					break;
				case keymap::key_left:
					key_left();
					break;
				case keymap::key_right:
					key_right();
					break;
				case keymap::key_mode:
					insert_mode = true;
					break;
				case keymap::key_save:
					save_file(file_path);
					break;
				case keymap::key_reload:
					if (text_modified) {
						await_process = await_process_type::reload;
						unsaved_status = unsaved_status_type::asking;
					}
					else {
						file_buffer.clear();
						load(file_path);
					}
					break;
				case keymap::key_quit:
					if (text_modified) {
						await_process = await_process_type::quit;
						unsaved_status = unsaved_status_type::asking;
					}
					else
						std::exit(0);
					break;
				default:
					insert_mode = true;
					break;
				}
			}
			return true;
		}
		else
			return false;
	}
	bool cursor_timer()
	{
		++cursor_count;
		if (cursor_count == 30 || cursor_count == 60)
			return true;
		else
			return false;
	}
	void render_linenum()
	{
		render_border = std::to_string(file_buffer.size()).size();
		for (std::size_t i = 0; i < text_area_height(); ++i) {
			pic->draw_line(2, i + 1, 1 + render_border, i + 1, pixel(' ', true, false, colors::white, colors::white));
			std::string txt = std::to_string(render_offy + i + 1);
			pic->draw_string(2 + (render_border - txt.size()), i + 1, txt, pixel(' ', true, false, colors::black, colors::white));
		}
	}
	void render_text()
	{
		for (std::size_t y = 0; y < text_area_height() && y + render_offy < file_buffer.size(); ++y) {
			for (std::size_t x = 0; x < text_area_width() && x + render_offx < file_buffer[y + render_offy].size(); ++x) {
				char ch = file_buffer[y + render_offy][x + render_offx];
				pic->draw_pixel(x + 3 + render_border, y + 1, pixel(ch, true, false, colors::white, colors::black));
			}
		}
	}
	void render_cursor()
	{
		if (cursor_count <= 30)
			darwin::runtime.get_drawable()->draw_pixel(cursor_x + 3 + render_border, cursor_y + 1, pixel(' ', true, false, colors::white, colors::white));
		else if (cursor_count == 60)
			cursor_count = 0;
	}
	void draw_basic_frame()
	{
		pic->fill(pixel(' ', true, false, colors::white, colors::black));
		pic->draw_line(0, 0, pic->get_width() - 1, 0, pixel(' ', true, false, colors::blue, colors::blue));
		if (text_modified)
			pic->draw_string(2, 0, "Darwin UCGL Texteditor: " + file_path + " (Unsaved)", pixel(' ', true, false, colors::white, colors::blue));
		else
			pic->draw_string(2, 0, "Darwin UCGL Texteditor: " + file_path, pixel(' ', true, false, colors::white, colors::blue));
		pic->draw_line(0, 0, 0, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::blue));
		pic->draw_line(1, 0, 1, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::blue));
		pic->draw_line(pic->get_width() - 1, 0, pic->get_width() - 1, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::blue));
		pic->draw_line(pic->get_width() - 2, 0, pic->get_width() - 2, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::blue));
		render_linenum();
		render_text();
	}
	void force_refresh()
	{
		cursor_count = 59;
	}
	void exec_await_process()
	{
		switch (await_process) {
		default:
			break;
		case await_process_type::quit:
			std::exit(0);
			break;
		case await_process_type::reload:
			text_modified = false;
			file_buffer.clear();
			load(file_path);
			break;
		}
		await_process = await_process_type::null;
	}
	void run_normal()
	{
		darwin::runtime.fit_drawable();
		if (window_resized() || keyboard_input() || cursor_timer()) {
			draw_basic_frame();
			pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::blue));
			if (insert_mode)
				pic->draw_string(2, pic->get_height() - 1, "INSERT (Press ESC to exit)", pixel(' ', true, false, colors::white, colors::blue));
			else
				pic->draw_string(2, pic->get_height() - 1, "WASD: Move Cursor I: Insert S: Save R: Reload Q: Exit", pixel(' ', true, false, colors::white, colors::blue));
			render_cursor();
			darwin::runtime.update_drawable();
		}
	}
	void run_unsaved_asking()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::red));
		pic->draw_string(2, pic->get_height() - 1, "File unsaved, continue without saving? (Yes(Y), No(N) or other key to cancel)", pixel(' ', true, false, colors::white, colors::red));
		if (darwin::runtime.is_kb_hit()) {
			switch (std::tolower(darwin::runtime.get_kb_hit())) {
			case 'y':
				exec_await_process();
				unsaved_status = unsaved_status_type::null;
				force_refresh();
				break;
			case 'n':
				unsaved_status = unsaved_status_type::confirm;
				char_buffer = file_path;
				break;
			default:
				await_process = await_process_type::null;
				unsaved_status = unsaved_status_type::null;
				force_refresh();
				break;
			}
		}
		darwin::runtime.update_drawable();
	}
	bool is_validate_path_char(int ch)
	{
		return std::isalnum(ch) || ch == ' ' || ch == '\\' || ch == '/';
	}
	void run_unsaved_confirm()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::cyan));
		pic->draw_string(2, pic->get_height() - 1, "Save to: " + char_buffer, pixel(' ', true, false, colors::white, colors::cyan));
		if (darwin::runtime.is_kb_hit()) {
			int key = 0;
			switch (key = std::tolower(darwin::runtime.get_kb_hit())) {
			case keymap::key_delete:
				if (!char_buffer.empty())
					char_buffer.pop_back();
				break;
			case keymap::key_enter:
				save_file(char_buffer);
				exec_await_process();
				unsaved_status = unsaved_status_type::null;
				force_refresh();
				break;
			default:
				if (is_validate_path_char(key))
					char_buffer.push_back(key);
				break;
			}
		}
		darwin::runtime.update_drawable();
	}

public:
	void run()
	{
		darwin::runtime.load("./darwin.module");
		darwin::runtime.fit_drawable();
		pic = darwin::runtime.get_drawable();
		darwin::sync_clock clock(60);
		while (true) {
			clock.reset();
			switch (unsaved_status) {
			case unsaved_status_type::null:
				run_normal();
				break;
			case unsaved_status_type::asking:
				run_unsaved_asking();
				break;
			case unsaved_status_type::confirm:
				run_unsaved_confirm();
				break;
			}
			clock.sync();
		}
	}
};

int main(int argc, char **argv)
{
	if (argc != 2)
		return -1;
	texteditor editor(argv[1]);
	editor.run();
	return 0;
}