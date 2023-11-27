#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "writer.h"
#include "utils.h"

void writePermission(int sfd, mode_t permission)
{
    dprintf(sfd, "drepturi de acces user: %s\n", userPermissionToString(permission & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup: %s\n", groupPermissionToString(permission & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii: %s\n\n", otherPermissionToString(permission & S_IRWXO));
}

void writePermissionLink(int sfd, mode_t permission)
{
    dprintf(sfd, "drepturi de acces user legatura: %s\n", userPermissionToString(permission & S_IRWXU));
    dprintf(sfd, "drepturi de acces grup legatura: %s\n", groupPermissionToString(permission & S_IRWXG));
    dprintf(sfd, "drepturi de acces altii legatura: %s\n\n", otherPermissionToString(permission & S_IRWXO));
}