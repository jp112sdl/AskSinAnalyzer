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

uint32_t getSPIFFSSizeKB() {
  return (uint32_t)SPIFFS.totalBytes() / 1024UL;
}

uint32_t getSPIFFSUsedKB() {
  return (uint32_t)SPIFFS.usedBytes() / 1024UL;
}

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
  Serial.print("SPIFFS: Total kB: ");
  Serial.println(getSPIFFSSizeKB());
  Serial.print("SPIFFS: Used  kB: ");
  Serial.println(getSPIFFSUsedKB());
  Serial.print("SPIFFS: Free  kb: ");
  Serial.println(getSPIFFSSizeKB() - getSPIFFSUsedKB());

  return true;
}

uint8_t IRAM_ATTR deleteCSV(const char * fileName, bool createBackup) {
  if (sdAvailable) {
    if (SD.exists(fileName)) {
      if (createBackup) {
        if (SD.exists(fileName)) {
          String bakFile = String(fileName) + ".bak";
          deleteFile(SD, bakFile.c_str());
          if (renameFile(SD, fileName, bakFile.c_str())) {
            Serial.println(F(" - SD created backup of CSV"));
            return RENAME_SUCCESSFUL;
          } else {
            Serial.println(F(" - SD create backup of CSV failed"));
            return RENAME_FAILED;
          }
        } else {
          Serial.println(F(" - SD file does not exist no need to rename"));
          return FILE_DOES_NOT_EXIST;
        }
      }
      if (deleteFile(SD, fileName)) {
        Serial.println(F(" - SD file deleted"));
        return NO_ERROR;
      } else {
        Serial.println(F(" - SD delete failed"));
        return DELETE_FAILED;
      }
    } else {
      Serial.println(F(" - SD file does not exist. no need to delete or rename"));
      return FILE_DOES_NOT_EXIST;
    }
  }
  else {
    if (spiffsAvailable) {
      if (SPIFFS.exists(fileName)) {
        if (createBackup) {
          if (SPIFFS.exists(fileName)) {
            String bakFile = String(fileName) + ".bak";
            SPIFFS.remove(bakFile);
            if (SPIFFS.rename(fileName, bakFile)) {
              Serial.println(F(" - SPIFFS created backup of CSV"));
              return RENAME_SUCCESSFUL;
            } else {
              Serial.println(F(" - SPIFFS create backup of CSV failed"));
              return RENAME_FAILED;
            }
          } else {
            Serial.println(F(" - SPIFFS file does not exist no need to rename"));
            return FILE_DOES_NOT_EXIST;
          }
        }
        if (SPIFFS.remove(fileName)) {
          Serial.println(F(" - SPIFFS file deleted"));
          return NO_ERROR;
        } else {
          Serial.println(F(" - SPIFFS delete failed"));
          return DELETE_FAILED;
        }
      } else {
        Serial.println(F(" - SPIFFS file does not exist. no need to delete or rename"));
        return FILE_DOES_NOT_EXIST;
      }
    } else {
      Serial.println(F("SPIFFS deleteCSV not done; SPIFFS not available!"));
      return SPIFFS_NOT_AVAILABLE;
    }
  }
}

void IRAM_ATTR writeCSV(const char * fileName, String &csvLine) {
  Serial.println(F(" - writing CSV file"));
  if (sdAvailable) {
    if (!SD.exists(fileName)) {
      Serial.println(F(" - SD failed to open file - creating new"));
      File file = SD.open(fileName, FILE_WRITE);
      if (!file) {
        Serial.println(F(" - SD failed to open file for writing"));
        return;
      } else {
        if (file.println(CSV_HEADER)) {
        } else {
          Serial.println(F(" - SD failed to write line into file"));
        }
        file.close();
      }
    }

    File file = SD.open(fileName, FILE_APPEND);
    if (!file) {
      Serial.println(F(" - SD csv : failed to open file for appending"));
    }
    if (file.println(csvLine)) {
      Serial.println(F(" - SD csv : message appended"));
      file.close();
    } else {
      Serial.println(F(" - SD csv : append failed"));
    }
  }
  else {
    if (spiffsAvailable) {
      if (!SPIFFS.exists(fileName)) {
        Serial.println(F(" - failed to open file - creating new"));
        File file = SPIFFS.open(fileName, FILE_WRITE);
        if (!file) {
          Serial.println(F(" - SPIFFS failed to open file for writing"));
          return;
        } else {
          if (file.println(CSV_HEADER)) {
          } else {
            Serial.println(F(" - SPIFFS failed to write line into file"));
          }
          file.close();
        }
      }

      File file = SPIFFS.open(fileName, FILE_APPEND);
      if (!file) {
        Serial.println(F(" - SPIFFS csv : failed to open file for appending"));
      }
      if (file.println(csvLine)) {
        Serial.println(F(" - SPIFFS csv : message appended"));
        file.close();
      } else {
        Serial.println(F(" - SPIFFS csv : append failed"));
      }
    } else {
      Serial.println(F("SPIFFS writeCSV not done; SPIFFS not available!"));
    }
  }
}
#endif
