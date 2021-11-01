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

#ifndef KEY_VAL_CONFIG_H
#define KEY_VAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BASE_PATH CONFIG_KVC_BASE_PATH
#define MAX_FILES CONFIG_KVC_MAX_FILES
#define MAX_FILE_LEN CONFIG_KVC_MAX_FILE_LEN
#define MAX_LINE_LEN CONFIG_KVC_MAX_LINE_LEN //!< max is 255 (uint8_t)

esp_err_t mount_configs();
void unmount_configs();
esp_err_t create_configs(const char *file_name, const char *config_value);
esp_err_t remove_configs(const char *file_name);
esp_err_t list_configs(const char *file_name);
esp_err_t set_config(const char *file_name, const char *key, const char *value);
esp_err_t unset_config(const char *file_name, const char *key);
char *get_config(const char *file_name, const char *key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KEY_VAL_CONFIG_H */
