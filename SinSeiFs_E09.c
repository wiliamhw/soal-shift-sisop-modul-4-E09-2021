#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *DIR_PATH = "/home/frain8/Documents/Sisop/temp";
static const char *LOG_PATH = "/home/frain8/Documents/Sisop/Modul_4/soal_shift_4";
static const char *VIGENERE_KEY = "SISOP";

/** Cipher Code **/
char *atBashEncode(const char *plaintext)
{
    // Receive plaintext (normal text) as input
    // Return  cyphertext (encrypted text) as output
}

char *atBashDecode(const char *cyphertext)
{
    // Receive cyphertext as input
    // Return  plaintext as output
}

char *ROT13Encode(const char *plaintext)
{
    // Receive plaintext as input
    // Return  cyphertext as output
}

char *ROT13Decode(const char *cyphertext)
{
    // Receive cyphertext as input
    // Return  plaintext as output
}

char *vigenereEncode(const char *plaintext)
{
    // Receive plaintext as input
    // Return  cyphertext as output
}

char *vigenereDecode(const char *cyphertext)
{
    // Receive cyphertext as input
    // Return  plaintext as output
}


/** Helpers **/
void Log(const char *cmd, const char *desc, const char *desc2)
{
    char *level = (strcmp(cmd, "MKDIR") == 0 
                || strcmp(cmd, "UNLINK") == 0) 
                    ? "WARNING"
                    : "INFO";
    
    // Write to log file
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char time[50], date[50];
    sprintf(time, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(date, "%02d%02d%04d", tm.tm_mday, tm.tm_mon, tm.tm_year + 1900);

    char output[1000];
    sprintf(output, "%s::%s-%s:%s::/%s", level, date, time, cmd, desc);

    // Add desc2 if exist
    if (desc2 != NULL) {
        strcat(output, "::/");
        strcat(output, desc2);
    }

    FILE *F_out = fopen("SinSeiFS.log", "a+");
    fprintf(F_out, "%s\n", output);
    fclose(F_out);
}

char *getAwalan(const char *path)
{
    // Receive path as input (example: "AtoZ_folder/DATA_PEN.....")
    // Return awalan (string before the first "_") as output (example: "AtoZ")
}


/** XMP Method **/
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    // Log("GETATTR", path, NULL);
    int res;
    char fpath[1000];

    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;
        sprintf(fpath, "%s", path);
    }
    else {
        sprintf(fpath, "%s%s", DIR_PATH, path);
    } 
    printf("Getattr: %s\n", fpath);

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    // Log("READDIR", path, NULL);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;
        sprintf(fpath, "%s", path);
    }
    else {
        sprintf(fpath, "%s%s", DIR_PATH, path);
    } 

    printf("Readdir: %s\n", fpath);
    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;

    dp = opendir(fpath);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..")) {
            continue;
        }

        struct stat st;
        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0)
            break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // Log("READ", path, NULL);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;

        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", DIR_PATH, path);

    printf("Read: %s\n", fpath);
    int res = 0;
    int fd = 0;

    (void)fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1)
        res = -errno;

    close(fd);

    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    // Log("WRITE", path, NULL);

    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", DIR_PATH, path);

    printf("Write: %s\n", fpath);
    int res = 0;
    int fd = 0;
    (void)fi;

    fd = open(fpath, O_WRONLY);

    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    return res;
}

static int xmp_truncate(const char *path, off_t size)
{
    // Log("TRUNCATE", path, NULL);

	printf("Trunc: %s\n", path);
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;
        sprintf(fpath, "%s", path);
    }
    else
        sprintf(fpath, "%s%s", DIR_PATH, path);

    printf("Trunc path: %s\n", fpath);
	
	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    // Log("MAKEDIR", path, NULL);

	int res;
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;
        sprintf(fpath, "%s", path);
    }
    else {
        sprintf(fpath, "%s%s", DIR_PATH, path);
    }
    printf("Mkdir path: %s\n", fpath);

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
    // Log("UNLINK", path, NULL);

	int res;
    char fpath[1000];
    if (strcmp(path, "/") == 0) {
        path = DIR_PATH;
        sprintf(fpath, "%s", path);
    }
    else {
        sprintf(fpath, "%s%s", DIR_PATH, path);
    }
    printf("Unlink path: %s\n", fpath);

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .write = xmp_write,
    .truncate = xmp_truncate,
    .mkdir = xmp_mkdir,
    .unlink = xmp_unlink
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}