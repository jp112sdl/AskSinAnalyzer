//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------


#ifndef __FILE__H_
#define __FILE__H_

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  DPRINT(F("Listing directory: ")); DPRINTLN(dirname);

  File root = fs.open(dirname);
  if (!root) {
    DPRINTLN("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    DPRINTLN("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      DPRINT("  DIR : ");
      DPRINTLN(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      DPRINT("  FILE: ");
      DPRINT(file.name());
      DPRINT("  SIZE: ");
      DPRINTLN(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path) {
  DPRINT(F("Creating Dir: ")); DPRINTLN(path);
  if (fs.mkdir(path)) {
    DPRINTLN("Dir created");
  } else {
    DPRINTLN("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path) {
  DPRINT(F("Removing Dir: ")); DPRINTLN(path);
  if (fs.rmdir(path)) {
    DPRINTLN("Dir removed");
  } else {
    DPRINTLN("rmdir failed");
  }
}

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

void appendFile(fs::FS &fs, const char * path, const char * message) {
  DPRINT(F("Appending to file: ")); DPRINTLN(path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    DPRINTLN("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    DPRINTLN("Message appended");
  } else {
    DPRINTLN("Append failed");
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

enum FFat_ERRORS {
  NO_ERROR = 0,
  DELETE_FAILED = 10,
  RENAME_FAILED = 11,
  FILE_DOES_NOT_EXIST = 2,
  RENAME_SUCCESSFUL = 4,
  FFat_NOT_AVAILABLE = 99
};

uint32_t getFFatSizeKB() {
  return (uint32_t)FFat.totalBytes() / 1024UL;
}

uint32_t getFFatUsedKB() {
  return ((uint32_t)FFat.totalBytes() / 1024UL) - ((uint32_t)FFat.freeBytes() / 1024UL);
}

void initSessionLogOnFFat() {
  if (ffatAvailable) {
    if (FFat.exists(FFat_SESSIONLOG_FILENAME)) {
      DPRINTLN(F(" - FFat deleting old Session Log file"));
      deleteFile(FFat, FFat_SESSIONLOG_FILENAME);
    }
  }
}

bool initFFat() {
  DPRINTLN(F("- INIT FFat"));
  if (!FFat.begin(true)) {
    DPRINTLN(F(" - FFat: Mount Failed. Trying to format..."));
    FFat.format();
    if (FFat.begin(true)) {
      DPRINTLN(F(" - FFat: Now it is working! "));
    } else {
      DPRINTLN(F(" - FFat: FATAL: FFat NOT MOUNTABLE!"));
      return false;
    }
  }
  DPRINTLN(" - FFat: Mount OK");
  DPRINT(F(" - FFat: Total kB: "));
  DPRINTLN(getFFatSizeKB());
  DPRINT(F(" - FFat: Used  kB: "));
  DPRINTLN(getFFatUsedKB());
  DPRINT(F(" - FFat: Free  kb: "));
  DPRINTLN(getFFatSizeKB() - getFFatUsedKB());

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
    if (ffatAvailable) {
      if (FFat.exists(fileName)) {
        if (createBackup) {
          if (FFat.exists(fileName)) {
            String bakFile = String(fileName) + ".bak";
            FFat.remove(bakFile);
            if (FFat.rename(fileName, bakFile)) {
              DPRINTLN(F(" - FFat created backup of CSV"));
              return RENAME_SUCCESSFUL;
            } else {
              DPRINTLN(F(" - FFat create backup of CSV failed"));
              return RENAME_FAILED;
            }
          } else {
            DPRINTLN(F(" - FFat file does not exist no need to rename"));
            return FILE_DOES_NOT_EXIST;
          }
        }
        if (FFat.remove(fileName)) {
          DPRINTLN(F(" - FFat file deleted"));
          return NO_ERROR;
        } else {
          DPRINTLN(F(" - FFat delete failed"));
          return DELETE_FAILED;
        }
      } else {
        DPRINTLN(F(" - FFat file does not exist. no need to delete or rename"));
        return FILE_DOES_NOT_EXIST;
      }
    } else {
      DPRINTLN(F(" - FFat deleteCSV not done; FFat not available!"));
      return FFat_NOT_AVAILABLE;
    }
  }
}

void writeCSVtoSD(const char * fileName, String &csvLine) {
  DPRINTLN(F("- writing CSV file"));
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

void writeSessionLogToFFat(_LogTable &lt) {
  if (ffatAvailable) {
    if (!FFat.exists(FFat_SESSIONLOG_FILENAME)) {
      DPRINTLN(F(" - failed to open file - creating new"));
      File file = FFat.open(FFat_SESSIONLOG_FILENAME, FILE_WRITE);
      if (!file) {
        DPRINTLN(F(" - FFat failed to open file for writing"));
        return;
      }
      file.close();
    }

    File file = FFat.open(FFat_SESSIONLOG_FILENAME, FILE_APPEND);
    if (!file) {
      DPRINTLN(F(" - FFat Session Log : failed to open file for appending"));
    }

    String logline =  createCSVFromLogTableEntry(lt, false);
    uint32_t freeBytes = FFat.freeBytes();
    if (freeBytes > logline.length()) {
      if (file.println(logline)) {
        DPRINTLN(F(" - FFat Session Log : message appended"));
        file.close();
      } else {
        DPRINTLN(F(" - FFat Session Log : append failed"));
      }
    } else {
      DPRINT(F(" - FFat Session Log : no space left. Free Bytes: ")); DDECLN(freeBytes);
    }

  } else {
    DPRINTLN(F(" - FFat Session Log not written; FFat not available!"));
  }
}

void writeLogEntryToSD(const _LogTable &lt) {
  if (sdAvailable) {
    String csvLine = createCSVFromLogTableEntry(lt, true);

    if (getSDCardTotalSpaceMB() - getSDCardUsedSpaceMB() > csvLine.length())
      writeCSVtoSD(CSV_FILENAME, csvLine);
    else
      DPRINTLN(F("writeLogEntryToCSV failed - not enough space"));
  }
}
#endif
