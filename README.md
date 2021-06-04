# Kelompok E09
* 05111940000074 - Nur Ahmad Khatim
* 05111940000087 - William Handi Wijaya
* 05111940000212 - Fadhil Dimas Sucahyo

# Info
* Tidak diperkenankan menggunakan system() dan exec*(), kecuali ada pengecualian di butir soal.
<br><br>

# Soal 1
<br><br>

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
5. Jalankan fungsi `Log()` pada semua fungsi fuse.

Implementasi dari langkah-langkah di atas adalah sebagai berikut:
```
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
```

# Kendala
* Fungsi static tidak bisa menerima return value dari fungsi non-static. Kami pun memutuskan untuk mengirim data dari fungsi static ke fungsi non-static menggunakan argumen fungsi yang berupa pointer.