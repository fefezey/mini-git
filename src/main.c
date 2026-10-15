 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define INDEX_PATH ".mgit/index"
#define OBJECTS_PATH ".mgit/objects"
#define COMMITS_PATH ".mgit/commits"
#define SNAPSHOTS_PATH ".mgit/snapshots"
#define FILE_MAP_PATH ".mgit/file-map"

static int ensure_directory(const char *path)
{
    if (mkdir(path, 0755) == 0)
        return 0;

    if (errno == EEXIST)
    {
        struct stat info;

        if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
            return 0;
    }

    printf("Klasor olusturulamadi: %s\n", path);
    return 1;
}

static int is_repository(void)
{
    struct stat info;

    return stat(".mgit", &info) == 0 && S_ISDIR(info.st_mode);
}

static int next_commit_id(void)
{
    FILE *log = fopen(COMMITS_PATH, "r");
    char line[2048];
    int count = 0;

    if (log == NULL)
        return 1;

    while (fgets(line, sizeof(line), log) != NULL)
        count++;

    fclose(log);
    return count + 1;
}

static int next_object_number(void)
{
    char path[128];
    int number = 1;

    while (1)
    {
        snprintf(path, sizeof(path),
            OBJECTS_PATH "/object-%d", number);

        if (access(path, F_OK) != 0)
            return number;

        number++;
    }
}

static int copy_file(const char *source_path,
    const char *target_path)
{
    FILE *source = fopen(source_path, "rb");

    if (source == NULL)
    {
        printf("Kaynak dosya acilamadi: %s\n", source_path);
        return 1;
    }

    FILE *target = fopen(target_path, "wb");

    if (target == NULL)
    {
        fclose(source);
        printf("Nesne dosyasi olusturulamadi: %s\n", target_path);
        return 1;
    }

    int ch;
    int error = 0;

    while ((ch = fgetc(source)) != EOF)
    {
        if (fputc(ch, target) == EOF)
        {
            error = 1;
            break;
        }
    }

    if (ferror(source))
        error = 1;

    if (fclose(source) != 0)
        error = 1;

    if (fclose(target) != 0)
        error = 1;

    if (error)
    {
        printf("Dosya icerigi kaydedilemedi: %s\n", source_path);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Lutfen bir komut girin.\n");
        return 1;
    }

    /* INIT */
    if (strcmp(argv[1], "init") == 0)
    {
        if (ensure_directory(".mgit") != 0
            || ensure_directory(OBJECTS_PATH) != 0
            || ensure_directory(".mgit/refs") != 0
            || ensure_directory(".mgit/refs/heads") != 0
            || ensure_directory(SNAPSHOTS_PATH) != 0)
        {
            return 1;
        }

        FILE *head = fopen(".mgit/HEAD", "r");

        if (head == NULL)
        {
            head = fopen(".mgit/HEAD", "w");

            if (head == NULL)
            {
                printf("HEAD dosyasi olusturulamadi.\n");
                return 1;
            }

            if (fputs("ref: refs/heads/main\n", head) == EOF)
            {
                fclose(head);
                return 1;
            }

            fclose(head);
        }
        else
        {
            fclose(head);
        }

        FILE *branch = fopen(".mgit/refs/heads/main", "a");

        if (branch == NULL)
        {
            printf("Branch dosyasi acilamadi.\n");
            return 1;
        }

        fclose(branch);
        printf("Mini-git deposu hazir.\n");
    }

    /* STATUS */
    else if (strcmp(argv[1], "status") == 0)
    {
        if (!is_repository())
        {
            printf("Bu klasor bir mini-git deposu degil.\n");
            return 1;
        }

        printf("Mini-git durumu:\n");
        printf("Depo mevcut.\n");
        printf("Aktif branch: main\n");
        printf("Staging alanindaki dosyalar:\n");

        FILE *index = fopen(INDEX_PATH, "r");

        if (index == NULL)
        {
            printf("Staging alani bos.\n");
            return 0;
        }

        char line[1024];
        int found = 0;

        while (fgets(line, sizeof(line), index) != NULL)
        {
            if (line[0] == '\n' || line[0] == '\0')
                continue;

            printf("- %s", line);
            found = 1;
        }

        if (!found)
            printf("Staging alani bos.\n");

        fclose(index);
    }

    /* ADD */
    else if (strcmp(argv[1], "add") == 0)
    {
        if (argc < 3)
        {
            printf("Kullanim: ./mgit add <dosya>\n");
            return 1;
        }

        if (!is_repository())
        {
            printf("Bu klasor bir mini-git deposu degil.\n");
            return 1;
        }

        struct stat info;

        if (stat(argv[2], &info) != 0 || !S_ISREG(info.st_mode))
        {
            printf("Dosya bulunamadi veya normal bir dosya degil.\n");
            return 1;
        }

        FILE *index = fopen(INDEX_PATH, "r");
        char line[1024];

        if (index != NULL)
        {
            while (fgets(line, sizeof(line), index) != NULL)
            {
                line[strcspn(line, "\n")] = '\0';

                if (strcmp(line, argv[2]) == 0)
                {
                    fclose(index);
                    printf("Dosya zaten staging alaninda.\n");
                    return 0;
                }
            }

            fclose(index);
        }

        index = fopen(INDEX_PATH, "a");

        if (index == NULL)
        {
            printf("Index dosyasi acilamadi.\n");
            return 1;
        }

        if (fprintf(index, "%s\n", argv[2]) < 0)
        {
            fclose(index);
            printf("Dosya index'e yazilamadi.\n");
            return 1;
        }

        if (fclose(index) != 0)
        {
            printf("Index dosyasi kapatilamadi.\n");
            return 1;
        }

        printf("%s staging alanina eklendi.\n", argv[2]);
    }

    /* COMMIT */
    else if (strcmp(argv[1], "commit") == 0)
    {
        if (argc < 4 || strcmp(argv[2], "-m") != 0)
        {
            printf("Kullanim: ./mgit commit -m \"mesaj\"\n");
            return 1;
        }

        if (!is_repository())
        {
            printf("Bu klasor bir mini-git deposu degil.\n");
            return 1;
        }

        FILE *index = fopen(INDEX_PATH, "r");

        if (index == NULL)
        {
            printf("Staging alani bos.\n");
            return 1;
        }

        char filepath[1024];
        int has_files = 0;

        while (fgets(filepath, sizeof(filepath), index) != NULL)
        {
            filepath[strcspn(filepath, "\n")] = '\0';

            if (filepath[0] != '\0')
                has_files = 1;
        }

        fclose(index);

        if (!has_files)
        {
            printf("Commit edilecek dosya yok.\n");
            return 1;
        }

        if (ensure_directory(OBJECTS_PATH) != 0
            || ensure_directory(SNAPSHOTS_PATH) != 0)
        {
            return 1;
        }

        int commit_id = next_commit_id();
        char snapshot_path[128];
        char temporary_path[128];

        snprintf(snapshot_path, sizeof(snapshot_path),
            SNAPSHOTS_PATH "/commit-%d", commit_id);

        snprintf(temporary_path, sizeof(temporary_path),
            SNAPSHOTS_PATH "/commit-%d.tmp", commit_id);

        FILE *snapshot = fopen(temporary_path, "w");

        if (snapshot == NULL)
        {
            printf("Snapshot dosyasi olusturulamadi.\n");
            return 1;
        }

        index = fopen(INDEX_PATH, "r");

        if (index == NULL)
        {
            fclose(snapshot);
            remove(temporary_path);
            printf("Index dosyasi acilamadi.\n");
            return 1;
        }

        int object_number = next_object_number();
        int error = 0;

        while (fgets(filepath, sizeof(filepath), index) != NULL)
        {
            filepath[strcspn(filepath, "\n")] = '\0';

            if (filepath[0] == '\0')
                continue;

            char object_path[128];

            snprintf(object_path, sizeof(object_path),
                OBJECTS_PATH "/object-%d", object_number);

            object_number++;

            if (copy_file(filepath, object_path) != 0)
            {
                error = 1;
                break;
            }

            if (fprintf(snapshot, "%s\t%s\n",
                filepath, object_path) < 0)
            {
                printf("Snapshot kaydi yazilamadi.\n");
                error = 1;
                break;
            }

            printf("%s dosyasi %s nesnesine kaydedildi.\n",
                filepath, object_path);
        }

        fclose(index);

        if (fclose(snapshot) != 0)
            error = 1;

        if (error)
        {
            remove(temporary_path);
            printf("Commit tamamlanamadi.\n");
            return 1;
        }

        if (rename(temporary_path, snapshot_path) != 0)
        {
            remove(temporary_path);
            printf("Snapshot kaydedilemedi.\n");
            return 1;
        }

        FILE *log = fopen(COMMITS_PATH, "a");

        if (log == NULL)
        {
            printf("Commit kaydi acilamadi.\n");
            return 1;
        }

        if (fprintf(log, "%d|%s\n", commit_id, argv[3]) < 0)
        {
            fclose(log);
            printf("Commit mesaji kaydedilemedi.\n");
            return 1;
        }

        if (fclose(log) != 0)
        {
            printf("Commit kaydi kapatilamadi.\n");
            return 1;
        }

        printf("Commit olusturuldu: #%d - %s\n",
            commit_id, argv[3]);
        printf("Snapshot: %s\n", snapshot_path);
    }

    /* LOG */
    else if (strcmp(argv[1], "log") == 0)
    {
        FILE *log = fopen(COMMITS_PATH, "r");

        if (log == NULL)
        {
            printf("Commit kaydi bulunamadi.\n");
            return 1;
        }

        char line[2048];

        while (fgets(line, sizeof(line), log) != NULL)
        {
            line[strcspn(line, "\n")] = '\0';

            char *separator = strchr(line, '|');

            if (separator != NULL)
            {
                *separator = '\0';
                printf("Commit #%s: %s\n",
                    line, separator + 1);
            }
            else
            {
                printf("%s\n", line);
            }
        }

        fclose(log);
    }

    else
    {
        printf("Bilinmeyen komut: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
