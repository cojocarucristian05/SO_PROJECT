#ifndef PRINTER_H
#define PRINTER_H

void writePermission(int sfd, mode_t permission);
void writePermissionLink(int sfd, mode_t permission);

#endif