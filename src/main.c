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

            int objects_result = mkdir(".mgit/objects", 0755);

            if (objects_result == 0)
            {
                printf("objects klasörü oluşturuldu.\n");
            }

            int refs_result = mkdir(".mgit/refs", 0755);

            if (refs_result == 0)
            {
                printf("refs klasörü oluşturuldu.\n");
            }

            int heads_result = mkdir(".mgit/refs/heads", 0755);

            if (heads_result == 0)
            {
                printf("heads klasörü oluşturuldu.\n");
            }

            FILE *head_file = fopen(".mgit/HEAD", "w");

            if (head_file != NULL)
            {
                printf("HEAD dosyası oluşturuldu.\n");

                fputs("ref: refs/heads/main\n", head_file);

                fclose(head_file);
            }
            else
            {
                printf("HEAD dosyası oluşturulamadı.\n");
            }

            FILE *main_file = fopen(".mgit/refs/heads/main", "w");

            if (main_file != NULL)
            {
                printf("main branch dosyası oluşturuldu.\n");

                fclose(main_file);
            }
            else
            {
                printf("main branch dosyası oluşturulamadı.\n");
            }
        }
        else
        {
            if (errno == EEXIST)
            {
                printf("mini-git klasörü zaten mevcut.\n");
            }
            else
            {
                printf("mini-git klasörü oluşturulamadı.\n");
                printf("errno = %d\n", errno);
            }
        }
    }

    return 0;
}