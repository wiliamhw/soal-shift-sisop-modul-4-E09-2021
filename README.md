# Kelompok E09
* 05111940000074 - Nur Ahmad Khatim
* 05111940000087 - William Handi Wijaya
* 05111940000212 - Fadhil Dimas Sucahyo

# Info
* Tidak diperkenankan menggunakan system() dan exec*(), kecuali ada pengecualian di butir soal.
<br><br>

# Soal 1
### Penjelasan Soal
* Cipher yang digunakan dalam soal ini adalah Atbash Cipher.
* Jika direktori bernama `AtoZ_` atau di-rename dengan awalan `AtoZ_`, maka direktori tersebut akan menjadi direktori ter-encode.
* Jika direktori yang terenkripsi di-rename menjadi tidak ter-encode, isi direktori tersebut juga akan terdecode.
* Setiap pembuatan direktori ter-encode (`mkdir` atau `rename`) akan tercatat ke sebuah log.
  * Format: `/home/<USER>/Downloads/<Nama Direktori> → /home/<USER>/Downloads/AtoZ_<Nama Direktori>`.
* Ekstensi tidak diencode.
* Metode encode berlaku rekursif.

### Penyelesaian Soal
1. Buat fungsi `Log()` dengan parameter alamat awal dan alamat baru.
   * Fungsi ini akan mencatat perubahan nama suatu direktori di di file bernama `SISOP.log`.
   * Format: `/home/<USER>/Downloads/<Nama Direktori> → /home/<USER>/Downloads/AtoZ_<Nama Direktori>`.
2. Buat fungsi `atBash()` untuk melakukan encode dan decode pada suatu string input.
3. Pada fungsi `readdir()`, encode semua nama file dari direktori dengan awalan `AtoZ_`.
4. Pada fungsi `truncate()`, decode path saat ini setelah menjalankan fungsi `truncate()`.
5. Pada fungsi `mkdir()`, decode path saaat ini setelah menjalankan fungsi `mkdir()`.
6. Pada semua fungsi FUSE, ganti semua path dari parameter menjadi `<DIR_PATH> + <path saat ini yang sudah didecode>`.
   1. Dapatkan awalan dari path saat ini.
   2. Decode path sesuai dengan awalan.
   3. Ganti path saat ini menjadi `<DIR_PATH> + <path saat ini yang sudah didecode>`.
7. Pada fungsi `mkdir()` dan `rename()`, jika awalan berupa `AtoZ_`, catat path sumber dan path destinasi dengan fungsi `Log()`.

Implementasi fungsi `Log()` ada sebagai berikut:
```
void Log(const char *origin_path, const char *atoz_path)
{
    FILE *F_out = fopen(LOG_PATH, "a+");
    fprintf(F_out, "%s -> %s\n", origin_path, atoz_path);
    fclose(F_out);
}
```
sedangkan implementasi dari fungsi atBash() adalah sebagai berikut:
```
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
```


# Soal 2
<br><br>

# Soal 3
<br><br>

# Soal 4
## Penjelasan Soal
* Buat log file bernama `SinSeiFS.log` pada direktori `/home/<user>/SinSeiFs.log`.
* Log ini akan menyimpan daftar perintah system call yang telah dijalankan.
* Log akan mempunyai dua level yaitu **INFO** dan **WARNING**.
* Log level **WARNING** digunakan untuk mencatat syscall `mkdir` dan `unlink`.
* Log level **INFO** akan mencatat sisanya.
* Format untuk logging adalah:
  * `<level>::<dd><mm><yyyy>-<HH>:<MM>:<SS>:<CMD>::[DESC :: DESC]`
  * Jam yang digunakan dalam format 24 jam.
  * DESC: informasi dan parameter tambahan
* Contoh dari penulisan loggin adalah:
  * `INFO::28052021-10:00:00:CREATE::/test.txt`
  * `INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt`

## Penyelesaian Soal
1. Catat path dari log file.
2. Pastikan fungsi fuse untuk `mkdir` dan `unlink` sudah diimplementasikan.
3. Buat fungsi `Log()` yang menerima tiga argumen, yaitu `CMD`, `DSC1`, `DSC2`.
   * Argument `CMD` harus berupa huruf kapital.
4. Implementasikan fungsi `Log()` untuk menulis ke log file sesuai format dan argument yang ada.
   * Jika `CMD` sama dengan `mkdir` atau `unlink`, `level = "WARNING"`.
   * Selain itu, `level = "INFO"`.
5. Jalankan fungsi `Log()` pada semua fungsi FUSE.

Implementasi dari langkah-langkah di atas adalah sebagai berikut:
```
void sysLog(const char *cmd, const char *desc, const char *desc2)
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

    FILE *F_out = fopen(SYS_LOG_PATH, "a+");
    fprintf(F_out, "%s\n", output);
    fclose(F_out);
}
```

# Kendala
* Fungsi static tidak bisa menerima return value dari fungsi non-static. Kami pun memutuskan untuk mengirim data dari fungsi static ke fungsi non-static menggunakan argumen fungsi yang berupa pointer. Hal ini kami lakukan saat ingin mengirim data dari fungsi non-static menuji fungsi static.