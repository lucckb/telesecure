#include <util.hpp>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

namespace util {

// Get home directory
std::string home_dir() 
{
    const char *homedir;
    if ((homedir = getenv("HOME")) == nullptr) {
        homedir = getpwuid(getuid())->pw_dir;
    }   
    return homedir;
}

}