#ifndef UTILS_H
#define UTILS_H

const char* getLastModified(struct timespec st_mtim);
const char* permissionToString(mode_t permission);

#endif