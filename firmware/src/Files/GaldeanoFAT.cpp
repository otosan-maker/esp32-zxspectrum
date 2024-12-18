#include <Arduino.h>
#include "GaldeanoFAT.h"
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include "esp_check.h"
#include "diskio_impl.h"
#include "diskio_wl.h"


//funciones de tusb_msc

esp_err_t tinyusb_msc_storage_mount(const char *base_path);
//static tinyusb_msc_storage_handle_s *s_storage_handle;
extern  esp_err_t storage_init_spiflash(wl_handle_t *wl_handle);
extern void storage_mount_changed_cb(tinyusb_msc_event_t *event);
extern void _mount(void);


GaldeanoFAT::GaldeanoFAT(const char *mountPoint)
{
  esp_err_t m_cod_err;

  m_cod_err=storage_init_spiflashG(&wl_handle);
  Serial.printf("storage_init_spiflashG  %d",m_cod_err);

  m_max_files = 5;
  
  tinyusb_msc_storage_mountG(mountPoint);

  Serial.printf("storage_init_spiflash  %d",m_cod_err);

   
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



 esp_err_t GaldeanoFAT::storage_init_spiflashG(wl_handle_t *wl_handle)
{
    ESP_LOGI(TAG, "Initializing wear levelling");

    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, NULL);
    if (data_partition == NULL) {
        ESP_LOGE(TAG, "Failed to find FATFS partition. Check the partition table.");
        return ESP_ERR_NOT_FOUND;
    }

    return wl_mount(data_partition, wl_handle);
}


esp_err_t GaldeanoFAT::tinyusb_msc_storage_mountG(const char *base_path)
{
    esp_err_t ret = ESP_OK;

    if (_isMounted) {
        return ESP_OK;
    }


    if (!base_path) {
        base_path = "/fs"; //CONFIG_TINYUSB_MSC_MOUNT_PATH;
    }

    // connect driver to FATFS
    BYTE pdrv = 0xFF;
    ff_diskio_get_drive(&pdrv);
    char drv[3] = {(char)('0' + pdrv), ':', 0};

    ff_diskio_register_wl_partition(pdrv, wl_handle);


    FATFS *fs = NULL;
    ret = esp_vfs_fat_register(base_path, drv, m_max_files, &fs);
  

    _isMounted = true;
    m_base_path = base_path;

    return ret;
}
