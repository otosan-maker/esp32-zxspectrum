#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <Arduino.h>
#include "GaldeanoFAT.h"
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include "esp_check.h"
#include "diskio_impl.h"
#include "diskio_wl.h"
#include "esp_vfs_fat.h"


GaldeanoFAT::GaldeanoFAT(const char *mountPoint)
{
  esp_err_t m_cod_err;

  ESP_LOGI(TAG, "Enter constructor");
  const esp_vfs_fat_mount_config_t mount_config = {
            .format_if_mount_failed = true,
            .max_files = 4,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    m_cod_err = esp_vfs_fat_spiflash_mount(mountPoint, "juegos", &mount_config, &wl_handle);
    if (m_cod_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(m_cod_err));
        Serial.printf("Failed to mount FATFS (%s)\n", esp_err_to_name(m_cod_err));
        return ;
    }
    Serial.printf("Rsult mount FATFS (%s)\n", esp_err_to_name(m_cod_err));
   
  if (m_cod_err==ESP_OK)
  {
    _isMounted = true;
    _mountPoint = mountPoint;
  } else
  {
    _isMounted = false;
    Serial.println("An error occurred while mounting FAT Filesystem");
  }
}

