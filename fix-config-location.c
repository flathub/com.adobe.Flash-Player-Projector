#define _GNU_SOURCE
#define __USE_LARGEFILE64

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>

#ifdef DEBUG
#define D(x...) printf(x)
#else
#define D(x...)
#endif

#define NUM_DIRS 2
struct {
	char *dirname;
	char *orig_name;
	char *dest_name;
} dirs[] = {
	{ ".adobe" },
	{ ".macromedia" }
};

static DIR *(*od)(const char *name) = 0;

static void
init (void)
{
	unsigned int i;
	const char *home;

	od = dlsym(RTLD_NEXT, "opendir");

	home = getenv ("HOME");
	assert (home);

	for (i = 0; i < NUM_DIRS; i++) {
		char buffer[PATH_MAX];

		/* ~/.adobe */
		memset (buffer, 0, PATH_MAX);
		strcat (buffer, home);
		if (buffer[strlen (buffer) - 1] != '/')
			strcat (buffer, "/");
		strcat (buffer, dirs[i].dirname);

		dirs[i].orig_name = strdup (buffer);
		D("orig name: %s\n", dirs[i].orig_name);

		/* ~/.local/share/com.adobe.Flash-Player-Projector/.adobe */
		memset (buffer, 0, PATH_MAX);
		strcat (buffer, home);
		if (buffer[strlen (buffer) - 1] != '/')
			strcat (buffer, "/");
		strcat (buffer, ".var/app/com.adobe.Flash-Player-Projector/");
		strcat (buffer, dirs[i].dirname);

		dirs[i].dest_name = strdup (buffer);
		D("dest name: %s\n", dirs[i].dest_name);
	}
}

DIR *
opendir (const char *name)
{
	DIR *dir;
	unsigned int i;

	if (!od)
		init();

	D("opendir(\"%s\")\n", name);

	for (i = 0; i < NUM_DIRS; i++) {
		if (strncmp (name, dirs[i].orig_name, strlen (dirs[i].orig_name)) == 0) {
			char dest[PATH_MAX];

			memset (dest, 0, PATH_MAX);
			strcat (dest, dirs[i].dest_name);
			strcat (dest, name + strlen (dirs[i].orig_name));
			D("redirection %s to %s\n", name, dest);
			dir = (*od) (dest);
			return dir;
		}
	}

	return (*od) (name);
}
