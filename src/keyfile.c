/*
  Author: Pascal Arlt <parlt@suse.com>
  Copyright (C) 2019 SUSE Linux GmbH

  Licensed under the GNU Lesser General Public License Version 2.1

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see
  <http://www.gnu.org/licenses/>.
*/

#include "../include/defines.h"
#include "../include/helpers.h"
#include "../include/keyfile.h"

#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void key_file_append(Key_File *kf) {
  if(kf->length++ >= kf->alloc_length) {
    kf->alloc_length++;
    kf->file_entry =
      realloc(kf->file_entry, (kf->alloc_length) * sizeof(struct file_entry));
    initialize(kf, kf->alloc_length - 1);
  }
}

/* --- GETTERS --- */

int32_t getIntValueNum(Key_File key_file, size_t num) {
  return strtol(key_file.file_entry[num].value, NULL, 10);
}

int64_t getInt64ValueNum(Key_File key_file, size_t num) {
  return strtol(key_file.file_entry[num].value, NULL, 10);
}

uint32_t getUIntValueNum(Key_File key_file, size_t num) {
  return strtoul(key_file.file_entry[num].value, NULL, 10);
}

uint64_t getUInt64ValueNum(Key_File key_file, size_t num) {
  return strtoul(key_file.file_entry[num].value, NULL, 10);
}

float getFloatValueNum(Key_File key_file, size_t num) {
  return strtof(key_file.file_entry[num].value, NULL);
}

double getDoubleValueNum(Key_File key_file, size_t num) {
  return strtod(key_file.file_entry[num].value, NULL);
}

char *getStringValueNum(Key_File key_file, size_t num) {
  return key_file.file_entry[num].value;
}

bool getBoolValueNum(Key_File key_file, size_t num) {
  size_t hash = hashstring(key_file.file_entry[num].value);
  if (hash == TRUE) {
    return 1;
  } else if (hash == FALSE) {
    return 0;
  }
  errno = EFAULT;
  return 0;
}

/* --- SETTERS --- */

void setGroup(Key_File *key_file, size_t num, char *value) {
  free(key_file->file_entry[num].group);
  key_file->file_entry[num].group = value;
}

void setKey(Key_File *key_file, size_t num, char *value) {
  free(key_file->file_entry[num].key);
  key_file->file_entry[num].key = strdup(value);
}

void setIntValueNum(Key_File *kf, size_t num, void *v) {
  int64_t *value = (int64_t*) v;
  free(kf->file_entry[num].value);
  size_t length = (*value == 0) ? 2 : log10(fabs(*value)) + (*value < 0) + 2;
  snprintf(kf->file_entry[num].value = malloc(length), length, "%" PRId64, *value);
}

void setUIntValueNum(Key_File *key_file, size_t num, void *v) {
  uint64_t *value = (uint64_t*) v;
  free(key_file->file_entry[num].value);
  size_t length = (*value == 0) ? 2 : log10(*value) + 2;
  snprintf(key_file->file_entry[num].value = malloc(length), length, "%" PRIu64, *value);
}

void setFloatValueNum(Key_File *key_file, size_t num, void *v) {
  float *value = (float*) v;
  free(key_file->file_entry[num].value);
  size_t length = snprintf(NULL, 0, "%.*g", FLT_DECIMAL_DIG - 1, *value);
  snprintf(key_file->file_entry[num].value = malloc(length + 1), length + 1, "%.*g",
           FLT_DECIMAL_DIG - 1, *value);
}

void setDoubleValueNum(Key_File *key_file, size_t num, void *v) {
  double *value = (double*) v;
  free(key_file->file_entry[num].value);
  size_t length = snprintf(NULL, 0, "%.*g", DBL_DECIMAL_DIG - 1, *value);
  snprintf(key_file->file_entry[num].value = malloc(length + 1), length + 1, "%.*g",
           DBL_DECIMAL_DIG - 1, *value);
}

void setStringValueNum(Key_File *key_file, size_t num, void *v) {
  char *value = (char*) v;
  free(key_file->file_entry[num].value);
  key_file->file_entry[num].value = strdup(value);
}

void setBoolValueNum(Key_File *kf, size_t num, void *v) {
  char *value = (char*) v;
  if (!*value) { errno = EINVAL; return; }

  char *tmp = strdup(value);
  size_t hash = hashstring(toLowerCase(tmp));

  if ((*value == '1' && strlen(tmp) == 1) || hash == YES || hash == TRUE) {
    free(kf->file_entry[num].value);
    kf->file_entry[num].value = strdup("true");
  } else if ((*value == '0' && strlen(tmp) == 1) || hash == NO || hash == FALSE) {
    free(kf->file_entry[num].value);
    kf->file_entry[num].value = strdup("false");
  } else if (hash == KEY_FILE_NULL_VALUE_HASH) {
    free(kf->file_entry[num].value);
    kf->file_entry[num].value = strdup(KEY_FILE_NULL_VALUE);
  } else { errno = EINVAL; }

  free(tmp);
}
