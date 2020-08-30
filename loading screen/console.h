#include <Windows.h>
#include <string>
#include <sstream>




namespace con_io {
	enum class fg_colors : short {
		black, blue, green, cyan, red, magenta, brown, lightgray, gray, lightblue, lightgreen, lightcyan,
		lightred, lightmagenta, yellow, white
	};

	enum class bg_colors : short {
		navyblue = 1, green, teal, maroon, purple, olive, silver, gray, blue, lime, cyan, red,
		magenta, yellow, white
	};


	class color {
	protected:
		short value;
	public:
		color(short value) : value{ value } {};
		color() : value{} {};
		operator WORD() const { return value; }
		virtual ~color() {};
	};

	class bg_color : public color {
	public:
		using color::color;

		bg_color(short number) : color{ number * 16 } {}
		bg_color(bg_colors value) : color{ static_cast<short>(value) * 16 } {}


		bg_color& operator ++() {
			value += 16;
			return *this;
		}

		bg_color operator ++(int) {
			bg_color tmp{ *this };
			operator++();
			return tmp;
		}

	};

	class fg_color : public color {
	public:
		using color::color;

		fg_color(fg_colors value) : color{ static_cast<short>(value) } {}


		fg_color& operator ++() {
			value++;
			return *this;
		}

		fg_color operator ++(int) {
			fg_color tmp{ *this };
			operator++();
			return tmp;
		}

	};


	template <typename T>
	concept to_string = requires (T u) {
		std::to_string(u);
	};


	class console {
	private:
		const HANDLE handle;
		WORD get_current_attribs() const {
			CONSOLE_SCREEN_BUFFER_INFO current_info;
			GetConsoleScreenBufferInfo(handle, &current_info);
			return current_info.wAttributes;
		}
		void set_attribs(WORD attribs) const {
			SetConsoleTextAttribute(handle, attribs);
		}

		class cursor {
			const HANDLE owner;
		public:
			cursor() : owner{} {}
			cursor(HANDLE owner) : owner{ owner } {};

			void reset_position() const {
				SetConsoleCursorPosition(owner, { 0, 0 });

			}
			void set_position(short x, short y) const {
				SetConsoleCursorPosition(owner, { x, y });
			}

			void set_position(COORD position) const {
				SetConsoleCursorPosition(owner, position);
			}

			void set_x(short x) const {
				auto [old_x, old_y] = get_position();
				SetConsoleCursorPosition(owner, { x, old_y });
			}

			void set_y(short y) const {
				auto [old_x, old_y] = get_position();
				SetConsoleCursorPosition(owner, { old_x, y });
			}

			void hide() const {
				CONSOLE_CURSOR_INFO info;
				GetConsoleCursorInfo(owner, &info);
				info.bVisible = false;
				SetConsoleCursorInfo(owner, &info);
			}
			void show() const {
				CONSOLE_CURSOR_INFO info;
				GetConsoleCursorInfo(owner, &info);
				info.bVisible = false;
				SetConsoleCursorInfo(owner, &info);
			}


			COORD get_position() const {
				CONSOLE_SCREEN_BUFFER_INFO current_info;
				GetConsoleScreenBufferInfo(owner, &current_info);
				return current_info.dwCursorPosition;
			}

		};

	public:
		const cursor cursor;
		console() : handle{ GetStdHandle(STD_OUTPUT_HANDLE) }, cursor{ handle } {}


		void set_bg_color_at_current_pos(bg_color color) const {
			WORD init_attribs;
			DWORD attribs_rw;

			COORD coords = cursor.get_position();

			ReadConsoleOutputAttribute(handle, &init_attribs, 1, coords, &attribs_rw);

			init_attribs &= ~0b11110000;

			FillConsoleOutputAttribute(handle, init_attribs | color, 1, coords, &attribs_rw);

		}
		void set_bg_color_at(COORD coords, bg_color color) const {
			WORD init_attribs;
			DWORD attribs_rw;

			ReadConsoleOutputAttribute(handle, &init_attribs, 1, coords, &attribs_rw);

			init_attribs &= ~0b11110000;

			FillConsoleOutputAttribute(handle, init_attribs | color, 1, coords, & attribs_rw);

		}

		void set_bg_color_at(short x, short y, bg_color color) const {
			WORD init_attribs;
			DWORD attribs_rw;

			ReadConsoleOutputAttribute(handle, &init_attribs, 1, { x, y }, &attribs_rw);

			init_attribs &= ~0b11110000;

			FillConsoleOutputAttribute(handle, init_attribs | color, 1, { x, y }, &attribs_rw);

		}

		void set_fg_color_at_current_pos(fg_color color) const {
			WORD init_attribs;
			DWORD attribs_rw;

			COORD coords = cursor.get_position();

			ReadConsoleOutputAttribute(handle, &init_attribs, 1, coords, &attribs_rw);

			init_attribs &= ~0b1111;

			FillConsoleOutputAttribute(handle, init_attribs | color, 1, coords, &attribs_rw);

		}

		void set_fg_color_at(COORD coords, fg_color color) const {
			WORD init_attribs;
			DWORD attribs_rw;

			ReadConsoleOutputAttribute(handle, &init_attribs, 1, coords, &attribs_rw);

			init_attribs &= ~0b1111;

			FillConsoleOutputAttribute(handle, init_attribs | color, 1, coords, &attribs_rw);

		}

		void set_fg_color_at(short x, short y, fg_color color) const {
			WORD init_attribs;
			DWORD attribs_rw;

			ReadConsoleOutputAttribute(handle, &init_attribs, 1, { x, y }, &attribs_rw);

			init_attribs &= ~0b1111;

			FillConsoleOutputAttribute(handle, init_attribs | color, 1, { x, y }, &attribs_rw);

		}

		void set_bg_color(bg_color color) const {
			WORD current_attribs = get_current_attribs();
			current_attribs &= ~0b11110000;
			set_attribs(current_attribs | color);
		}

		void set_fg_color(fg_color color) const {
			WORD current_attribs = get_current_attribs();
			current_attribs &= ~0b1111;
			set_attribs(current_attribs | color);
		}

		void reset_bg_color() const {
			WORD current_attribs = get_current_attribs();
			current_attribs &= ~0b11110000;
			set_attribs(current_attribs);
		}

		void reset_fg_color() const {
			WORD current_attribs = get_current_attribs();
			current_attribs &= ~0b1111;
			set_attribs(current_attribs);
		}



		char at(COORD coords) const {
			char character;
			DWORD read;
			ReadConsoleOutputCharacterA(handle, &character, 1, coords, &read);
			return character;
		}

		 
		char at(short x, short y) const {
			char character;
			DWORD read;
			ReadConsoleOutputCharacterA(handle, &character, 1, { x, y }, &read);
			return character;
		}

		char at_current_pos() const {
			char character;
			DWORD read;
			ReadConsoleOutputCharacterA(handle, &character, 1, cursor.get_position(), &read);
			return character;

		}

		void reset_attribs() const {
			set_attribs(0b111);
		}

		void set_title(const char* title) const {
			SetConsoleTitleA(title);
		}

		void set_title(std::string title) const {
			SetConsoleTitleA(title.c_str());
		}

		void enable_resizing() const {
			HWND console_window{ GetConsoleWindow() };
			LONG init_style{ GetWindowLong(console_window, GWL_STYLE) };

			init_style |= (WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);

			SetWindowLong(console_window, GWL_STYLE, init_style);
		}

		void disable_resizing() const {
			HWND console_window{ GetConsoleWindow() };
			LONG init_style{ GetWindowLong(console_window, GWL_STYLE) };

			init_style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
	
			SetWindowLong(console_window, GWL_STYLE, init_style);
		}

		void resize(short x, short y) const {

			SMALL_RECT minimal_window = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(handle, TRUE, &minimal_window);
	
			SetConsoleScreenBufferSize(handle, { x, y });


			SMALL_RECT window = { 0, 0, x - 1, y - 1 };
			SetConsoleWindowInfo(handle, true, &window);
;
		}

		void resize(COORD size) const {

			SMALL_RECT minimal_window = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(handle, TRUE, &minimal_window);

			SetConsoleScreenBufferSize(handle, size);


			SMALL_RECT window = { 0, 0, size.X - 1, size.Y - 1 };
			SetConsoleWindowInfo(handle, true, &window);
			;
		}

		void clear() const {
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(handle, &info);
			DWORD size = info.dwSize.X * info.dwSize.Y;
			DWORD written;
			FillConsoleOutputCharacterA(handle, ' ', size, { 0, 0 }, &written);
			FillConsoleOutputAttribute(handle, 0b111, size, { 0, 0 }, &written);
			cursor.reset_position();
			reset_attribs();
		}
		void write(const char to_write) const {
			WriteConsoleA(handle, &to_write, 1, nullptr, nullptr);
		}

		void write(const char* to_write) const {
			WriteConsoleA(handle, to_write, strlen(to_write), nullptr, nullptr);
		}

		void write(std::string to_write) const {
			WriteConsoleA(handle, to_write.c_str(), to_write.size(), nullptr, nullptr);
		}

		template <to_string T>
		void write(T to_write) const {
			auto u = std::to_string(to_write);
			WriteConsoleA(handle, u.c_str(), u.size(), nullptr, nullptr);
		}

		template <typename T>
		void write(T to_write) const {
			std::stringstream u;
			u << to_write;
			auto v = u.str();
			WriteConsoleA(handle, v.c_str(), v.size(), nullptr, nullptr);
		}

		template <typename T, typename... Types>
		void write(T to_write, Types... rest) const {
			write(to_write);
			write(rest...);
		}

		operator HANDLE() const { return handle; }

		~console() {
			CloseHandle(handle);
		}
	};


	const console con;

	template <typename T>
	const console& operator << (const console& console, const T& to_write) {
		console.write(to_write);
		return console;
	}

	namespace manip {


		struct set_bg_color_at_current_pos {
			bg_color color;
			set_bg_color_at_current_pos(bg_color color) : color{ color } {}
		};

		const console& operator << (const console& console, const set_bg_color_at_current_pos set_bg_color_at_current_pos) {
			console.set_bg_color_at_current_pos(set_bg_color_at_current_pos.color);
			return console;
		}


		struct set_fg_color_at_current_pos {
			fg_color color;
			set_fg_color_at_current_pos(fg_color color) : color{ color } {}
		};

		const console& operator << (const console& console, const set_fg_color_at_current_pos set_fg_color_at_current_pos) {
			console.set_fg_color_at_current_pos(set_fg_color_at_current_pos.color);
			return console;
		}

		struct set_fg_color_at {
			fg_color color;
			COORD coords;
			set_fg_color_at(COORD coords, fg_color color) : coords{ coords }, color { color } {}
			set_fg_color_at(short x, short y, fg_color color) : coords{ x, y }, color{ color } {}
		};

		const console& operator << (const console& console, const set_fg_color_at set_fg_color_at) {
			console.set_fg_color_at(set_fg_color_at.coords, set_fg_color_at.color);
			return console;
		}


		struct set_bg_color_at {
			bg_color color;
			COORD coords;
			set_bg_color_at(COORD coords, bg_color color) : coords{ coords }, color{ color } {}
			set_bg_color_at(short x, short y, bg_color color) : coords{ x, y }, color{ color } {}
		};

		const console& operator << (const console& console, const set_bg_color_at set_bg_color_at) {
			console.set_bg_color_at(set_bg_color_at.coords, set_bg_color_at.color);
			return console;
		}

		struct {} nextl;

		const console& operator << (const console& console, const decltype(nextl) nextl) {
			auto [old_x, old_y] = console.cursor.get_position();
			console.cursor.set_position(old_x, old_y + 1);
			return console;
		}

		struct {} prevl;

		const console& operator << (const console& console, const decltype(prevl) nextl) {
			auto [old_x, old_y] = console.cursor.get_position();
			console.cursor.set_position(old_x, old_y - 1);
			return console;
		}


		struct {} nextc;

		const console& operator << (const console& console, const decltype(nextc) nextc) {
			auto [old_x, old_y] = console.cursor.get_position();
			console.cursor.set_position(old_x + 1, old_y);
			return console;
		}

		struct {} prevc;

		const console& operator << (const console& console, const decltype(prevc) nextc) {
			auto [old_x, old_y] = console.cursor.get_position();
			console.cursor.set_position(old_x - 1, old_y);
			return console;
		}

		struct set_y {
			int y;
			set_y(int y) : y{ y } {};

		};

		const console& operator << (const console& console, const set_y& set_y) {
			console.cursor.set_y(set_y.y);
			return console;
		}

		struct set_x {
			int x;
			set_x(int x) : x{ x } {};
		};

		const console& operator << (const console& console, const set_x& set_x) {
			console.cursor.set_x(set_x.x);
			return console;
		}


		struct set_coords {
			short x, y;
			set_coords(COORD coords) : x{ coords.X }, y{ coords.Y }{}
			set_coords(short x, short y) : x{ x }, y{ y } {}
		};



		const console& operator << (const console& console, const set_coords& set_coords) {
			console.cursor.set_position(set_coords.x, set_coords.y);
			return console;
		}


		struct set_bg_color {
			bg_color color;
			set_bg_color(bg_colors color) : color{ color } {}
			set_bg_color(bg_color color) : color{ color } {}

		};

		const console& operator << (const console& console, const set_bg_color& set_bg_color) {
			console.set_bg_color(set_bg_color.color);
			return console;
		}

		struct set_fg_color {
			fg_color color;
			set_fg_color(fg_color color) : color{ color } {}
			set_fg_color(fg_colors color) : color{ color } {}
		};


		const console& operator << (const console& console, const set_fg_color& set_fg_color) {
			console.set_fg_color(set_fg_color.color);
			return console;
		}


		struct {} endl;

		const console& operator << (const console& console, const decltype(endl) endl) {
			auto [old_x, old_y] = console.cursor.get_position();
			console.cursor.set_position(0, old_y + 1);
			return console;
		}


	}


}