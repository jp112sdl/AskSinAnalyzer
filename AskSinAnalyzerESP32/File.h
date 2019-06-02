//
// 2019-05-30 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//

#ifndef __FILE__H_
#define __FILE__H_

enum SPIFFS_ERRORS {
  NO_ERROR = 0,
  DELETE_FAILED = 10,
  RENAME_FAILED = 11,
  FILE_DOES_NOT_EXIST = 2,
  RENAME_SUCCESSFUL = 4,
  SPIFFS_NOT_AVAILABLE = 99
};

bool initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println(F("SPIFFS: Mount Failed. Trying to format..."));
    SPIFFS.format();
    if (SPIFFS.begin(true)) {
      Serial.println(F("SPIFFS: Now it is working! "));
    } else {
      Serial.println(F("SPIFFS: FATAL: SPIFFS NOT MOUNTABLE!"));
      return false;
    }
  }
  Serial.println("SPIFFS: Mount OK");
  Serial.print("SPIFFS: Total Bytes: ");
  Serial.println(SPIFFS.totalBytes());
  Serial.print("SPIFFS: Used Bytes : ");
  Serial.println(SPIFFS.usedBytes());
  Serial.print("SPIFFS: Free Bytes : ");
  Serial.println(SPIFFS.totalBytes() - SPIFFS.usedBytes());

  return true;
}

uint8_t IRAM_ATTR deleteCSV(const char * fileName, bool createBackup) {
  if (spiffsAvailable) {
    if (SPIFFS.exists(fileName)) {
      if (createBackup) {
        if (SPIFFS.exists(fileName)) {
          String bakFile = String(fileName) + ".bak";
          SPIFFS.remove(bakFile);
          if (SPIFFS.rename(fileName, bakFile)) {
            Serial.println(" - created backup of CSV");
            return RENAME_SUCCESSFUL;
          } else {
            Serial.println(" - create backup of CSV failed");
            return RENAME_FAILED;
          }
        } else {
          Serial.println(" - file does not exist no need to rename");
          return FILE_DOES_NOT_EXIST;
        }
      }
      if (SPIFFS.remove(fileName)) {
        Serial.println(" - file deleted");
        return NO_ERROR;
      } else {
        Serial.println(" - delete failed");
        return DELETE_FAILED;
      }
    } else {
      Serial.println(" - file does not exist. no need to delete or rename");
      return FILE_DOES_NOT_EXIST;
    }
  } else {
    Serial.println("deleteCSV not done; SPIFFS not available!");
    return SPIFFS_NOT_AVAILABLE;
  }
}

void IRAM_ATTR writeCSV(const char * fileName, String &csvLine) {
  if (spiffsAvailable) {
    Serial.println(" - writing CSV file");
    if (!SPIFFS.exists(fileName)) {
      Serial.println(" - failed to open file - creating new");
      File file = SPIFFS.open(fileName, FILE_WRITE);
      if (!file) {
        Serial.println(" - failed to open file for writing");
        return;
      } else {
        if (file.println(CSV_HEADER)) {
        } else {
          Serial.println(" - failed to write line into file");
        }
        file.close();
      }
    }

    File file = SPIFFS.open(fileName, FILE_APPEND);
    if (!file) {
      Serial.println(" - csv : failed to open file for appending");
    }
    if (file.println(csvLine)) {
      Serial.println(" - csv : message appended");
      file.close();
    } else {
      Serial.println(" - csv : append failed");
    }
  } else {
    Serial.println("writeCSV not done; SPIFFS not available!");
  }
}
#endif
