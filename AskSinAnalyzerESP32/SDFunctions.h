//- -----------------------------------------------------------------------------------------------------------------------
// AskSinAnalyzer
// 2019-06-01 jp112sdl Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

#ifndef SDFUNCTIONS_H_
#define SDFUNCTIONS_H_

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
    DPRINTLN(F("SD File renamed"));
    return true;
  } else {
    DPRINTLN(F("SD Rename failed"));
    return false;
  }
}

bool deleteFile(fs::FS &fs, const char * path) {
  DPRINT(F("Deleting file: ")); DPRINTLN(path);
  if (fs.remove(path)) {
    DPRINTLN(F("SD File deleted"));
    return true;
  } else {
    DPRINTLN(F("SD Delete failed"));
    return false;
  }
}

#endif
