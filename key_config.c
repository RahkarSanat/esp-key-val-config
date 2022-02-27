/*
esp-key-val-config - a key value configuration storage component on esp-idf
Copyright (C) 2021 Amin Mokhtari - amin.mokhtari94@gmail.com

This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "key_config.h"

static const char *TAG = "key-val-config";

esp_err_t mount_configs() {
  ESP_LOGI(TAG, "mount SPIFFS at " BASE_PATH);
  esp_vfs_spiffs_conf_t conf = {
      .base_path = BASE_PATH, .partition_label = NULL, .max_files = MAX_FILES, .format_if_mount_failed = true};

  // Use settings defined above to initialize and mount SPIFFS filesystem.
  // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
    }
    return ret;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }

  return ret;
}

void unmount_configs() {
  // unmount partition and disable SPIFFS
  esp_err_t ret = esp_vfs_spiffs_unregister(NULL);
  if (ret != ESP_OK) {
    ESP_LOGW(TAG, "SPIFFS Already unmounted!");
  } else {
    ESP_LOGI(TAG, "SPIFFS unmounted");
  }
}

static char *get_file_path(const char *file_name) {
  static char buff[30];
  sprintf(buff, "%s/%s", BASE_PATH, file_name);
  return buff;
}

esp_err_t create_configs(const char *file_name, const char *config_value) {
  // Check if destination file exists before renaming
  struct stat st;
  if (stat(get_file_path(file_name), &st) == 0) {
    // file exists
    ESP_LOGE(TAG, "'%s' config file already exists, please remove it before creating.", file_name);
    return ESP_FAIL;
  }

  FILE *f = fopen(get_file_path(file_name), "w+r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open '%s' config file for reading and writing", file_name);
    return ESP_FAIL;
  }
  fprintf(f, config_value);

  char line[MAX_LINE_LEN];
  while (fgets(line, sizeof(line), f))
    printf(line);

  fclose(f);
  return ESP_OK;
}

esp_err_t remove_configs(const char *file_name) {
  // Check if destination file exists before renaming
  struct stat st;
  if (stat(get_file_path(file_name), &st) == 0) {
    // Delete it if it exists
    unlink(get_file_path(file_name));
    ESP_LOGW(TAG, "'%s' config file removed!", file_name);
    return ESP_OK;
  }

  ESP_LOGW(TAG, "Failed to removed '%s' config file, Not Exists!", file_name);
  return ESP_FAIL;
}

esp_err_t list_configs(const char *file_name) {

  FILE *f = fopen(get_file_path(file_name), "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open '%s' config file for reading", file_name);
    return ESP_FAIL;
  }

  char line[MAX_LINE_LEN];
  while (fgets(line, sizeof(line), f))
    printf(line);

  fclose(f);
  return ESP_OK;
}

esp_err_t set_config(const char *file_name, const char *key, const char *value) {

  FILE *f = fopen(get_file_path(file_name), "r+w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open '%s' config file for reading and writing", file_name);
    return ESP_FAIL;
  }

  char line[MAX_LINE_LEN];
  char file_buff[MAX_FILE_LEN] = {0};
  bool find = 0;
  while (fgets(line, sizeof(line), f)) {
    char *pos = strchr(line, '=');
    uint8_t eqi = (uint8_t)(pos - (char *)line);
    if (strlen(key) == eqi && !strncmp(line, key, strlen(key))) {
      find = 1;
      sprintf(file_buff + strlen(file_buff), "%s=%s\n", key, value);
    } else {
      sprintf(file_buff + strlen(file_buff), line);
    }
  }

  if (find) {
    fclose(f);

    unlink(get_file_path(file_name));
    f = fopen(get_file_path(file_name), "w+r");
    if (f == NULL) {
      ESP_LOGE(TAG, "Failed to open '%s' config file for reading and writing", file_name);
      return ESP_FAIL;
    }
    fprintf(f, file_buff);
  } else {
    fprintf(f, "%s=%s\n", key, value);
  }

  fclose(f);
  return ESP_OK;
}

esp_err_t unset_config(const char *file_name, const char *key) {

  FILE *f = fopen(get_file_path(file_name), "r+w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open '%s' config file for reading and writing", file_name);
    return ESP_FAIL;
  }

  char line[MAX_LINE_LEN];
  char file_buff[MAX_FILE_LEN] = {0};
  bool find = 0;
  while (fgets(line, sizeof(line), f)) {
    char *pos = strchr(line, '=');
    uint8_t eqi = (uint8_t)(pos - (char *)line);
    if (strlen(key) == eqi && !strncmp(line, key, strlen(key))) {
      find = 1;
    } else {
      sprintf(file_buff + strlen(file_buff), line);
    }
  }

  if (find) {
    fclose(f);

    unlink(get_file_path(file_name));
    f = fopen(get_file_path(file_name), "w+r");
    if (f == NULL) {
      ESP_LOGE(TAG, "Failed to open '%s' config file for reading and writing", file_name);
      return ESP_FAIL;
    }
    fprintf(f, file_buff);
    fclose(f);
    return ESP_OK;
  }

  fclose(f);
  return ESP_FAIL;
}

char *get_config(const char *file_name, const char *key) {
  FILE *f = fopen(get_file_path(file_name), "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open '%s' config file for writing", file_name);
    return 0;
  }

  char line[MAX_LINE_LEN];
  while (fgets(line, sizeof(line), f)) {
    char *pos = strchr(line, '\n');
    if (pos) {
      *pos = '\0';
    }
    pos = strchr(line, '=');

    if (pos) {
      uint8_t eqindex = (uint8_t)(pos - (char *)line);
      if (strlen(key) == eqindex && !strncmp(line, key, strlen(key))) {
        static char buff[20];
        sprintf(buff, "%s", line + eqindex + 1);
        fclose(f);
        return buff;
      }
    }
  }

  fclose(f);
  return 0;
}
