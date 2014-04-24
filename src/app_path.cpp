#include <string>
#include <iostream>
#include "turan.h"

using namespace std;

bool initialized = false;
string argv0;
string app_path;

void set_argv0(const char* path) {
  argv0 = string(path);

  size_t last_slash = argv0.rfind("/");

  if (last_slash != string::npos)
    app_path = argv0.substr(0, last_slash + 1);
  else
    app_path = "";

  initialized = true;
}

std::string get_app_path() {
  if (!initialized)
    fatal_error("get_app_path() called before set_argv0()");
  return app_path;
}

