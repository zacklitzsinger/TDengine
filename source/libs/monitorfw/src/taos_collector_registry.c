/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <regex.h>
#include <stdio.h>

// Public
#include "taos_alloc.h"
#include "taos_collector.h"
#include "taos_collector_registry.h"

// Private
#include "taos_assert.h"
#include "taos_collector_registry_t.h"
#include "taos_collector_t.h"
#include "taos_errors.h"
#include "taos_log.h"
#include "taos_map_i.h"
#include "taos_metric_formatter_i.h"
#include "taos_metric_i.h"
#include "taos_metric_t.h"
#include "taos_string_builder_i.h"

#define ALLOW_FORBID_FUNC
#include "tjson.h"

taos_collector_registry_t *TAOS_COLLECTOR_REGISTRY_DEFAULT;

taos_collector_registry_t *taos_collector_registry_new(const char *name) {
  int r = 0;

  taos_collector_registry_t *self = (taos_collector_registry_t *)taos_malloc(sizeof(taos_collector_registry_t));

  self->disable_process_metrics = false;

  self->name = taos_strdup(name);
  self->collectors = taos_map_new();
  taos_map_set_free_value_fn(self->collectors, &taos_collector_free_generic);
  taos_map_set(self->collectors, "default", taos_collector_new("default"));

  self->metric_formatter = taos_metric_formatter_new();
  self->string_builder = taos_string_builder_new();
  self->out = taos_string_builder_new();
  self->lock = (pthread_rwlock_t *)taos_malloc(sizeof(pthread_rwlock_t));
  r = pthread_rwlock_init(self->lock, NULL);
  if (r) {
    TAOS_LOG("failed to initialize rwlock");
    return NULL;
  }
  return self;
}

int taos_collector_registry_default_init(void) {
  if (TAOS_COLLECTOR_REGISTRY_DEFAULT != NULL) return 0;

  TAOS_COLLECTOR_REGISTRY_DEFAULT = taos_collector_registry_new("default");
  //if (TAOS_COLLECTOR_REGISTRY_DEFAULT) {
  //  return taos_collector_registry_enable_process_metrics(TAOS_COLLECTOR_REGISTRY_DEFAULT);
  //}
  return 1;
}

int taos_collector_registry_destroy(taos_collector_registry_t *self) {
  if (self == NULL) return 0;

  int r = 0;
  int ret = 0;

  r = taos_map_destroy(self->collectors);
  self->collectors = NULL;
  if (r) ret = r;

  r = taos_metric_formatter_destroy(self->metric_formatter);
  self->metric_formatter = NULL;
  if (r) ret = r;

  r = taos_string_builder_destroy(self->string_builder);
  self->string_builder = NULL;
  if (r) ret = r;

  r = taos_string_builder_destroy(self->out);
  self->out = NULL;
  if (r) ret = r;

  r = pthread_rwlock_destroy(self->lock);
  taos_free(self->lock);
  self->lock = NULL;
  if (r) ret = r;

  taos_free((char *)self->name);
  self->name = NULL;

  taos_free(self);
  self = NULL;

  return ret;
}

int taos_collector_registry_register_metric(taos_metric_t *metric) {
  TAOS_ASSERT(metric != NULL);

  taos_collector_t *default_collector =
      (taos_collector_t *)taos_map_get(TAOS_COLLECTOR_REGISTRY_DEFAULT->collectors, "default");

  if (default_collector == NULL) {
    return 1;
  }

  return taos_collector_add_metric(default_collector, metric);
}

taos_metric_t *taos_collector_registry_must_register_metric(taos_metric_t *metric) {
  int err = taos_collector_registry_register_metric(metric);
  if (err != 0) {
    //exit(err);
    return NULL;
  }
  return metric;
}

int taos_collector_registry_register_collector(taos_collector_registry_t *self, taos_collector_t *collector) {
  TAOS_ASSERT(self != NULL);
  if (self == NULL) return 1;

  int r = 0;

  r = pthread_rwlock_wrlock(self->lock);
  if (r) {
    TAOS_LOG(TAOS_PTHREAD_RWLOCK_LOCK_ERROR);
    return 1;
  }
  if (taos_map_get(self->collectors, collector->name) != NULL) {
    TAOS_LOG("the given taos_collector_t* is already registered");
    int rr = pthread_rwlock_unlock(self->lock);
    if (rr) {
      TAOS_LOG(TAOS_PTHREAD_RWLOCK_UNLOCK_ERROR);
      return rr;
    } else {
      return 1;
    }
  }
  r = taos_map_set(self->collectors, collector->name, collector);
  if (r) {
    int rr = pthread_rwlock_unlock(self->lock);
    if (rr) {
      TAOS_LOG(TAOS_PTHREAD_RWLOCK_UNLOCK_ERROR);
      return rr;
    } else {
      return r;
    }
  }
  r = pthread_rwlock_unlock(self->lock);
  if (r) {
    TAOS_LOG(TAOS_PTHREAD_RWLOCK_UNLOCK_ERROR);
    return 1;
  }
  return 0;
}

int taos_collector_registry_validate_metric_name(taos_collector_registry_t *self, const char *metric_name) {
  regex_t r;
  int ret = 0;
  ret = regcomp(&r, "^[a-zA-Z_:][a-zA-Z0-9_:]*$", REG_EXTENDED);
  if (ret) {
    TAOS_LOG(TAOS_REGEX_REGCOMP_ERROR);
    regfree(&r);
    return ret;
  }

  ret = regexec(&r, metric_name, 0, NULL, 0);
  if (ret) {
    TAOS_LOG(TAOS_REGEX_REGEXEC_ERROR);
    regfree(&r);
    return ret;
  }
  regfree(&r);
  return 0;
}

const char *taos_collector_registry_bridge(taos_collector_registry_t *self, char *ts, char *format) {
  taos_metric_formatter_clear(self->metric_formatter);
  SJson* pJson = tjsonCreateObject();
  tjsonAddStringToObject(pJson, "ts", ts);
  tjsonAddDoubleToObject(pJson, "protocol", 2);
  taos_metric_formatter_load_metrics(self->metric_formatter, self->collectors, ts, format, pJson);
  char *out = taos_metric_formatter_dump(self->metric_formatter);

  int r = 0;
  r = taos_string_builder_add_str(self->out, out);
  if (r) return NULL;
  taos_free(out);

  //return taos_string_builder_str(self->out);
  return tjsonToString(pJson);
}

int taos_collector_registry_clear_out(taos_collector_registry_t *self){
  return taos_string_builder_clear(self->out);
}
