#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <ctype.h>

static const char *DIR_PATH = "/home/frain8/Documents/Sisop/temp";
static const char *LOG_PATH = "/home/frain8/SinSeiFS.log";
static const char *VIGENERE_KEY = "SISOP";

/** Cipher Code **/
// Encryption and Decryption atbash function
void atBash(char *input)
{
    char *text = strstr(input, "/");
    if (!text) {
        text = input;
    } else {
        text +=  1;
    }

    printf("In cipher: %s\n", text);
    int i = 0;
    while (text[i] != '\0') {
        if (isalpha(text[i])) {
            if (isupper(text[i])) {
                text[i] = 'Z' + 'A' - text[i];
            } else {
                text[i] = 'z' + 'a' - text[i];
            }
        } else if (text[i] == '.') {
            break;
        }
        i++;
    }
    printf("Out cipher: %s\n", text);
}

// Encryption and Decryption ROT13 function
// void ROT13(const char *plaintext, char *return_val)
// {
//     int len = plaintext.size();
//     int ascii_char;
//     for (int i=0; i<len; i++) {
//         ascii_char = plaintext[i];
//         if ((ascii_char >= 97 && ascii_char <= 122) || (ascii_char >= 65 && ascii_char <= 90)) {
//             if (ascii_char > 109 || (ascii_char > 77 && ascii_char < 91)) {
//                 ascii_char -= 13;
//                 return_val = return_val + ascii_char;
//             }
//             else {
//                 ascii_char += 13;
//                 return_val = return_val + ascii_char;
//             }
//         }
//     }
// }

// void vigenereEncode(char *text)
// {
//     text[0] += 1;
// }

// void vigenereDecode(const char *cyphertext, char *return_val)
// {
//     // Receive cyphertext as input
//     // Return  plaintext as output
//     strcpy(return_val, cyphertext);
//     return_val[0] -= 1;
// }


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
    sprintf(output, "%s::%s-%s:%s::%s", level, date, time, cmd, desc);

    // Add desc2 if exist
    if (desc2 != NULL) {
        strcat(output, "::");
        strcat(output, desc2);
    }

    FILE *F_out = fopen(LOG_PATH, "a+");
    fprintf(F_out, "%s\n", output);
    fclose(F_out);
}

void changePath(char *fpath, const char *path)
{
    sprintf(fpath, "%s", DIR_PATH);
    if (strcmp(path, "/") != 0) {
        strcat(fpath, path);
    } 
}

void getAwalan(const char *path, char *return_str, char *return_type)
{
    // Receive path as input (example: "AtoZ_folder/DATA_PEN.....")
    // Return awalan (string before the first "_") as output (example: "AtoZ")
    char type[3][9] = {"/AtoZ_", "/RX_", "/A_is_a_"};
    char *awalan = NULL;

    for (int i = 0; i < 3; i++) {
        awalan = strstr(path, type[i]);

        if (awalan != NULL) {
            printf("getAwalan: %s\n", type[i] + 1);
            strcpy(return_str, awalan);

            type[i][strlen(type[i]) - 1] = '\0';
            strcpy(return_type, type[i] + 1);
            return;
        }
    }
    printf("getAwalan: %s\n", path);
    strcpy(return_str, path);
    strcpy(return_type, "empty");
}

/** XMP Method **/
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    Log("GETATTR", path, NULL);

    int res;
    char fpath[1000], awalan[9];
    // char c_path[1000],
    // getAwalan(path, c_path, awalan);
    // if (strcmp(awalan, "AtoZ") == 0) {
    //     atBash(c_path);
    // }
    char *c_path = strstr(path, "AtoZ_");
    if (c_path) printf("%s\n", c_path);
    if (c_path != NULL && strchr(c_path, '/') != NULL) {
        atBash(c_path);
    }
    sprintf(fpath, "%s%s", DIR_PATH, path);
    printf("WARNING::Getattr: %s\n", fpath);

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    Log("READDIR", path, NULL);

    char fpath[1000], awalan[9];
    // char c_path[1000];
    // getAwalan(path, c_path, awalan);
    // if (strcmp(awalan, "AtoZ") == 0) {
    //     atBash(c_path);
    // }
    char *c_path = strstr(path, "AtoZ_");
    if (c_path) printf("%s\n", c_path);
    if (c_path != NULL && strchr(c_path, '/') != NULL) {
        atBash(c_path);
    }
    changePath(fpath, path);
    printf("WARNING::Readdir: %s\n", fpath);

    int res = 0;
    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;

    dp = opendir(fpath);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        // Encode filename
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
            if (c_path != NULL) {
                atBash(de->d_name);
            }
        }

        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0)
            break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    Log("READ", path, NULL);

    char fpath[1000];
    changePath(fpath, path);
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
    Log("WRITE", path, NULL);

    char fpath[1000];
    changePath(fpath, path);
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
    Log("TRUNCATE", path, NULL);

	printf("Trunc: %s\n", path);
    char fpath[1000];
    changePath(fpath, path);
    printf("Trunc path: %s\n", fpath);
	
	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    Log("MKDIR", path, NULL);

	int res;
    char fpath[1000];
    changePath(fpath, path);
    printf("Mkdir path: %s\n", fpath);

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
    Log("UNLINK", path, NULL);

	int res;
    char fpath[1000];
    changePath(fpath, path);
    printf("Unlink path: %s\n", fpath);

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    Log("RENAME", from, to);

	int res;
    char _from[1000], _to[1000];
    changePath(_from, from);
    changePath(_to, to);
    printf("Rename path: %s --> %s\n", _from, _to);

	res = rename(_from, _to);
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
    .unlink = xmp_unlink,
    .rename = xmp_rename
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}