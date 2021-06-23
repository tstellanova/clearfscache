/*
 * Project clearfs
 * Description:
 * Author:
 * Date:
 */

#include "Particle.h"
#include <dirent.h>
#include <fcntl.h>


SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

SerialLogHandler logHandler(115200, LOG_LEVEL_ALL);

/// List files in a directory
static void _ls(String &path, int level, DIR *dir) {
    struct dirent *entry;
    String indent;

    indent.reserve(level);
    for(int i=0; i < level; i++) {
        indent += '\t';
    }

    while((entry = readdir(dir)) != NULL) {
        String new_path;
        struct stat st;
        
        if(path.length() && path[path.length()-1] == '/') {
            new_path = String::format("%s%s", path.c_str(), entry->d_name);
        }
        else  {
            new_path = String::format("%s/%s", path.c_str(), entry->d_name);
        }

        if(entry->d_type == DT_REG && !stat(new_path, &st)) {
            Log.info("%s%s %lu bytes", indent.c_str(), entry->d_name, st.st_size);
        }
        else  {
            Log.info("%s%s", indent.c_str(), entry->d_name);
        }

        if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            DIR *new_dir = opendir(new_path);
            if(new_dir) {
                _ls(new_path, level + 1, new_dir);
                closedir(new_dir);
            }
        }
    }
}

/// Delete all files in a directory
static void _delete_all_entries(String &path, int level, DIR *dir) {
    struct dirent *entry;
    String indent;

    indent.reserve(level);
    for(int i=0; i < level; i++) {
        indent += '\t';
    }

    while((entry = readdir(dir)) != NULL) {
        String new_path;
        struct stat st;
        
        if(path.length() && path[path.length()-1] == '/') {
            new_path = String::format("%s%s", path.c_str(), entry->d_name);
        }
        else  {
            new_path = String::format("%s/%s", path.c_str(), entry->d_name);
        }

        if(entry->d_type == DT_REG && !stat(new_path, &st)) {
            //Log.info("%s%s %lu bytes", indent.c_str(), entry->d_name, st.st_size);
            const char* full_path = new_path.c_str();
            int rc = remove(full_path);
            Log.info("%s removing %lu rc: %d", full_path, st.st_size, rc);
        }
        else  {
            Log.info("%s%s", indent.c_str(), entry->d_name);
        }

        // if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
        //     DIR *new_dir = opendir(new_path);
        //     if(new_dir)
        //     {
        //         _ls(new_path, level + 1, new_dir);
        //         closedir(new_dir);
        //     }
        // }
    }
}

/// List files in a path
static void ls(String path) {
    DIR *dir;
    dir = opendir(path);
    if(dir)
    {
        _ls(path, 0, dir);
        closedir(dir);
    }
}


/// Remove all files at a directory path, then remove the directory
static void cleanup_dir(String path) {
    DIR *dir;
    dir = opendir(path);
    if( dir) {
        Log.info("cleaning: %s", path);
        _delete_all_entries(path, 0, dir);
        closedir(dir);
        remove(path);
    }
}


// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  Serial.begin();
  while (!Serial.isConnected()) {
    delay(1000);
  }

  Log.info("start...");
  // // The core of your code will likely live here.
//   ls("/usr");

  cleanup_dir("/usr/my_cache");
  cleanup_dir("/usr/my_cache1");
  cleanup_dir("/usr/my_cache2");
  cleanup_dir("/usr/my_cache3");
  cleanup_dir("/usr/my_cache4");
  cleanup_dir("/usr/my_cache5");
  cleanup_dir("/usr/my_cache6");
  cleanup_dir("/usr/my_cache7");
  cleanup_dir("/usr/my_cache8");
  cleanup_dir("/usr/my_cache9");
  cleanup_dir("/usr/my_cache10");
  cleanup_dir("/usr/my_cache11");
  cleanup_dir("/usr/my_cache12");
  cleanup_dir("/usr/my_cache14");
  cleanup_dir("/usr/my_cache16");
  cleanup_dir("/usr/my_cache18");

  ls("/usr");

  Log.info("....done");
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  delay(3000);

}