#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* configuration */
static const float step = 10.0;
static const float minimal = 10.0;
static const float maximal = 100.0;
static const char maxbrightnesspath[] = "/sys/devices/pci0000:00/0000:00:02.0/drm/card0/card0-eDP-1/intel_backlight/max_brightness";
static const char brightnesspath[] = "/sys/devices/pci0000:00/0000:00:02.0/drm/card0/card0-eDP-1/intel_backlight/brightness";

/* helpers */
void
die(const char *errstr, ...) {
    va_list ap;

    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

int
read_int(const char *path)
{
    FILE *fp;
    int value;
    fp=fopen(path, "r");
    if(fp == NULL)
        die("cannot open in read-only mode: %s\n", path);
    if(fscanf(fp, "%u", &value)!=1)
        die("cannot parse value from: %s\n", path);
    fclose(fp);
    return value;
}

void
write_int(const char *path, const int value)
{
    FILE *fp;
    fp=fopen(path, "w");
    if(fp == NULL)
        die("cannot open in write mode: %s\n", path);
    fprintf(fp, "%u", value);
    fclose(fp);
}

float
edge(const float a, const float b, const float c)
{
    if(b < a)
        return a;
    if(b > c)
        return c;
    return b;
}

/* core procedure */
void
step_brightness(const int inc)
{
    float max,level,next_level;
    max=(float) read_int(maxbrightnesspath);
    level=(float) read_int(brightnesspath) / max;
    next_level=level + (step * (float) inc) / 100.0;
    next_level = edge((minimal / 100.0), next_level, (maximal / 100.0));
    printf("%.0f%% -> %.0f%%\n", (level * 100.0), (next_level * 100.0));
    write_int(brightnesspath, (int) (next_level * max));
}

/* main function */
int
main(int argc, char *argv[]) {
    if(argc == 2 && !strcmp("up", argv[1]))
        step_brightness(+1);
    else if(argc == 2 && !strcmp("down", argv[1]))
        step_brightness(-1);
    else
        die("usage: %s <up|down>\n", argv[0]);
    return 0;
}
