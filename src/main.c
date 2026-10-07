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