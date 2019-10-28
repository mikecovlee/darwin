// 关闭Darwin UCGL的扩展功能和日志功能
#define DARWIN_FORCE_BUILTIN
#define DARWIN_DISABLE_LOG
// 引入Darwin UCGL
#include <darwin/darwin.hpp>
// 其他STL
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
// 引入Darwin UCGL名称空间
using namespace darwin;
// 按键映射
namespace keymap {
	constexpr int key_esc = 27;
	constexpr int key_enter = 10;
	constexpr int key_delete = 127;
	constexpr int key_tab = '\t';
	constexpr int key_up = 'w';
	constexpr int key_down = 's';
	constexpr int key_left = 'a';
	constexpr int key_right = 'd';
	constexpr int key_mode = 'i';
	constexpr int key_quit = 'q';
	constexpr int key_reload = 'r';
	constexpr int key_save = 'x';
	constexpr int key_find = 'f';
	constexpr int key_info = 'v';
} // namespace keymap
// 文本编辑器类
class texteditor final {
// 渲染器属性
	std::size_t render_border = 0, render_offx = 0, render_offy = 0;
// 光标属性
	std::size_t cursor_count = 0, cursor_x = 0, cursor_y = 0;
// 窗口属性
	std::size_t last_win_width = 0, last_win_height = 0;
// 文本 Buffer
	std::vector<std::string> file_buffer;
// 查找高亮属性
	std::size_t find_x = 0, find_y = 0;
// 帧 Buffer
	darwin::drawable *pic = nullptr;
// 状态
	bool text_modified = false;
	bool insert_mode = false;
	bool found_text = false;
	bool expect_txt = false;
// 字符 Buffer
	std::string find_target;
	std::string char_buffer;
	std::string file_path;
// 挂起任务
	enum class await_process_type {
		null,
		quit,
		reload
	} await_process = await_process_type::null;
// 编辑器状态
	enum class editor_status_type {
		null,
		asking,
		confirm,
		info,
		setup,
		notfound,
		finding,
		replace
	} editor_status = editor_status_type::null;

public:
// Tab 缩进宽度
	unsigned int tab_indent = 4;
// 构造函数 & 赋值函数
	texteditor() = default;
	texteditor(const texteditor&) = delete;
	texteditor &operator=(const texteditor&) = delete;
private:
	// 文件操作
	void load_file(const std::string &path)
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
	void save_file(const std::string &path)
	{
		std::ofstream out(path);
		for (std::size_t idx = 0; idx < file_buffer.size() - 1; ++idx)
			out << file_buffer[idx] << std::endl;
		out << file_buffer[file_buffer.size() - 1] << std::flush;
		text_modified = false;
	}
	// 各类属性
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
	// 工具
	void force_refresh()
	{
		cursor_count = 59;
	}
	bool is_validate_path_char(int ch)
	{
		return std::isalnum(ch) || ch == ' ' || ch == '\\' || ch == '/';
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
	// 文本查找
	void reset_find()
	{
		find_x = find_y = 0;
		found_text = false;
		expect_txt = false;
	}
	void find()
	{
		while (find_y < file_buffer.size()) {
			auto &line = file_buffer[find_y];
			auto pos = line.find(find_target, expect_txt ? find_x + 1 : 0);
			if (pos != std::string::npos) {
				if (!expect_txt || pos > find_x) {
					found_text = true;
					expect_txt = true;
					find_x = pos;
					break;
				}
			}
			expect_txt = false;
			find_x = 0;
			++find_y;
		}
		if (found_text) {
			if (find_y == file_buffer.size()) {
				reset_find();
				find();
			}
			cursor_x = render_offx = 0;
			cursor_y = 0;
			if (find_y < file_buffer.size() - text_area_height())
				render_offy = find_y > 4 ? find_y - 4 : 0;
			else
				render_offy = file_buffer.size() - text_area_height();
			if (find_x + find_target.size() > text_area_width())
				render_offx = find_x + find_target.size() - text_area_width() + 1;
		}
	}
	// 键盘事件处理
	void key_up()
	{
		if (cursor_y > 0)
			--cursor_y;
		else if (render_offy > 0)
			--render_offy;
		if (cursor_x + render_offx > current_line().size())
			adjust_cursor(current_line().size());
	}
	void key_down()
	{
		if (cursor_y < text_area_height() - 1)
			++cursor_y;
		else if (render_offy + text_area_height() < file_buffer.size())
			++render_offy;
		if (cursor_x + render_offx > current_line().size())
			adjust_cursor(current_line().size());
	}
	void key_left()
	{
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
		if (cursor_x < text_area_width()) {
			if (text_offset_x() < current_line().size())
				++cursor_x;
			else if (text_offset_y() < file_buffer.size() - 1) {
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
	// 事件响应
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
			cursor_count = 0;
			if (insert_mode) {
				int key = darwin::runtime.get_kb_hit();
				if (key != keymap::key_esc) {
					auto &line = current_line();
					if (key == keymap::key_enter) {
						auto line_current = line.substr(0, text_offset_x());
						auto line_next = line.substr(text_offset_x());
						line = line_current;
						file_buffer.insert(file_buffer.begin() + text_offset_y() + 1, line_next);
						key_down();
						cursor_x = render_offx = 0;
						text_modified = true;
					}
					else if (key == keymap::key_delete) {
						if (cursor_x + render_offx == 0) {
							if (text_offset_y() != 0) {
								key_up();
								adjust_cursor(current_line().size());
								current_line().append(line);
								file_buffer.erase(file_buffer.begin() + text_offset_y() + 1);
								text_modified = true;
							}
						}
						else {
							if (line.size() > 0) {
								line.erase(text_offset_x() - 1, 1);
								key_left();
								text_modified = true;
							}
						}
					}
					else {
						line.insert(line.begin() + text_offset_x(), key);
						key_right();
						text_modified = true;
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
						editor_status = editor_status_type::asking;
					}
					else {
						file_buffer.clear();
						load_file(file_path);
					}
					break;
				case keymap::key_quit:
					if (text_modified) {
						await_process = await_process_type::quit;
						editor_status = editor_status_type::asking;
					}
					else
						std::exit(0);
					break;
				case keymap::key_find:
					find_target.clear();
					char_buffer.clear();
					editor_status = editor_status_type::setup;
					break;
				case keymap::key_info: {
					std::size_t char_count = 0;
					for (auto &line : file_buffer)
						char_count += line.size();
					char_buffer = std::to_string(file_buffer.size()) + " line(s), " + std::to_string(char_count) + " character(s)";
					editor_status = editor_status_type::info;
					break;
				}
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
	// 渲染函数
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
				if (found_text && x + render_offx >= find_x && x + render_offx < find_x + find_target.size() && y + render_offy == find_y)
					pic->draw_pixel(x + 3 + render_border, y + 1, pixel(ch, true, false, colors::white, colors::pink));
				else
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
	// 处理挂起任务
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
			load_file(file_path);
			break;
		}
		await_process = await_process_type::null;
	}
	// 运行状态实现
	void run_normal()
	{
		darwin::runtime.fit_drawable();
		if (window_resized() || keyboard_input() || cursor_timer()) {
			draw_basic_frame();
			pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::blue, colors::blue));
			if (insert_mode)
				pic->draw_string(2, pic->get_height() - 1, "INSERT (Press ESC to exit)", pixel(' ', true, false, colors::white, colors::blue));
			else
				pic->draw_string(2, pic->get_height() - 1, "WASD: Move I: Insert X: Save R: Reload F: Find V: Info Q: Exit", pixel(' ', true, false, colors::white, colors::blue));
			render_cursor();
			darwin::runtime.update_drawable();
		}
	}
	void run_unsaved_asking()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::red, colors::red));
		pic->draw_string(2, pic->get_height() - 1, "File unsaved, continue without saving? (Yes(Y), No(N) or other key to cancel)", pixel(' ', true, false, colors::white, colors::red));
		if (darwin::runtime.is_kb_hit()) {
			switch (std::tolower(darwin::runtime.get_kb_hit())) {
			case 'y':
				exec_await_process();
				editor_status = editor_status_type::null;
				force_refresh();
				break;
			case 'n':
				editor_status = editor_status_type::confirm;
				char_buffer = file_path;
				break;
			default:
				await_process = await_process_type::null;
				editor_status = editor_status_type::null;
				force_refresh();
				break;
			}
		}
		darwin::runtime.update_drawable();
	}
	void run_unsaved_confirm()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::cyan, colors::cyan));
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
				editor_status = editor_status_type::null;
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
	void run_find_setup()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::white, colors::white));
		pic->draw_string(2, pic->get_height() - 1, "Find: " + find_target, pixel(' ', true, false, colors::black, colors::white));
		if (darwin::runtime.is_kb_hit()) {
			int key = 0;
			switch (key = std::tolower(darwin::runtime.get_kb_hit())) {
			case keymap::key_delete:
				if (!find_target.empty())
					find_target.pop_back();
				break;
			case keymap::key_enter:
				if (find_target.empty()) {
					editor_status = editor_status_type::null;
					force_refresh();
					break;
				}
				find();
				if (found_text)
					editor_status = editor_status_type::finding;
				else
					editor_status = editor_status_type::notfound;
				break;
			default:
				if (!std::iscntrl(key))
					find_target.push_back(key);
				break;
			}
		}
		darwin::runtime.update_drawable();
	}
	void run_notfound()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::white, colors::white));
		pic->draw_string(2, pic->get_height() - 1, "Find: \"" + find_target + "\" not found (Press Q to close)", pixel(' ', true, false, colors::black, colors::white));
		if (darwin::runtime.is_kb_hit()) {
			if (std::tolower(darwin::runtime.get_kb_hit()) == 'q') {
				editor_status = editor_status_type::null;
				force_refresh();
			}
		}
		darwin::runtime.update_drawable();
	}
	void run_finding()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::white, colors::white));
		pic->draw_string(2, pic->get_height() - 1, "Find: \"" + find_target + "\" (Next(N) Replace(R) Quit(Q))", pixel(' ', true, false, colors::black, colors::white));
		if (darwin::runtime.is_kb_hit()) {
			switch (std::tolower(darwin::runtime.get_kb_hit())) {
			case 'n':
				find();
				break;
			case 'r':
				editor_status = editor_status_type::replace;
				break;
			case 'q':
				reset_find();
				if (text_offset_x() > current_line().size())
					adjust_cursor(current_line().size());
				editor_status = editor_status_type::null;
				force_refresh();
				break;
			}
		}
		darwin::runtime.update_drawable();
	}
	void run_replace()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::pink, colors::pink));
		pic->draw_string(2, pic->get_height() - 1, "Replace: " + char_buffer, pixel(' ', true, false, colors::white, colors::pink));
		if (darwin::runtime.is_kb_hit()) {
			int key = 0;
			switch (key = std::tolower(darwin::runtime.get_kb_hit())) {
			case keymap::key_delete:
				if (!char_buffer.empty())
					char_buffer.pop_back();
				break;
			case keymap::key_enter:
				if (char_buffer != find_target) {
					text_modified = true;
					file_buffer[find_y].replace(find_x, find_target.size(), char_buffer);
					find();
					if (found_text)
						editor_status = editor_status_type::finding;
					else {
						reset_find();
						if (text_offset_x() > current_line().size())
							adjust_cursor(current_line().size());
						editor_status = editor_status_type::notfound;
					}
				}
				break;
			default:
				if (!std::iscntrl(key))
					char_buffer.push_back(key);
				break;
			}
		}
		darwin::runtime.update_drawable();
	}
	void run_info()
	{
		darwin::runtime.fit_drawable();
		draw_basic_frame();
		pic->draw_line(2, pic->get_height() - 1, pic->get_width() - 3, pic->get_height() - 1, pixel(' ', true, false, colors::yellow, colors::yellow));
		pic->draw_string(2, pic->get_height() - 1, "File Info: " + char_buffer + " (Press Q to close)", pixel(' ', true, false, colors::white, colors::yellow));
		if (darwin::runtime.is_kb_hit()) {
			if (std::tolower(darwin::runtime.get_kb_hit()) == 'q') {
				editor_status = editor_status_type::null;
				force_refresh();
			}
		}
		darwin::runtime.update_drawable();
	}

public:
	// 主函数
	void run(const std::string& path)
	{
		file_path = path;
		load_file(file_path);
		darwin::runtime.load("./darwin.module");
		darwin::runtime.fit_drawable();
		pic = darwin::runtime.get_drawable();
		darwin::sync_clock clock(60);
		while (true) {
			clock.reset();
			switch (editor_status) {
			case editor_status_type::null:
				run_normal();
				break;
			case editor_status_type::asking:
				run_unsaved_asking();
				break;
			case editor_status_type::confirm:
				run_unsaved_confirm();
				break;
			case editor_status_type::setup:
				run_find_setup();
				break;
			case editor_status_type::notfound:
				run_notfound();
				break;
			case editor_status_type::finding:
				run_finding();
				break;
			case editor_status_type::replace:
				run_replace();
				break;
			case editor_status_type::info:
				run_info();
				break;
			}
			clock.sync();
		}
	}
};
// 启动进程
int main(int argc, char **argv)
{
	if (argc != 2)
		return -1;
	texteditor editor;
	editor.run(argv[1]);
	return 0;
}