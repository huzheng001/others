//g++ build.cpp -o build $(pkg-config --cflags --libs gtk+-3.0)

// ./build 中华大藏经

#include <string>

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

void string_replace(std::string & strBig, const std::string & strsrc, const std::string &strdst)
{
	std::string::size_type pos=0;
	std::string::size_type srclen=strsrc.size();
	std::string::size_type dstlen=strdst.size();
	while( (pos=strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

void convert_txt(const gchar *dirname, const gchar *txtfilename, const gchar *locale_sutra_name)
{
	std::string fullfilename;
	fullfilename = dirname;
	fullfilename += "/txt/";
	fullfilename += txtfilename;

	std::string file_contents;
	file_contents = "<HTML><HEAD>\r\n<META http-equiv=Content-Type content=\"text/html; charset=GBK\">\r\n<TITLE>";
	file_contents += locale_sutra_name;
	file_contents += "</TITLE>\r\n</HEAD>\r\n<BODY>";

	gchar *contents;
	g_file_get_contents(fullfilename.c_str(), &contents, NULL, NULL);
	
	std::string html_contents;
	html_contents = contents;
	string_replace(html_contents, "\r\n", "<BR>\r\n");

	g_free(contents);

	file_contents += html_contents;

	file_contents += "</BODY>\r\n</HTML>";

	g_file_set_contents(fullfilename.c_str(), file_contents.c_str(), -1, NULL);

	std::string oldpath;
	oldpath = dirname;
	oldpath += "/txt/";
	oldpath += txtfilename;

	gchar tmpfilename1[256];
	strcpy(tmpfilename1, txtfilename);
	gchar *p;
	p = strrchr(tmpfilename1, '.');
	if (p) {
		*p = '\0';
	}

	std::string newpath;
	newpath = dirname;
	newpath += "/html/";
	newpath += tmpfilename1;
	newpath += ".html";

	rename(oldpath.c_str(), newpath.c_str());
}

void build_zip(const gchar *dirname, const gchar *zipfilename)
{

	gchar sutra_name[256];
	strcpy(sutra_name, zipfilename);
	gchar *p;
	p = strrchr(sutra_name, '.');
	if (p) {
		*p = '\0';
	}

	GIConv locale_converter;
	locale_converter = g_iconv_open("gb18030","UTF-8");

	gchar *locale_sutra_name;
	locale_sutra_name = g_convert_with_iconv(sutra_name,-1,locale_converter,NULL,NULL,NULL);
	g_iconv_close(locale_converter);

	gchar *quotefilename;

	std::string cmd;
	cmd = "mv ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += '/';
	quotefilename = g_shell_quote(zipfilename);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += ' ';
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/txt";
	system(cmd.c_str());

	cmd = "unzip ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/txt/";
	quotefilename = g_shell_quote(zipfilename);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += " -d ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/txt";
	system(cmd.c_str());

	cmd = "mv ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/txt/";
	quotefilename = g_shell_quote(zipfilename);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += ' ';
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	system(cmd.c_str());

	std::string fullfilename;
	fullfilename = dirname;
	fullfilename += "/txt";

	GDir *dir = g_dir_open(fullfilename.c_str(), 0, NULL);
	const gchar *txtfilename;
	while ((txtfilename = g_dir_read_name(dir))!=NULL) {
		fullfilename = dirname;
		fullfilename += "/txt/";
		fullfilename += txtfilename;
		if (g_file_test(fullfilename.c_str(), G_FILE_TEST_IS_REGULAR)) {
			convert_txt(dirname, txtfilename, locale_sutra_name);
		}
	}
	g_dir_close(dir);

	g_free(locale_sutra_name);
}

void build_dir(const gchar *dirname)
{
	gchar *quotefilename;

	std::string cmd;
	cmd = "mkdir ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/txt";
	system(cmd.c_str());
	
	cmd = "mkdir ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/html";
	system(cmd.c_str());

	GDir *dir = g_dir_open(dirname, 0, NULL);
	const gchar *zipfilename;
	std::string fullfilename;
	while ((zipfilename = g_dir_read_name(dir))!=NULL) {
		fullfilename = dirname;
		fullfilename += '/';
		fullfilename += zipfilename;
		if (g_file_test(fullfilename.c_str(), G_FILE_TEST_IS_REGULAR)) {
			build_zip(dirname, zipfilename);
		}
	}
	g_dir_close(dir);

	cmd = "rmdir ";
	quotefilename = g_shell_quote(dirname);
	cmd += quotefilename;
	g_free(quotefilename);
	cmd += "/txt";
	system(cmd.c_str());
}

int main(int argc, char **argv)
{
	setlocale(LC_ALL, "");

	GDir *dir = g_dir_open(argv[1], 0, NULL);
	
	const gchar *filename;
	std::string fullfilename;
	while ((filename = g_dir_read_name(dir))!=NULL) {
		fullfilename = argv[1];
		fullfilename += '/';
		fullfilename += filename;
		if (g_file_test(fullfilename.c_str(), G_FILE_TEST_IS_DIR)) {
			build_dir(fullfilename.c_str());
		}
	}
	g_dir_close(dir);

	return FALSE;
}
