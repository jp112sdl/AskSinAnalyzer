//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#ifndef __FILE__H_
#define __FILE__H_

//Session Log
const uint8_t  maxSessionFiles           = 11;
const uint32_t maxLinesPerSessionFile    = 100;
uint8_t        currentSessionFileNum     = 0;
uint32_t       currentLinesInSessionFile = 0;
//

String readFile(fs::FS &fs, const char * path) {
  DPRINT(F("Reading file: ")); DPRINTLN(path);

  File file = fs.open(path);
  if (!file) {
    DPRINTLN("Failed to open file for reading");
    return "";
  }

  DPRINT("Read from file: ");
  String fileContent = "";
  while (file.available()) {
    fileContent += (char)file.read();
  }
  file.close();
  DPRINTLN(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  DPRINT(F("Writing file: ")); DPRINTLN(path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    DPRINTLN("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    DPRINTLN("File written");
  } else {
    DPRINTLN("Write failed");
  }
  file.close();
}

bool renameFile(fs::FS &fs, const char * path1, const char * path2) {
  DPRINT(F("Renaming file ")); DPRINT(path1); DPRINT(F(" to ")); DPRINTLN(path2);
  if (fs.rename(path1, path2)) {
    DPRINTLN(F("- File renamed"));
    return true;
  } else {
    DPRINTLN(F("- Rename failed"));
    return false;
  }
}

bool deleteFile(fs::FS &fs, const char * path) {
  DPRINT(F("Deleting file: ")); DPRINTLN(path);
  if (fs.remove(path)) {
    DPRINTLN(F("- File deleted"));
    return true;
  } else {
    DPRINTLN(F("- Delete failed"));
    return false;
  }
}

uint32_t getSDCardSizeMB() {
  return (uint32_t)SD.cardSize() / (1024 * 1024UL);
}

uint32_t getSDCardTotalSpaceMB() {
  return (uint32_t)SD.totalBytes() / (1024 * 1024UL);
}

uint32_t getSDCardUsedSpaceMB() {
  return (uint32_t)SD.usedBytes() / (1024 * 1024UL);
}

bool SdInit() {
  DPRINTLN(F("- INIT SD CARD"));
  if (!SD.begin(SD_CS)) {
    delay(250);
    if (!SD.begin(SD_CS)) {
      DPRINTLN(F(" - Card Mount Failed "));
      return false;
    }
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    DPRINTLN(F(" - No SD card attached"));
    return false;
  }

  DPRINT(F(" - SD Card Type  : "));
  if (cardType == CARD_MMC) {
    DPRINTLN(F("MMC"));
  } else if (cardType == CARD_SD) {
    DPRINTLN(F("SDSC"));
  } else if (cardType == CARD_SDHC) {
    DPRINTLN(F("SDHC"));
  } else {
    DPRINTLN(F("UNKNOWN"));
  }

  DPRINT(F(" - SD Card Size MB: "));
  DPRINTLN(getSDCardSizeMB());
  DPRINT(F(" - Total space  MB: "));
  DPRINTLN(getSDCardTotalSpaceMB());
  DPRINT(F(" - Used space   MB: "));
  DPRINTLN(getSDCardUsedSpaceMB());

  return true;
}

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
  //return ((uint32_t)SPIFFS.totalBytes() / 1024UL) - ((uint32_t)SPIFFS.freeBytes() / 1024UL);
  return (uint32_t)SPIFFS.usedBytes() / 1024UL;
}

String getSessionFileName(uint8_t fileNum) {
  return "/" + String(fileNum) + ".log";
}

void initSessionLogOnSPIFFS() {
  if (SPIFFSAvailable) {
    DPRINTLN(F(" - SPIFFS deleting old Session Log files"));
    for (uint8_t i = 0; i < maxSessionFiles; i++) {
      //deleteFile(SPIFFS, getSessionFileName(i).c_str());
      File file = SPIFFS.open(getSessionFileName(i).c_str(), FILE_WRITE);
      file.close();
      currentSessionFileNum     = 0;
      currentLinesInSessionFile = 0;
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

uint8_t deleteCSV(const char * fileName, bool createBackup) {
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
    if (SPIFFSAvailable) {
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

void writeCSVtoSD(const char * fileName, String &csvLine) {
#ifdef VDEBUG
  DPRINTLN(F("- writing CSV file"));
#endif
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
#ifdef VDEBUG
      DPRINTLN(F(" - SD csv : message appended"));
#endif
      file.close();
    } else {
      DPRINTLN(F(" - SD csv : append failed"));
    }
  }
}

void writeSessionLogToSPIFFS(_LogTable &lt) {
  if (SPIFFSAvailable) {
    //uint32_t freeBytes = SPIFFS.freeBytes();
    uint32_t freeBytes = SPIFFS.totalBytes() - SPIFFS.usedBytes();
    String logline =  createCSVFromLogTableEntry(lt, false);
    if (freeBytes > logline.length()) {
      File file = SPIFFS.open(getSessionFileName(currentSessionFileNum).c_str(), FILE_APPEND);
      if (!file) {
        DPRINTLN(F(" - SPIFFS Session Log : failed to open file for appending"));
      }

      if (file.println(logline)) {
#ifdef VDEBUG
        DPRINT(F(" - SPIFFS Session Log : message appended, F: ")); DDEC(currentSessionFileNum); DPRINT(F(" L: ")); DDECLN(currentLinesInSessionFile);
#endif
        file.close();
      } else {
        DPRINTLN(F(" - SPIFFS Session Log : append failed"));
      }

      currentLinesInSessionFile++;
      if (currentLinesInSessionFile >= maxLinesPerSessionFile) {
        currentLinesInSessionFile = 0;
        currentSessionFileNum++;
        if (currentSessionFileNum >= maxSessionFiles) {
          currentSessionFileNum = 0;
        }
        if (SPIFFS.exists(getSessionFileName(currentSessionFileNum).c_str())) {
          //deleteFile(SPIFFS, getSessionFileName(currentSessionFileNum).c_str());
          File file = SPIFFS.open(getSessionFileName(currentSessionFileNum).c_str(), FILE_WRITE);
          file.close();
        }
      }
    } else {
      DPRINT(F(" - SPIFFS Session Log : no space left. Free Bytes: ")); DDECLN(freeBytes);
    }
  } else {
    DPRINTLN(F(" - SPIFFS Session Log not written; SPIFFS not available!"));
  }
}

void writeLogEntryToSD(const _LogTable &lt) {
  if (sdAvailable) {
    String csvLine = createCSVFromLogTableEntry(lt, true);

    if (getSDCardTotalSpaceMB() - getSDCardUsedSpaceMB() > csvLine.length())
      writeCSVtoSD(CSV_FILENAME().c_str(), csvLine);
    else
      DPRINTLN(F("writeLogEntryToCSV failed - not enough space"));
  }
}
#endif
