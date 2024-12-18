#include "USB.h"
#include "FirmwareMSC.h"
#include "USBMSC.h"
#include "FFat.h"
#include <SPIFFS.h>
#include "FS.h"

FirmwareMSC MSC_Update;
// USB Mass Storage Class (MSC) object
USBMSC msc;


// Block size of flash memory (in bytes) (4KB)
#define BLOCK_SIZE 4096


// Flash memory object
EspClass _flash;

// Partition information object
const esp_partition_t* Partition;


void listDir(fs::FS& fs, const char* dirname, int numTabs = 0);


const esp_partition_t* partition() {
  const esp_partition_t* Partition;
  // Return the first FAT partition found (should be the only one)
  Partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
  if (Partition==NULL)
    Serial.println("La particion es: NULL");
  return Partition;
}


static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) {
  //Serial.println("Write lba: " + String(lba) + " offset: " + String(offset) + " bufsize: " + String(bufsize));

  // Erase block before writing as to not leave any garbage
  _flash.partitionEraseRange(Partition, offset + (lba * BLOCK_SIZE), bufsize);

  // Write data to flash memory in blocks from buffer
  _flash.partitionWrite(Partition, offset + (lba * BLOCK_SIZE), (uint32_t*)buffer, bufsize);

  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) {
  //Serial.println("Read lba: " + String(lba) + " offset: " + String(offset) + " bufsize: " + String(bufsize));

  // Read data from flash memory in blocks and store in buffer
  _flash.partitionRead(Partition, offset + (lba * BLOCK_SIZE), (uint32_t*)buffer, bufsize);

  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
  Serial.println("onStartStop");
  return true;
}

static void usbEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t * data = (arduino_usb_event_data_t*)event_data;
    switch (event_id) {
      case ARDUINO_USB_STARTED_EVENT:
        Serial.println("USB PLUGGED");
        break;
      case ARDUINO_USB_STOPPED_EVENT:
        Serial.println("USB UNPLUGGED");
        break;
      case ARDUINO_USB_SUSPEND_EVENT:
        Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
        break;
      case ARDUINO_USB_RESUME_EVENT:
        Serial.println("USB RESUMED");
        break;

      default:
        break;
    }
  } else if (event_base == ARDUINO_FIRMWARE_MSC_EVENTS) {
    arduino_firmware_msc_event_data_t * data = (arduino_firmware_msc_event_data_t*)event_data;
    switch (event_id) {
      case ARDUINO_FIRMWARE_MSC_START_EVENT:
        Serial.println("MSC Update Start");
        break;
      case ARDUINO_FIRMWARE_MSC_WRITE_EVENT:
        //HWSerial.printf("MSC Update Write %u bytes at offset %u\n", data->write.size, data->write.offset);
        Serial.print(".");
        break;
      case ARDUINO_FIRMWARE_MSC_END_EVENT:
        Serial.printf("\nMSC Update End: %u bytes\n", data->end.size);
        break;
      case ARDUINO_FIRMWARE_MSC_ERROR_EVENT:
        Serial.printf("MSC Update ERROR! Progress: %u bytes\n", data->error.size);
        break;
      case ARDUINO_FIRMWARE_MSC_POWER_EVENT:
        Serial.printf("MSC Update Power: power: %u, start: %u, eject: %u", data->power.power_condition, data->power.start, data->power.load_eject);
        break;

      default:
        break;
    }
  }
}


void usb_msc_init() {
    Serial.println("Initializing MSC");
    SPIFFS.end();
    delay(3000);
    // Initialize USB metadata and callbacks for MSC (Mass Storage Class)
      Serial.println("Getting partition info");
      // Get partition information
      Partition = partition();
      msc.vendorID("ESP32");
      msc.productID("USB_MSC");
      msc.productRevision("1.0");
      msc.onRead(onRead);
      msc.onWrite(onWrite);
      msc.onStartStop(onStartStop);
      msc.mediaPresent(true);
      Serial.println("go to msc begin");
      msc.begin(Partition->size / BLOCK_SIZE, BLOCK_SIZE);

    Serial.println("go to usb begin");
    USB.begin();

    Serial.println("Printing flash size");

    //Print flash size
    char buff[50];
    sprintf(buff, "Flash Size: %d", Partition->size);
    Serial.println(buff);
  
}


void listDir(fs::FS& fs, const char* dirname, int numTabs) {
  File dir = fs.open(F(dirname));

  if (!dir) {
    Serial.print("Failed to open directory: ");
    Serial.println(dirname);
    return;
  }
  if (!dir.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  while (true) {
    File file =  dir.openNextFile();
    if (!file) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(file.name());
    if (file.isDirectory()) {
      Serial.println("/");
      // Ugly string concatenation to get the full path
      listDir(fs, ((std::string)"/" + (std::string)file.name()).c_str(), numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t\tSize: ");
      Serial.println(file.size(), DEC);
    }
    file.close();
  }
}