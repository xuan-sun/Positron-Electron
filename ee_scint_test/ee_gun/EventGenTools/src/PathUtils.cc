#include "PathUtils.hh"
#include "strutils.hh"
#include "SMExcept.hh"
#include <dirent.h>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

bool fileExists(std::string f) {
	return !system(("test -r '" + f + "'").c_str());
}

bool dirExists(std::string d) {
	return !system(("test -d '" + d + "'").c_str());
}

void makePath(std::string p, bool forFile) {
	std::vector<std::string> pathels = split(p,"/");
	if(forFile && pathels.size())
		pathels.pop_back();
	if(!pathels.size())
		return;
	std::string thepath;
	if(p[0]=='/')
		thepath += "/";
	for(unsigned int i=0; i<pathels.size(); i++) {
		thepath += pathels[i] + "/";
		if(!dirExists(thepath)) {
			std::string cmd = "mkdir -p '"+thepath+"'";
			int err = system(cmd.c_str());
			if(err || !dirExists(thepath)) {
				SMExcept e("badPath");
				e.insert("pathName",thepath);
				e.insert("errnum",errno);
				e.insert("errname",strerror(errno));
				throw(e);
//			std::cout << "Error. Bad path. Located in makePath method of PathUtils.cc." << std::endl;
			}
		}
	}
}

double fileAge(const std::string& fname) {
	if(!(fileExists(fname) || dirExists(fname)))
		return -1.;
	struct stat attrib;
	stat(fname.c_str(), &attrib);
	time_t timenow = time(NULL);
	return timenow - attrib.st_mtime;
}

std::vector<std::string> listdir(const std::string& dir, bool includeHidden) {
	std::vector<std::string> dirs;
	dirent* entry;
	DIR* dp = opendir(dir.c_str());
	if (dp == NULL)
		return dirs;
	while((entry = readdir(dp)))
		if(includeHidden || entry->d_name[0] != '.')
			dirs.push_back(entry->d_name);
	closedir(dp);
	std::sort(dirs.begin(),dirs.end());
	return dirs;
}

std::string getEnvSafe(const std::string& v, const std::string& dflt) {
	const char* envv = getenv(v.c_str());
	if(!envv) {
		if(dflt == "FAIL_IF_MISSING") {
			SMExcept e("missingEnv");
			e.insert("var",v);
			throw(e);
//			std::cout << "Error. Missing env (?) due to FAIL_IF_MISSING flag. In getEnvSafe method of PathUtils.cc." << std::endl;
		}
		return dflt;
	}
	return envv;
}
