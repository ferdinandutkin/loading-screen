#include "console.h"
#include <array>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace con_io;

void draw_square(const COORD start, const int dim, const bg_color color) {
	con << manip::set_bg_color{ color } << manip::set_y{ start.Y };
	for (int i{}; i < dim; i++) {
		con << manip::set_x{ start.X } << std::string(dim * 2, ' ') << manip::endl;
	}
}


void draw_loading_bar(const COORD start, const short segments_count, const short segment_width){
	//верх

	con << manip::set_coords{ start } << (char)218;

	for (int i{}; i < segments_count - 1; i++) {
		con << std::string(segment_width, (char)196) << (char)194;
	}
	con << std::string(segment_width, (char)196) << (char)191 << manip::endl;

	//середина
	con << manip::set_coords{ start.X, start.Y + 1 };
	for (int i{}; i < segments_count + 1; i++) {
		con << (char)179 << std::string(segment_width, ' ');
	}

	//низ

	con << manip::set_coords{ start.X, start.Y + 2 } << (char)192;

	for (int i{}; i < segments_count - 1; i++) {
		con << std::string(segment_width, (char)196) << (char)193;
	}
	con << std::string(segment_width, (char)196) << (char)217 << manip::endl;

}


void loading_screen() {
	const short width{ 5 };
	const short start_x{ 45 };
	const short start_y{ 2 };

	draw_square({ start_x, start_y }, width, bg_colors::red);
	draw_square({ start_x + width * 2, start_y }, width, bg_colors::green);
	draw_square({ start_x, start_y + width }, width, bg_colors::blue);
	draw_square({ start_x + width * 2, start_y + width }, width, bg_colors::olive);

	

	con.reset_attribs();

	const COORD bar_start{ 42, 15 };
	const short segment_count{ 5 };
	const short segment_width{ 4 };


	draw_loading_bar(bar_start, segment_count, segment_width);

	con << manip::set_coords{ bar_start.X + 9, bar_start.Y - 1 } << "Loading...";

	const bg_color bar_color = bg_colors::silver;

	std::array<bg_color, segment_width * segment_count + segment_count - 1> bar_colors{ bar_color, bar_color, bar_color };


	for (int i{}; i <= 100; i++) {
		con.set_title(std::to_string(i) + "%");
		con << manip::set_coords{ bar_start.X + 1, bar_start.Y + 1 };

		for (auto color : bar_colors) {
			con << manip::set_bg_color_at_current_pos{ color } << manip::nextc;
			Sleep(2);

		}
		std::rotate(bar_colors.rbegin(), bar_colors.rbegin() + 1, bar_colors.rend());

	}

	con.reset_bg_color();
	con.clear();
	

}

int main(int argc, char* argv[]) {
	con.cursor.hide();
	con.resize(115, 30);
	con.disable_resizing();


	loading_screen();


	std::string filename = [&] {
		if (argc == 2) {
			std::string filename{ argv[1] };
			if (not filename.ends_with(".txt")) {
				filename += ".txt";
			}
			return filename;
		}
		else return std::string{ R"(C:\Users\ferdinand\Desktop\duck.txt)" };
	} ();

	con.set_title(filename);

	std::ifstream input{ filename};

	std::vector<std::string> lines;


	for (std::string line; std::getline(input, line);) {
		lines.push_back(line);
	}

	using namespace con_io;

	std::unordered_map<char, bg_colors> table{ {'1', bg_colors::white}, {'2', bg_colors::red},
		{'3', bg_colors::yellow}
	};


	using namespace manip;



	for (short y{}; const auto & line : lines) {
		for (short x{40}; const char& character : line) {
			if (table.contains(character)) {
				con << set_coords{ x, y } << set_bg_color{ table[character] } << "  ";
			}
			x += 2;

		}
		Sleep(100);
		y++;

	}
	con.reset_bg_color();
	
	con << set_y{ 23 };
	con.cursor.show();
	con.enable_resizing();
 

 

}