 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char **argv)
{
    printf("argc = %d\n", argc);

    if (argc > 1 && strcmp(argv[1], "init") == 0)
    {
        printf("init komutu geldi\n");

        int result = mkdir(".mgit", 0755);

        if (result == 0)
        {
            printf("mini-git klasörü oluşturuldu.\n");
        }
        else if (errno == EEXIST)
        {
            printf("mini-git klasörü zaten mevcut.\n");
        }
        else
        {
            printf("mini-git klasörü oluşturulamadı.\n");
            return 1;
        }

        if (mkdir(".mgit/objects", 0755) == 0)
        {
            printf("objects klasörü oluşturuldu.\n");
        }
        else if (errno != EEXIST)
        {
            printf("objects klasörü oluşturulamadı.\n");
            return 1;
        }

        if (mkdir(".mgit/refs", 0755) == 0)
        {
            printf("refs klasörü oluşturuldu.\n");
        }
        else if (errno != EEXIST)
        {
            printf("refs klasörü oluşturulamadı.\n");
            return 1;
        }

        if (mkdir(".mgit/refs/heads", 0755) == 0)
        {
            printf("heads klasörü oluşturuldu.\n");
        }
        else if (errno != EEXIST)
        {
            printf("heads klasörü oluşturulamadı.\n");
            return 1;
        }

        FILE *head_file = fopen(".mgit/HEAD", "r");

        if (head_file == NULL)
        {
            head_file = fopen(".mgit/HEAD", "w");

            if (head_file == NULL)
            {
                printf("HEAD dosyası oluşturulamadı.\n");
                return 1;
            }

            fputs("ref: refs/heads/main\n", head_file);
            fclose(head_file);

            printf("HEAD dosyası oluşturuldu.\n");
        }
        else
        {
            fclose(head_file);
            printf("HEAD dosyası zaten mevcut.\n");
        }

        FILE *main_file = fopen(".mgit/refs/heads/main", "a");

        if (main_file == NULL)
        {
            printf("main branch dosyası oluşturulamadı.\n");
            return 1;
        }

        fclose(main_file);
        printf("main branch dosyası hazır.\n");
    }
    else if (argc > 1 && strcmp(argv[1], "status") == 0)
    {
        struct stat info;

        if (stat(".mgit", &info) == 0 && S_ISDIR(info.st_mode))
        {
            printf("mini-git durumu:\n");
            printf("depo mevcut.\n");
            printf("aktif branch: main\n");
        }
        else
        {
            printf("Hata: Bu klasor bir mini-git deposu degil.\n");
        }
    }
    else if (argc > 1)
    {
        printf("Bilinmeyen komut: %s\n", argv[1]);
    }
    else
    {
        printf("Lutfen bir komut girin.\n");
    }
}
