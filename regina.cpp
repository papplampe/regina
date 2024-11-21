// g++ -s -O2 -o regina regina.cc -lfltk -lfltk_images -ltre

#include <cstdio>
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_PNG_Image.H>
#include <tre/tre.h>

static Fl_Input *input_regex;
static Fl_Text_Editor *edit_test;
static Fl_Text_Buffer *buf_test;
static Fl_Text_Editor *edit_out;
static Fl_Text_Buffer *buf_out;

static void do_test_match(regex_t *regex, char *text, size_t textlen)
{
	regmatch_t m;
	if(tre_regexec(regex, text, 1, &m, 0) == 0
		&& m.rm_so == 0 && (size_t)m.rm_eo == textlen
	) {
		buf_out->append("matches: yes\n");
	} else {
		buf_out->append("matches: no\n");
	}
}

static void do_test_find(regex_t *regex, char *text, size_t textlen)
{
	#define MATCHES 32
	regmatch_t m[MATCHES];
	int len;
	char *pm = text;
	char *buf = (char *)malloc(sizeof(char) * textlen);
	while(tre_regexec(regex, pm, MATCHES, m, 0) == 0
		&& m[0].rm_so > -1)
	{
		int o = pm - text;
		len = m[0].rm_eo - m[0].rm_so;
		buf_out->append("find(");
		std::sprintf(buf, "%d", o + m[0].rm_so);
		buf_out->append(buf);
		buf_out->append(",");
		std::sprintf(buf, "%d", o + m[0].rm_eo);
		buf_out->append(buf);
		buf_out->append("):");
		memcpy(buf, pm + m[0].rm_so, len);
		buf[len] = 0;
		buf_out->append(buf);
		buf_out->append("\n");
		for(int g = 1; g < MATCHES && m[g].rm_so > -1; g++) {
			len = m[g].rm_eo - m[g].rm_so;
			buf_out->append("  ");
			std::sprintf(buf, "%d", g);
			buf_out->append(buf);
			buf_out->append(":");
			memcpy(buf, pm + m[g].rm_so, len);
			buf[len] = 0;
			buf_out->append(buf);
			buf_out->append("\n");
		}
		pm += m[0].rm_eo;
	}
	free(buf);
}

static void input_regex_callback(Fl_Widget*, void*)
{
	if(input_regex->size() == 0) {
		return;
	}
	buf_out->text("");
	regex_t regex;
	if(tre_regcomp(&regex, input_regex->value(), REG_EXTENDED|REG_NEWLINE) != 0) {
		buf_out->append("regex is malformed\n");
		return;
	}
	char *text = buf_test->text();
	do_test_match(&regex, text, buf_test->length());
	do_test_find(&regex, text, buf_test->length());
	tre_regfree(&regex);
	free(text);
}

int main(int argc, char **argv)
{
	Fl_Double_Window win(400, 500, "REgina");
	input_regex = new Fl_Input(5, 5, win.w()-5-5, 25);
	input_regex->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
	input_regex->callback(input_regex_callback);
	buf_test = new Fl_Text_Buffer();
	buf_out = new Fl_Text_Buffer();
	win.icon(new Fl_PNG_Image("regina.png"));
	Fl_Tile tile(5, 5+25+5, win.w()-5-5, win.h()-25-5-5-5);
	{
		edit_test = new Fl_Text_Editor(5, 5+25+5, win.w()-5-5, win.h()-25-5-5-160-5);
		edit_test->buffer(buf_test);
		edit_out = new Fl_Text_Editor(5, 5+25+5+win.h()-25-5-5-160-5, win.w()-5-5, 160);
		edit_out->buffer(buf_out);
		tile.resizable(tile);
	}
	tile.end();
	win.resizable(tile);
	win.position((Fl::w() - win.w())/2, (Fl::h() - win.h())/2);
	win.size_range(win.w(), win.h());
	win.show(argc, argv);
	return(Fl::run());
}
