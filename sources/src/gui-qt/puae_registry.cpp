/*
 * PUAE
 *
 * Copyright 2010 Mustafa 'GnoStiC' TUFAN
 *
 */

#include <QSettings>
extern "C" {
#include "sysconfig.h"
#include "sysdeps.h"
#include "limits.h"
#include "crc32.h"
#include <ctype.h>
#include "puae_registry.h"
}

QSettings settings;

int regsetstr (const char *name, const char *str)
{
	settings.setValue(name, str);
	return 1;
}

int regsetint (const char *name, int val)
{
	char tmp[100];
	printf (tmp, "%d", val);
	settings.setValue(name, tmp);
	return 1;
}

int regqueryint (const char *name, int *val)
{
	int ret = 0;

	*val = settings.value(name).toInt();

	ret = 1;
	return ret;
}

int regquerystr (const char *name, char *str, int *size)
{
	QString foo = settings.value(name).toString();

	return 1;
}

int regenumstr (int idx, char *name, int *nsize, char *str, int *size)
{
	name[0] = 0;
	str[0] = 0;
	int ret = 0;
	int tmpsize = 65536;
	char *tmp = xmalloc (char, tmpsize);

	*tmp = settings.value(name, 0).toInt();
		int i;
		char *p = tmp, *p2;
		for (i = 0; i < idx; i++) {
			if (p[0] == 0)
				break;
			p += strlen (p) + 1;
		}
		if (p[0]) {
			p2 = strchr (p, '=');
			*p2++ = 0;
			strcpy (name, p);
			strcpy (str, p2);
			ret = 1;
		}

	xfree (tmp);
	return ret;
}

int regquerydatasize (const char *name, int *size)
{
	int ret = 0;
	int csize = 65536;
	char *tmp = xmalloc (char, csize);
	if (regquerystr (name, tmp, &csize)) {
		*size = strlen (tmp);
		ret = 1;
	}
	xfree (tmp);
	return ret;
}

int regsetdata (const char *name, const void *str, int size)
{
	uae_u8 *in = (uae_u8*)str;
	int ret;
	int i;
	char *tmp = xmalloc (char, size * 2 + 1);
	for (i = 0; i < size; i++)
		printf (tmp + i * 2, "%02X", in[i]);
	settings.setValue(name, tmp);
	ret = 1;
	xfree (tmp);
	return ret;
}

int regquerydata (const char *name, void *str, int *size)
{
	int csize = (*size) * 2 + 1;
	int i, j;
	int ret = 0;
	char *tmp = xmalloc (char, csize);
	uae_u8 *out = (uae_u8*)str;

	if (!regquerystr (name, tmp, &csize))
		goto err;
	j = 0;
	for (i = 0; i < strlen (tmp); i += 2) {
		char c1 = toupper(tmp[i + 0]);
		char c2 = toupper(tmp[i + 1]);
		if (c1 >= 'A')
			c1 -= 'A' - 10;
		else if (c1 >= '0')
			c1 -= '0';
		if (c1 > 15)
			goto err;
		if (c2 >= 'A')
			c2 -= 'A' - 10;
		else if (c2 >= '0')
			c2 -= '0';
		if (c2 > 15)
			goto err;
		out[j++] = c1 * 16 + c2;
	}
	ret = 1;
err:
	xfree (tmp);
	return ret;
}

int regdelete (const char *name)
{
	if (strlen(name) > 0)
		settings.remove(name);

	return 1;
}

int regexists (const char *name)
{
	return settings.contains(name);
}

void regdeletetree (const char *name)
{
	if (strlen(name) > 0)
		settings.remove(name);
}

int regexiststree (const char *name)
{
	return settings.contains(name);
}

void regstatus (void)
{
	write_log ("WARNING: Unsupported '%s' enabled\n", "");
}

int reginitializeinit (const char *ppath)
{
	QSettings settings("PUAE", "2.5.0");

	settings.beginGroup("Warning");
	settings.setValue("info1", "This is unsupported file. Compatibility between versions is not guaranteed.");
	settings.setValue("info2", "Incompatible ini-files may be re-created from scratch!");
	settings.endGroup();

	return 1;
}

int getregmode (void)
{
	return 1;
}

