#ifndef UTILS_H
#define UTILS_H

char* extrageNumeIntrare(const char* numeComplet);
const char* getLastModified(struct timespec st_mtim);
const char* userPermissionToString(mode_t permission);
const char* groupPermissionToString(mode_t permission);
const char* otherPermissionToString(mode_t permission);

#endif