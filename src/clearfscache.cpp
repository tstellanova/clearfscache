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

/// List files in a directory to logger
static void list_directory_files(String &path, int level, DIR *dir, bool recursive=false) {
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
            if (recursive) {
                DIR *new_dir = opendir(new_path);
                if(new_dir) {
                    list_directory_files(new_path, level + 1, new_dir, recursive);
                    closedir(new_dir);
                }
            }
        }
    }
}

/// Delete all files in a directory
static void delete_all_directory_files(String &path, int level, DIR *dir) {
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
            Log.info("skip %s%s", indent.c_str(), entry->d_name);
        }
    }
}

/// List files in a path
static void list_files_at_path(String path, bool recursive=false) {
    Log.info("ls %s recursive: %u", path.c_str(), recursive);
    DIR *dir;
    dir = opendir(path);
    if(dir)  {
        list_directory_files(path, 0, dir, recursive);
        closedir(dir);
    }
}


/// Remove all files at a directory path, then remove the directory
static void cleanup_dir(String path) {
    DIR *dir;
    dir = opendir(path);
    if( dir) {
        Log.info("cleaning: %s", path.c_str());
        delete_all_directory_files(path, 0, dir);
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
  list_files_at_path("/", false);

  list_files_at_path("/usr", false);
  cleanup_dir("/usr/my_cache18");

  list_files_at_path("/usr", true);

  Log.info("....done");
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  delay(3000);

}