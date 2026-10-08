#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

void	create_structure(void)
{
	int	objects_result;
	int	refs_result;
	int	heads_result;

	objects_result = mkdir(".mgit/objects", 0755);
	if (objects_result == 0)
		printf("objects klasörü oluşturuldu.\n");
	refs_result = mkdir(".mgit/refs", 0755);
	if (refs_result == 0)
		printf("refs klasörü oluşturuldu.\n");
	heads_result = mkdir(".mgit/refs/heads", 0755);
	if (heads_result == 0)
		printf("heads klasörü oluşturuldu.\n");
}

void	create_head(void)
{
	FILE	*head_file;

	head_file = fopen(".mgit/HEAD", "w");
	if (head_file != NULL)
	{
		printf("HEAD dosyası oluşturuldu.\n");
		fputs("ref: refs/heads/main\n", head_file);
		fclose(head_file);
	}
	else
		printf("HEAD dosyası oluşturulamadı.\n");
}

void	init_repository(void)
{
	int	result;

	result = mkdir(".mgit", 0755);
	if (result == 0)
	{
		printf("mini-git klasörü oluşturuldu.\n");
		create_structure();
		create_head();
	}
	else if (errno == EEXIST)
		printf("mini-git klasörü zaten mevcut.\n");
	else
	{
		printf("mini-git klasörü oluşturulamadı.\n");
		printf("errno = %d\n", errno);
	}
}

int	main(int argc, char **argv)
{
	printf("argc = %d\n", argc);
	if (argc > 1 && strcmp(argv[1], "init") == 0)
	{
		printf("init komutu geldi\n");
		init_repository();
	}
	return (0);
}