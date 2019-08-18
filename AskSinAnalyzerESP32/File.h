//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


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

void initSessionLogOnSPIFFS() {
  if (spiffsAvailable) {
    if (SPIFFS.exists(SPIFFS_SESSIONLOG_FILENAME)) {
      DPRINTLN(F(" - SPIFFS deleting old Session Log file"));
      deleteFile(SPIFFS, SPIFFS_SESSIONLOG_FILENAME);
    }
  }
}

bool initSPIFFS() {
  DPRINTLN(F("- INIT SPIFFS"));
  if (!SPIFFS.begin(true)) {
    DPRINTLN(F(" - SPIFFS: Mount Failed. Trying to format..."));
    SPIFFS.format();
    if (SPIFFS.begin(true)) {
      DPRINTLN(F(" - SPIFFS: Now it is working! "));
    } else {
      DPRINTLN(F(" - SPIFFS: FATAL: SPIFFS NOT MOUNTABLE!"));
      return false;
    }
  }
  DPRINTLN(" - SPIFFS: Mount OK");
  DPRINT(F(" - SPIFFS: Total kB: "));
  DPRINTLN(getSPIFFSSizeKB());
  DPRINT(F(" - SPIFFS: Used  kB: "));
  DPRINTLN(getSPIFFSUsedKB());
  DPRINT(F(" - SPIFFS: Free  kb: "));
  DPRINTLN(getSPIFFSSizeKB() - getSPIFFSUsedKB());
  
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
            DPRINTLN(F(" - SD created backup of CSV"));
            return RENAME_SUCCESSFUL;
          } else {
            DPRINTLN(F(" - SD create backup of CSV failed"));
            return RENAME_FAILED;
          }
        } else {
          DPRINTLN(F(" - SD file does not exist no need to rename"));
          return FILE_DOES_NOT_EXIST;
        }
      }
      if (deleteFile(SD, fileName)) {
        DPRINTLN(F(" - SD file deleted"));
        return NO_ERROR;
      } else {
        DPRINTLN(F(" - SD delete failed"));
        return DELETE_FAILED;
      }
    } else {
      DPRINTLN(F(" - SD file does not exist. no need to delete or rename"));
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
              DPRINTLN(F(" - SPIFFS created backup of CSV"));
              return RENAME_SUCCESSFUL;
            } else {
              DPRINTLN(F(" - SPIFFS create backup of CSV failed"));
              return RENAME_FAILED;
            }
          } else {
            DPRINTLN(F(" - SPIFFS file does not exist no need to rename"));
            return FILE_DOES_NOT_EXIST;
          }
        }
        if (SPIFFS.remove(fileName)) {
          DPRINTLN(F(" - SPIFFS file deleted"));
          return NO_ERROR;
        } else {
          DPRINTLN(F(" - SPIFFS delete failed"));
          return DELETE_FAILED;
        }
      } else {
        DPRINTLN(F(" - SPIFFS file does not exist. no need to delete or rename"));
        return FILE_DOES_NOT_EXIST;
      }
    } else {
      DPRINTLN(F(" - SPIFFS deleteCSV not done; SPIFFS not available!"));
      return SPIFFS_NOT_AVAILABLE;
    }
  }
}

void IRAM_ATTR writeCSVtoSD(const char * fileName, String &csvLine) {
  DPRINTLN(F(" - writing CSV file"));
  if (sdAvailable) {
    if (!SD.exists(fileName)) {
      DPRINTLN(F(" - SD failed to open file - creating new"));
      File file = SD.open(fileName, FILE_WRITE);
      if (!file) {
        DPRINTLN(F(" - SD failed to open file for writing"));
        return;
      } else {
        if (file.println(CSV_HEADER)) {
        } else {
          DPRINTLN(F(" - SD failed to write line into file"));
        }
        file.close();
      }
    }

    File file = SD.open(fileName, FILE_APPEND);
    if (!file) {
      DPRINTLN(F(" - SD csv : failed to open file for appending"));
    }
    if (file.println(csvLine)) {
      DPRINTLN(F(" - SD csv : message appended"));
      file.close();
    } else {
      DPRINTLN(F(" - SD csv : append failed"));
    }
  }
}

void IRAM_ATTR writeSessionLogToSPIFFS(_LogTable &lt) {
  if (spiffsAvailable) {
    if (!SPIFFS.exists(SPIFFS_SESSIONLOG_FILENAME)) {
      DPRINTLN(F(" - failed to open file - creating new"));
      File file = SPIFFS.open(SPIFFS_SESSIONLOG_FILENAME, FILE_WRITE);
      if (!file) {
        DPRINTLN(F(" - SPIFFS failed to open file for writing"));
        return;
      }
    }

    File file = SPIFFS.open(SPIFFS_SESSIONLOG_FILENAME, FILE_APPEND);
    if (!file) {
      DPRINTLN(F(" - SPIFFS Session Log : failed to open file for appending"));
    }

    String logline =  createCSVFromLogTableEntry(lt, false);

    if (file.println(logline)) {
      DPRINTLN(F(" - SPIFFS Session Log : message appended"));
      file.close();
    } else {
      DPRINTLN(F(" - SPIFFS Session Log : append failed"));
    }
  } else {
    DPRINTLN(F(" - SPIFFS Session Log not written; SPIFFS not available!"));
  }
}

void writeLogEntryToCSV(const _LogTable &lt) {
  // Write to CSV
  DPRINTLN(F("Preprocessing CSV"));

  String csvLine = createCSVFromLogTableEntry(lt, true);

  if (getSDCardTotalSpaceMB() - getSDCardUsedSpaceMB() > csvLine.length())
    writeCSVtoSD(CSV_FILENAME, csvLine);
  else
    DPRINTLN(F("writeLogEntryToCSV failed - not enough space"));
}
#endif
