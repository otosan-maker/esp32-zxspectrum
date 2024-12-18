#pragma once
#include "sdkconfig.h"
#include "esp_console.h"
#include "esp_check.h"
#include "esp_partition.h"
#include "driver/gpio.h"
#include <tinyusb.h>
#include "tusb_msc_storage.h"

static const char *TAG = "GaldeanoFAT";

class GaldeanoFAT
{
private:
  bool _isMounted;
  const char *_mountPoint;

  const char *m_base_path;
  int m_max_files;
  wl_handle_t wl_handle;

  esp_err_t storage_init_spiflashG(wl_handle_t *wl_handle);
  esp_err_t tinyusb_msc_storage_mountG(const char *base_path);

public:
  GaldeanoFAT(const char *mountPoint);
  ~GaldeanoFAT();
  bool isMounted() {
    return _isMounted;
  }
  const char *mountPoint() {
    return _mountPoint;
  }
};