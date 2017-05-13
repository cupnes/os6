#ifndef _SHELL_H_
#define _SHELL_H_

#define MAX_LINE_SIZE	512
#define MAX_ARGS	100
#define MAX_FILE_BUF	1024
#define MAX_FILE_LEN	32

void execute_line(unsigned short *buf);
void shell(void);

#endif /* _SHELL_H_ */
