#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Lutfen bir komut girin.\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0)
    {
        if (mkdir(".mgit", 0755) == 0)
            printf("Mini-git deposu olusturuldu.\n");
        else if (errno == EEXIST)
            printf("Mini-git deposu zaten mevcut.\n");
        else
        {
            printf("Depo olusturulamadi.\n");
            return 1;
        }

        if (mkdir(".mgit/objects", 0755) != 0 && errno != EEXIST)
            return 1;

        if (mkdir(".mgit/refs", 0755) != 0 && errno != EEXIST)
            return 1;

        if (mkdir(".mgit/refs/heads", 0755) != 0 && errno != EEXIST)
            return 1;

        FILE *file = fopen(".mgit/HEAD", "r");

        if (file == NULL)
        {
            file = fopen(".mgit/HEAD", "w");

            if (file == NULL)
                return 1;

            fputs("ref: refs/heads/main\n", file);
            fclose(file);
        }
        else
            fclose(file);

        file = fopen(".mgit/refs/heads/main", "a");

        if (file == NULL)
            return 1;

        fclose(file);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        struct stat info;

        if (stat(".mgit", &info) == 0 && S_ISDIR(info.st_mode))
        {
            printf("Mini-git durumu:\n");
            printf("Depo mevcut.\n");
            printf("Aktif branch: main\n");

            FILE *file = fopen(".mgit/index", "r");

            if (file == NULL)
                printf("Staging alani bos.\n");
            else
            {
                char line[100];

                printf("Staging alanindaki dosyalar:\n");

                while (fgets(line, sizeof(line), file) != NULL)
                    printf("- %s", line);

                fclose(file);
            }
        }
        else
            printf("Bu klasor bir mini-git deposu degil.\n");
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3)
        {
            printf("Eklenecek dosyanin adini girin.\n");
            return 1;
        }

        FILE *file = fopen(".mgit/index", "a");

        if (file == NULL)
        {
            printf("Index dosyasi acilamadi.\n");
            return 1;
        }

        fprintf(file, "%s\n", argv[2]);
        fclose(file);

        printf("%s staging alanina eklendi.\n", argv[2]);
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        printf("Commit ozelligini mevcut kodumuzla koruyalim.\n");
    }
    else if (strcmp(argv[1], "log") == 0)
    {
        printf("Log ozelligini mevcut kodumuzla koruyalim.\n");
    }
    else
        printf("Bilinmeyen komut: %s\n", argv[1]);

    return 0;
}
