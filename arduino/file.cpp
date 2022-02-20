#include "FS.h"
#include "SD_MMC.h"
#include "SD.h"
#include "config.h"

bool writeFile(const char *path, const unsigned char *data, unsigned long len)
{
	Serial.printf("Writing file: %s\n", path);
#if USE_SD
  File file = SD.open(path, FILE_WRITE);
#else
  File file = SD_MMC.open(path, FILE_WRITE);
#endif
	if (!file)
	{
		Serial.println("Failed to open file for writing");
		return false;
	}
	if (file.write(data, len))
	{
		Serial.println("File written");
	}
	else
	{
		Serial.println("Write failed");
		return false;
	}
	file.close();
	return true;
}

bool appendFile(const char *path, const unsigned char *data, unsigned long len)
{
	Serial.printf("Appending to file: %s\n", path);
#if USE_SD
  File file = SD.open(path, FILE_APPEND);
#else
  File file = SD_MMC.open(path, FILE_APPEND);
#endif
	if (!file)
	{
		Serial.println("Failed to open file for writing");
		return false;
	}
	if (file.write(data, len))
	{
		Serial.println("File written");
	}
	else
	{
		Serial.println("Write failed");
		return false;
	}
	file.close();
	return true;
}

bool initFileSystem()
{
#if USE_SD
  Serial.println("SD Card work with SPI mode!");
  SPI.begin(14,2,15,13);
  if (!SD.begin(13, SPI, 1000000)) 
  {
    Serial.println("Card Mount Failed");
    return false;
  }
 uint8_t cardType = SD.cardType();
#else
  Serial.println("SD Card work with SDIO mode!");
  if (!SD_MMC.begin())
  {
    Serial.println("Card Mount Failed");
    return false;
  }
 uint8_t cardType = SD_MMC.cardType();
#endif

	if (cardType == CARD_NONE)
	{
		Serial.println("No SD card attached");
		return false;
	}
	Serial.print("SD Card Type: ");
	if (cardType == CARD_MMC)
		Serial.println("MMC");
	else if (cardType == CARD_SD)
		Serial.println("SDSC");
	else if (cardType == CARD_SDHC)
		Serial.println("SDHC");
	else

		Serial.println("UNKNOWN");
#if USE_SD
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
#else
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD_MMC.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
#endif
	return true;
}

bool createDir(const char *path)
{
	Serial.printf("Creating Dir: %s\n", path);
#if USE_SD
  if (SD.mkdir(path))
#else
  if (SD_MMC.mkdir(path))
#endif
	{
		Serial.println("Dir created");
	}
	else
	{
		Serial.println("mkdir failed");
		return false;
	}
	return true;
}

bool fileExists(const char *path)
{
#if USE_SD
  return SD.exists(path);
#else
  return SD_MMC.exists(path);
#endif
}
